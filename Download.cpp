#include "Download.h"
#include <SDL3/SDL.h>

#include <filesystem>
#include <fstream>
#include <string>

#include <curl/curl.h>

CURL* easy_handle = nullptr;
CURLM* multi_handle = curl_multi_init();

FILE* file = nullptr;

std::vector<std::string> urls_to_download;
std::string last_url = "";

std::function<void()> download_callback = nullptr;

void write_data(void* ptr, size_t size, size_t nmemb, FILE* stream) {
  size_t written = fwrite(ptr, size, nmemb, stream);
  if (written != size * nmemb) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to write data to file");
  }
}

void downloadList(std::string url) {
  if (!easy_handle) {
    easy_handle = curl_easy_init();
    if (!easy_handle) {
      SDL_LogError(SDL_LOG_CATEGORY_ERROR, "curl_easy_init failed");
    }
  }
  curl_easy_setopt(easy_handle, CURLOPT_URL, url.c_str());
  curl_easy_setopt(easy_handle, CURLOPT_FOLLOWLOCATION, 1L);
  curl_easy_setopt(easy_handle, CURLOPT_WRITEFUNCTION, write_data);

  long max_file_size_bytes = 25 * 1024 * 1024L;  // 25 MB
  curl_easy_setopt(easy_handle, CURLOPT_MAXFILESIZE, max_file_size_bytes);
  std::string filename = "creatures/" + url.substr(url.find_last_of("/") + 1);
  SDL_Log("Downloading %s to %s", url.c_str(), filename.c_str());
  // create directory if it doesn't exist
  std::filesystem::create_directories("creatures");
  file = fopen(filename.c_str(), "wb");
  if (!file) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to open file %s for writing",
                 filename.c_str());
    return;
  }

  curl_easy_setopt(easy_handle, CURLOPT_WRITEDATA, file);
}

void downloadCreatures(std::function<void()> callback) {
  if (!multi_handle) { // so make it god damn it >:(
    multi_handle = curl_multi_init();
    if (!multi_handle) {
      SDL_LogError(SDL_LOG_CATEGORY_ERROR, "curl_multi_init failed");
      if (callback) callback();
      return;
    }
  }

  easy_handle = curl_easy_init();
  if (!easy_handle) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "curl_easy_init failed");
    curl_multi_cleanup(multi_handle);
    if (callback) callback();
    return;
  }

  // Grab the list of creatures to download
  std::ifstream infile("creature_mirrors.txt");
  if (!infile.is_open()) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to open creature_mirrors.txt");
    curl_easy_cleanup(easy_handle);
    curl_multi_cleanup(multi_handle);
    if (callback) callback();
    return;
  }

  download_callback = callback;

  // line by line, download each URL to the creatures/ directory
  std::string url;
  while (std::getline(infile, url)) {
    urls_to_download.push_back(url);
  }

  infile.close();
  if (urls_to_download.empty()) {
    SDL_Log("No URLs to download");
    curl_easy_cleanup(easy_handle);
    curl_multi_cleanup(multi_handle);
    if (callback) callback();
    return;
  }

  last_url = urls_to_download.back();
  downloadList(last_url);
  urls_to_download.pop_back();
  curl_multi_add_handle(multi_handle, easy_handle);
}

curlStatus downloadCallbacks() {
  int still_running;
  CURLMcode mc = curl_multi_perform(multi_handle, &still_running);
  curlStatus status;

  // Get current downloaded size
  curl_off_t total_downloaded = 0;
  curl_off_t total_size = 0;
  curl_easy_getinfo(easy_handle, CURLINFO_SIZE_DOWNLOAD_T, &total_downloaded);
  curl_easy_getinfo(easy_handle, CURLINFO_CONTENT_LENGTH_DOWNLOAD_T,
                    &total_size);

  status.downloaded = static_cast<size_t>(total_downloaded);
  status.total = static_cast<size_t>(total_size);

  if (mc != CURLM_OK) {
    SDL_Log("curl_multi_perform() failed, code %d.", mc);

    // Cleanup, retry

    if (file) {
      fclose(file);
      file = nullptr;
    }

    curl_multi_remove_handle(multi_handle, easy_handle);
    curl_easy_cleanup(easy_handle);

    downloadList(last_url);
    curl_multi_add_handle(multi_handle, easy_handle);
    return status;
  }

  if (!still_running) {
    SDL_Log("Downloaded %s, total %zu bytes", last_url.c_str(),
            static_cast<size_t>(total_downloaded));

    if (file) {
      fclose(file);
      file = nullptr;
    }

    if (!urls_to_download.empty()) {
      // Start next download

      last_url = urls_to_download.back();
      urls_to_download.pop_back();
      SDL_Log("Starting next download, %zu remaining", urls_to_download.size());

      curl_multi_remove_handle(multi_handle, easy_handle);
      curl_easy_cleanup(easy_handle);
      easy_handle = nullptr;
      downloadList(last_url);
      curl_multi_add_handle(multi_handle, easy_handle);
      return status;
    }

    curl_multi_remove_handle(multi_handle, easy_handle);

    curl_easy_cleanup(easy_handle);
    easy_handle = nullptr;

    curl_multi_cleanup(multi_handle);
    multi_handle = nullptr;

    if (download_callback) {
      download_callback();
      download_callback = nullptr;
    }
  }

  return status;
}

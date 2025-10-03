#include "Menu.h"
#include "imgui/imgui.h"

#include <SDL3_image/SDL_image.h>
#include <curl/curl.h>

#include <fstream>
#include <filesystem>

#include <vector>

std::string image_data;
std::string image_filename = "downloaded_image.tmp";
CURL* creature_easy = nullptr;
CURLM* creature_multi = curl_multi_init();

std::vector<std::string> splitStringByNewline(std::string& str) {
    std::vector<std::string> result;
    size_t start = 0;
    size_t end = str.find('\n');
    while (end != std::string::npos) {
        result.push_back(str.substr(start, end - start));
        start = end + 1;
        end = str.find('\n', start);
    }
    result.push_back(str.substr(start));
    return result;
}

void getBufferFromOptionalString(const std::optional<std::string>& optStr, char* buffer, size_t bufferSize) {
    if (optStr.has_value()) {
        strncpy(buffer, optStr->c_str(), bufferSize);
        buffer[bufferSize - 1] = '\0'; // Ensure null-termination
    } else {
        buffer[0] = '\0'; // Empty string if no value
    }
}

SDL_Texture* loadTextureFromFile(SDL_Renderer* renderer, const std::string& filePath) {
    SDL_Surface* surface = IMG_Load(filePath.c_str());
    if (!surface) {
      SDL_LogError(SDL_LOG_CATEGORY_ERROR, "IMG_Load failed: %s",
                   SDL_GetError());
        return nullptr;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_DestroySurface(surface);
    if (!texture) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_CreateTextureFromSurface failed: %s", SDL_GetError());
        return nullptr;
    }
    return texture;
}

void displayAbilityScore(std::string name, int& score, bool sameLine = false)
{
  if (sameLine) ImGui::SameLine(150);
  ImGui::Text("%s:", name.c_str());
  ImGui::SameLine();
  ImGui::SetNextItemWidth(75);
  ImGui::InputInt(("##" + name).c_str(), &score);
  ImGui::SameLine(); // modifier
  int mod = std::floor(((float)score - 10.0f) / 2.0f);
  ImGui::Text("%d", mod);
}

size_t curlWriteFunc(char* data, size_t size, size_t nmemb,
                     std::string* buffer) {
  size_t result = 0;

  if (buffer != NULL) {
    buffer->append(data, size * nmemb);
    result = size * nmemb;
  }
  return result;
}

void startImageDownload(const std::string& url) {
  creature_easy = curl_easy_init();
  if (!creature_easy) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "curl_easy_init failed");
    return;
  }

  image_filename = url.substr(url.find_last_of("/\\") + 1);

  SDL_Log("Starting image download from URL: %s", url.c_str());

  curl_easy_setopt(creature_easy, CURLOPT_URL, url.c_str());
  curl_easy_setopt(creature_easy, CURLOPT_FOLLOWLOCATION, 1L);
  curl_easy_setopt(creature_easy, CURLOPT_WRITEDATA, &image_data);
  curl_easy_setopt(creature_easy, CURLOPT_WRITEFUNCTION, curlWriteFunc);
  curl_multi_add_handle(creature_multi, creature_easy);
}

void imageCallbacks(void* s)
{
  ToolState* state = (ToolState*)s;
  int still_running;
  CURLMcode mc = curl_multi_perform(creature_multi, &still_running);

  if (mc != CURLM_OK) {
    SDL_Log("curl_multi_perform() failed, code %d.", mc);
  }

  if (!still_running) {
    SDL_Log("Image download completed.");

    std::ofstream outFile("images/" + image_filename, std::ios::binary);
    outFile.write(image_data.data(), image_data.size());
    outFile.close();

    state->current_creature_texture =
        loadTextureFromFile(state->renderer, "images/" + image_filename);

    state->is_downloading_image = false;
    if (state->current_creature_texture == nullptr) state->image_error = true;

    // Download complete
    curl_multi_remove_handle(creature_multi, creature_easy);
    curl_easy_cleanup(creature_easy);
    creature_easy = nullptr;

    image_data.clear();
  }

}

void checkImage(ToolState* state)
{
  std::string url = state->current_creature.get_img_url().value();
  if (url._Starts_with("http")) {
    std::filesystem::create_directories("images");
    // Check if the image is already downloaded
    if (std::filesystem::exists("images/" +
                                url.substr(url.find_last_of("/\\") + 1))) {
      SDL_Log("Loading cached image from URL: %s", url.c_str());
      state->current_creature_texture = loadTextureFromFile(
          state->renderer, "images/" + url.substr(url.find_last_of("/\\") + 1));
    }
    if (!state->current_creature_texture) {
      state->is_downloading_image = true;
      startImageDownload(url);
    }
  } else {
    SDL_Log("Loading local image from URL: %s", url.c_str());
    SDL_Texture* texture = loadTextureFromFile(state->renderer, url);
    if (texture) state->current_creature_texture = texture;
  }
}

void displayCreatureEdit(void* s) {
  ToolState* state = (ToolState*)s;
  if (!state->current_creature.get_name().has_value()) {
    ImGui::Text("No creature selected");
    return;
  }

  if (state->clear_image)
  {
    if (state->current_creature_texture) {
      SDL_DestroyTexture(state->current_creature_texture);
      state->current_creature_texture = nullptr;
    }
    state->clear_image = false;
    state->is_downloading_image = false;
    state->image_error = false;
    image_data.clear();
    if (creature_easy) {
      curl_multi_remove_handle(creature_multi, creature_easy);
      curl_easy_cleanup(creature_easy);
      creature_easy = nullptr;
    }
  }

  if (state->current_creature_texture == nullptr &&
      state->current_creature.get_img_url().has_value() &&
      !state->is_downloading_image && !state->image_error)
  {
    checkImage(state);
  }

  ImGui::BeginChild("##CreatureEdit", ImVec2(0, 0));
  char nameBuffer[256];
  getBufferFromOptionalString(state->current_creature.get_name(), nameBuffer,
                              sizeof(nameBuffer));
  ImGui::Text("Name:");
  ImGui::PushItemWidth(175);
  if (ImGui::InputText("##Name", nameBuffer, sizeof(nameBuffer))) {
    state->current_creature.set_name(std::string(nameBuffer));
  }
  ImGui::PopItemWidth();

  if (state->is_downloading_image) {
    ImGui::SameLine(200);
    ImGui::Text("Downloading image...");
    imageCallbacks(s);
  }

  if (state->current_creature_texture) {
    ImGui::SameLine(200);
    ImGui::Image((ImTextureID)(intptr_t)state->current_creature_texture,
                 ImVec2(128, 128));
  } else {
    ImGui::Text("No Image");
  }
  char metaBuffer[256];
  getBufferFromOptionalString(state->current_creature.get_meta(), metaBuffer,
                              sizeof(metaBuffer));
  if (ImGui::InputTextMultiline("##Meta", metaBuffer, sizeof(metaBuffer))) {
    state->current_creature.set_meta(std::string(metaBuffer));
  }

  ImGui::Separator();

  char acBuffer[64];
  getBufferFromOptionalString(state->current_creature.get_armor_class(),
                              acBuffer, sizeof(acBuffer));
  ImGui::Text("AC:");
  ImGui::SameLine();
  if (ImGui::InputText("##ArmorClass", acBuffer, sizeof(acBuffer))) {
    state->current_creature.set_armor_class(std::string(acBuffer));
  }
  char hpBuffer[64];
  getBufferFromOptionalString(state->current_creature.get_hit_points(),
                              hpBuffer, sizeof(hpBuffer));
  ImGui::Text("HP:");
  ImGui::SameLine();
  if (ImGui::InputText("##HitPoints", hpBuffer, sizeof(hpBuffer))) {
    state->current_creature.set_hit_points(std::string(hpBuffer));
  }
  char speedBuffer[64];
  getBufferFromOptionalString(state->current_creature.get_speed(), speedBuffer,
                              sizeof(speedBuffer));
  ImGui::Text("Speed:");
  ImGui::SameLine();
  if (ImGui::InputText("##Speed", speedBuffer, sizeof(speedBuffer))) {
    state->current_creature.set_speed(std::string(speedBuffer));
  }

  ImGui::Separator();

  int str = std::stoi(state->current_creature.get_str().value_or("0"));
  int dex = std::stoi(state->current_creature.get_dex().value_or("0"));
  int con = std::stoi(state->current_creature.get_con().value_or("0"));
  int intel =
      std::stoi(state->current_creature.get_creature_int().value_or("0"));
  int wis = std::stoi(state->current_creature.get_wis().value_or("0"));
  int cha = std::stoi(state->current_creature.get_cha().value_or("0"));

  displayAbilityScore("STR", str);
  displayAbilityScore("DEX", dex, true);
  displayAbilityScore("CON", con);
  displayAbilityScore("INT", intel, true);
  displayAbilityScore("WIS", wis);
  displayAbilityScore("CHA", cha, true);

  // this kinda bad but whatever
  state->current_creature.set_str(std::to_string(str));
  state->current_creature.set_dex(std::to_string(dex));
  state->current_creature.set_con(std::to_string(con));
  state->current_creature.set_creature_int(std::to_string(intel));
  state->current_creature.set_wis(std::to_string(wis));
  state->current_creature.set_cha(std::to_string(cha));

  ImGui::Separator();

  char savesBuffer[256];
  getBufferFromOptionalString(state->current_creature.get_saving_throws(),
                              savesBuffer, sizeof(savesBuffer));
  ImGui::Text("Saves:");
  ImGui::SameLine();
  ImGui::PushItemWidth(250);
  if (ImGui::InputText("##Saves", savesBuffer, sizeof(savesBuffer))) {
    state->current_creature.set_saving_throws(std::string(savesBuffer));
  }
  char skillsBuffer[256];
  getBufferFromOptionalString(state->current_creature.get_skills(),
                              skillsBuffer, sizeof(skillsBuffer));
  ImGui::Text("Skills:");
  ImGui::SameLine();
  ImGui::PushItemWidth(250);
  if (ImGui::InputText("##Skills", skillsBuffer, sizeof(skillsBuffer))) {
    state->current_creature.set_skills(std::string(skillsBuffer));
  }
  char sensesBuffer[256];
  getBufferFromOptionalString(state->current_creature.get_senses(),
                              sensesBuffer, sizeof(sensesBuffer));
  ImGui::Text("Senses:");
  ImGui::SameLine();
  ImGui::PushItemWidth(250);
  if (ImGui::InputText("##Senses", sensesBuffer, sizeof(sensesBuffer))) {
    state->current_creature.set_senses(std::string(sensesBuffer));
  }
  char languagesBuffer[256];
  getBufferFromOptionalString(state->current_creature.get_languages(),
                              languagesBuffer, sizeof(languagesBuffer));
  ImGui::Text("Languages:");
  ImGui::SameLine();
  ImGui::PushItemWidth(250);
  if (ImGui::InputText("##Languages", languagesBuffer, sizeof(languagesBuffer))) {
    state->current_creature.set_languages(std::string(languagesBuffer));
  }
  char challengeBuffer[64];
  getBufferFromOptionalString(state->current_creature.get_challenge(),
                              challengeBuffer, sizeof(challengeBuffer));
  ImGui::Text("Challenge:");
  ImGui::SameLine();
  ImGui::PushItemWidth(250);
  if (ImGui::InputText("##Challenge", challengeBuffer, sizeof(challengeBuffer))) {
    state->current_creature.set_challenge(std::string(challengeBuffer));
  }

  char imgUrlBuffer[256];
  getBufferFromOptionalString(state->current_creature.get_img_url(),
                              imgUrlBuffer, sizeof(imgUrlBuffer));
  ImGui::Text("Image URL:");
  ImGui::SameLine();
  ImGui::PushItemWidth(250);
  if (ImGui::InputText("##ImageURL", imgUrlBuffer, sizeof(imgUrlBuffer))) {
    state->current_creature.set_img_url(std::string(imgUrlBuffer));
  }
  ImGui::PopItemWidth();
  if (ImGui::Button("Reload Image"))
  {
    state->clear_image = true;
  }

  ImGui::EndChild();
}
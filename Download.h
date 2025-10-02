#ifndef _DOWNLOAD_H
#define _DOWNLOAD_H

#include <functional>

struct curlStatus {
  size_t downloaded;
  size_t total;
};

void downloadCreatures(std::function<void()>);

curlStatus downloadCallbacks();

#endif
#include "Storage.h"
#include <FS.h>

namespace storage {

Storage &Storage::setup() {
  SPIFFS.begin();
  started_ = true;
  return *this;
}

template<typename T>
bool Storage::get(String key, T &v) {
  return get(key, v, sizeof(T));
}

template<typename T>
bool Storage::get(String key, T &v, uint8_t length) {
  if (started_) {
    File file = SPIFFS.open(key, "r");
    if (file) {
      uint8_t bytesRead = file.readBytes((char *) &v, length);
      file.close();
      return bytesRead > 0;
    }
  }
  return false;
}

template<typename T>
bool Storage::put(String key, T &v) {
  return put(key, v, sizeof(v));
}

template<typename T>
bool Storage::put(String key, T &v, uint8_t length) {
  if (started_) {
    File file = SPIFFS.open(key, "w");
    if (file) {
      uint8_t bytesWritten = file.write((char *) &v, length);
      file.close();
      return bytesWritten > 0;
    }
  }
  return false;
}

}
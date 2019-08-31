#ifndef LIGHTSWITCHCORE_INCLUDE_STORAGE_HPP_
#define LIGHTSWITCHCORE_INCLUDE_STORAGE_HPP_

#include <FS.h>

namespace storage {

class Storage {

  bool started_;

 public:
  Storage &setup() {
    SPIFFS.begin();
    started_ = true;
    return *this;
  }

 protected:
  Storage() = default;

  template<typename T>
  bool get(const String key, T &v) {
    return get(key, v, sizeof(T));
  }

  template<typename T>
  bool get(const String key, T &v, uint8_t length) {
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
  bool put(const String key, T &v) {
    return put(key, v, sizeof(v));
  }

  template<typename T>
  bool put(const String key, T &v, uint8_t length) {
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

};

}

#endif //LIGHTSWITCHCORE_INCLUDE_STORAGE_HPP_

#ifndef LIGHTSWITCHCORE_INCLUDE_STORAGE_H_
#define LIGHTSWITCHCORE_INCLUDE_STORAGE_H_

#include <WString.h>

namespace storage {

class Storage {

  bool started_;

 public:
  Storage &setup();

 protected:
  Storage() = default;

  template<typename T>
  bool get(String key, T &v);

  template<typename T>
  bool get(String key, T &v, uint8_t length);

  template<typename T>
  bool put(String key, T &v);

  template<typename T>
  bool put(String key, T &v, uint8_t length);

};

}

#endif //LIGHTSWITCHCORE_INCLUDE_STORAGE_H_

#ifndef LIGHTSWITCHCORE_INCLUDE_STORAGE_HPP_
#define LIGHTSWITCHCORE_INCLUDE_STORAGE_HPP_

#include <ESP_EEPROM.h>

#define EEPROM_SIZE_BYTES 512

namespace storage {

template<int EEPROM_SIZE>
class Storage {

  bool started_;

 public:
  Storage &setup() {
    EEPROM.begin(EEPROM_SIZE);
    started_ = true;
    return *this;
  }

  bool commit() {
    return started_ && EEPROM.commit();
  }

 protected:
  Storage() = default;

  template<typename T>
  Storage &get(uint8_t addr, T &v) {
    if (started_) {
      EEPROM.get(addr, v);
    }
    return *this;
  }

  template<typename T>
  Storage &put(uint8_t addr, T &v) {
    if (started_) {
      EEPROM.put(addr, v);
    }
    return *this;
  }

};

}

#endif //LIGHTSWITCHCORE_INCLUDE_STORAGE_HPP_

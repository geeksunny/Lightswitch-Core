#ifndef LIGHTSWITCHCORE_INCLUDE_STORAGE_H_
#define LIGHTSWITCHCORE_INCLUDE_STORAGE_H_

#include <stdint.h>

#define EEPROM_SIZE_BYTES 512

namespace storage {

template<int EEPROM_SIZE>
class Storage {
  bool started_;
 public:
  Storage &setup();
  bool commit();
 protected:
  Storage() = default;
  template<typename T>
  Storage &get(uint8_t addr, T &v);
  template<typename T>
  Storage &put(uint8_t addr, T &v);
};

}

#endif //LIGHTSWITCHCORE_INCLUDE_STORAGE_H_

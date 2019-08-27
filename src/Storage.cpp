#include "Storage.h"
#include <ESP_EEPROM.h>

namespace storage {

template<int EEPROM_SIZE>
Storage<EEPROM_SIZE> &Storage<EEPROM_SIZE>::setup() {
  EEPROM.begin(EEPROM_SIZE);
  started_ = true;
  return &this;
}

template<int EEPROM_SIZE>
template<typename T>
Storage<EEPROM_SIZE> &Storage<EEPROM_SIZE>::get(uint8_t addr, T &v) {
  if (started_) {
    EEPROM.get(addr, v);
  }
  return &this;
}

template<int EEPROM_SIZE>
template<typename T>
Storage<EEPROM_SIZE> &Storage<EEPROM_SIZE>::put(uint8_t addr, T &v) {
  if (started_) {
    EEPROM.put(addr, v);
  }
  return &this;
}

template<int EEPROM_SIZE>
bool Storage<EEPROM_SIZE>::commit() {
  return started_ && EEPROM.commit();
}

}

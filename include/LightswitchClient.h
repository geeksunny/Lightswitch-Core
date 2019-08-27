#ifndef LIGHTSWITCHCORE_INCLUDE_LIGHTSWITCHCLIENT_H_
#define LIGHTSWITCHCORE_INCLUDE_LIGHTSWITCHCLIENT_H_

#include "Lightswitch.h"
#include "Storage.h"
#include <WiFiClient.h>
#include <WiFiUdp.h>

namespace lightswitch {

class ClientStorage : public storage::Storage<EEPROM_SIZE_BYTES> {
 public:
  ClientStorage() = default;
  ClientStorage &getClicks(uint8_t &dest);
  ClientStorage &getServerAddress(IPAddress &dest);
  ClientStorage &setClicks(uint8_t &clicks);
  ClientStorage &setServerAddress(IPAddress &ip_address);
};

class LightswitchClient {
  ClientStorage storage_;
  LS_MSG_FIXED msg_;
  WiFiClient tcp_;
  WiFiUDP udp_;
 public:
  LightswitchClient() = default;
  void setup();
  void loop();
  void sendPerformAction(uint8_t action, uint8_t value);
 private:
  void sendPerformActionDirect(uint8_t action, uint8_t value);
  void sendPerformActionBroadcast(uint8_t action, uint8_t value);
};

}

#endif //LIGHTSWITCHCORE_INCLUDE_LIGHTSWITCHCLIENT_H_

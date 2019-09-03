#ifndef LIGHTSWITCHCORE_INCLUDE_LIGHTSWITCHCLIENT_H_
#define LIGHTSWITCHCORE_INCLUDE_LIGHTSWITCHCLIENT_H_

#include "Lightswitch.h"
#include "Storage.hpp"
#include <WiFiClient.h>
#include <WiFiUdp.h>

namespace lightswitch {

class ClientStorage : public storage::Storage {
 public:
  ClientStorage() = default;
  bool getClicks(uint8_t &dest);
  bool getServerAddress(IPAddress &dest);
  bool setClicks(uint8_t &clicks);
  bool setServerAddress(IPAddress &ip_address);
};

enum ConnectionMode {
  DIRECT,
  BROADCAST
};

class LightswitchClient {
  ConnectionMode mode_;
  ClientStorage storage_{};
  LS_MSG_FIXED msg_{};
  WiFiClient tcp_;
  WiFiUDP udp_;
 public:
  LightswitchClient() = default;
  void setup();
  void loop();
  ClientStorage &getStorage();
  void sendPerformAction(uint8_t action, uint8_t value);
 private:
  void sendPerformActionDirect();
  void sendPerformActionBroadcast();
};

}

#endif //LIGHTSWITCHCORE_INCLUDE_LIGHTSWITCHCLIENT_H_

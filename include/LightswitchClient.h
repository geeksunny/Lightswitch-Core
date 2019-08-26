#ifndef WIFIBUDDON_INCLUDE_LIGHTSWITCHCLIENT_H_
#define WIFIBUDDON_INCLUDE_LIGHTSWITCHCLIENT_H_

#include "Lightswitch.h"
#include <WiFiClient.h>
#include <WiFiUdp.h>

namespace lightswitch {

class LightswitchClient {
  LS_MSG_FIXED msg_;
  WiFiClient tcp_;
  WiFiUDP udp_;
 public:
  LightswitchClient();
  void setup();
  void loop();
  void sendPerformAction(uint8_t action, uint8_t value);
 private:
  void sendPerformActionDirect(uint8_t action, uint8_t value);
  void sendPerformActionBroadcast(uint8_t action, uint8_t value);
};

}

#endif //WIFIBUDDON_INCLUDE_LIGHTSWITCHCLIENT_H_

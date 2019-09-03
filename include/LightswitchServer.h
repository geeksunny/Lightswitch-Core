#ifndef LIGHTSWITCHCORE_INCLUDE_LIGHTSWITCHSERVER_H_
#define LIGHTSWITCHCORE_INCLUDE_LIGHTSWITCHSERVER_H_

#include "Lightswitch.h"
#include "Storage.hpp"
#include <WiFiServer.h>
#include <WiFiUdp.h>

namespace lightswitch {

class ActionHandler {
 friend class LightswitchServer;
 virtual bool onAction(uint8_t action, uint8_t value) = 0;
};

class ServerStorage : public storage::Storage {
 public:
  ServerStorage() = default;
  // TODO: getters & setters
};

// TODO: Implement optional DHCP mode that uses DHCP monitoring to detect clicks, ala Amazon Dash Buttons

class LightswitchServer {
  ActionHandler &handler_;
  ServerStorage storage_{};
  LS_MSG_FIXED msg_{};
  WiFiServer server_;
  WiFiUDP udp_;
 public:
  LightswitchServer(ActionHandler &handler);
  void setup();
  void loop();
  ServerStorage &getStorage();
 private:
  bool dispatchAction(uint8_t action, uint8_t value);
  void prepOutgoingPacket(bool result);
  void readTCP();
  void readUDP();
};

}

#endif //LIGHTSWITCHCORE_INCLUDE_LIGHTSWITCHSERVER_H_

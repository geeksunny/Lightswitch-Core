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
  bool getAction(uint8_t &dest);
  bool getClicks(uint16_t &dest);
  bool getServerAddress(IPAddress &dest);
  bool setAction(uint8_t &action);
  bool setClicks(uint16_t &clicks);
  bool setServerAddress(IPAddress &ip_address);
};

enum ConnectionMode {
  DIRECT,
  BROADCAST
};

class LightswitchClient {
 public:
  LightswitchClient() = default;
  void setup();
  void loop();
  ClientStorage &getStorage();
  void sendPerformAction(uint8_t action, uint8_t value);
 protected:
  ClientStorage storage_{};
 private:
  virtual void clientSetup() = 0;
  virtual void clientLoop() = 0;
  virtual void sendAction(uint8_t action, uint8_t value) = 0;
};

class LightswitchEspNowClient : public LightswitchClient {
  static LightswitchEspNowClient *active_client;
  static void on_send(uint8_t *mac_addr, uint8_t status);
 public:
  explicit LightswitchEspNowClient();
 private:
  uint8_t server_mac_addr[6] = LIGHTSWITCH_SERVER_MAC_ADDRESS;
  bool sent_ = false;
  bool send_status_ = false;

  void clientSetup() override;
  void clientLoop() override;
  void sendAction(uint8_t action, uint8_t value) override;
};

class LightswitchWifiClient : public LightswitchClient {
 public:
 private:
  ConnectionMode mode_{};
  LS_MSG_FIXED msg_{};
  WiFiClient tcp_;
  WiFiUDP udp_;

  void clientSetup() override;
  void clientLoop() override;
  void sendAction(uint8_t action, uint8_t value) override;
  void sendPerformActionDirect();
  void sendPerformActionBroadcast();
};

}

#endif //LIGHTSWITCHCORE_INCLUDE_LIGHTSWITCHCLIENT_H_

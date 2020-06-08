#ifndef LIGHTSWITCHCORE_INCLUDE_LIGHTSWITCHSERVER_H_
#define LIGHTSWITCHCORE_INCLUDE_LIGHTSWITCHSERVER_H_

#include "Lightswitch.h"
#include "Storage.hpp"
#include <WiFiServer.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include <deque>

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

class ServerInterface {
 public:
  virtual void setup() = 0;
  virtual bool read(LS_ACTION &dest) = 0;
  virtual void onResult(bool success) = 0;
};

class NetworkInterface : public ServerInterface {
 protected:
  LS_MSG_FIXED msg_{};
  void prepOutgoingPacket(bool result);
};

class TcpInterface : public NetworkInterface {
 public:
  explicit TcpInterface();

  void setup() override;
  bool read(LS_ACTION &dest) override;
  void onResult(bool success) override;

 private:
  WiFiServer server_;
  WiFiClient client_;
};

class UdpInterface : public NetworkInterface {
 public:
  void setup() override;
  bool read(LS_ACTION &dest) override;
  void onResult(bool success) override;
 private:
  LS_MSG_FIXED msg_{};
  WiFiUDP udp_;
};

class EspNowInterface : public ServerInterface {
  static EspNowInterface *active_interface;
  static void on_recv(uint8_t *mac_addr, uint8_t *data, uint8_t len);
 public:
  explicit EspNowInterface();
  void setup() override;
  bool read(LS_ACTION &dest) override;
  void onResult(bool success) override;
 private:
  LS_MSG_FIXED_MINI msg_{};
  bool received_ = false;
};

// TODO: Refactor LightswitchServer to iterate over one-or-more ServerInterface objects for task operations
class LightswitchServer {
  ActionHandler &handler_;
  ServerStorage storage_{};
  std::deque<ServerInterface*> interfaces_;
  LS_ACTION action_{};
 public:
  explicit LightswitchServer(ActionHandler &handler);
  void registerInterface(ServerInterface *an_interface);
  void setup();
  void loop();
  ServerStorage &getStorage();
 private:
  bool dispatchAction(uint8_t action, uint8_t value);
};

}

#endif //LIGHTSWITCHCORE_INCLUDE_LIGHTSWITCHSERVER_H_

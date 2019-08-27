#include "LightswitchClient.h"
#include <ESP8266WiFi.h>

#define ADDRESS_CLICK_COUNT   0x00
#define ADDRESS_SERVER_IP     0x01

#define LENGTH_CLICK_COUNT    1
#define LENGTH_SERVER_IP      4

namespace lightswitch {

ClientStorage &ClientStorage::getClicks(uint8_t &dest) {
  put(ADDRESS_CLICK_COUNT, dest);
  return *this;
}

ClientStorage &ClientStorage::getServerAddress(IPAddress &dest) {
  get(ADDRESS_SERVER_IP, dest);
  return *this;
}

ClientStorage &ClientStorage::setClicks(uint8_t &clicks) {
  put(ADDRESS_CLICK_COUNT, clicks);
  return *this;
}

ClientStorage &ClientStorage::setServerAddress(IPAddress &ip_address) {
  put(ADDRESS_SERVER_IP, ip_address);
  return *this;
}

void LightswitchClient::setup() {
  storage_.setup();

  IPAddress server;
  storage_.getServerAddress(server);
  if (server.isV4() && tcp_.connect(server, LIGHTSWITCH_PORT_SERVER)) {
    mode_ = ConnectionMode::DIRECT;
  } else {
    // TODO: if IP exists but was invalid, clear stored IP?
    udp_.begin(LIGHTSWITCH_PORT_CLIENT);
    mode_ = ConnectionMode::BROADCAST;
  }
}

// TODO: Tune-up parsing logic, reduce redundant code
void LightswitchClient::loop() {
  switch (mode_) {
    case ConnectionMode::DIRECT: {
      // Check for TCP traffic
      while (tcp_.connected() || tcp_.available()) {
        if (tcp_.available()) {
          msg_.reset();
          parseLightswitchPacket(tcp_, msg_);
          switch (msg_.type) {
            case PacketType::NOTIFY_RESULT: {
              // TODO: Parse value of `value` to find result. 0=success/1=error ?
              // TODO: We are done here - POWER DOWN
              break;
            }
            case PacketType::PERFORM_ACTION:
            default: {
              // Shouldn't be here
              break;
            }
          }
        }
      }
      break;
    }
    case ConnectionMode::BROADCAST: {
      // Check for UDP traffic
      if (udp_.parsePacket()) {
        if (udp_.remotePort() == LIGHTSWITCH_PORT_SERVER) {
          msg_.reset();
          parseLightswitchPacket(udp_, msg_);
          switch (msg_.type) {
            case PacketType::NOTIFY_RESULT: {
              IPAddress ip = udp_.remoteIP();
              storage_.setServerAddress(ip).commit();
              // TODO: Parse value of `value` to find result. 0=success/1=error ?
              // TODO: We are done here - POWER DOWN
              break;
            }
            case PacketType::PERFORM_ACTION:
            default:
              // Nothing to do here for client.
              break;
          }
          udp_.flush();
        }
      }
      break;
    }
    default: {
      // Shouldn't be here
      break;
    }
  }
}

void LightswitchClient::sendPerformAction(uint8_t action, uint8_t value) {
  // Populate outgoing message
  msg_.reset();
  msg_.type = PacketType::PERFORM_ACTION;
  msg_.action = action;
  msg_.value = value;
  WiFi.macAddress(msg_.mac);
  // Send the message
  switch (mode_) {
    case ConnectionMode::DIRECT: {
      sendPerformActionDirect();
      break;
    }
    case ConnectionMode::BROADCAST: {
      sendPerformActionBroadcast();
      break;
    }
    default:
      // Shouldn't be here.
      break;
  }
  // Increment stored click count
  uint8_t count;
  storage_.getClicks(count).setClicks(++count).commit();
}

void LightswitchClient::sendPerformActionDirect() {
  if (tcp_.connected()) {
    // TODO: Do we need to check for tcp_.available() & flush before sending anything?
    tcp_.write((char *) &msg_, LIGHTSWITCH_PACKET_BUFFER_SIZE);
  }
}

void LightswitchClient::sendPerformActionBroadcast() {
  udp_.beginPacket({255, 255, 255, 255}, LIGHTSWITCH_PORT_SERVER);
  udp_.write((uint8_t *) &msg_, LIGHTSWITCH_PACKET_BUFFER_SIZE);
  udp_.endPacket();
}

}
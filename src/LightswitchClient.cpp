#include "LightswitchClient.h"
#include <ESP8266WiFi.h>
#include "DebugLog.hpp"

#define KEY_ACTION        "/cfg/action"
#define KEY_CLICK_COUNT   "/cfg/clicks"
#define KEY_SERVER_IP     "/cfg/server"

#ifndef CFG_DEFAULT_CLIENT_CLICK_COUNT
#define CFG_DEFAULT_CLIENT_CLICK_COUNT 0
#endif
#ifndef CFG_DEFAULT_CLIENT_ACTION
#define CFG_DEFAULT_CLIENT_ACTION 0
#endif

namespace lightswitch {

bool ClientStorage::getAction(uint8_t &dest) {
  return get(KEY_ACTION, dest);
}

bool ClientStorage::getClicks(uint16_t &dest) {
  return get(KEY_CLICK_COUNT, dest);
}

bool ClientStorage::getServerAddress(IPAddress &dest) {
  return get(KEY_SERVER_IP, dest);
}

bool ClientStorage::setAction(uint8_t &action) {
  return put(KEY_ACTION, action);
}

bool ClientStorage::setClicks(uint16_t &clicks) {
  return put(KEY_CLICK_COUNT, clicks);
}

bool ClientStorage::setServerAddress(IPAddress &ip_address) {
  return put(KEY_SERVER_IP, ip_address);
}

void LightswitchClient::setup() {
  storage_.setup();

  IPAddress server;
  storage_.getServerAddress(server);
  if (server.isV4() && tcp_.connect(server, LIGHTSWITCH_PORT_SERVER)) {
    DEBUG("Connecting to stored server address:", server.toString().c_str())
    mode_ = ConnectionMode::DIRECT;
  } else {
    // TODO: if IP exists but was invalid, clear stored IP?
    DEBUG("Performing UDP broadcast!")
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
              DEBUG("TCP Recv NOTIFY_RESULT:", unsigned(msg_.value))
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
              storage_.setServerAddress(ip);
              // TODO: Parse value of `value` to find result. 0=success/1=error ?
              // TODO: We are done here - POWER DOWN
              DEBUG("UDP Recv NOTIFY_RESULT:", unsigned(msg_.value))
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

ClientStorage &LightswitchClient::getStorage() {
  return storage_;
}

void LightswitchClient::sendPerformAction(uint8_t action, uint8_t value) {
  DEBUG("sendPerformAction(action:", unsigned(msg_.action), ", value:", unsigned(msg_.value), ")")
  // Populate outgoing message
  msg_.reset();
  msg_.type = PacketType::PERFORM_ACTION;
  // TODO: pull action from storage, store default value if none set
  msg_.action = CFG_DEFAULT_CLIENT_ACTION;
  msg_.value = value;
  WiFi.macAddress(msg_.mac);
  // Send the message
  switch (mode_) {
    case ConnectionMode::DIRECT: {
      sendPerformActionDirect();
      DEBUG("Sent direct TCP packet.")
      break;
    }
    case ConnectionMode::BROADCAST: {
      sendPerformActionBroadcast();
      DEBUG("Sent broadcast UDP packet.")
      break;
    }
    default:
      // Shouldn't be here.
      break;
  }
  // Increment stored click count
  uint16_t count = CFG_DEFAULT_CLIENT_CLICK_COUNT;
  bool hasClicks = storage_.getClicks(count);
  count += 1;
  DEBUG("New click count:", unsigned(count), "| Had clicks:", (hasClicks ? "YES" : "NO"))
  storage_.setClicks(count);
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
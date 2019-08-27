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
  // TODO: Check for saved server IP address
  //  EXISTS: Open TCP connection to server
  //   TIMEOUT: Fallback to UDP Broadcast
  //  NOT EXISTS: Send UDP broadcast
  //   Store updated server IP from any response received

  // TODO: Should UDP only be enabled if TCP connection can't be opened?
  udp_.begin(LIGHTSWITCH_PORT_CLIENT);
}

void LightswitchClient::loop() {
  // Check for TCP traffic
  // TODO: tcp_ parse?
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
}

void LightswitchClient::sendPerformAction(uint8_t action, uint8_t value) {
  // Populate outgoing message
  msg_.reset();
  msg_.type = PacketType::PERFORM_ACTION;
  msg_.action = action;
  msg_.value = value;
  WiFi.macAddress(msg_.mac);
  // TODO: if TCP connection opened -> sendPerformActionDirect
  //  else -> sendPerformActionBroadcast
}

void LightswitchClient::sendPerformActionDirect(uint8_t action, uint8_t value) {
  // TODO
}

void LightswitchClient::sendPerformActionBroadcast(uint8_t action, uint8_t value) {
  udp_.beginPacket({255,255,255,255}, LIGHTSWITCH_PORT_SERVER);
  udp_.write((uint8_t *) &msg_, LIGHTSWITCH_PACKET_BUFFER_SIZE);
  udp_.endPacket();
}

}
#include "LightswitchClient.h"
#include <ESP8266WiFi.h>

namespace lightswitch {

LightswitchClient::LightswitchClient() {
  // = default?
}

void LightswitchClient::setup() {
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
          // TODO: Store value of `ip` in flash storage
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
#include "LightswitchServer.h"
#include <WiFiClient.h>

#ifdef DEBUG_MODE
#include <iostream>
#endif

namespace lightswitch {

LightswitchServer::LightswitchServer(ActionHandler &handler) : handler_(handler), server_(LIGHTSWITCH_PORT_SERVER) {
  //
}

void LightswitchServer::setup() {
  // TCP server
#ifdef DEBUG_MODE
  std::cout << "Starting TCP server on port " << LIGHTSWITCH_PORT_SERVER << std::endl;
#endif
  server_.begin();
  server_.setNoDelay(true);
  // UDP server
#ifdef DEBUG_MODE
  std::cout << "Starting UDP monitoring on port " << LIGHTSWITCH_PORT_SERVER << std::endl;
#endif
  udp_.begin(LIGHTSWITCH_PORT_SERVER);
}

void LightswitchServer::loop() {
  readTCP();
  readUDP();
}

ServerStorage &LightswitchServer::getStorage() {
  return storage_;
}

bool LightswitchServer::dispatchAction(const uint8_t action, const uint8_t value) {
  return handler_.onAction(action, value);
}

void LightswitchServer::prepOutgoingPacket(bool result) {
  msg_.reset();
  msg_.type = PacketType::NOTIFY_RESULT;
  msg_.value = result;
}

void LightswitchServer::readTCP() {
  WiFiClient client = server_.available();
  if (!client) {
    return;
  }
#ifdef DEBUG_MODE
  std::cout << "TCP client available!" << std::endl;
#endif
  int bytesRead = 0;
  while (client.connected()) {
    if (client.available()) {
      msg_.reset();
      bytesRead = parseLightswitchPacket(client, msg_);
      if (bytesRead > 0) {
        break;
      }
    }
  }
#ifdef DEBUG_MODE
  std::cout << "TCP bytes read: " << bytesRead << std::endl;
#endif
  if (bytesRead > 0 && msg_.type == PacketType::PERFORM_ACTION) {
    bool result = dispatchAction(msg_.action, msg_.value);
    prepOutgoingPacket(result);
    client.write((char *) &msg_, LIGHTSWITCH_PACKET_BUFFER_SIZE);
    delay(1); // Allow time for the client to receive.
  }
  // Closing the connection
  client.stop();
#ifdef DEBUG_MODE
  std::cout << "Closed TCP connection." << std::endl;
#endif
}

void LightswitchServer::readUDP() {
  if (udp_.parsePacket()) {
#ifdef DEBUG_MODE
    std::cout << "UDP packet available!" << std::endl;
#endif
    if (udp_.remotePort() == LIGHTSWITCH_PORT_CLIENT) {
      msg_.reset();
      int bytesRead = parseLightswitchPacket(udp_, msg_);
#ifdef DEBUG_MODE
      std::cout << "UDP bytes read: " << bytesRead << std::endl;
#endif
      if (bytesRead == 0) {
        return;
      }
      switch (msg_.type) {
        case PacketType::PERFORM_ACTION: {
          bool result = dispatchAction(msg_.action, msg_.value);
          prepOutgoingPacket(result);
          udp_.beginPacket(udp_.remoteIP(), LIGHTSWITCH_PORT_CLIENT);
          udp_.write((uint8_t *) &msg_, LIGHTSWITCH_PACKET_BUFFER_SIZE);
          udp_.endPacket();
          break;
        }
        case PacketType::NOTIFY_RESULT:
        default: {
          // Nothing to do here for server.
          break;
        }
      }
      udp_.flush();
#ifdef DEBUG_MODE
      std::cout << "Flushed UDP stream." << std::endl;
#endif
    }
  }
}

}

#include "LightswitchServer.h"
#include "DebugLog.hpp"
#include "EspNowTools.h"


namespace lightswitch {

////////////////////////////////////////////////////////////////
// Class : NetworkInterface ////////////////////////////////////
////////////////////////////////////////////////////////////////

void NetworkInterface::prepOutgoingPacket(bool result) {
  msg_.reset();
  msg_.type = PacketType::NOTIFY_RESULT;
  msg_.value = result;
}

////////////////////////////////////////////////////////////////
// Class : TcpInterface ////////////////////////////////////////
////////////////////////////////////////////////////////////////

TcpInterface::TcpInterface() : server_(LIGHTSWITCH_PORT_SERVER) {
  //
}

void TcpInterface::setup() {
  // TCP server
  DEBUG("Starting TCP server on port", LIGHTSWITCH_PORT_SERVER)
  server_.begin();
  server_.setNoDelay(true);
}

bool TcpInterface::read(LS_ACTION &dest) {
  client_ = server_.available();
  if (!client_) {
    return false;
  }
  DEBUG("TCP client available!")
  int bytesRead = 0;
  while (client_.connected()) {
    if (client_.available()) {
      msg_.reset();
      bytesRead = parseLightswitchPacket(client_, msg_);
      if (bytesRead > 0) {
        break;
      }
    }
  }
  DEBUG("TCP bytes read:", bytesRead)
  if (bytesRead > 0 && msg_.type == PacketType::PERFORM_ACTION) {
    // Action found.
    dest.action = msg_.action;
    dest.value = msg_.value;
    return true;
  }
  // Closing the connection
  client_.stop();
  DEBUG("Closed TCP connection.")
  return false;
}

void TcpInterface::onResult(bool success) {
  if (!client_) {
    return;
  }
  prepOutgoingPacket(success);
  client_.write((char *) &msg_, LIGHTSWITCH_PACKET_BUFFER_SIZE);
  delay(1); // Allow time for the client to receive.
  // Closing the connection
  client_.stop();
  DEBUG("Closed TCP connection.")
}

////////////////////////////////////////////////////////////////
// Class : UdpInterface ////////////////////////////////////////
////////////////////////////////////////////////////////////////

void UdpInterface::setup() {
  // UDP server
  DEBUG("Starting UDP monitoring on port", LIGHTSWITCH_PORT_SERVER)
  udp_.begin(LIGHTSWITCH_PORT_SERVER);
}

bool UdpInterface::read(LS_ACTION &dest) {
  if (udp_.parsePacket()) {
    DEBUG("UDP packet available!")
    if (udp_.remotePort() == LIGHTSWITCH_PORT_CLIENT) {
      msg_.reset();
      int bytesRead = parseLightswitchPacket(udp_, msg_);
      DEBUG("UDP bytes read:", bytesRead)
      if (bytesRead == 0) {
        return false;
      }
      if (msg_.type == PacketType::PERFORM_ACTION) {
        // Action found.
        dest.action = msg_.action;
        dest.value = msg_.value;
        return true;
      }
      udp_.flush();
      DEBUG("Flushed UDP stream.")
    }
  }
  return false;
}

void UdpInterface::onResult(bool success) {
  prepOutgoingPacket(success);
  udp_.beginPacket(udp_.remoteIP(), LIGHTSWITCH_PORT_CLIENT);
  udp_.write((uint8_t *) &msg_, LIGHTSWITCH_PACKET_BUFFER_SIZE);
  udp_.endPacket();
  udp_.flush();
  DEBUG("Flushed UDP stream.")
}

////////////////////////////////////////////////////////////////
// Class : EspNowInterface /////////////////////////////////////
////////////////////////////////////////////////////////////////

/* static */
EspNowInterface *EspNowInterface::active_interface;

/* static */
void EspNowInterface::on_recv(uint8_t *mac_addr, uint8_t *data, uint8_t len) {
  ESP_NOW_GET_DATA(EspNowInterface::active_interface->msg_, data);
  EspNowInterface::active_interface->received_ = true;
  DEBUG("ESP-NOW data received!")
  // TODO: Improve debug output, display mac address, action details.
}

EspNowInterface::EspNowInterface() {
  EspNowInterface::active_interface = this;
}

void EspNowInterface::setup() {
  esp_now_tools::EspNow::configure(esp_now_tools::EspNow::Role::SERVER)
      .setOnRecvCallback(EspNowInterface::on_recv)
      .commit();
}

bool EspNowInterface::read(LS_ACTION &dest) {
  if (received_) {
    dest.action = msg_.action;
    dest.value = msg_.value;
    dest.reset(); // TODO: Is reset() necessary here?
    received_ = false;
    return true;
  }
  return false;
}

void EspNowInterface::onResult(bool success) {
  // Nothing to be done here.
}

////////////////////////////////////////////////////////////////
// Class : LightswitchServer ///////////////////////////////////
////////////////////////////////////////////////////////////////

LightswitchServer::LightswitchServer(ActionHandler &handler) : handler_(handler) {
  //
}

void LightswitchServer::registerInterface(ServerInterface *an_interface) {
  interfaces_.push_back(an_interface);
}

void LightswitchServer::setup() {
  for (auto & interface : interfaces_) {
    interface->setup();
  }
}

void LightswitchServer::loop() {
  for (auto & interface : interfaces_) {
    bool action_found = interface->read(action_);
    if (action_found) {
      interface->onResult(dispatchAction(action_.action, action_.value));
    }
  }
}

ServerStorage &LightswitchServer::getStorage() {
  return storage_;
}

bool LightswitchServer::dispatchAction(const uint8_t action, const uint8_t value) {
  return handler_.onAction(action, value);
}

}

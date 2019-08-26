#ifndef WIFIBUDDON_INCLUDE_LIGHTSWITCH_H_
#define WIFIBUDDON_INCLUDE_LIGHTSWITCH_H_

#include <stdint.h>
#include <cstring>

#define LIGHTSWITCH_PORT_CLIENT           44099
#define LIGHTSWITCH_PORT_SERVER           44100

#define LIGHTSWITCH_PACKET_BUFFER_SIZE    sizeof(lightswitch::LS_MSG_FIXED)

namespace lightswitch {

enum PacketType {
  // Sent from server
  NOTIFY_RESULT,
  // Sent from client
  PERFORM_ACTION
};

typedef struct __attribute__((packed)) _LS_MSG_FIXED {
  uint8_t type;
  uint8_t action;
  uint8_t value;
  uint8_t mac[6];
  void reset() {
    memset(this, 0, sizeof(_LS_MSG_FIXED));
  }
} LS_MSG_FIXED;

template <typename ClientStream, typename Container>
inline int parseLightswitchPacket(ClientStream &src, Container &dest) {
  return src.read((uint8_t *) &dest, sizeof(Container));
}

}

#endif //WIFIBUDDON_INCLUDE_LIGHTSWITCH_H_

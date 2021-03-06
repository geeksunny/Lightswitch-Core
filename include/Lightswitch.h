#ifndef LIGHTSWITCHCORE_INCLUDE_LIGHTSWITCH_H_
#define LIGHTSWITCHCORE_INCLUDE_LIGHTSWITCH_H_

#include <stdint.h>
#include <cstring>

#define LIGHTSWITCH_PORT_CLIENT           44099
#define LIGHTSWITCH_PORT_SERVER           44100

#define LIGHTSWITCH_PACKET_BUFFER_SIZE    sizeof(lightswitch::LS_MSG_FIXED)

#define LS_MAC_ADDR_TO_TEXT(mac_array) LS_MAC_ADDR_TO_TEXT_B(mac_array)
#define LS_MAC_ADDR_TO_TEXT_B(a, b, c, d, e, f) LS_MAC_ADDR_TO_TEXT_C(a b c d e f)
#define LS_MAC_ADDR_TO_TEXT_C(a) #a
#ifndef LIGHTSWITCH_SERVER_MAC_ADDRESS
  #define LIGHTSWITCH_SERVER_MAC_ADDRESS {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF}
  #pragma message ("Using DEFAULT hard-coded MAC address for LightswitchServer! " LS_MAC_ADDR_TO_TEXT(LIGHTSWITCH_SERVER_MAC_ADDRESS))
#else
  #pragma message ("Using pre-configured hard-coded MAC address for LightswitchServer! " LS_MAC_ADDR_TO_TEXT(LIGHTSWITCH_SERVER_MAC_ADDRESS))
#endif

namespace lightswitch {

enum PacketType {
  // Sent from server
      NOTIFY_RESULT,
  // Sent from client
      PERFORM_ACTION
};

enum LightswitchAction {
  // Toggle power state
      TOGGLE,
  // Turn on
      TURN_ON,
  // Turn off
      TURN_OFF,
  // Set color
      SET_COLOR,
  // Set scene
      SET_SCENE,
  // Adjust brightness
      BRIGHTNESS,
  // Color cycler
      CYCLE
};

/**
 * Data structure for a Lightswitch action.
 */
typedef struct __attribute__((packed)) _LS_ACTION {
  uint8_t action;
  uint8_t value;
  void reset() {
    memset(this, 0, sizeof(_LS_ACTION));
  }
} LS_ACTION;

/**
 * Data structure for a Lightswitch message packet.
 */
typedef struct __attribute__((packed)) _LS_MSG_FIXED {
  uint8_t type;
  uint8_t action;
  uint8_t value;
  uint8_t mac[6];
  void reset() {
    memset(this, 0, sizeof(_LS_MSG_FIXED));
  }
} LS_MSG_FIXED;

/**
 * Data structure for a mini Lightswitch message packet.
 *
 * This does not include the sender's MAC address.
 */
typedef struct __attribute__((packed)) _LS_MSG_FIXED_MINI {
  uint8_t type;
  uint8_t action;
  uint8_t value;
  void reset() {
    memset(this, 0, sizeof(_LS_MSG_FIXED_MINI));
  }
} LS_MSG_FIXED_MINI;

template<typename ClientStream, typename Container>
inline int parseLightswitchPacket(ClientStream &src, Container &dest) {
  return src.read((uint8_t *) &dest, sizeof(Container));
}

}

#endif //LIGHTSWITCHCORE_INCLUDE_LIGHTSWITCH_H_

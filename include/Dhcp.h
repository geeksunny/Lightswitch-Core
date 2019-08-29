#ifndef LIGHTSWITCHCORE_INCLUDE_DHCP_H_
#define LIGHTSWITCHCORE_INCLUDE_DHCP_H_

/* Values and struct borrowed from Espressif framework's Ethernet library, Dhcp.h */

/* UDP port numbers for DHCP */
#define DHCP_SERVER_PORT    67    /* from server to client */
#define DHCP_CLIENT_PORT    68    /* from client to server */

/* DHCP message OP code */
#define DHCP_BOOTREQUEST    1

/* DHCP Packet structure */
typedef struct __attribute__((packed)) _RIP_MSG_FIXED {
  uint8_t op;
  uint8_t htype;
  uint8_t hlen;
  uint8_t hops;
  uint32_t xid;
  uint16_t secs;
  uint16_t flags;
  uint8_t ciaddr[4];
  uint8_t yiaddr[4];
  uint8_t siaddr[4];
  uint8_t giaddr[4];
  uint8_t chaddr[6];
} RIP_MSG_FIXED;

#endif //LIGHTSWITCHCORE_INCLUDE_DHCP_H_

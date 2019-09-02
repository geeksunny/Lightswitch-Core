#include <ESP8266WiFi.h>

#include "WifiTools.h"

#if AP_MAC_IN_NAME
#include <sstream>
#include <iomanip>
#endif

#ifdef DEBUG_MODE
#include <iostream>
#endif

namespace wifi_tools {

String apName() {
#if AP_MAC_IN_NAME
  uint8_t mac[WL_MAC_ADDR_LENGTH];
  WiFi.macAddress(mac);
  std::ostringstream os(AP_NAME);
  os << "_" << std::hex << std::setfill('0') << std::setw(2)
     << mac[0] << mac[1] << mac[2] << mac[3] << mac[4] << mac[5];
  return String{os.str().c_str()};
#else
  return AP_NAME;
#endif
}

bool startAP() {
  String name = apName();

  WiFi.mode(WIFI_AP);
#if AP_ENCRYPTED
  bool started = WiFi.softAP(name, AP_PASSWORD);
#else
  bool started = WiFi.softAP(name);
#endif

#ifdef DEBUG_MODE
  if (started) {
    std::cout << "Started AP with name: " << name << " | Encrypted: " << (AP_ENCRYPTED ? "YES" : "NO") << std::endl;
    std::cout << "Server IP: " << WiFi.softAPIP().toString() << std::endl;
  } else {
    std::cout << "FAILED to start wifi AP!";
  }
#endif

  return started;
}

bool startClient() {
  // Check for presence of wifi shield / module
  if (WiFi.status() == WL_NO_SHIELD) {
#ifdef DEBUG_MODE
    std::cout << ("WiFi shield not present") << std::endl;
#endif
    while (true); // Halt
    // TODO: Properly handle halt with power down
  }

  // Wifi mode set
  WiFi.mode(WIFI_STA);

  // Attempt to connect to network
  if (WiFi.getAutoConnect()) {
#ifdef DEBUG_MODE
    std::cout << "Attempting to connect using SAVED SSID!" << std::endl;
#endif
    // Connect to saved network:
//    WiFi
//    .begin();
  } else {
#if CLIENT_ENCRYPTED
#ifdef DEBUG_MODE
    std::cout << "Attempting to connect to WPA SSID: " << CLIENT_SSID << std::endl;
#endif
    // Connect to WPA/WPA2 network:
    WiFi.begin(CLIENT_SSID, CLIENT_PASS);
#else
#ifdef DEBUG_MODE
    std::cout << "Attempting to connect to OPEN WIFI SSID: " << CLIENT_SSID << std::endl;
#endif
    // Connect to unencrypted wifi network:
    WiFi.begin(CLIENT_SSID);
#endif
  }

  while (WiFi.status() != WL_CONNECTED) {
    // wait 0.5 seconds for connection:
    delay(500);
#ifdef DEBUG_MODE
    std::cout << ".";
#endif
    // TODO: Handle a wifi timeout period, return false on timeout
  }

#ifdef DEBUG_MODE
  // Connected to network!
  std::cout << "Connected to network!" << std::endl;
#endif
  return true;
}

}
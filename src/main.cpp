#ifdef LIGHTSWITCH_CORE_EXAMPLE
#include <Arduino.h>

void setup() {
#ifdef DEBUG_MODE
  Serial.begin(BAUD_RATE);
  delay(3000);  // Give serial console a chance to get ready
  Serial.println("Serial console ready.");
#endif
}

void loop() {
  //
}

#endif

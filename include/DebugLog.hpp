#ifndef LIGHTSWITCHCORE_INCLUDE_DEBUGLOG_HPP_
#define LIGHTSWITCHCORE_INCLUDE_DEBUGLOG_HPP_

#ifdef DEBUG_MODE
  #include <iostream>

  #define SETUP_SERIAL(baud_rate, delay_ms, ready_msg)\
    Serial.begin(baud_rate);\
    delay(delay_ms);\
    Serial.println(ready_msg);

  template<typename Arg, typename... Args>
  void debug_print(Arg&& arg, Args&&... args) {
    std::cout << std::forward<Arg>(arg);
    using expander = int[];
    // TODO: make ' ' character optional / customizable?
    (void)expander{0, (void(std::cout << ' ' << std::forward<Args>(args)), 0)...};
    std::cout << std::endl;
  }

  #define DEBUG(...) debug_print(__VA_ARGS__);
#else
  #define SETUP_SERIAL(baud_rate, delay, ready_msg)
  #define DEBUG(...)
#endif

#endif //LIGHTSWITCHCORE_INCLUDE_DEBUGLOG_HPP_

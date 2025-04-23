//! debug_magloop.h
// 2024-04-23 renamed from debug.h

//! !!!!! DO NOT AUTOFORMAT !!!!!
/* 
  In C++, a backslash (\) at the end of a line is used as a 
  line continuation character in preprocessor directives (such as macros). 
  It tells the preprocessor that the current line continues onto the next 
  line, effectively joining them into a single logical line.
*/

#ifndef DEBUG_MAGLOOP_H
#define DEBUG_MAGLOOP_H

#ifdef DEBUG_MAGLOOP
  #define DEBUG_PRINTF(fmt, ...) do { \
    char buffer[128]; /* Adjust buffer size as needed */ \
    snprintf(buffer, sizeof(buffer), fmt, ##__VA_ARGS__); /* Format the message */ \
    String message = String("-> ") + String(buffer); \
    Serial.printf("%s\n", message.c_str()); /* Output to Serial Monitor */ \
    notifyClients(message); /* Send to WebSocket clients */ \
  } while (0)
#else
  #define DEBUG_PRINTF(...) /* No-op: Debugging disabled */
#endif // DEBUG_MAGLOOP

//! Legacy Serial Monitor debug macro
// #ifdef DEBUG_MAGLOOP
//   #define DEBUG_PRINTF(...) Serial.printf("-> " __VA_ARGS__); Serial.print("\r\n")
// #else
//   #define DEBUG_PRINTF(...) /* No-op: Debugging disabled */
// #endif // DEBUG_MAGLOOP_H

#endif
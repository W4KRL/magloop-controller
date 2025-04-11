//! debug.h
// 2024-01-29

//! !!!!! DO NOT AUTOFORMAT !!!!!
/* 
   Line 11 must look like this unbroken on one line:
   #define DEBUG_PRINTF(...) Serial.printf("-> " __VA_ARGS__); Serial.print("\r\n")
   Usage: DEBUG_PRINT("%s %s", "string", StringVariable.c_str());
*/

#ifndef DEBUG_MAGLOOP_H
#define DEBUG_MAGLOOP_H

// #ifdef DEBUG_MAGLOOP
//   #define DEBUG_PRINTF(...) Serial.printf("-> " __VA_ARGS__); Serial.print("\r\n")
// #else
//   #define DEBUG_PRINTF(...) /* No-op: Debugging disabled */
// #endif // DEBUG_MAGLOOP_H

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

#endif
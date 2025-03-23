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

#ifdef DEBUG_MAGLOOP
  #define DEBUG_PRINTF(...) Serial.printf("-> " __VA_ARGS__); Serial.print("\r\n")
#else
  #define DEBUG_PRINTF(...) /* No-op: Debugging disabled */
#endif // DEBUG_MAGLOOP_H

#endif
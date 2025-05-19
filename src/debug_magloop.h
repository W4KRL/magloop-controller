

/**
 * @file debug_magloop.h
 * @brief Debugging utilities for the magloop controller project.
 *
 * This header provides a macro, DEBUG_PRINTF, for formatted debug output.
 * When DEBUG_MAGLOOP is defined, DEBUG_PRINTF formats a message, prefixes it
 * with "dbg~", prints it to the Serial Monitor, and sends it to WebSocket clients.
 * If DEBUG_MAGLOOP is not defined, DEBUG_PRINTF is a no-op.
 *
 * Usage:
 *   DEBUG_PRINTF("Value: %d", value);
 *
 * Notes:
 * - The buffer size for formatted messages can be adjusted as needed.
 * - The macro uses a line continuation character (backslash) for multi-line definition.
 * - Do not autoformat this file to preserve macro formatting.
 */

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
    String message = String("dbg~") + String(buffer); /* Prefix with 'dbg~' */ \
    Serial.println(message); /* Output to Serial Monitor */ \
    notifyClients(message); /* Send to WebSocket clients */ \
} while (0)
#else
#define DEBUG_PRINTF(...) /* No-op: Debugging disabled */
#endif // DEBUG_MAGLOOP

#endif
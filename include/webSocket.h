/**
 * @file webSocket.h
 * @brief WebSocket interface for ESP32-based magloop controller.
 *
 * This header defines the interface for handling WebSocket communication
 * using the ESPAsyncWebServer library. It uses the '~' character as a delimiter
 * for WebSocket commands to avoid conflicts with SCPI commands that use ':' and ';'.
 *
 * Externally declares the web server and WebSocket objects, and provides
 * functions for handling WebSocket events, notifying clients, and initializing
 * the WebSocket subsystem.
 *
 * Dependencies:
 *   - Arduino.h
 *   - ESPAsyncWebServer.h (https://github.com/ESP32Async/ESPAsyncWebServer)
 * 
 * @author Karl Berger
 * @date 2025-05-19
 */

#ifndef WEBSOCKET_H
#define WEBSOCKET_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h> // https://github.com/ESP32Async/ESPAsyncWebServer

extern AsyncWebServer server;
extern AsyncWebSocket ws;

//! Web Socket event handler
void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);
void notifyClients(const String &message);//! Notify all connected clients with a message
void websocketBegin();
void websocketCleanup();

#endif
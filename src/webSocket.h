//! @file webSocket.h
// 2025-04-03 revised message parsing for button events
// 2025-04-04 revised SCPI handling
// 2025-04-11 added /debug page for debugging

//! Use '~' as a delimiter for web socket commands
//! to avoid conflict with SCPI commands using ':' and ';' as delimiters

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

#endif
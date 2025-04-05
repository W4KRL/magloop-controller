//! webSocket.h
// 2025-04-03 revised message parsing for button events
// 2025-04-04 revised SCPI handling

//! Use '~' as a delimiter for web socket commands
//! to avoid conflict with SCPI commands using ':' and ';' as delimiters

#ifndef WEBSOCKET_H
#define WEBSOCKET_H
#define WS_DELIM '~' // Delimiter for WebSocket messages

#include <Arduino.h>           // PlatformIO requires this for ESP32
#include <LittleFS.h>          // for index.html, styles.css, and script.js
#include <ESPAsyncWebServer.h> // https://github.com/ESP32Async/ESPAsyncWebServer

// Function prototypes
void initLedStates();                                      // ledControl.h
void initButtonStates();                                   // buttonHandler.h
void processButtonEvent(String &buttonId, String &action); // buttonHandler.h
void notifyClients(const String &message);                 // webSocket.h

//! Instantiate WebSocket server on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

//! Web Socket event handler
void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
  switch (type)
  {
  case WS_EVT_CONNECT:
    DEBUG_PRINTF("%s", "WS client connected");
    DEBUG_PRINTF("Free Heap: %u bytes", ESP.getFreeHeap());
    DEBUG_PRINTF("WiFi Signal Strength (RSSI): %d dBm", WiFi.RSSI());
    initLedStates();    // Initialize LEDs
    initButtonStates(); // Initialize button states
    break;
  case WS_EVT_DISCONNECT:
    DEBUG_PRINTF("%s", "WS client disconnected");
    break;
  case WS_EVT_DATA:
    String message = String((char *)data).substring(0, len);
    DEBUG_PRINTF("%s: %s", "WS msg rcvd", message.c_str());

    // Extract the part of the message before the "~" character
    // All messages are prefixed with four characters: "btn~" or "scp~"
    String initialStr = message.substring(0, message.indexOf(WS_DELIM));
    if (initialStr == "btn")
    {
      String buttonId = message.substring(4, 5); // Extract string for numeric ID
      String action = message.substring(6);      // Extract action (pressed/released)
      processButtonEvent(buttonId, action);      // Pass to button handler
    }
    else if (initialStr == "scp")
    {
      String scpiCommand = message.substring(4);                       // Remove "scp~" prefix
      char scpiCommandBuf[scpiCommand.length() + 1];                   // Create a buffer for the command
      scpiCommand.toCharArray(scpiCommandBuf, sizeof(scpiCommandBuf)); // Copy to buffer
      StreamString(responseStream);                                    // Create a stream to capture the response
      String scpiResponse;                                             // Initialize response string
      scpi.Execute(scpiCommandBuf, responseStream);                    // Execute SCPI command
      scpiResponse = "scp" + WS_DELIM + responseStream;                // format for JavaScript client
      notifyClients(scpiResponse);                                     // broadcast the response to all clients
    }
    else
    {
      DEBUG_PRINTF("Rcvd unknown message type.");
    }
    break;
  }
} // onWsEvent()

//! Notify all connected clients with a message
void notifyClients(const String &message)
{
  if (ws.count() > 0) // Check if there are connected clients
  {
    DEBUG_PRINTF("%s: %s", "WS msg sent", message.c_str());
    ws.textAll(message);
  }
  else
  {
    DEBUG_PRINTF("%s", "No clients connected to WebSocket");
  }
} // notifyClients()

void websocketBegin()
{
  // Initialize LittleFS for html/css/js files in <project>\data
  if (!LittleFS.begin())
  {
    Serial.println("Error mounting LittleFS");
    return;
  }
  // Load the web page from LittleFS Filesystem Image
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/index.html", "text/html"); });

  server.on("/styles.css", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/styles.css", "text/css"); });

  server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/script.js", "application/javascript"); });

  server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(LittleFS, "/favicon.ico", "image/x-icon"); });

  ws.onEvent(onWsEvent);
  server.addHandler(&ws);
  server.begin();
} // websocketBegin()

#endif
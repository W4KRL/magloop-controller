//! webSocket.h
//! 2025-03-28 changes for notifyClients() and elimination of fileSystem.h

#ifndef WEBSOCKET_H
#define WEBSOCKET_H

#include <Arduino.h>           // PlatformIO requires this for ESP32
#include <LittleFS.h>          // for index.html, styles.css, and script.js
#include <ESPAsyncWebServer.h> // https://github.com/ESP32Async/ESPAsyncWebServer

// Function prototypes
void initLedStates();                                                  // ledControl.h
void initButtonStates();                                               // buttonHandler.h
void buttonHandler(String &buttonId, String &action);                  // buttonHandler.h
void processSCPICommand(AsyncWebSocketClient *client, String command); // scpiControl.h

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
    //! Handle button press events
    if (message.startsWith("btn"))
    {
      String buttonId = message.substring(0, 4);
      String action = message.substring(5);
      buttonHandler(buttonId, action);
    }
    //! Handle SCPI commands
    else if (message.startsWith("SCPI~"))
    {
      String scpiCommand = message.substring(5); // Remove "SCPI~" prefix
      processSCPICommand(client, scpiCommand);
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
}

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

  ws.onEvent(onWsEvent);
  server.addHandler(&ws);
  server.begin();
}

#endif
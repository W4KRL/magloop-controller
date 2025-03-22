//! webSocket.h
//! 2025-02-10
//! 2025-03-16 added DEBUG to onWsEvent

#ifndef WEBSOCKET_H
#define WEBSOCKET_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h> // https://github.com/ESP32Async/ESPAsyncWebServer

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

//! Web Socket event handler
void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{

  switch (type)
  {
  case WS_EVT_CONNECT:
    // Push the current state of the buttons, LEDs, and SWR gauge to all clients
    DEBUG_PRINTF("%s", "WS client connected");
    DEBUG_PRINTF("Free Heap: %u bytes", ESP.getFreeHeap());
    DEBUG_PRINTF("WiFi Signal Strength (RSSI): %d dBm", WiFi.RSSI());
    for (auto &led : ledStates)
    {
      client->text(led.id + "~" + led.color);
    }
    for (auto &button : buttonStates)
    {
      client->text("buttonState~" + button.id + "~false~dimGray");
    }
    client->text("SWR" + String(swrValue(), 2));
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

void websocketBegin()
{
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

/**
 * @file webSocket.cpp
 * @brief Implements WebSocket server functionality for the magloop-controller project.
 *
 * This file sets up an asynchronous WebSocket server using ESPAsyncWebServer and handles
 * real-time communication between the ESP device and web clients. It serves static files
 * (HTML, CSS, JS) from LittleFS and processes incoming WebSocket messages for button events
 * and SCPI commands.
 *
 * Main Features:
 * - Serves web interface files from LittleFS.
 * - Handles WebSocket connections, disconnections, and data events.
 * - Processes button events and SCPI commands received via WebSocket.
 * - Broadcasts responses to all connected WebSocket clients.
 *
 * Dependencies:
 * - LittleFS for file storage.
 * - ESPAsyncWebServer for HTTP and WebSocket handling.
 * - Custom modules: debug_magloop, scpiControl, ledControl, buttonHandler.
 *
 * Functions:
 * - onWsEvent: Handles WebSocket events (connect, disconnect, data).
 * - notifyClients: Sends a message to all connected WebSocket clients.
 * - websocketBegin: Initializes the filesystem, sets up HTTP routes, and starts the server.
 *
 * @author Karl Berger
 * @date 2025-05-19
 */
#include "webSocket.h"

#include <LittleFS.h>	   // for index.html, styles.css, and script.js
#include "debug_magloop.h" // for DEBUG_PRINT, DEBUG_PRINTLN
#include "scpiControl.h"   // for processSCPIcommand()
#include "ledControl.h"	   // for initLedStates()
#include "buttonHandler.h" // for initButtonStates()

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
		DEBUG_PRINTF("%s: %u", "Free Heap", ESP.getFreeHeap());
		DEBUG_PRINTF("%s %d %s", "RSSI", WiFi.RSSI(), "dBm");
		initLedStates();	// Initialize LEDs
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
		String initialStr = message.substring(0, message.indexOf("~"));
		if (initialStr == "btn")
		{
			String buttonId = message.substring(4, 5); // Extract string for numeric ID
			String action = message.substring(6);	   // Extract action (pressed/released)
			processButtonEvent(buttonId, action);	   // Pass to button handler
		}
		else if (initialStr == "scp")
		{
			String scpiCommand = message.substring(4);			   // Remove "scp~" prefix
			String scpiResponse = processSCPIcommand(scpiCommand); // Process the command and get the response
			DEBUG_PRINTF("%s", scpiResponse.c_str());			   // Print the response to Serial Monitor
			notifyClients(scpiResponse);						   // broadcast the response to all clients
		}
		else
		{
			DEBUG_PRINTF("%s", "Rcvd unknown message type.");
		}
		break;
	}
} // onWsEvent()

//! Notify all connected clients with a message
void notifyClients(const String &message)
{
	if (ws.count() > 0) // Check if there are connected clients
	{
		ws.textAll(message);
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

	server.on("/debug", HTTP_GET, [](AsyncWebServerRequest *request)
			  { request->send(LittleFS, "/debug.html", "text/html"); });

	ws.onEvent(onWsEvent);
	server.addHandler(&ws);
	server.begin();
} // websocketBegin()

void websocketCleanup()
{
	/*
  The cleanupClients() function is called periodically to free up
  resources by removing disconnected or inactive clients from
  the server's client list. Otherwise, you might encounter
  issues like resource leaks or memory exhaustion over time,
  particularly if clients connect and disconnect frequently.
*/
	unsigned long cleanInterval = 5000; // 5 seconds
	static unsigned long cleanTime = millis() + cleanInterval;
	if (millis() > cleanTime)
	{
		ws.cleanupClients();
		cleanTime = millis() + cleanInterval;
	}
}
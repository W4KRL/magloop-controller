# Notes for Mag Loop Controller
2025-03-25

## Programming Environment
```
[env:esp32doit-devkit-v1]
platform = espressif32
board = esp32doit-devkit-v1
framework = arduino
monitor_speed = 115200
upload_speed = 921600
board_build.filesystem = littlefs
lib_deps = 
	https://github.com/me-no-dev/ESPAsyncWebServer.git
	https://github.com/me-no-dev/AsyncTCP.git
	vrekrer/Vrekrer SCPI parser@^0.5.0
```

## How to load web server files
- PlatformIO defaults to SPIFFS unless board_build.filesystem = littlefs is in platformio.ini
- Create a \data folder in the root of the project file folder
- Create index.html, styles.css, and script.js in the data folder
- Connect a USB cable to the device. 
- Ensure the COM port is not in use by clicking the plug icon for the Serial Monitor and click Terminate the Task
- Click the PlatformIO alien head on the left of the screen
- Open **PLATFORM TASKS**
- Open the **Platform** branch
- Click **Erase Flash** if starting fresh
- Click **Build Filesystem Image**
- Click **Upload Filesystem Image**
- Compile the program
- Repeat Build, Upload, Compile if a file is changed

## script.js Event Listeners

Question: 
Whys is the eventlistener used in JavaScript?

Perplexity:
The document.addEventListener was needed for several important reasons:

1. To ensure proper initialization: By wrapping code in a DOMContentLoaded event listener, it guarantees that the JavaScript runs only after the HTML document has been completely loaded and parsed[5]. This prevents errors that could occur if the script tries to interact with elements that haven't yet been created in the DOM.

2. To set up event handling: It allows you to attach event handlers to the document object, which can respond to events that occur anywhere in the document[2]. This is particularly useful for global event handling or for events that don't have a specific target element.

3. To enable dynamic interactivity: It provides a way to react to user actions or browser events without hard-coding event handlers into HTML[1]. This separation of concerns improves code maintainability and flexibility.

4. To support multiple event listeners: Unlike older methods (like using onXYZ properties), addEventListener allows you to attach multiple event listeners to a single element without overwriting existing ones[3]. This is crucial for complex applications or when working with third-party libraries.

5. To control event flow: It gives you finer-grained control over the event propagation phase (capturing vs. bubbling) in which the listener should be activated[3].

By using document.addEventListener, developers can create more robust, interactive, and maintainable web applications that respond effectively to user interactions and browser events[6][7][8].

Citations:
```
[1] https://www.freecodecamp.org/news/javascript-addeventlistener-example-code/
[2] https://www.w3schools.com/jsref/met_document_addeventlistener.asp
[3] https://developer.mozilla.org/en-US/docs/Web/API/EventTarget/addEventListener
[4] https://coderpad.io/blog/development/addeventlistener-javascript/
[5] https://developer.mozilla.org/en-US/docs/Learn_web_development/Core/Scripting/Events
[6] https://webdesign.tutsplus.com/introduction-to-javascript-event-listeners--cms-35236t
[7] https://www.shecodes.io/athena/67582-what-is-addeventlistener-in-javascript
[8] https://education.launchcode.org/intro-to-web-dev-curriculum/dom-and-events/reading/event-listeners/index.html
```

## Wrapping Header Code

- Name the file xxx.h
- Before any code, place:
#ifndef XXX_H
#define XXX_H
- After all your code, place:
#endif
- It is good practice to #include <Arduino.h> in every header file.
- Other includes or function prototypes may be needed to resolve definitions.

## Setting a Static IP in the router

When the ESP32 connects to your local Wi-Fi network, it is assigned an IP address from the available pool. This address is displayed on the Serial Monitor at startup, and you’ll need it to access the antenna controller through your browser. If the same IP address is still available during the next startup, the ESP32 will retain it. However, if it’s no longer available, a new address will be assigned, requiring you to update the browser with this new address to reconnect to the controller. To avoid this inconvenience, you can configure your router to assign a static IP address to the ESP32. The method for setting up a static IP depends on your router’s make and model.

### TP-Link Router static IP

- Log in to the router configuration page (usually 192.168.0.1)
- Click **Advanced Routing** button on the left side of the screen, then select Static Routing List
- Click **Add New...**
- Enter the IP address you have chosen for the Mag Loop Controller
- Subnet mask: 255.255.255.255
- Gateway: 192.168.0.1
- Status: Enabled
- Click **Save**

## Logarithmic Gauge
See https://www.chartjs.org/

The chart is modified from the Bar Chart type. Basically, it is a single bar shown horizontally. All configuration is done in script.js. The scale has type set to "logarithmic".

initWebSocket calls updateGauge() when an "SWR" webssocket message is received. This function passes the SWR value to chart dataset. It also chooses a background color based on teh SWR value.
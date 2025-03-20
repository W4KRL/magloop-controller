# Notes for Mag Loop Controller
02/06/2025


## Programming Environment
[env:esp32doit-devkit-v1]
platform = espressif32
board = esp32doit-devkit-v1
framework = arduino
monitor_speed = 115200
monitor_filters = send_on_enter
monitor_echo = yes
monitor_eol = LF
upload_speed = 921600
board_build.filesystem = littlefs

lib_deps = 
	https://github.com/me-no-dev/ESPAsyncWebServer.git
	https://github.com/me-no-dev/AsyncTCP.git
	vrekrer/Vrekrer SCPI parser@^0.5.0

## How to load web server files
- PlatformIO defaults to SPIFFS unless board_build.filesystem = littlefs is in platformio.ini
- Create a \data folder in the root of the project file folder
- Create index.html, styles.css, and script.js in the data folder
- Connect a USB cable to the device. 
- Ensure the COM port is not in use by clicking the plug icon for the Serial Monitor and click Terminate the Task
- Click the PlatformIO alien head on the left of the screen
- Open the Platform branch
- Click Erase Flash
- Click Build Filesystem Image
- Click Upload Filesystem Image
- Compile the program
- Repeat Build, Upload, Compile if a file is changed

## script.js

Question: 
The eventlistener is only for the SCPI send button. The other buttons are handled elsewhere. Why should they be handled differently?

Perplexity:
The document.addEventListener was needed for several important reasons:

1. To ensure proper initialization: By wrapping code in a DOMContentLoaded event listener, it guarantees that the JavaScript runs only after the HTML document has been completely loaded and parsed[5]. This prevents errors that could occur if the script tries to interact with elements that haven't yet been created in the DOM.

2. To set up event handling: It allows you to attach event handlers to the document object, which can respond to events that occur anywhere in the document[2]. This is particularly useful for global event handling or for events that don't have a specific target element.

3. To enable dynamic interactivity: It provides a way to react to user actions or browser events without hard-coding event handlers into HTML[1]. This separation of concerns improves code maintainability and flexibility.

4. To support multiple event listeners: Unlike older methods (like using onXYZ properties), addEventListener allows you to attach multiple event listeners to a single element without overwriting existing ones[3]. This is crucial for complex applications or when working with third-party libraries.

5. To control event flow: It gives you finer-grained control over the event propagation phase (capturing vs. bubbling) in which the listener should be activated[3].

By using document.addEventListener, developers can create more robust, interactive, and maintainable web applications that respond effectively to user interactions and browser events[6][7][8].

Citations:
[1] https://www.freecodecamp.org/news/javascript-addeventlistener-example-code/
[2] https://www.w3schools.com/jsref/met_document_addeventlistener.asp
[3] https://developer.mozilla.org/en-US/docs/Web/API/EventTarget/addEventListener
[4] https://coderpad.io/blog/development/addeventlistener-javascript/
[5] https://developer.mozilla.org/en-US/docs/Learn_web_development/Core/Scripting/Events
[6] https://webdesign.tutsplus.com/introduction-to-javascript-event-listeners--cms-35236t
[7] https://www.shecodes.io/athena/67582-what-is-addeventlistener-in-javascript
[8] https://education.launchcode.org/intro-to-web-dev-curriculum/dom-and-events/reading/event-listeners/index.html

---
Answer from Perplexity: pplx.ai/share

## Wrapping Header Code

- Name the file xxx.h
- Before any code, place:
#ifndef XXX_H
#define XXX_H
- After all your code, place:
#endif
- It is good practice to #include <Arduino.h> in every header file.
- Other includes or function prototypes may be needed to resolve definitions.

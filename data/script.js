//  2025-03-23 refactor initWebsocket() and processWebSocketMessage() functions

// Global variables
let socket;

// WebSocket initialization function
function initWebSocket() {
  socket = new WebSocket("ws://" + window.location.hostname + "/ws");

  socket.onopen = function (event) {};

  socket.onmessage = function (event) {
    console.log("Message received:", event.data);
    processWebSocketMessage(event.data);
  };

  socket.onerror = function (error) {
    console.error("WebSocket Error: ", error);
  };

  socket.onclose = function (event) {
    // Attempt to reconnect after a delay
    setTimeout(initWebSocket, 5000);
  };
}

// Function to process WebSocket messages
function processWebSocketMessage(message) {
  const scpiResponseArea = document.getElementById("scpiResponse");

  if (message.includes("~")) {
    const parts = message.split("~");
    const type = parts[0];
    switch (type) {
      case "SCPI":
        updateSCPIResponse(scpiResponseArea, parts[1]);
        break;
      case "SWR":
        updateGauge(parseFloat(parts[1]));
        break;
      case "led1":
      case "led2":
        updateLED(type, parts[1]);
        break;
      case "buttonState":
          updateButtonState(parts[1], parts[2] === "true", parts[3]);
        break;
      case "beep":
          beep(parseFloat(parts[1]), parseInt(parts[2]));
        break;
      default:
        console.log("Unknown message type:", type);
    }
  }
}

// Update SCPI response area
function updateSCPIResponse(scpiResponseArea, response) {
  if (scpiResponseArea) {
    scpiResponseArea.value += response + "\n";
    scpiResponseArea.scrollTop = scpiResponseArea.scrollHeight;
  }
}

// Update Gauge
function updateGauge(swrValue) {
  if (gauge && !isNaN(swrValue)) {
    gauge.value = swrValue;
    const color = swrValue < 3 ? "green" : swrValue < 5 ? "yellow" : "red";
    gauge.update({ colorBarProgress: color });
  }
}

// Update LED state
function updateLED(ledId, color) {
  const led = document.getElementById(ledId);
  if (led) {
    led.style.backgroundColor = color;
    if (color === "red") {
      triggerBeep(180, 100);
    }
  }
}

function updateButtonState(id, depressed, color) {
  const button = document.getElementById(id);
  if (button) {
    // Update the button appearance
    button.style.setProperty("background-color", color);

    if (depressed) {
      button.classList.add("depressed");

      // Trigger beep for ON state only
      switch (id) {
        case "btn1":
        case "btn3":
          beep(880, 100); // Frequency for btn1/btn3 ON
          break;
        case "btn2":
        case "btn4":
          beep(440, 100); // Frequency for btn2/btn4 ON
          break;
      }
    } else {
      button.classList.remove("depressed");

      // Explicitly prevent beeps for OFF state for momentary btn3 and btn4
      if (id === "btn1") {
        beep(784, 100); // Frequency for btn1 OFF
      } else if (id === "btn2") {
        beep(392, 100); // Frequency for btn2 OFF
      }
    }
  }
}

// beep(Hertz, ms)
let activeAudioContext = null;

function beep(frequency, duration) {
  // Close any existing AudioContext to prevent overlapping
  if (activeAudioContext) {
    activeAudioContext.close();
  }

  const audioContext = new (window.AudioContext || window.webkitAudioContext)();
  activeAudioContext = audioContext;

  const oscillator = audioContext.createOscillator();
  const gain = audioContext.createGain();

  oscillator.connect(gain);
  gain.connect(audioContext.destination);

  oscillator.frequency.value = frequency;
  oscillator.type = "sine";
  oscillator.start();

  setTimeout(() => {
    oscillator.stop();
    audioContext.close();
    if (activeAudioContext === audioContext) {
      activeAudioContext = null;
    }
  }, duration);
}

// Button press handlers
function sendButtonPress(buttonId) {
  if (socket && socket.readyState === WebSocket.OPEN) {
    socket.send(buttonId + "~pressed");
  }
}

function releaseButton(buttonId) {
  if (socket && socket.readyState === WebSocket.OPEN) {
    socket.send(buttonId + "~released");
  }
}

// Chart drawing function
let gauge; // Declare the gauge variable globally

function drawChart() {
  if (!gauge) {
    // Create only once
    gauge = new LinearGauge({
      renderTo: document.getElementById("gaugeSWR"),
      width: 330,
      height: 150,
      type: "linear-gauge",
      minValue: 1,
      maxValue: 10,
      majorTicks: [1, 2, 3, 4, 5, 6, 7, 8, 9, 10],
      minorTicks: 2,
      value: 2,
      barBeginCircle: false,
      barWidth: 30,
      colorBarProgress: "green",
      colorBar: "white",
      borderShadowWidth: 0,
      needleCircleSize: 0,
      needleCircleOuter: false,
      needleCircleInner: false,
      colorNeedle: "black",
      highlights: [
        { from: 1, to: 3, color: "green" },
        { from: 3, to: 5, color: "yellow" },
        { from: 5, to: 10, color: "red" },
      ],
      borders: true,
      valueBox: true,
      animationDuration: 500,
      animationRule: "linear",
      animation: {
        animateOnInit: false,
        delay: 0,
      },
    }).draw();
  }

  initWebSocket(); // get new value from WebSocket
}

document.addEventListener("DOMContentLoaded", function () {
  drawChart(); // Call the drawChart function after the DOM is fully loaded

  // Add event listener to SCPI input field
  document
    .getElementById("scpiInput")
    .addEventListener("keydown", function (event) {
      if (event.key === "Enter") {
        sendSCPICommand();
      }
    });

  // Add event listeners for buttons
  const buttons = document.querySelectorAll(".controlButton");

  // Add mousedown listeners for all buttons
  buttons.forEach((button) => {
    button.addEventListener("mousedown", function () {
      const buttonId = button.id; // Get the ID of the button
      sendButtonPress(buttonId); // Call sendButtonPress() for all buttons
    });

    // Add mouseup listeners for btn3 and btn4 only
    if (button.id === "btn3" || button.id === "btn4") {
      button.addEventListener("mouseup", function () {
        const buttonId = button.id; // Get the ID of the button
        releaseButton(buttonId); // Call releaseButton() for btn3 and btn4 only
      });
    }
  });

  // Add event listener for the "clear SCPI input" button
  document
    .getElementById("clearScpiInputButton")
    .addEventListener("click", function () {
      clearSCPIInput(); // Call clearSCPIInput() on click
    });

  // Add event listener for the "clear response" button
  document
    .getElementById("clearResponseButton")
    .addEventListener("click", function () {
      clearSCPIResponse(); // Call clearSCPIResponse() on click
    });
});

function sendSCPICommand() {
  const command = document.getElementById("scpiInput").value;
  if (command && socket && socket.readyState === WebSocket.OPEN) {
    socket.send("SCPI~" + command);
    document.getElementById("scpiInput").value = "";
  }
}

function clearSCPIInput() {
  const scpiInput = document.getElementById("scpiInput");
  scpiInput.value = "";
}
function clearSCPIResponse() {
  const scpiInput = document.getElementById("scpiResponse");
  scpiResponse.value = "";
}



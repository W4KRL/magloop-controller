// 2025-02-14
// 2025-03-16 debug added to initWebSocket()
// 2025-03-17 added beep function to updateButtonState()
// 2025-03-18 improved beep function to reuse audio and prevent overlapping

// Global variables
var gaugeData;
var chart;
var socket;
var options;

// WebSocket initialization function
function initWebSocket() {
  // Create the WebSocket connection
  const socket = new WebSocket("ws://" + window.location.hostname + "/ws");

  socket.onopen = function (event) {
    console.log("WebSocket connection established.");
  };

  socket.onmessage = function (event) {
    const message = event.data; // Message received from server
    const scpiResponseArea = document.getElementById("scpiResponse");

    if (message.includes("~")) {
      const parts = message.split("~");
      const type = parts[0];

      switch (type) {
        case "SCPI":
          // Handle SCPI response
          if (scpiResponseArea) {
            scpiResponseArea.value += parts[1] + "\n";
            scpiResponseArea.scrollTop = scpiResponseArea.scrollHeight;
          }
          break;

        case "SWR":
          // Update the LinearGauge with the new SWR value
          const swrValue = parseFloat(parts[1]);
          if (gauge && !isNaN(swrValue)) {
            gauge.value = swrValue; // Update the gauge value
          }
          break;

        case "led1":
        case "led2":
        case "led3":
        case "led4":
          // Update LED status
          const led = document.getElementById(type);
          if (led) {
            led.style.backgroundColor = parts[1];
          }
          break;

        case "buttonState":
          // Update button states if applicable
          if (parts.length >= 4) {
            updateButtonState(parts[1], parts[2] === "true", parts[3]);
          }
          break;

        case "beep":
          // Handle beeps if applicable
          if (parts.length >= 3) {
            beep(parseFloat(parts[1]), parseInt(parts[2]));
          }
          break;

        default:
          console.log("Unknown message type:", type);
      }
    }
  };

  socket.onerror = function (error) {
    console.error("WebSocket error:", error);
  };

  socket.onclose = function (event) {
    console.log("WebSocket connection closed:", event);
  };
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

      // Explicitly prevent beeps for OFF state for btn3 and btn4
      if (id === "btn1") {
        beep(784, 100); // Frequency for btn1 OFF
      } else if (id === "btn2") {
        beep(392, 100); // Frequency for btn2 OFF
      } else if (id === "btn3" || id === "btn4") {
        // No beep for OFF state for btn3 and btn4
      }
    }
  }
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
function drawChart() {
  gaugeData = google.visualization.arrayToDataTable([
    ["Label", "Value"],
    ["SWR", 2],
  ]);

  options = {
    width: 200,
    height: 120,
    min: 1,
    max: 10,
    redFrom: 5,
    redTo: 10,
    yellowFrom: 3,
    yellowTo: 5,
    majorTicks: ["1", "2", "3", "4", "5", "6", "7", "8", "9", "10"],
    minorTicks: 2,
  };

  chart = new google.visualization.Gauge(document.getElementById("gauge_div"));
  chart.draw(gaugeData, options);

  initWebSocket();
}

document.addEventListener("DOMContentLoaded", function () {
  // Existing Google Charts code
  google.charts.load("current", { packages: ["gauge"] });
  google.charts.setOnLoadCallback(drawChart);

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
  var command = document.getElementById("scpiInput").value;
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

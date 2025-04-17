//! \file script.js
//! 2025-04-17 zero-based indexing for button and LEDs

// Global variables
let socket;
let chart;

// WebSocket initialization function
function initWebSocket() {
  if (socket && socket.readyState !== WebSocket.CLOSED) {
    socket.close();
  }
  socket = new WebSocket("ws://" + window.location.hostname + "/ws");

  socket.onopen = function (event) { };

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
      case "bep":
        beep(parseFloat(parts[1]), parseInt(parts[2])); // frequency, duration
        break;
      case "btn":
        updateButtonState(parts[1], parts[2] === "true", parts[3]); // id, depressed, color
        break;
      case "led":
        updateLedState(parts[1], parts[2]); // id, color
        break;
      case "scp":
        updateSCPIResponse(scpiResponseArea, parts[1]); // response
        break;
      case "swr":
        updateSWRgauge(parseFloat(parts[1])); // swrValue
        break;
      default:
        console.log("Unknown message type:", type);
    }
  }
}

// beep(Hertz, ms)
let activeAudioContext = null;

function beep(frequency, duration) {
  // Close any existing AudioContext if it's still open
  if (activeAudioContext && activeAudioContext.state !== "closed") {
    activeAudioContext.close(); // Only close if the state is not "closed"
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
    // Check the state before closing
    if (audioContext.state !== "closed") {
      audioContext.close(); // Close only if it's valid to do so
      if (activeAudioContext === audioContext) {
        activeAudioContext = null;
      }
    }
  }, duration);
}

function updateButtonState(buttonId, depressed, color) {
  const id = "btn" + buttonId; // Construct the button ID
  const button = document.getElementById(id);
  if (button) {
    // Update the button appearance
    button.style.setProperty("background-color", color);

    if (depressed) {
      button.classList.add("depressed");

      // Trigger beep for ON state only
      switch (id) {
        case "btn0": // scan up
        case "btn2": // jog up
          beep(880, 100); // ON beep for btn0/btn2
          break;
        case "btn1": // scan down
        case "btn3": // jog down
          beep(440, 100); // ON beep for btn1/btn3
          break;
      }
    } else {
      button.classList.remove("depressed");
      // Explicitly prevent beeps for OFF state for momentary btn3 and btn4
      if (id === "btn0") {
        beep(784, 100); // OFF beep for btn0
      } else if (id === "btn1") {
        beep(392, 100); // OFF beep for btn1
      }
    }
  }
}

// Button press handlers
function sendButtonPress(buttonId) {
  const newID = buttonId.replace("btn", "btn~");
  if (socket && socket.readyState === WebSocket.OPEN) {
    socket.send(newID + "~pressed");
  }
}

function releaseButton(buttonId) {
  const newID = buttonId.replace("btn", "btn~");
  if (socket && socket.readyState === WebSocket.OPEN) {
    socket.send(newID + "~released");
  }
}

// Update LED state
function updateLedState(id, color) {
  const ledId = "led" + id;
  const led = document.getElementById(ledId);
  if (led) {
    led.style.backgroundColor = color;
    if (color === "red") {
      beep(180, 100);
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

function updateSWRgauge(swrValue) {
  if (!isNaN(swrValue)) {
    let color;
    if (swrValue < 3) {
      color = "green";
    } else if (swrValue < 5) {
      color = "yellow";
    } else {
      color = "red";
    }
    chart.data.datasets[0].backgroundColor = color;
    chart.data.datasets[0].data = [swrValue];
    chart.update();

    const valueboxSWR = document.getElementById("valueboxSWR");
    if (valueboxSWR) {
      if (swrValue < 3) {
        valueboxSWR.value = swrValue.toFixed(2) + ":1";
      } else {
        valueboxSWR.value = swrValue.toFixed(1) + ":1";
      }
    }
  }
}

function drawSWRgauge() {
  const ctx = document.getElementById("gaugeSWR").getContext("2d");
  const data = {
    labels: ["Gauge"],
    datasets: [
      {
        data: [2.5], // Initial gauge value
        backgroundColor: "green",
        barPercentage: 0.6, // Bar height %
        categoryPercentage: 1,
      },
    ],
  };
  const config = {
    type: "bar",
    data: data,
    options: {
      indexAxis: "y",
      responsive: true,
      scales: {
        x: {
          type: "logarithmic",
          min: 1,
          max: 10,
          ticks: {
            callback: function (value, index, values) {
              if (
                value === 1 ||
                value === 1.5 ||
                value === 2 ||
                value === 3 ||
                value === 5 ||
                value === 7 ||
                value === 10
              ) {
                return value.toString();
              }
              return "";
            },
            color: "black",
            font: {
              size: 12,
            },
            maxRotation: 0,
            autoSkip: false,
          },
          grid: {
            display: true,
            color: "rgba(0, 0, 0, 0.5)", // Darker grid lines
          },
        },
        y: { display: false },
      },
      plugins: {
        legend: { display: false },
        tooltip: { enabled: false },
      },
    },
  };

  chart = new Chart(ctx, config);
}

document.addEventListener("DOMContentLoaded", function () {
  // Initialize the SWR gauge to display initial values and set up its configuration
  drawSWRgauge();

  // Establish a WebSocket connection to enable real-time communication with the server

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

    // Add mouseup listeners for btn2 and btn3 only
    if (button.id === "btn2" || button.id === "btn3") {
      button.addEventListener("mouseup", function () {
        const buttonId = button.id; // Get the ID of the button
        releaseButton(buttonId); // Call releaseButton() for btn2 and btn3 only
      });
    }
  });

  // Add event listener for the "clear response" button
  document
    .getElementById("clearResponseButton")
    .addEventListener("click", function () {
      clearSCPIResponse(); // Call clearSCPIResponse() on click
    });
  initWebSocket();
});

function sendSCPICommand() {
  const command = document.getElementById("scpiInput").value;
  if (command && socket && socket.readyState === WebSocket.OPEN) {
    socket.send("scp~" + command);
    document.getElementById("scpiInput").value = "";
  }
}

function clearSCPIInput() {
  const scpiInput = document.getElementById("scpiInput");
  scpiInput.value = "";
}
function clearSCPIResponse() {
  const scpiResponse = document.getElementById("scpiResponse");
  scpiResponse.value = "";
}

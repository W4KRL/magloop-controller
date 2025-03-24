// 2025-03-23 refactor initWebsocket() and processWebSocketMessage() functions
// 2025-03-24 added valueboxSWR
// 2025-03-25 moved to chart.js

// Global variables
let socket;
let chart; // Declare chart in the global scope

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

// Update LED state
function updateLED(ledId, color) {
  const led = document.getElementById(ledId);
  if (led) {
    led.style.backgroundColor = color;
    if (color === "red") {
      beep(180, 100);
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

function drawChart() {
  // swr gauge
  const ctx = document.getElementById("gaugeSWR").getContext("2d");
  const data = {
    labels: ["Gauge"],
    datasets: [
      {
        data: [2.5], // Initial gauge value
        backgroundColor: "green",
        barPercentage: 0.6, // Bar width %
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

function updateGaugeValue(newValue) {
  chart.data.datasets[0].data = [newValue];
  chart.update();
}

function updateGauge(swrValue) {
  if (!isNaN(swrValue)) {
    const color = swrValue < 3 ? "green" : swrValue < 5 ? "yellow" : "red";
    chart.data.datasets[0].backgroundColor = color;
    chart.data.datasets[0].data = [swrValue];
    chart.update();

    const valueboxSWR = document.getElementById("valueboxSWR");
    if (valueboxSWR) {
      valueboxSWR.value = swrValue.toFixed(2) + ":1";
    }
  }
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

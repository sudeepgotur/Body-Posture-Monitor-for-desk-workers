const API_URL = "/posture";

function updateUI(data) {
  const pitchEl    = document.getElementById("pitchValue");
  const refPitchEl = document.getElementById("refPitchValue");
  const devEl      = document.getElementById("devValue");
  const badSecEl   = document.getElementById("badSecondsValue");
  const statusEl   = document.getElementById("postureStatus");
  const msgEl      = document.getElementById("postureMessage");
  const lastUpdateEl = document.getElementById("lastUpdate");

  const goodPercentTextEl = document.getElementById("goodPercentText");
  const badPercentTextEl  = document.getElementById("badPercentText");
  const goodBarEl = document.getElementById("goodBar");
  const badBarEl  = document.getElementById("badBar");

  // guard in case fields are missing
  const pitch      = Number(data.pitch)      || 0;
  const refPitch   = Number(data.refPitch)   || 0;
  const deviation  = Number(data.deviation)  || 0;
  const badSeconds = Number(data.badSeconds) || 0;

  const goodPercent = Number(data.goodPercent) || 0;
  const badPercent  = Number(data.badPercent)  || 0;

  pitchEl.textContent    = pitch.toFixed(1);
  refPitchEl.textContent = refPitch.toFixed(1);
  devEl.textContent      = deviation.toFixed(1);
  badSecEl.textContent   = badSeconds.toFixed(0);

  // status & message
  statusEl.classList.remove("status-good", "status-warn", "status-bad");

  if (data.status === "GOOD") {
    statusEl.textContent = "GOOD";
    statusEl.classList.add("status-good");
    msgEl.textContent = "Nice! Your posture is within a healthy range.";
  } else if (data.status === "POTENTIALLY_BAD") {
    statusEl.textContent = "WARNING";
    statusEl.classList.add("status-warn");
    msgEl.textContent = "You are bent. If this continues, it may strain your back.";
  } else if (data.status === "BAD") {
    statusEl.textContent = "BAD";
    statusEl.classList.add("status-bad");
    msgEl.textContent = "You have been in bad posture for too long. Please straighten up.";
  } else {
    statusEl.textContent = data.status || "UNKNOWN";
    statusEl.classList.add("status-warn");
    msgEl.textContent = "Unknown status from device.";
  }

  // Time of last update
  const now = new Date();
  lastUpdateEl.textContent = "Last update: " + now.toLocaleTimeString();

  // Percentages + bars
  goodPercentTextEl.textContent = goodPercent.toFixed(1) + " %";
  badPercentTextEl.textContent  = badPercent.toFixed(1) + " %";

  goodBarEl.style.width = Math.max(0, Math.min(100, goodPercent)) + "%";
  badBarEl.style.width  = Math.max(0, Math.min(100, badPercent)) + "%";
}

async function fetchPosture() {
  try {
    const res = await fetch(API_URL);
    if (!res.ok) throw new Error("HTTP " + res.status);
    const data = await res.json();
    updateUI(data);
  } catch (err) {
    console.error("Error fetching posture:", err);
    document.getElementById("lastUpdate").textContent =
      "Last update: error connecting to device.";
  }
}

// first call and then poll every second
fetchPosture();
setInterval(fetchPosture, 1000);

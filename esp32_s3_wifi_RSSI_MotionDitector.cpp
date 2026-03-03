#include <WiFi.h>
#include <WebServer.h>
#include <Adafruit_NeoPixel.h>

#define LED_PIN 48
#define LED_COUNT 1

Adafruit_NeoPixel pixel(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

const char* ssid = "Hafiz_EXT";
const char* password = "Tom@hunt";

WebServer server(80);

long lastRSSI = 0;
float smoothedDiff = 0;
float activityLevel = 0;
bool presence = false;

void setLED(float level) {
  if (level < 0.8) {
    pixel.setPixelColor(0, pixel.Color(0, 255, 0));
  } 
  else if (level < 2) {
    pixel.setPixelColor(0, pixel.Color(255, 180, 0));
  } 
  else {
    pixel.setPixelColor(0, pixel.Color(255, 0, 0));
  }
  pixel.show();
}

String getHTML() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<title>WiFi Motion Lab</title>
<script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
</head>
<body style="background:black;color:white;text-align:center;transition:0.3s;">
<h2>ESP32 WiFi Motion Lab</h2>
<h3 id="status">Status: Idle</h3>
<canvas id="chart" width="400" height="200"></canvas>
<br>
<meter id="meter" min="0" max="4" value="0" style="width:80%;height:30px;"></meter>

<script>
let ctx = document.getElementById('chart').getContext('2d');
let data = {
  labels: [],
  datasets: [{
    label: 'Motion Intensity',
    borderColor: 'lime',
    data: [],
    tension: 0.3
  }]
};

let chart = new Chart(ctx, {
  type: 'line',
  data: data,
  options: { scales: { y: { min: 0, max: 4 } } }
});

async function updateData() {
  let response = await fetch("/data");
  let obj = await response.json();

  let value = obj.level;
  let presence = obj.presence;

  if (data.labels.length > 40) {
    data.labels.shift();
    data.datasets[0].data.shift();
  }

  data.labels.push('');
  data.datasets[0].data.push(value);
  chart.update();

  document.getElementById("meter").value = value;

  if (value < 1.5) {
    document.body.style.background = "black";
    document.getElementById("status").innerText = "Status: Idle";
  } 
  else if (value < 4) {
    document.body.style.background = "#403000";
    document.getElementById("status").innerText = "Status: Movement Detected";
  } 
  else {
    document.body.style.background = "darkred";
    document.getElementById("status").innerText = "Status: HIGH MOTION!";
    beep();
  }

  if (presence) {
    document.getElementById("status").innerText += " (Presence Confirmed)";
  }
}

function beep() {
  let ctx = new AudioContext();
  let oscillator = ctx.createOscillator();
  oscillator.type = "square";
  oscillator.frequency.setValueAtTime(600, ctx.currentTime);
  oscillator.connect(ctx.destination);
  oscillator.start();
  oscillator.stop(ctx.currentTime + 0.1);
}

setInterval(updateData, 200);
</script>

</body>
</html>
)rawliteral";

  return html;
}

void handleRoot() {
  server.send(200, "text/html", getHTML());
}

void handleData() {
  String json = "{\"level\":" + String(activityLevel) + 
                ",\"presence\":" + String(presence ? "true" : "false") + "}";
  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(115200);

  pixel.begin();
  pixel.setBrightness(80);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
  }

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  lastRSSI = WiFi.RSSI();

  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.begin();
}

void loop() {
  server.handleClient();

  long currentRSSI = WiFi.RSSI();
  long diff = abs(currentRSSI - lastRSSI);

  smoothedDiff = 0.4 * smoothedDiff + 0.6 * diff;
  activityLevel = smoothedDiff;

  presence = (activityLevel > 0.7);

  setLED(activityLevel);

  lastRSSI = currentRSSI;

  delay(80);
}

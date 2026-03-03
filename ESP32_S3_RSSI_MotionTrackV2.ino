#include <WiFi.h>
#include <WebServer.h>
#include <Adafruit_NeoPixel.h>

#define LED_PIN 48
#define LED_COUNT 1

Adafruit_NeoPixel pixel(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

const char* ssid = "YOUR_WIFI";
const char* password = "YOUR_PASS";

WebServer server(80);

float filteredRSSI = 0;
float baseline = 0;
float deltaValue = 0;
float energy = 0;
float speedVal = 0;
float confidence = 0;

float history[20];
int indexPos = 0;

unsigned long lastTime = 0;
long lastRSSI = 0;

String activityLabel = "Idle";

void setLED(float conf) {
  if (conf < 20)
    pixel.setPixelColor(0, pixel.Color(0, 255, 0));
  else if (conf < 60)
    pixel.setPixelColor(0, pixel.Color(255, 180, 0));
  else
    pixel.setPixelColor(0, pixel.Color(255, 0, 0));
  pixel.show();
}

String getHTML() {
  return R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<title>WiFi Motion Lab Pro</title>
<script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
<style>
body{background:#0f172a;color:white;text-align:center;font-family:sans-serif;}
.card{background:#1e293b;padding:20px;margin:15px;border-radius:12px;}
</style>
</head>
<body>
<h2>WiFi Motion Lab - Pro Dashboard</h2>

<div class="card">
<h3 id="activity">Activity: Idle</h3>
<h3 id="confidence">Confidence: 0%</h3>
</div>

<div class="card">
<canvas id="chart"></canvas>
</div>

<script>
let ctx=document.getElementById('chart').getContext('2d');
let data={labels:[],datasets:[{label:'Motion Energy',borderColor:'cyan',data:[],tension:0.3}]};
let chart=new Chart(ctx,{type:'line',data:data,options:{scales:{y:{min:0,max:10}}}});

async function update(){
let res=await fetch("/data");
let obj=await res.json();

if(data.labels.length>40){data.labels.shift();data.datasets[0].data.shift();}
data.labels.push('');
data.datasets[0].data.push(obj.energy);
chart.update();

document.getElementById("activity").innerText="Activity: "+obj.activity;
document.getElementById("confidence").innerText="Confidence: "+obj.conf+"%";
}

setInterval(update,200);
</script>
</body>
</html>
)rawliteral";
}

void handleRoot(){ server.send(200,"text/html",getHTML()); }

void handleData(){
String json="{\"energy\":"+String(energy)+
",\"conf\":"+String((int)confidence)+
",\"activity\":\""+activityLabel+"\"}";
server.send(200,"application/json",json);
}

void setup(){
Serial.begin(115200);
pixel.begin();
pixel.setBrightness(50);

WiFi.begin(ssid,password);
while(WiFi.status()!=WL_CONNECTED){ delay(300); }

Serial.println(WiFi.localIP());

lastRSSI = WiFi.RSSI();
filteredRSSI = lastRSSI;
baseline = lastRSSI;

server.on("/",handleRoot);
server.on("/data",handleData);
server.begin();
}

void loop(){
server.handleClient();

long currentRSSI = WiFi.RSSI();

filteredRSSI = 0.8*filteredRSSI + 0.2*currentRSSI;

baseline = 0.995*baseline + 0.005*filteredRSSI;

deltaValue = abs(filteredRSSI - baseline);

history[indexPos] = deltaValue;
indexPos = (indexPos + 1) % 20;

energy = 0;
for(int i=1;i<20;i++)
  energy += abs(history[i]-history[i-1]);

unsigned long now = millis();
float dt = (now-lastTime)/1000.0;
if(dt>0)
  speedVal = abs(currentRSSI-lastRSSI)/dt;

confidence = constrain(energy*15,0,100);

if(confidence < 20)
  activityLabel="Idle";
else if(confidence < 60)
  activityLabel="Light Movement";
else
  activityLabel="Heavy Movement";

setLED(confidence);

lastRSSI=currentRSSI;
lastTime=now;

delay(80);
}
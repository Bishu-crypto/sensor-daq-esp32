#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>

const char* SSID     = "Bishuwifi";
const char* PASSWORD = "00000000";

#define DHT_PIN       4
#define DHT_TYPE      DHT11
#define LDR_PIN       36
#define SOUND_PIN     39
#define IR_PIN        19
#define VIBRATION_PIN 18

DHT dht(DHT_PIN, DHT_TYPE);
WebServer server(80);

float temperature  = 0;
float humidity     = 0;
int   ldr_raw      = 0;
int   sound_raw    = 0;
bool  ir_detected  = false;
bool  vibration    = false;
int   vib_count    = 0;
unsigned long lastRead = 0;

void readSensors() {
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  if (!isnan(t)) temperature = t;
  if (!isnan(h)) humidity    = h;

  long ldr_sum = 0, sound_sum = 0;
  for (int i = 0; i < 64; i++) {
    ldr_sum   += analogRead(LDR_PIN);
    sound_sum += analogRead(SOUND_PIN);
  }
  ldr_raw   = ldr_sum   / 64;
  sound_raw = sound_sum / 64;

  ir_detected = (digitalRead(IR_PIN) == LOW);

  bool vib_now = (digitalRead(VIBRATION_PIN) == HIGH);
  if (vib_now && !vibration) vib_count++;
  vibration = vib_now;
}

void handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>Sensor DAQ</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <meta http-equiv="refresh" content="2">
  <style>
    *{box-sizing:border-box;margin:0;padding:0}
    body{background:#0d1117;color:#e6edf3;font-family:monospace;padding:20px}
    h1{font-size:1.2em;color:#58a6ff;margin-bottom:4px}
    .sub{font-size:0.75em;color:#8b949e;margin-bottom:16px}
    .grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(150px,1fr));gap:12px;margin-bottom:16px}
    .card{background:#161b22;border:1px solid #30363d;border-radius:8px;padding:16px;text-align:center}
    .icon{font-size:1.8em;margin-bottom:6px}
    .label{font-size:0.7em;color:#8b949e;margin-bottom:4px;text-transform:uppercase}
    .value{font-size:1.6em;font-weight:bold;color:#3fb950}
    .unit{font-size:0.7em;color:#8b949e}
    .warn{color:#f78166!important}
    .ok{color:#3fb950!important}
    .bar-bg{background:#21262d;border-radius:4px;height:8px;margin-top:8px}
    .bar{height:8px;border-radius:4px}
  </style>
</head>
<body>
  <h1>&#9889; Multi-Sensor DAQ System</h1>
  <div class="sub">Live &#8226; ESP32 &#8226; Vaibhav Parashari | IIT Madras</div>
  <div class="grid">
    <div class="card">
      <div class="icon">&#127777;</div>
      <div class="label">Temperature</div>
      <div class="value">)rawliteral" + String(temperature,1) + R"rawliteral( &deg;C</div>
    </div>
    <div class="card">
      <div class="icon">&#128167;</div>
      <div class="label">Humidity</div>
      <div class="value">)rawliteral" + String(humidity,1) + R"rawliteral( %</div>
    </div>
    <div class="card">
      <div class="icon">&#9728;</div>
      <div class="label">Light Level</div>
      <div class="value">)rawliteral" + String(ldr_raw) + R"rawliteral(</div>
      <div class="unit">ADC raw</div>
      <div class="bar-bg"><div class="bar" style="width:)rawliteral" + String(ldr_raw/40.95) + R"rawliteral(%;background:#f0e68c"></div></div>
    </div>
    <div class="card">
      <div class="icon">&#127908;</div>
      <div class="label">Sound Level</div>
      <div class="value )rawliteral" + String(sound_raw>2000?"warn":"") + R"rawliteral(">)rawliteral" + String(sound_raw) + R"rawliteral(</div>
      <div class="unit">ADC raw</div>
      <div class="bar-bg"><div class="bar" style="width:)rawliteral" + String(sound_raw/40.95) + R"rawliteral(%;background:#58a6ff"></div></div>
    </div>
    <div class="card">
      <div class="icon">&#128065;</div>
      <div class="label">IR Obstacle</div>
      <div class="value )rawliteral" + String(ir_detected?"warn":"ok") + R"rawliteral(">)rawliteral" + String(ir_detected?"DETECTED":"CLEAR") + R"rawliteral(</div>
    </div>
    <div class="card">
      <div class="icon">&#128288;</div>
      <div class="label">Vibration</div>
      <div class="value )rawliteral" + String(vibration?"warn":"ok") + R"rawliteral(">)rawliteral" + String(vibration?"ACTIVE":"NONE") + R"rawliteral(</div>
      <div class="unit">events: )rawliteral" + String(vib_count) + R"rawliteral(</div>
    </div>
  </div>
</body>
</html>
)rawliteral";
  server.send(200, "text/html", html);
}

void handleData() {
  readSensors();
  String json = "{";
  json += "\"temp\":"      + String(temperature, 1) + ",";
  json += "\"humi\":"      + String(humidity,    1) + ",";
  json += "\"ldr\":"       + String(ldr_raw)        + ",";
  json += "\"sound\":"     + String(sound_raw)      + ",";
  json += "\"ir\":"        + String(ir_detected  ? 1 : 0) + ",";
  json += "\"vib\":"       + String(vibration    ? 1 : 0) + ",";
  json += "\"vib_count\":" + String(vib_count)      + "}";
  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(115200);
  analogSetAttenuation(ADC_11db);
  dht.begin();
  pinMode(IR_PIN,        INPUT);
  pinMode(VIBRATION_PIN, INPUT);

  WiFi.begin(SSID, PASSWORD);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) { delay(400); Serial.print("."); }
  Serial.println("\nIP: " + WiFi.localIP().toString());
  Serial.println("Open: http://" + WiFi.localIP().toString());

  server.on("/",     handleRoot);
  server.on("/data", handleData);
  server.begin();
}

void loop() {
  server.handleClient();
  readSensors();

  if (millis() - lastRead > 2000) {
    Serial.printf("T:%.1f H:%.1f LDR:%d Sound:%d IR:%d Vib:%d Count:%d\n",
      temperature, humidity, ldr_raw, sound_raw,
      ir_detected, vibration, vib_count);
    lastRead = millis();
  }
}
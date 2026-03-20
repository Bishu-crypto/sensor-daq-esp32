#include <Arduino.h>

#define STRAIN_PIN     34
#define OFFSET_PIN     35
#define ALARM_LED      2

const float VEX          = 5.0;
const float AMP_GAIN     = 495.0;
const float GAUGE_FACTOR = 2.0;
const int   AVG_SAMPLES  = 64;
const float WARN_THRESH  = 500.0;

float baseline_V  = 0.0;
float peakStrain  = 0.0;
float minStrain   = 0.0;
unsigned long lastPrint   = 0;
unsigned long lastSummary = 0;
unsigned long startTime   = 0;

float readVoltage(int pin) {
  long sum = 0;
  for (int i = 0; i < AVG_SAMPLES; i++) sum += analogRead(pin);
  return (sum / (float)AVG_SAMPLES / 4095.0) * 3.3;
}

float toMicrostrain(float V) {
  float dV = V - baseline_V;
  return (4.0 * dV / (AMP_GAIN * GAUGE_FACTOR * VEX)) * 1e6;
}

void calibrate() {
  Serial.println(">> Calibrating baseline...");
  long sum = 0;
  for (int i = 0; i < 256; i++) sum += analogRead(STRAIN_PIN);
  baseline_V = (sum / 256.0 / 4095.0) * 3.3;
  peakStrain = 0.0;
  minStrain  = 0.0;
  Serial.print(">> Baseline set: ");
  Serial.println(baseline_V, 5);
}

void setup() {
  Serial.begin(115200);
  analogSetAttenuation(ADC_11db);
  pinMode(ALARM_LED, OUTPUT);
  digitalWrite(ALARM_LED, LOW);
  Serial.println("============================================");
  Serial.println("  Strain Gauge DAQ -- Wokwi Simulation");
  Serial.println("  Vaibhav Parashari | IIT Madras");
  Serial.println("============================================");
  Serial.println("POT1=strain  POT2=offset  c=calibrate  r=reset");
  Serial.println("--------------------------------------------");
  delay(1000);
  calibrate();
  startTime = millis();
}

void loop() {
  float V      = readVoltage(STRAIN_PIN);
  float offset = readVoltage(OFFSET_PIN) * 0.1;
  float us     = toMicrostrain(V + offset);

  if (us > peakStrain) peakStrain = us;
  if (us < minStrain)  minStrain  = us;

  bool alarm = (abs(us) > WARN_THRESH);
  digitalWrite(ALARM_LED, alarm ? HIGH : LOW);

  if (millis() - lastPrint >= 100) {
    String out = "Voltage:" + String(V, 4);
    out += "\tMicrostrain:" + String(us, 1);
    out += "\tPeak:" + String(peakStrain, 1);
    out += "\tAlarm:" + String(alarm ? 500 : 0);
    Serial.println(out);
    lastPrint = millis();
  }

  if (millis() - lastSummary >= 5000) {
    Serial.println("----------");
    Serial.print("Live: ");   Serial.print(us, 2);         Serial.println(" ue");
    Serial.print("Peak+: ");  Serial.print(peakStrain, 2); Serial.println(" ue");
    Serial.print("Peak-: ");  Serial.print(minStrain, 2);  Serial.println(" ue");
    Serial.print("Volts: ");  Serial.println(V, 5);
    Serial.print("Uptime: "); Serial.print((millis()-startTime)/1000.0, 1);
    Serial.println(" s");
    Serial.println("----------");
    lastSummary = millis();
  }

  if (Serial.available()) {
    char cmd = Serial.read();
    if (cmd == 'c' || cmd == 'C') calibrate();
    if (cmd == 'r' || cmd == 'R') {
      peakStrain = 0;
      minStrain  = 0;
      Serial.println(">> Peak reset.");
    }
  }

  delay(20);
}
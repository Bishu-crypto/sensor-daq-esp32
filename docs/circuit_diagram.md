# Circuit Diagram

## Wiring Table

| Sensor | VCC | GND | Signal | ESP32 Pin |
|---|---|---|---|---|
| DHT11 | 3.3V | GND | DATA | GPIO4 |
| LDR | 3.3V | GND | AO | GPIO36 (SVP) |
| Sound Sensor | 3.3V | GND | AO | GPIO39 (SVN) |
| IR Obstacle | 3.3V | GND | OUT | GPIO19 |
| Vibration Switch | 3.3V | GND | OUT | GPIO18 |

## ESP32 Pin Map
```
        ESP32-32S
   ┌──────────────────┐
   │ 3.3V ── VCC (all sensors)
   │ GND  ── GND (all sensors)
   │ GPIO4  ── DHT11 DATA
   │ GPIO36 ── LDR AO      (SVP)
   │ GPIO39 ── Sound AO    (SVN)
   │ GPIO19 ── IR OUT
   │ GPIO18 ── Vibration OUT
   └──────────────────┘
```

## Power Notes
- All sensors powered from ESP32 3.3V pin
- Total current draw < 200mA — safe for ESP32 onboard regulator
- For stable analog readings use 64-sample averaging in firmware

## Signal Types
| Sensor | Signal Type | Range |
|---|---|---|
| DHT11 | Digital one-wire | 0/1 |
| LDR | Analog | 0–4095 ADC |
| Sound | Analog | 0–4095 ADC |
| IR | Digital | HIGH/LOW |
| Vibration | Digital | HIGH/LOW |
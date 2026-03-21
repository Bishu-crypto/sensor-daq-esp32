# Calibration Guide

## DHT11
- No calibration needed
- Allow 2 seconds between readings
- Valid range: 0–50°C, 20–90% RH

## LDR (Photosensitive)
- Cover sensor completely → ADC reads ~0 (dark)
- Point at bright light → ADC reads ~4095 (bright)
- Adjust threshold in code based on your environment

## Sound Sensor
- Has onboard potentiometer (blue trimmer)
- Rotate clockwise → more sensitive
- Rotate anticlockwise → less sensitive
- ADC > 2000 = loud sound threshold in firmware

## IR Obstacle Sensor
- Has onboard potentiometer for distance adjustment
- Rotate to set detection range (5–30cm typical)
- OUTPUT LOW = obstacle detected
- OUTPUT HIGH = clear

## Vibration Switch
- No calibration needed
- Physical tap or vibration triggers the switch
- Firmware counts rising edge events
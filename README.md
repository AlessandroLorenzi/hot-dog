# Hot Dog

Temperature and humidity monitor for Fenny, built on an ESP32 with a DHT11 sensor
and SSD1306 OLED display.

## How it works

The device wakes up from deep sleep every 30 seconds, reads temperature and humidity,
and shows the data on the display. If the temperature exceeds **27°C**, the display
shows `FENNY HOT` as a warning.

Maximum temperature and humidity values are stored in RTC memory, persisting across
deep sleep cycles.

## Hardware

| Component | Details |
| --- | --- |
| Microcontroller | ESP32 |
| Sensor | DHT11 (GPIO pin 4) |
| Display | OLED SSD1306 128x64 (I2C, address 0x3C) |

## Display

```txt
T:  22.5C  H:  45.0%
MT: 26.3C  MH: 50.0%
FENNY OK
```

- **Line 1** — current temperature and humidity
- **Line 2** — maximum values recorded since boot
- **Line 3** — `FENNY OK` if temp < 27°C, `FENNY HOT` otherwise

## Dependencies

- [DHT sensor library](https://github.com/adafruit/DHT-sensor-library) — Adafruit
- [Adafruit GFX Library](https://github.com/adafruit/Adafruit-GFX-Library)
- [Adafruit SSD1306](https://github.com/adafruit/Adafruit_SSD1306)

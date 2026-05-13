# Hot Dog

> **Questo progetto è stato riscritto.** La nuova versione è disponibile su [AlessandroLorenzi/hot-dog-2](https://github.com/AlessandroLorenzi/hot-dog-2).

Temperature and humidity monitor for Fenny, built on an ESP32 with a DHT11 sensor
and SSD1306 OLED display. Publishes Telegram status notifications based on the
temperature threshold.

## How it works

The device reads temperature and humidity every 10 seconds and updates the OLED display.
If the temperature exceeds **27°C** (configurable via `THRESHOLD`), the display shows
`FENNY HOT`.

Wi-Fi connection handling is non-blocking: the sketch keeps running even if the network
is down, and retries connection in the background every 5 seconds.

Telegram notifications are rate-limited by status:

- `FENNY HOT` notification: at most once per minute while temperature is above threshold
- `FENNY OK` notification: at most once every 10 minutes while temperature is below threshold

Maximum temperature and humidity values are tracked while the device is running.

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

## Configuration

Copy `config.h.example` to `config.h` and fill in your credentials:

```h
#define WIFI_SSID     "your_ssid"
#define WIFI_PASSWORD "your_password"
#define BOT_TOKEN     "123456789:AAxxxx..."
#define CHAT_ID       "123456789"
```

`config.h` is listed in `.gitignore` and will not be committed.

To get a bot token, talk to [@BotFather](https://t.me/BotFather) on Telegram.
To get your chat ID, use [@userinfobot](https://t.me/userinfobot).

## Build & Upload

Requires [arduino-cli](https://arduino.github.io/arduino-cli/).

```bash
# Compile and upload to /dev/ttyUSB0
make all

# Install required libraries (once)
make install-libs

# Compile
make compile

# Compile and upload to /dev/ttyUSB0
make upload

# Open serial monitor
make monitor

# Override port
make upload PORT=/dev/ttyUSB1
```

## Dependencies

- [DHT sensor library](https://github.com/adafruit/DHT-sensor-library) — Adafruit
- [Adafruit GFX Library](https://github.com/adafruit/Adafruit-GFX-Library)
- [Adafruit SSD1306](https://github.com/adafruit/Adafruit_SSD1306)
- [UniversalTelegramBot](https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot)
  — Brian Lough

// Temp & Humidity
#include <DHT.h>

// Display imports
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

// Imports wifi
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include "config.h"

// Setup DHT sensor
#define DHTPIN 4
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

// Setup display
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET     -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Deep sleep settings
#define uS_TO_S_FACTOR 1000000ULL /* Conversion factor for micro seconds to seconds */

// Function prototypes
void read_data();
void print_on_serial();
void print_on_display();
void connect_to_wifi();
void telegram_alert();
void telegram_keepalive();

// Global variables
float temp = 0.0;
float humid = 0.0;
RTC_DATA_ATTR float max_temp = 0.0;
RTC_DATA_ATTR float max_humid = 0.0;
RTC_DATA_ATTR int last_keepalive = 0;

WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

void setup() {
  Serial.begin(115200);

  pinMode(DHTPIN, INPUT_PULLUP);
  dht.begin();

  // Wait for sensor to stabilize
  delay(2000);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("Display not found!");
    while (true);
  }

  // Clear the buffer
  display.clearDisplay();
  display.setTextColor(WHITE);

  // Read sensor data, print on display and serial
  read_data();
  print_on_display();
  print_on_serial();

  // Send Telegram alert if temperature exceeds threshold
  if (temp >= THRESHOLD) {
    connect_to_wifi();
    telegram_alert();
    WiFi.disconnect(true);
  }
  telegram_keepalive();

  // Configure deep sleep
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  esp_deep_sleep_start();
}


void loop() {
  // never called because of deep sleep
}


void read_data(){
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  if(isnan(t) || isnan(h)){
    Serial.println("DHT read error!");
    return;
  }

  temp = t;
  humid = h;
  if (t > max_temp) {
    max_temp = t;
  }
  if (h > max_humid) {
    max_humid = h;
  }
}


void print_on_serial() {
  Serial.print("Temp:      "); Serial.println(temp);
  Serial.print("Max Temp:  "); Serial.println(max_temp);
  Serial.print("Humid:     "); Serial.println(humid);
  Serial.print("Max Humid: "); Serial.println(max_humid);
}


void print_on_display() {
  display.clearDisplay();
  display.setTextSize(1);

  // first line: T: 22.5C H 30%
  display.setCursor(0, 0);
  display.print("T:  ");
  display.print(temp, 1);
  display.print("C ");
  display.print("H:  ");
  display.print(humid, 1);
  display.println("%");
 
  // second line max: MT: 26.3C MH 50%
  display.setCursor(0, 16);
  display.print("MT: ");
  display.print(max_temp, 1);
  display.print("C ");
  display.print("MH: ");
  display.print(max_humid, 1);
  display.println("%");
 
  // Third line (BIG): Fenny OK / HOT DOG
  display.setTextSize(2);
  display.setCursor(0, 32);
  if (temp < THRESHOLD) {
    display.println("FENNY OK");
  } else {
    display.println("FENNY HOT");
  }

  display.display();
}

void connect_to_wifi() {
  Serial.print("Connecting to WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected!");
}

void telegram_alert() {
  client.setInsecure(); // Disable SSL certificate verification
  String msg = "Fenny is HOT!\n";
  msg += "Temp: " + String(temp, 1) + "C\n";
  msg += "Humidity: " + String(humid, 1) + "%";
  msg += "\nMax Temp: " + String(max_temp, 1) + "C\n";
  msg += "Max Humidity: " + String(max_humid, 1) + "%";
  bot.sendMessage(CHAT_ID, msg, "");
  Serial.println("Telegram alert sent.");
}

void telegram_keepalive() {
  if (last_keepalive == 0) {
      connect_to_wifi();

      client.setInsecure(); // Disable SSL certificate verification
      bot.sendMessage(CHAT_ID, "Fenny keepalive", "");
      Serial.println("Telegram keepalive sent.");
      
      WiFi.disconnect(true);
  }
  last_keepalive ++;
  if (last_keepalive == 10) {
    last_keepalive = 0;
  }
}
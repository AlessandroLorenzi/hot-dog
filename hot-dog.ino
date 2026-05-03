// Temp & Humidity
#include <DHT.h>

// Display imports
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

// Setup DHT sensor
#define DHTPIN 4
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

// Setup display
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET     -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Threshold for "FENNY HOT" status
#define THRESHOLD 27.0

// Deep sleep settings
#define uS_TO_S_FACTOR 1000000ULL /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  30         /* Time ESP32 will go to sleep (in seconds) */

// Function prototypes
void read_data();
void print_on_serial();
void print_on_display();

// Global variables
float temp = 0.0;
float humid = 0.0;
RTC_DATA_ATTR float max_temp = 0.0;
RTC_DATA_ATTR float max_humid = 0.0;

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

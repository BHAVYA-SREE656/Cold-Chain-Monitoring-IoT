#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
const char* ssid = "realme C53";
const char* password = "cybad5MSSB";
String apiKey = " VMWEP6BC1TNYCO9F";
const char* server = "http://api.thingspeak.com/update";
float tempLimit = 8.0;
float humLimit = 80.0;
void setup() {
  Serial.begin(115200);
  dht.begin();
  // OLED init
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED failed");
    while (true);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  // WiFi connect
  WiFi.begin(ssid, password);
  Serial.print("Connecting WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
}
void loop() {
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();
  if (isnan(temp) || isnan(hum)) {
    Serial.println("Failed to read from DHT");
    return;
  }
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Temp: ");
  display.print(temp);
  display.println(" C");
  display.print("Hum : ");
  display.print(hum);
  display.println(" %");
  if (temp > tempLimit) {
    display.println("Status: ALERT!");
  } else {
    display.println("Status: SAFE");
  }
  display.display();
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = String(server) +
                 "?api_key=" + apiKey +
                 "&field1=" + String(temp) +
                 "&field2=" + String(hum);
    http.begin(url);
    int httpResponseCode = http.GET();
    Serial.print("ThingSpeak response: ");
    Serial.println(httpResponseCode);
    http.end();
  }
  delay(20000); // ThingSpeak requires 15 sec minimum
}

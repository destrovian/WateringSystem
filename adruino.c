#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

// Network and Telegram Bot credentials
const char* ssid = "ssid";
const char* password = "pw";
const char* telegramToken = "your_TELEGRAM_BOT_TOKEN";
const char* telegramChatId = "your_TELEGRAM_CHAT_ID";

// Sensor and pump pins
const int sensorPin1 = 36;
const int sensorPin2 = 39;
const int sensorPin3 = 34;
const int sensorPin4 = 35;
const int pumpPin1 = 23;
const int pumpPin2 = 22;
const int pumpPin3 = 1;
const int pumpPin4 = 3;
const int sensorPowerPin = 18;

// Moisture thresholds
const int dryThreshold = 3400;
const int moistThreshold = 3000;

// Deep sleep duration
const uint64_t deepSleepDuration = 12ULL * 60ULL * 60ULL * 1000000ULL;

// Initialize Telegram bot client
WiFiClientSecure client;
UniversalTelegramBot bot(telegramToken, client);

void setup() {
 Serial.begin(9600);
 WiFi.mode(WIFI_MODE_STA);
 WiFi.setSleep(true); // Enable sleep mode
 //WiFi.setSleepMode(WIFI_PS_MODEM);
 WiFi.begin(ssid, password);

 while (WiFi.status() != WL_CONNECTED) {
    delay(5000);
    Serial.println("Connecting to WiFi...");
    // Check if the connection failed due to incorrect credentials
    if (WiFi.status() == WL_CONNECT_FAILED) {
        Serial.println("Failed to connect to WiFi. Check your credentials.");
        break; // Exit the loop if the connection failed
    }
 }

 if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Connected to WiFi");
 } else {
    Serial.println("Failed to connect to WiFi. Please check your credentials.");
 }

 pinMode(pumpPin1, OUTPUT);
 pinMode(pumpPin2, OUTPUT);
 pinMode(pumpPin3, OUTPUT);
 pinMode(pumpPin4, OUTPUT);
 digitalWrite(pumpPin1, LOW);
 digitalWrite(pumpPin2, LOW);
 digitalWrite(pumpPin3, LOW);
 digitalWrite(pumpPin4, LOW);

 pinMode(sensorPowerPin, OUTPUT);
 digitalWrite(sensorPowerPin, HIGH);
 delay(1000); // Wait for sensors to stabilize
}

void loop() {
 unsigned long startTime = millis();
 digitalWrite(sensorPowerPin, HIGH);
 delay(1000); // Wait for sensors to stabilize

 int sensorValue1 = analogRead(sensorPin1);
 int sensorValue2 = analogRead(sensorPin2);
 int sensorValue3 = analogRead(sensorPin3);
 int sensorValue4 = analogRead(sensorPin4);

 Serial.print("Moisture Sensor 1: ");
 Serial.println(sensorValue1);
 Serial.print("Moisture Sensor 2: ");
 Serial.println(sensorValue2);
 Serial.print("Moisture Sensor 3: ");
 Serial.println(sensorValue3);
 Serial.print("Moisture Sensor 4: ");
 Serial.println(sensorValue4);

 controlPumps(sensorValue1, pumpPin1);
 controlPumps(sensorValue2, pumpPin2);
 controlPumps(sensorValue3, pumpPin3);
 controlPumps(sensorValue4, pumpPin4);

 unsigned long elapsedTime = millis() - startTime;
 if (elapsedTime >= 60000) { // 1 minute
    sendMessageAndSleep();
 }
}

void controlPumps(int sensorValue, int pumpPin) {
 while (sensorValue > dryThreshold) {
    digitalWrite(pumpPin, HIGH);
 }
 // end the pumping
 digitalWrite(pumpPin, LOW);
}

void sendMessageAndSleep() {
 digitalWrite(sensorPowerPin, LOW);
 digitalWrite(pumpPin1, LOW);
 digitalWrite(pumpPin2, LOW);
 digitalWrite(pumpPin3, LOW);
 digitalWrite(pumpPin4, LOW);

 String message = "Cycle Complete. Total active time: " + String(millis() / 1000) + " seconds";
 bot.sendMessage(telegramChatId, message);

 ESP.deepSleep(deepSleepDuration);
}
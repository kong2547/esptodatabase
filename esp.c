#include <WiFi.h>
#include <WiFiManager.h>   // https://github.com/tzapu/WiFiManager
#include <HTTPClient.h>
#include <ArduinoJson.h>

// ---------------- CONFIG ----------------
const char* deviceID   = "ESP32_001"; 
const char* serverPOST = "http://172.20.10.6/esp_status.php";   // ส่งสถานะ
const char* serverGET  = "http://172.20.10.6/get_status.php";   // ดึงสถานะ

// ปุ่มและ Relay ที่สัมพันธ์กัน
const int buttonPins[] = {0, 5, 15};     // ปุ่ม
const int relayPins[]  = {26, 25, 32};   // Relay/LED
const int numPins = sizeof(buttonPins) / sizeof(buttonPins[0]);

int relayStatus[numPins]       = {0, 0, 0};   // relay state
int lastButtonState[numPins]   = {HIGH, HIGH, HIGH}; // button state ก่อนหน้า

unsigned long lastSync = 0;
const unsigned long syncInterval = 5000; // ดึงจากเว็บทุก 5 วิ

// ---------------- SETUP ----------------
void setup() {
  Serial.begin(115200);

  for (int i=0; i<numPins; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);  // ปุ่มต่อลง GND
    pinMode(relayPins[i], OUTPUT);
    digitalWrite(relayPins[i], LOW);
  }

  WiFiManager wm;
  if (!wm.autoConnect("ESP32_ConfigAP")) {
    Serial.println("WiFi Connect Failed");
    ESP.restart();
  }
  Serial.println("Connected to WiFi!");
}

// ---------------- LOOP ----------------
void loop() {
  // ตรวจจับการกดปุ่มจริง
  for (int i=0; i<numPins; i++) {
    int buttonState = digitalRead(buttonPins[i]);
    if (lastButtonState[i]==HIGH && buttonState==LOW) {   // detect press
      relayStatus[i] = !relayStatus[i];                   // toggle
      digitalWrite(relayPins[i], relayStatus[i]);
      sendStatus(relayPins[i], relayStatus[i]);           // ส่งค่าไป server
      delay(300); // กันเด้ง
    }
    lastButtonState[i] = buttonState;
  }

  // sync กับเว็บทุกๆ 5 วิ
  if (millis() - lastSync > syncInterval) {
    fetchStatusFromServer();
    lastSync = millis();
  }
}

// ---------------- ส่งข้อมูลไป PHP ----------------
void sendStatus(int pin, int status) {
  if (WiFi.status()!=WL_CONNECTED) return;

  HTTPClient http;
  http.begin(serverPOST);
  http.addHeader("Content-Type", "application/json");

  StaticJsonDocument<200> doc;
  doc["device_id"] = deviceID;
  JsonArray states = doc.createNestedArray("states");
  JsonObject s = states.createNestedObject();
  s["pin"]   = pin;
  s["value"] = status;

  String jsonStr;
  serializeJson(doc, jsonStr);
  Serial.println("POST JSON: " + jsonStr);

  int code = http.POST(jsonStr);
  if (code>0) Serial.println("Response: " + http.getString());
  else        Serial.println("POST Error: " + String(code));
  http.end();
}

// ---------------- ดึงสถานะจากเว็บ ----------------
void fetchStatusFromServer() {
  if (WiFi.status()!=WL_CONNECTED) return;

  HTTPClient http;
  String url = String(serverGET) + "?device_id=" + deviceID;
  http.begin(url);
  int code = http.GET();

  if (code==200) {
    String payload = http.getString();
    Serial.println("GET: " + payload);

    StaticJsonDocument<500> doc;
    if (!deserializeJson(doc, payload)) {
      if (doc["ok"]) {
        for (JsonObject s : doc["states"].as<JsonArray>()) {
          int pin   = s["pin"];
          int state = s["state"];
          for (int i=0; i<numPins; i++) {
            if (relayPins[i]==pin) {
              relayStatus[i] = state;
              digitalWrite(relayPins[i], relayStatus[i]);
            }
          }
        }
      }
    }
  } else {
    Serial.println("GET Error: " + String(code));
  }
  http.end();
}

#include <WiFi.h>
#include <WiFiManager.h>          // https://github.com/tzapu/WiFiManager
#include <HTTPClient.h>
#include <ArduinoJson.h>

// ---------------- CONFIG ----------------
const char* deviceID = "ESP32_001"; // ตั้งชื่ออุปกรณ์ (แก้ผ่าน Config WiFiManager ได้ถ้าเพิ่ม Parameter)
const char* serverURL = "http://172.20.10.6/esp_status.php"; // เปลี่ยนเป็น IP เครื่อง Ubuntu ของคุณ

// GPIO ที่ควบคุม (สามารถใส่กี่ขาก็ได้)
const int pins[] = {25, 26, 27};  // ตัวอย่าง LED หรือ relay
const int numPins = sizeof(pins) / sizeof(pins[0]);

// เก็บสถานะ LED
int pinStatus[numPins] = {0, 0, 0};

void setup() {
  Serial.begin(115200);

  // ตั้งค่า GPIO เป็น OUTPUT
  for(int i=0; i<numPins; i++){
    pinMode(pins[i], OUTPUT);
    digitalWrite(pins[i], LOW);
  }

  // ---------------- WiFi ----------------
  WiFiManager wifiManager;
  //wifiManager.resetSettings(); // ถ้าต้องการล้าง WiFi เก่า
  if(!wifiManager.autoConnect("ESP32_ConfigAP")) {
    Serial.println("Failed to connect WiFi");
    ESP.restart();
  }
  Serial.println("Connected to WiFi!");
}

void loop() {
  // ตรวจสอบปุ่ม/LED (ตัวอย่างสมมติว่ากดเปลี่ยนสถานะ)
  for(int i=0; i<numPins; i++){
    // สลับสถานะทุก 5 วินาทีเป็นตัวอย่าง
    pinStatus[i] = !pinStatus[i];
    digitalWrite(pins[i], pinStatus[i]);
    
    // ส่งค่า JSON ไป PHP
    sendStatus(pins[i], pinStatus[i]);

    delay(5000); // กำหนดตามต้องการ
  }
}

// ---------------- ส่งข้อมูลไป PHP ----------------
void sendStatus(int pin, int status){
  if(WiFi.status() == WL_CONNECTED){
    HTTPClient http;
    http.begin(serverURL);
    http.addHeader("Content-Type", "application/json");

    // สร้าง JSON
    StaticJsonDocument<200> doc;
    doc["device_id"] = deviceID;
    doc["pin"] = pin;
    doc["status"] = status;

    String jsonStr;
    serializeJson(doc, jsonStr);

    int httpResponseCode = http.POST(jsonStr);

    if(httpResponseCode>0){
      String response = http.getString();
      Serial.println("Response: " + response);
    } else {
      Serial.println("Error sending POST: " + String(httpResponseCode));
    }

    http.end();
  } else {
    Serial.println("WiFi not connected!");
  }
}

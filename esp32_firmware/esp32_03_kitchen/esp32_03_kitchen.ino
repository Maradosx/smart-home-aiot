/**
 * ============================================================
 * Smart Home AIoT — ESP32 #3 Kitchen
 * ============================================================
 *

 * Board   : ESP32 DevKit V1 (ESP32-WROOM-32)
 * Zone    : ห้องครัว (Kitchen)
 *
 * Wiring:
 *   GPIO2  → 220Ω → LED(+) → LED(−) → GND         ไฟครัว (direct)
 *   GPIO4  → DHT22 DATA (10kΩ pull-up → 3.3V)      อุณหภูมิ/ความชื้น
 *   GPIO13 ← MQ-2 D0                               แก๊ส/ควัน (digital)
 *   GPIO25 → Relay Stove Sim IN (Active LOW)         จำลองเตาแก๊ส (demo)
 *   GPIO26 → Relay Kitchen Fan IN (Active LOW)       พัดลมระบายอากาศครัว
 *   GPIO34 ← MQ-2 A0                               แก๊ส/ควัน (analog, input-only)
 *   GPIO36 ← LDR: 3.3V→10kΩ→GPIO36→LDR→GND        แสงสว่าง (input-only)
 *
 * MQ-2: D0 = LOW เมื่อแก๊สเกิน threshold (ตั้งค่าด้วย potentiometer บน module)
 * MQ warm-up: 60 วินาทีหลัง boot
 * Auto-safety: gas alert → เตา OFF + พัดลม ON อัตโนมัติ
 *
 * WiFi    : Primary=Maradosx / Fallback=SmartHome-Local
 * MQTT    : 54.226.244.180:1883  Client=esp32-kitchen
 *
 * Publish:
 *   home/kitchen/temperature    float °C
 *   home/kitchen/humidity       float %
 *   home/kitchen/gas_lpg        int ADC 0-4095 (MQ-2 analog)
 *   home/kitchen/gas_alert      ON/OFF (MQ-2 digital, LOW=alert)
 *   home/kitchen/ldr            int ADC 0-4095
 *   home/kitchen/light/state    ON/OFF
 *   home/kitchen/fan/state      ON/OFF
 *   home/kitchen/stove/state    ON/OFF
 *
 * Subscribe:
 *   home/kitchen/light/set      ON/OFF
 *   home/kitchen/fan/set        ON/OFF
 *   home/kitchen/stove/set      ON/OFF  (ถูก block อัตโนมัติถ้า gas alert active)
 *   home/broadcast/#
 *
 * Libraries: PubSubClient, DHT sensor library
 * ============================================================
 */

#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// ─── WiFi ─────────────────────────────────────────────────
const char* WIFI_SSID_1 = "Chawee";
const char* WIFI_PASS_1 = "YOUR_WIFI_PASSWORD";
const char* WIFI_SSID_2 = "SmartHome-Local";
const char* WIFI_PASS_2 = "YOUR_AP_PASSWORD";

// ─── MQTT ─────────────────────────────────────────────────
const char* MQTT_SERVER = "YOUR_EC2_IP";   // EC2 t3.small
const int   MQTT_PORT   = 1883;
const char* MQTT_CLIENT = "esp32-kitchen";

// ─── Pins ─────────────────────────────────────────────────
#define LED_PIN      2    // ไฟครัว (direct, no relay)
#define DHT_PIN      4
#define DHT_TYPE     DHT22
#define MQ2_DIGITAL  13   // D0: LOW = แก๊สเกิน threshold
#define RELAY_STOVE  25   // Active HIGH — จำลองเตาแก๊ส (HIGH=ON, LOW=OFF)
#define RELAY_FAN    23   // Active HIGH — พัดลมระบายอากาศ (HIGH=ON, LOW=OFF)
#define MQ2_ANALOG   34   // A0: ADC1 input-only
#define LDR_PIN      36   // ADC1 input-only + R 10kΩ ภายนอก (3.3V→R→GPIO36→LDR→GND)

// ─── MQTT Topics ──────────────────────────────────────────
#define T_TEMP         "home/kitchen/temperature"
#define T_HUM          "home/kitchen/humidity"
#define T_GAS          "home/kitchen/gas_lpg"
#define T_GAS_ALERT    "home/kitchen/gas_alert"
#define T_LDR          "home/kitchen/ldr"
#define T_LIGHT_STATE  "home/kitchen/light/state"
#define T_FAN_STATE    "home/kitchen/fan/state"
#define T_STOVE_STATE  "home/kitchen/stove/state"
#define T_LIGHT_SET    "home/kitchen/light/set"
#define T_FAN_SET      "home/kitchen/fan/set"
#define T_STOVE_SET    "home/kitchen/stove/set"
#define T_BROADCAST    "home/broadcast/#"

// ─── Timing ───────────────────────────────────────────────
const unsigned long SENSOR_MS  = 30000;  // DHT22 + MQ-2 analog
const unsigned long GAS_MS     =  2000;  // MQ-2 digital safety check ทุก 2s
const unsigned long LDR_MS     = 10000;
const unsigned long MQ_WARMUP  = 60000;
const unsigned long RECONNECT  =  5000;

// ─── State ────────────────────────────────────────────────
WiFiClient   wifiClient;
PubSubClient mqtt(wifiClient);
DHT          dht(DHT_PIN, DHT_TYPE);

unsigned long tSensor = 0, tGas = 0, tLDR = 0, tReconn = 0, bootTime = 0;
bool lightState   = false;
bool fanState     = false;
bool stoveState   = false;
bool mqReady      = false;
bool lastGasAlert = false;

// ══════════════════════════════════════════════════════════
void setup() {
  Serial.begin(115200);
  Serial.println("\n[BOOT] ESP32 #3 Kitchen");

  // ─── Force all outputs LOW ทันที (กัน GPIO float HIGH ตอน boot strapping = relay กระตุก ON) ───
  pinMode(LED_PIN,     OUTPUT); digitalWrite(LED_PIN,     LOW);   // OFF (direct LED)
  pinMode(RELAY_STOVE, OUTPUT); digitalWrite(RELAY_STOVE, LOW);   // OFF (Active HIGH relay)
  pinMode(RELAY_FAN,   OUTPUT); digitalWrite(RELAY_FAN,   LOW);   // OFF (Active HIGH relay)
  // Re-assert LOW อีกครั้งกัน noise ตอน boot
  delay(50);
  digitalWrite(RELAY_STOVE, LOW);
  digitalWrite(RELAY_FAN,   LOW);
  pinMode(MQ2_DIGITAL, INPUT);
  // GPIO34, 36 = input-only, ไม่ต้อง pinMode

  dht.begin();
  bootTime = millis();

  connectWiFi();
  mqtt.setServer(MQTT_SERVER, MQTT_PORT);
  mqtt.setCallback(mqttCallback);
  mqtt.setBufferSize(512);
  connectMQTT();

  Serial.println("[MQ-2] Warming up 60s...");
}

void loop() {
  if (!mqtt.connected()) {
    if (millis() - tReconn > RECONNECT) { tReconn = millis(); connectMQTT(); }
  } else {
    mqtt.loop();
  }

  if (!mqReady && millis() - bootTime > MQ_WARMUP) {
    mqReady = true;
    Serial.println("[MQ-2] Warm-up done ✓");
  }

  if (millis() - tGas    > GAS_MS)    { tGas    = millis(); checkGasAlert(); }  // safety poll
  if (millis() - tSensor > SENSOR_MS) { tSensor = millis(); publishSensors(); }
  if (millis() - tLDR    > LDR_MS)    { tLDR    = millis(); publishLDR(); }
}

// ─── WiFi ─────────────────────────────────────────────────
void connectWiFi() {
  WiFi.begin(WIFI_SSID_1, WIFI_PASS_1);
  Serial.print("[WiFi] " + String(WIFI_SSID_1));
  for (int i = 0; i < 20 && WiFi.status() != WL_CONNECTED; i++) { delay(500); Serial.print("."); }
  if (WiFi.status() == WL_CONNECTED) { Serial.println(" ✓ " + WiFi.localIP().toString()); return; }

  WiFi.begin(WIFI_SSID_2, WIFI_PASS_2);
  Serial.print("\n[WiFi] Fallback");
  for (int i = 0; i < 20 && WiFi.status() != WL_CONNECTED; i++) { delay(500); Serial.print("."); }
  if (WiFi.status() == WL_CONNECTED) { Serial.println(" ✓ " + WiFi.localIP().toString()); }
  else { Serial.println(" ✗ Restart"); ESP.restart(); }
}

// ─── MQTT ─────────────────────────────────────────────────
void connectMQTT() {
  Serial.print("[MQTT] Connecting...");
  if (mqtt.connect(MQTT_CLIENT)) {
    Serial.println(" ✓");
    mqtt.subscribe(T_LIGHT_SET);
    mqtt.subscribe(T_FAN_SET);
    mqtt.subscribe(T_STOVE_SET);
    mqtt.subscribe(T_BROADCAST);
    pubBool(T_LIGHT_STATE, lightState);
    pubBool(T_FAN_STATE,   fanState);
    pubBool(T_STOVE_STATE, stoveState);
  } else {
    Serial.println(" ✗ rc=" + String(mqtt.state()));
  }
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String t   = String(topic);
  String msg = "";
  for (unsigned int i = 0; i < length; i++) msg += (char)payload[i];
  msg.toUpperCase();
  Serial.println("[MQTT IN] " + t + " → " + msg);

  if      (t == T_LIGHT_SET) { setLight(msg == "ON"); }
  else if (t == T_FAN_SET)   { setFan(msg == "ON"); }
  else if (t == T_STOVE_SET) {
    if (msg == "ON" && lastGasAlert) {
      Serial.println("[STOVE] ⛔ Blocked — gas alert active!");
      pubBool(T_STOVE_STATE, false);  // ยืนยันว่า OFF
    } else {
      setStove(msg == "ON");
    }
  }
  else if (t.startsWith("home/broadcast/")) {
    String sub = t.substring(15);
    if (sub == "light/set") setLight(msg == "ON");
    if (sub == "fan/set")   setFan(msg == "ON");
    // ไม่รับ stove จาก broadcast — safety
  }
}

// ─── Gas Alert Safety (ตรวจทุก 2 วินาที) ─────────────────
void checkGasAlert() {
  if (!mqReady) {
    long rem = (MQ_WARMUP - (millis() - bootTime)) / 1000;
    Serial.printf("[MQ-2] Warming up... %lds remaining\n", rem > 0 ? rem : 0);
    return;
  }
  int  gasADC   = analogRead(MQ2_ANALOG);                  // 0-4095
  bool digitalAlert = (digitalRead(MQ2_DIGITAL) == LOW);   // D0 LOW = แก๊สเกิน

  // Interpret level
  const char* level;
  if      (gasADC < 600)  level = "🟢 SAFE";
  else if (gasADC < 1000) level = "🟡 WARN";
  else if (gasADC < 1800) level = "🟠 HIGH";
  else                    level = "🔴 DANGER";

  // Log ทุกรอบ (2s) — ดูค่า real-time
  Serial.printf("[MQ-2] ADC=%4d  D0=%s  level=%s\n",
                gasADC,
                digitalAlert ? "ALERT" : "ok   ",
                level);

  // ทำ action เมื่อ alert เปลี่ยน state เท่านั้น (กัน spam)
  if (digitalAlert == lastGasAlert) return;

  lastGasAlert = digitalAlert;
  mqtt.publish(T_GAS_ALERT, digitalAlert ? "ON" : "OFF", true);

  if (digitalAlert) {
    Serial.println("[MQ-2] ⚠️  GAS ALERT! Auto-safety: stove OFF + fan ON");
    setStove(false);  // ปิดเตาทันที (safety)
    setFan(true);     // เปิดพัดลมระบายอากาศ
  } else {
    Serial.println("[MQ-2] ✓ Gas cleared");
  }
}

// ─── Sensors (ทุก 30s) ────────────────────────────────────
void publishSensors() {
  // DHT22
  float temp = dht.readTemperature(), hum = dht.readHumidity();
  if (!isnan(temp) && !isnan(hum)) {
    char buf[10];
    dtostrf(temp, 4, 1, buf); mqtt.publish(T_TEMP, buf, true);
    dtostrf(hum,  4, 1, buf); mqtt.publish(T_HUM,  buf, true);
    Serial.printf("[DHT22] %.1f°C  %.1f%%\n", temp, hum);
  } else {
    Serial.println("[DHT22] ✗ Read failed");
  }

  // MQ-2 analog (หลัง warm-up)
  if (mqReady) {
    int  gas = analogRead(MQ2_ANALOG);
    char buf[10];
    itoa(gas, buf, 10);
    mqtt.publish(T_GAS, buf, true);
    Serial.printf("[MQ-2] ADC=%d  Alert=%s\n", gas, lastGasAlert ? "⚠ GAS!" : "OK");
  } else {
    long rem = (MQ_WARMUP - (millis() - bootTime)) / 1000;
    Serial.printf("[MQ-2] Warming up... %lds\n", rem > 0 ? rem : 0);
  }
}

void publishLDR() {
  int v = analogRead(LDR_PIN);
  char buf[10]; itoa(v, buf, 10);
  mqtt.publish(T_LDR, buf, true);
  Serial.printf("[LDR] ADC=%d (%s)\n", v, v > 2000 ? "มืด" : "สว่าง");
}

// ─── Actuators ────────────────────────────────────────────
void setLight(bool on) {
  lightState = on;
  digitalWrite(LED_PIN, on ? HIGH : LOW);
  pubBool(T_LIGHT_STATE, on);
  Serial.printf("[LED] ไฟครัว → %s\n", on ? "ON" : "OFF");
}

void setFan(bool on) {
  fanState = on;
  digitalWrite(RELAY_FAN, on ? HIGH : LOW);   // Active HIGH (HIGH=ON, LOW=OFF)
  pubBool(T_FAN_STATE, on);
  Serial.printf("[FAN] พัดลมครัว → %s\n", on ? "ON" : "OFF");
}

void setStove(bool on) {
  stoveState = on;
  digitalWrite(RELAY_STOVE, on ? HIGH : LOW); // Active HIGH (HIGH=ON, LOW=OFF)
  pubBool(T_STOVE_STATE, on);
  Serial.printf("[STOVE] เตา simulator → %s\n", on ? "ON 🔥" : "OFF");
}

// ─── Helpers ──────────────────────────────────────────────
void pubBool(const char* t, bool v) { mqtt.publish(t, v ? "ON" : "OFF", true); }

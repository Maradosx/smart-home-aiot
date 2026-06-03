/**
 * ============================================================
 * Smart Home AIoT — ESP32 #4 Bathroom + Garden
 * ============================================================
 *
 * Board   : ESP32 DevKit V1 (ESP32-WROOM-32)
 * Zone    : ห้องน้ำ + สวน (Bathroom + Garden)
 *
 * Wiring:
 *   GPIO2  → 220Ω → LED(+) → LED(−) → GND         ไฟห้องน้ำ (direct)
 *   GPIO16 ← LD2410C TX  (UART2 RX)                mmWave Presence + Fall
 *   GPIO17 → LD2410C RX  (UART2 TX)
 *   GPIO25 → 220Ω → LED1(+) → LED1(−) → GND        ไฟทางเดินสวน ดวงที่ 1 (direct)
 *          └→ 220Ω → LED2(+) → LED2(−) → GND        ไฟทางเดินสวน ดวงที่ 2 (parallel)
 *   GPIO26 → Relay ปั๊มน้ำ IN (Active LOW)          ปั๊มรดน้ำ
 *   GPIO27 → 220Ω → LED(+) → LED(−) → GND         ไฟสวน (direct)
 *   GPIO32 ← Soil Moisture AOUT (VCC=3.3V)          ความชื้นดิน (ADC1)
 *   GPIO36 ← LDR (bath):   3.3V→10kΩ→GPIO36→LDR→GND   แสงห้องน้ำ (VP, input-only)
 *   GPIO39 ← LDR (garden): 3.3V→10kΩ→GPIO39→LDR→GND   แสงสวน  (VN, input-only)
 *
 * Zigbee (ผ่าน Pi Zero 2W + Zigbee2MQTT):
 *   SNZB-05 Water Leak → Zigbee เท่านั้น (ไม่ต่อ ESP32)
 *
 * Soil ADC: ~3300=ดินแห้ง(0%)  ~1100=ดินชุ่ม(100%)
 * Auto pump: < 30% → ON,  > 70% → OFF
 *
 * WiFi    : Primary=Maradosx / Fallback=SmartHome-Local
 * MQTT    : 54.226.244.180:1883  Client=esp32-bathroom
 *
 * Publish:
 *   home/bathroom/presence       ON/OFF
 *   home/bathroom/fall_alert     ON/OFF
 *   home/bathroom/light/state    ON/OFF
 *   home/bathroom/ldr            int ADC 0-4095  (LDR ห้องน้ำ — GPIO36)
 *   home/garden/ldr              int ADC 0-4095  (LDR สวน — GPIO39)
 *   home/garden/soil_moisture    int % (0-100)
 *   home/garden/soil_raw         int ADC 0-4095
 *   home/garden/pump_state       ON/OFF
 *   home/garden/light/state      ON/OFF
 *   home/garden/pathway/state    ON/OFF
 *
 * Subscribe:
 *   home/bathroom/light/set      ON/OFF
 *   home/garden/pump/set         ON/OFF
 *   home/garden/light/set        ON/OFF
 *   home/garden/pathway/set      ON/OFF
 *   home/broadcast/#
 *
 * Libraries: PubSubClient
 * ============================================================
 */

#include <WiFi.h>
#include <PubSubClient.h>

// ─── WiFi ─────────────────────────────────────────────────
const char* WIFI_SSID_1 = "Chawee";
const char* WIFI_PASS_1 = "YOUR_WIFI_PASSWORD";
const char* WIFI_SSID_2 = "SmartHome-Local";
const char* WIFI_PASS_2 = "YOUR_AP_PASSWORD";

// ─── MQTT ─────────────────────────────────────────────────
const char* MQTT_SERVER = "YOUR_EC2_IP";   // EC2 t3.small
const int   MQTT_PORT   = 1883;
const char* MQTT_CLIENT = "esp32-bathroom";

// ─── Pins ─────────────────────────────────────────────────
#define LED_BATH     2    // ไฟห้องน้ำ (direct, no relay)
#define LD2410_RX    16
#define LD2410_TX    17
#define LED_PATHWAY  25   // ไฟทางเดินสวน (direct, no relay)
#define RELAY_PUMP   26   // Active LOW — ปั๊มน้ำ (relay module แบบ LOW trigger)
#define LED_GARDEN   27   // ไฟสวน (direct, no relay)
#define SOIL_PIN     32   // ADC1_CH4
#define LDR_BATH_PIN    36   // GPIO36 (VP) ADC1_CH0 input-only — แสงห้องน้ำ
#define LDR_GARDEN_PIN  39   // GPIO39 (VN) ADC1_CH3 input-only — แสงสวน

// ─── Soil Calibration ─────────────────────────────────────
const int SOIL_DRY   = 3300;
const int SOIL_WET   = 1100;
const int PUMP_ON    = 30;   // moisture % → เปิดปั๊ม
const int PUMP_OFF   = 70;   // moisture % → ปิดปั๊ม

// ─── MQTT Topics ──────────────────────────────────────────
#define T_PRESENCE      "home/bathroom/presence"
#define T_FALL          "home/bathroom/fall_alert"
#define T_BATH_LIGHT    "home/bathroom/light/state"
#define T_LDR_BATH      "home/bathroom/ldr"
#define T_LDR_GARDEN    "home/garden/ldr"
#define T_SOIL          "home/garden/soil_moisture"
#define T_SOIL_RAW      "home/garden/soil_raw"
#define T_PUMP_STATE    "home/garden/pump_state"
#define T_GARDEN_LIGHT  "home/garden/light/state"
#define T_PATHWAY_STATE "home/garden/pathway/state"

#define T_BATH_LIGHT_SET   "home/bathroom/light/set"
#define T_PUMP_SET         "home/garden/pump/set"
#define T_GARDEN_LIGHT_SET "home/garden/light/set"
#define T_PATHWAY_SET      "home/garden/pathway/set"
#define T_GARDEN_ALL_SET   "home/garden/all/set"     // ใหม่: master — toggle ทั้ง 2 ดวงพร้อมกัน
#define T_GARDEN_ALL_STATE "home/garden/all/state"   // ใหม่: state รวม
#define T_BROADCAST        "home/broadcast/#"

// ─── Timing ───────────────────────────────────────────────
const unsigned long SENSOR_MS  = 30000;
const unsigned long MMWAVE_MS  =   500;
const unsigned long LDR_MS     = 10000;
const unsigned long RECONNECT  =  5000;

// ─── State ────────────────────────────────────────────────
WiFiClient     wifiClient;
PubSubClient   mqtt(wifiClient);
HardwareSerial LD2410Serial(2);

unsigned long tSensor = 0, tMMWave = 0, tLDR = 0, tReconn = 0;
bool bathLight    = false;
bool pumpState    = false;
bool gardenLight  = false;
bool pathwayLight = false;
bool lastPresence = false;
bool lastFall     = false;

// ══════════════════════════════════════════════════════════
void setup() {
  Serial.begin(115200);
  Serial.println("\n[BOOT] ESP32 #4 Bathroom + Garden");

  pinMode(LED_BATH,    OUTPUT); digitalWrite(LED_BATH,    LOW);
  pinMode(LED_PATHWAY, OUTPUT); digitalWrite(LED_PATHWAY, LOW);
  pinMode(LED_GARDEN,  OUTPUT); digitalWrite(LED_GARDEN,  LOW);
  pinMode(RELAY_PUMP,  OUTPUT); digitalWrite(RELAY_PUMP,  LOW);  // OFF (Active LOW relay)

  LD2410Serial.begin(256000, SERIAL_8N1, LD2410_RX, LD2410_TX);
  Serial.println("[LD2410] UART2 256000 baud");

  connectWiFi();
  mqtt.setServer(MQTT_SERVER, MQTT_PORT);
  mqtt.setCallback(mqttCallback);
  mqtt.setBufferSize(512);
  connectMQTT();

  Serial.printf("[SOIL] dry=%d(0%%)  wet=%d(100%%)\n", SOIL_DRY, SOIL_WET);
}

void loop() {
  if (!mqtt.connected()) {
    if (millis() - tReconn > RECONNECT) { tReconn = millis(); connectMQTT(); }
  } else {
    mqtt.loop();
  }
  if (millis() - tSensor > SENSOR_MS) { tSensor = millis(); publishSoil(); }
  if (millis() - tMMWave > MMWAVE_MS) { tMMWave = millis(); checkMMWave(); }
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
    mqtt.subscribe(T_BATH_LIGHT_SET);
    mqtt.subscribe(T_PUMP_SET);
    mqtt.subscribe(T_GARDEN_LIGHT_SET);
    mqtt.subscribe(T_PATHWAY_SET);
    mqtt.subscribe(T_GARDEN_ALL_SET);                              // ใหม่
    mqtt.subscribe(T_BROADCAST);
    pubBool(T_BATH_LIGHT,    bathLight);
    pubBool(T_PUMP_STATE,    pumpState);
    pubBool(T_GARDEN_LIGHT,  gardenLight);
    pubBool(T_PATHWAY_STATE, pathwayLight);
    pubBool(T_GARDEN_ALL_STATE, gardenLight && pathwayLight);      // ใหม่ — initial combined state
    pubBool(T_PRESENCE,      false);   // initial state
    pubBool(T_FALL,          false);   // initial state
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

  if      (t == T_BATH_LIGHT_SET)   setBathLight(msg == "ON");
  else if (t == T_PUMP_SET)         setPump(msg == "ON", "MQTT");
  else if (t == T_GARDEN_LIGHT_SET) setGardenLight(msg == "ON");
  else if (t == T_PATHWAY_SET)      setPathway(msg == "ON");
  else if (t == T_GARDEN_ALL_SET)   setGardenAll(msg == "ON");     // ใหม่ — toggle 2 ดวงพร้อมกัน
  else if (t.startsWith("home/broadcast/")) {
    String sub = t.substring(15);
    if (sub == "bathroom/light/set")    setBathLight(msg == "ON");
    if (sub == "garden/light/set")      setGardenLight(msg == "ON");
    if (sub == "garden/pathway/set")    setPathway(msg == "ON");
    if (sub == "garden/all/set")        setGardenAll(msg == "ON"); // ใหม่ via broadcast
    if (sub == "garden/pump/set")        setPump(msg == "ON", "broadcast");
  }
}

// ─── Garden Lights: toggle ทั้ง 2 ดวงพร้อมกัน (real-time, no delay) ────
void setGardenAll(bool on) {
  // เขียน GPIO 2 ตัวติดกัน — ESP32 จะทำในไม่กี่ microsecond → ตามนุษย์เห็นเป็น real-time
  digitalWrite(LED_GARDEN,   on ? HIGH : LOW);    // ไฟสวน 2
  digitalWrite(LED_PATHWAY,  on ? HIGH : LOW);    // ไฟสวน 1 (pathway)

  // Update state variables + publish state ทีหลัง (ลำดับนี้ไม่กระทบ user — แสงเปลี่ยนพร้อมกันแล้ว)
  gardenLight  = on;
  pathwayLight = on;
  pubBool(T_GARDEN_LIGHT,     on);
  pubBool(T_PATHWAY_STATE,    on);
  pubBool(T_GARDEN_ALL_STATE, on);   // master state

  Serial.printf("[GARDEN] 🌳 ALL → %s (real-time)\n", on ? "ON" : "OFF");
}

// ─── Soil + Auto Pump ─────────────────────────────────────
void publishSoil() {
  long sum = 0;
  for (int i = 0; i < 10; i++) { sum += analogRead(SOIL_PIN); delay(10); }
  int rawADC   = sum / 10;
  int moisture = constrain(map(rawADC, SOIL_DRY, SOIL_WET, 0, 100), 0, 100);

  char buf[10];
  itoa(moisture, buf, 10); mqtt.publish(T_SOIL,     buf, true);
  itoa(rawADC,   buf, 10); mqtt.publish(T_SOIL_RAW, buf, true);
  Serial.printf("[SOIL] ADC=%d  Moisture=%d%%\n", rawADC, moisture);

  // Auto pump (HA จะตรวจ weather API ก่อนส่ง pump/set จาก HA side)
  if (!pumpState && moisture < PUMP_ON)  setPump(true,  "auto");
  if ( pumpState && moisture > PUMP_OFF) setPump(false, "auto");
}

// ─── mmWave LD2410C ───────────────────────────────────────
// Fall logic:
//   - คนนิ่ง (state==2 only) เกิน FALL_THRESHOLD_MS → ALERT
//   - ขยับปกติ (state==1 หรือ 3) → reset timer
//   - ไม่มีคน (state==0) → reset timer + clear alert
// LD2410C frame: F4 F3 F2 F1 ... F8 F7 F6 F5 (23 bytes)
const unsigned long FALL_THRESHOLD_MS = 5UL * 60UL * 1000UL;  // 5 นาที

uint8_t lastState = 0;
unsigned long stillStartTime = 0;  // เวลาที่เริ่มนิ่ง

void checkMMWave() {
  uint8_t state;
  if (!parseLD2410(state)) return;

  bool presence = (state != 0);
  bool fall = false;

  // ── Fall Detection: นิ่งนาน ──
  if (state == 2) {
    // เริ่มจับเวลานิ่ง (ครั้งแรก)
    if (stillStartTime == 0) {
      stillStartTime = millis();
      Serial.println("[LD2410] เริ่มนิ่ง — เริ่มจับเวลา fall");
    }
    // เกิน threshold = ล้ม
    if (millis() - stillStartTime > FALL_THRESHOLD_MS) {
      fall = true;
    }
  } else {
    // ขยับ (state 1, 3) หรือไม่มีคน (state 0) → reset timer
    if (stillStartTime != 0) {
      Serial.println("[LD2410] ขยับ/ออก — reset fall timer");
      stillStartTime = 0;
    }
  }

  // ── Publish presence change ──
  if (presence != lastPresence) {
    lastPresence = presence;
    pubBool(T_PRESENCE, presence);
    Serial.printf("[LD2410] Presence → %s (state=%d)\n",
                  presence ? "DETECTED" : "Clear", state);
  }

  // ── Publish fall change ──
  if (fall != lastFall) {
    lastFall = fall;
    pubBool(T_FALL, fall);
    if (fall) {
      unsigned long mins = (millis() - stillStartTime) / 60000;
      Serial.printf("[LD2410] ⚠️  FALL ALERT! นิ่ง %lu นาที\n", mins);
    } else {
      Serial.println("[LD2410] Fall cleared ✓");
    }
  }

  lastState = state;
}

bool parseLD2410(uint8_t &state) {
  static uint8_t buf[32];
  static int idx = 0;
  static unsigned long lastByte = 0;

  if (idx > 0 && millis() - lastByte > 200) idx = 0;

  while (LD2410Serial.available()) {
    uint8_t b = LD2410Serial.read();
    lastByte = millis();

    if (idx == 0 && b != 0xF4) continue;
    if (idx == 1 && b != 0xF3) { idx = 0; continue; }
    if (idx == 2 && b != 0xF2) { idx = 0; continue; }
    if (idx == 3 && b != 0xF1) { idx = 0; continue; }

    if (idx < (int)sizeof(buf)) buf[idx] = b;
    idx++;

    if (idx >= 23) {
      if (buf[19] == 0xF8 && buf[20] == 0xF7 && buf[21] == 0xF6 && buf[22] == 0xF5) {
        state = buf[8];
        idx = 0;
        return true;
      }
      idx = 0;
    }
  }
  return false;
}

void publishLDR() {
  // LDR ห้องน้ำ
  int vBath = analogRead(LDR_BATH_PIN);
  char b1[10]; itoa(vBath, b1, 10);
  mqtt.publish(T_LDR_BATH, b1, true);
  Serial.printf("[LDR-BATH]   ADC=%d (%s)\n", vBath, vBath > 2000 ? "มืด" : "สว่าง");

  // LDR สวน
  int vGarden = analogRead(LDR_GARDEN_PIN);
  char b2[10]; itoa(vGarden, b2, 10);
  mqtt.publish(T_LDR_GARDEN, b2, true);
  Serial.printf("[LDR-GARDEN] ADC=%d (%s)\n", vGarden, vGarden > 2000 ? "มืด" : "สว่าง");
}

// ─── Actuators ────────────────────────────────────────────
void setBathLight(bool on) {
  bathLight = on;
  digitalWrite(LED_BATH, on ? HIGH : LOW);
  pubBool(T_BATH_LIGHT, on);
  Serial.printf("[LED] ไฟห้องน้ำ → %s\n", on ? "ON" : "OFF");
}

void setPump(bool on, const char* src) {
  pumpState = on;
  digitalWrite(RELAY_PUMP, on ? HIGH : LOW);   // Active LOW relay
  pubBool(T_PUMP_STATE, on);
  Serial.printf("[PUMP] → %s  (src=%s)\n", on ? "ON 💧" : "OFF", src);
}

void setGardenLight(bool on) {
  gardenLight = on;
  digitalWrite(LED_GARDEN, on ? HIGH : LOW);
  pubBool(T_GARDEN_LIGHT, on);
  Serial.printf("[LED] ไฟสวน → %s\n", on ? "ON" : "OFF");
}

void setPathway(bool on) {
  pathwayLight = on;
  digitalWrite(LED_PATHWAY, on ? HIGH : LOW);
  pubBool(T_PATHWAY_STATE, on);
  Serial.printf("[LED] ไฟทางเดิน → %s\n", on ? "ON" : "OFF");
}

// ─── Helpers ──────────────────────────────────────────────
void pubBool(const char* t, bool v) { mqtt.publish(t, v ? "ON" : "OFF", true); }

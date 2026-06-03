/**
 * ============================================================
 * Smart Home AIoT — ESP32 #5 Garage
 * ============================================================
 *
 * Board   : ESP32 DevKit V1 (ESP32-WROOM-32)
 * Zone    : โรงรถ (Garage)
 *
 * Wiring:
 *   GPIO2  → 220Ω → LED(+) → LED(−) → GND         ไฟโรงรถ (direct)
 *   GPIO13 ← MQ-2 D0                               แก๊ส/ควัน (digital)
 *   GPIO25 → 220Ω → LED(+) → LED(−) → GND         ไฟทางเดิน (direct)
 *   GPIO26 → Servo SG90 Signal (5V VCC, GND shared) ประตูโรงรถ
 *   GPIO27 ← PIR HC-SR501 OUT                      ตรวจจับการเคลื่อนไหว (HIGH=motion)
 *   GPIO34 ← MQ-2 A0                               แก๊ส/ควัน (analog, input-only)
 *   GPIO36 ← LDR: 3.3V→10kΩ→GPIO36→LDR→GND        แสงสว่าง (input-only)
 *
 * Servo SG90 ประตูโรงรถ:
 *   OPEN   = 180° (ปรับได้ด้วย SERVO_OPEN)
 *   CLOSED =   0° (ปรับได้ด้วย SERVO_CLOSED)
 *   คุมผ่านองศาล้วนๆ ไม่มี Limit Switch
 *
 * MQ-2: D0 = LOW เมื่อแก๊สเกิน threshold
 * MQ warm-up: 60 วินาทีหลัง boot
 * Auto-safety: gas alert → เปิดประตูระบายอากาศอัตโนมัติ
 *
 * WiFi    : Primary=Maradosx / Fallback=SmartHome-Local
 * MQTT    : 54.226.244.180:1883  Client=esp32-garage
 *
 * Publish:
 *   home/garage/smoke_lpg        int ADC 0-4095 (MQ-2 analog)
 *   home/garage/smoke_alert      ON/OFF (MQ-2 digital, LOW=alert)
 *   home/garage/motion           ON/OFF (PIR)
 *   home/garage/door/state       OPEN/CLOSED/STOPPED
 *   home/garage/light/state      ON/OFF
 *   home/garage/pathway/state    ON/OFF
 *   home/garage/ldr              int ADC 0-4095
 *
 * Subscribe:
 *   home/garage/door/set         OPEN/CLOSE/STOP
 *   home/garage/light/set        ON/OFF
 *   home/garage/pathway/set      ON/OFF
 *   home/broadcast/#
 *
 * Libraries: PubSubClient, ESP32Servo
 * ============================================================
 */

#include <WiFi.h>
#include <PubSubClient.h>
#include <ESP32Servo.h>

// ─── WiFi ─────────────────────────────────────────────────
const char* WIFI_SSID_1 = "Chawee";
const char* WIFI_PASS_1 = "YOUR_WIFI_PASSWORD";
const char* WIFI_SSID_2 = "SmartHome-Local";
const char* WIFI_PASS_2 = "YOUR_AP_PASSWORD";

// ─── MQTT ─────────────────────────────────────────────────
const char* MQTT_SERVER = "YOUR_EC2_IP";   // EC2 t3.small
const int   MQTT_PORT   = 1883;
const char* MQTT_CLIENT = "esp32-garage";

// ─── Pins ─────────────────────────────────────────────────
#define LED_GARAGE   2    // ไฟโรงรถ (direct)
#define MQ2_DIGITAL  13   // D0: LOW = แก๊สเกิน threshold
#define LED_PATHWAY  25   // ไฟทางเดิน (direct)
#define SERVO_PIN    26   // Servo SG90 Signal
#define PIR_PIN      23   // PIR OUT: HIGH = มีการเคลื่อนไหว
#define MQ2_ANALOG   34   // A0: ADC1 input-only
#define LDR_PIN      36   // ADC1 input-only

// ─── Servo 180° standard — control by ANGLE (เหมือนประตูหน้าบ้าน) ───
// 0° = ปิด · 90° = เปิด · step-by-step → เปิดปิดช้านุ่ม
const int DOOR_CLOSED_ANGLE = 0;     // ปิด
const int DOOR_OPEN_ANGLE   = 90;    // เปิด
const int SERVO_MIN_ANGLE   = 0;     // safety guard
const int SERVO_MAX_ANGLE   = 90;    // safety guard
const unsigned long DOOR_STEP_MS = 40;  // step 1° ทุก 40ms = 90° ใช้ ~3.6 วินาที (ช้านุ่ม)

// ─── MQTT Topics ──────────────────────────────────────────
#define T_SMOKE          "home/garage/smoke_lpg"
#define T_SMOKE_ALERT    "home/garage/smoke_alert"
#define T_DOOR_STATE     "home/garage/door/state"
#define T_MOTION         "home/garage/motion"
#define T_LIGHT_STATE    "home/garage/light/state"
#define T_PATHWAY_STATE  "home/garage/pathway/state"
#define T_LDR            "home/garage/ldr"
#define T_DOOR_SET       "home/garage/door/set"
#define T_LIGHT_SET      "home/garage/light/set"
#define T_PATHWAY_SET    "home/garage/pathway/set"
#define T_ALL_LIGHT_SET   "home/garage/all/set"      // ใหม่: master toggle 2 ดวงพร้อมกัน
#define T_ALL_LIGHT_STATE "home/garage/all/state"    // ใหม่: state รวม
#define T_BROADCAST      "home/broadcast/#"

// ─── Timing ───────────────────────────────────────────────
const unsigned long SENSOR_MS = 30000;
const unsigned long PIR_MS    =   500;  // ตรวจ PIR ทุก 500ms
const unsigned long LDR_MS    = 10000;
const unsigned long RECONNECT =  5000;
const unsigned long MQ_WARMUP = 60000;

// ─── State ────────────────────────────────────────────────
WiFiClient   wifiClient;
PubSubClient mqtt(wifiClient);
Servo        garageDoor;

enum DoorState { DOOR_OPEN, DOOR_CLOSED, DOOR_OPENING, DOOR_CLOSING, DOOR_STOPPED };
DoorState doorState = DOOR_CLOSED;

unsigned long tSensor = 0, tPIR = 0, tLDR = 0, tReconn = 0, bootTime = 0;
unsigned long tDoorStep = 0;            // เวลา step ล่าสุด
int currentDoorAngle = DOOR_CLOSED_ANGLE;  // องศาปัจจุบัน
int targetDoorAngle  = DOOR_CLOSED_ANGLE;  // องศาเป้าหมาย
bool doorMoving = false;
bool garageLight  = false;
bool pathwayLight = false;
bool mqReady      = false;
bool lastGasAlert = false;
bool lastMotion   = false;

// ══════════════════════════════════════════════════════════
void setup() {
  Serial.begin(115200);
  Serial.println("\n[BOOT] ESP32 #5 Garage");

  pinMode(LED_GARAGE,  OUTPUT); digitalWrite(LED_GARAGE,  LOW);
  pinMode(LED_PATHWAY, OUTPUT); digitalWrite(LED_PATHWAY, LOW);
  pinMode(MQ2_DIGITAL, INPUT);
  pinMode(PIR_PIN,     INPUT);
  // GPIO34, 36 = input-only

  ESP32PWM::allocateTimer(0);
  garageDoor.setPeriodHertz(50);
  // Attach + ตั้งให้ปิด (0°) ตอน boot
  garageDoor.attach(SERVO_PIN, 500, 2400);
  garageDoor.write(DOOR_CLOSED_ANGLE);
  Serial.printf("[SERVO] 180° angle-based — closed=%d°  open=%d°  step=%lums (full travel ~%.1fs)\n",
                DOOR_CLOSED_ANGLE, DOOR_OPEN_ANGLE, DOOR_STEP_MS,
                (DOOR_OPEN_ANGLE - DOOR_CLOSED_ANGLE) * DOOR_STEP_MS / 1000.0);

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

  // ─── Door servo: step 1° ทุก DOOR_STEP_MS ms (เปิดปิดนุ่ม) ───
  if (doorMoving && millis() - tDoorStep > DOOR_STEP_MS) {
    tDoorStep = millis();

    if      (currentDoorAngle < targetDoorAngle) currentDoorAngle++;
    else if (currentDoorAngle > targetDoorAngle) currentDoorAngle--;

    currentDoorAngle = constrain(currentDoorAngle, SERVO_MIN_ANGLE, SERVO_MAX_ANGLE);
    garageDoor.write(currentDoorAngle);

    // ถึงเป้าหมายแล้ว
    if (currentDoorAngle == targetDoorAngle) {
      doorMoving = false;
      doorState  = (targetDoorAngle == DOOR_OPEN_ANGLE) ? DOOR_OPEN : DOOR_CLOSED;
      pubDoorState();
      Serial.printf("[SERVO] ✓ %s ที่ %d° (servo holding position)\n",
                    doorState == DOOR_OPEN ? "OPEN 🔓" : "CLOSED 🔒", currentDoorAngle);
    }
  }

  // ─── Periodic refresh: re-write angle ทุก 5 วินาที (กัน servo drift) ──
  static unsigned long tHoldRefresh = 0;
  if (!doorMoving && millis() - tHoldRefresh > 5000) {
    tHoldRefresh = millis();
    garageDoor.write(currentDoorAngle);
  }

  if (millis() - tPIR    > PIR_MS)    { tPIR    = millis(); checkPIR(); }
  if (millis() - tSensor > SENSOR_MS) { tSensor = millis(); publishGas(); }
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
    mqtt.subscribe(T_DOOR_SET);
    mqtt.subscribe(T_LIGHT_SET);
    mqtt.subscribe(T_PATHWAY_SET);
    mqtt.subscribe(T_ALL_LIGHT_SET);          // ใหม่
    mqtt.subscribe(T_BROADCAST);
    pubDoorState();
    pubBool(T_LIGHT_STATE,     garageLight);
    pubBool(T_PATHWAY_STATE,   pathwayLight);
    pubBool(T_ALL_LIGHT_STATE, garageLight && pathwayLight);  // ใหม่
    pubBool(T_MOTION,          false);
    pubBool(T_SMOKE_ALERT,     false);
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

  if      (t == T_DOOR_SET)        handleDoorCommand(msg);
  else if (t == T_LIGHT_SET)       setGarageLight(msg == "ON");
  else if (t == T_PATHWAY_SET)     setPathway(msg == "ON");
  else if (t == T_ALL_LIGHT_SET)   setGarageAll(msg == "ON");      // ใหม่ — toggle 2 ดวงพร้อมกัน
  else if (t.startsWith("home/broadcast/")) {
    String sub = t.substring(15);
    if (sub == "garage/door/set")    handleDoorCommand(msg);
    if (sub == "garage/light/set")   setGarageLight(msg == "ON");
    if (sub == "garage/pathway/set") setPathway(msg == "ON");
    if (sub == "garage/all/set")     setGarageAll(msg == "ON");    // ใหม่ via broadcast
  }
}

// ─── Door / Servo ─────────────────────────────────────────
void handleDoorCommand(const String& cmd) {
  if      (cmd == "OPEN"  || cmd == "ON")  openDoor();
  else if (cmd == "CLOSE" || cmd == "OFF") closeDoor();
  else if (cmd == "STOP")                  stopDoor();
}

void openDoor() {
  if (doorState == DOOR_OPEN && !doorMoving) {
    Serial.println("[DOOR] Already OPEN");
    pubDoorState();
    return;
  }
  targetDoorAngle = DOOR_OPEN_ANGLE;
  doorState  = DOOR_OPENING;
  doorMoving = true;
  tDoorStep  = millis();
  if (!garageDoor.attached()) garageDoor.attach(SERVO_PIN, 500, 2400);
  pubDoorState();
  Serial.printf("[SERVO] 🔓 OPENING: %d° → %d° (step 1°/%lums)\n",
                currentDoorAngle, targetDoorAngle, DOOR_STEP_MS);
}

void closeDoor() {
  if (doorState == DOOR_CLOSED && !doorMoving) {
    Serial.println("[DOOR] Already CLOSED");
    pubDoorState();
    return;
  }
  targetDoorAngle = DOOR_CLOSED_ANGLE;
  doorState  = DOOR_CLOSING;
  doorMoving = true;
  tDoorStep  = millis();
  if (!garageDoor.attached()) garageDoor.attach(SERVO_PIN, 500, 2400);
  pubDoorState();
  Serial.printf("[SERVO] 🔒 CLOSING: %d° → %d° (step 1°/%lums)\n",
                currentDoorAngle, targetDoorAngle, DOOR_STEP_MS);
}

void stopDoor() {
  // หยุดทันทีตรงนี้ — servo ค้างองศาปัจจุบัน
  targetDoorAngle = currentDoorAngle;
  doorMoving = false;
  doorState  = DOOR_STOPPED;
  pubDoorState();
  Serial.printf("[SERVO] STOP ที่ %d° (servo holding)\n", currentDoorAngle);
}

// ─── Garage Lights ALL: toggle 2 ดวงพร้อมกัน real-time ──────
void setGarageAll(bool on) {
  // เขียน GPIO 2 ตัวติดกัน — ห่างกัน <1µs = ตามองเห็นพร้อมกัน
  digitalWrite(LED_GARAGE,  on ? HIGH : LOW);   // ไฟโรงรถ
  digitalWrite(LED_PATHWAY, on ? HIGH : LOW);   // ไฟทางเดิน

  garageLight  = on;
  pathwayLight = on;
  pubBool(T_LIGHT_STATE,     on);
  pubBool(T_PATHWAY_STATE,   on);
  pubBool(T_ALL_LIGHT_STATE, on);

  Serial.printf("[GARAGE] 💡 ALL → %s (real-time)\n", on ? "ON" : "OFF");
}

// ─── PIR Motion (Smart Edge Detection) ────────────────────
// Logic:
//   1. ตรวจ "เริ่มขยับ" (rising edge LOW→HIGH) → reset timer
//   2. ถ้าไม่มี rising edge ใหม่ใน MOTION_TIMEOUT → Clear
//   3. ป้องกัน PIR ค้าง HIGH (stuck): auto-clear แม้ GPIO ยัง HIGH
//   4. Debounce: ต้อง stable เกิน DEBOUNCE_MS ก่อน count เป็น edge
const unsigned long MOTION_TIMEOUT  = 30000;  // 30 วิ ไม่มี edge → Clear
const unsigned long DEBOUNCE_MS     =   200;  // กัน noise / spike

bool lastRawState = false;
unsigned long lastEdgeTime  = 0;   // เวลาที่เกิด rising edge ล่าสุด
unsigned long lastChangeTime = 0;
bool motionReported = false;

void checkPIR() {
  bool rawState = (digitalRead(PIR_PIN) == HIGH);
  unsigned long now = millis();

  // Track raw state changes (debounce)
  if (rawState != lastRawState) {
    if (now - lastChangeTime > DEBOUNCE_MS) {
      // Rising edge: LOW → HIGH = "เริ่มขยับ"
      if (rawState && !lastRawState) {
        lastEdgeTime = now;
        Serial.printf("[PIR] ⚡ Rising edge — เริ่มขยับ\n");
      }
      lastRawState = rawState;
      lastChangeTime = now;
    }
  }

  // ── Decide motion state ──
  bool motion;
  if (lastEdgeTime == 0) {
    // ยังไม่เคยมี edge ตั้งแต่ boot
    motion = false;
  } else if (now - lastEdgeTime < MOTION_TIMEOUT) {
    // มี edge ภายใน 30 วิ = ยังถือว่ามีคน
    motion = true;
  } else {
    // นานเกินไป = clear (auto-clear แม้ PIR ค้าง HIGH)
    motion = false;
  }

  // Publish เมื่อสถานะเปลี่ยน
  if (motion != motionReported) {
    motionReported = motion;
    pubBool(T_MOTION, motion);
    Serial.printf("[PIR] 🔔 สถานะ → %s\n",
                  motion ? "DETECTED 🚨" : "Clear ✓");
  }

  // Log ทุก 10 วิ
  static unsigned long lastLog = 0;
  if (now - lastLog > 10000) {
    lastLog = now;
    if (lastEdgeTime > 0) {
      unsigned long sinceEdge = (now - lastEdgeTime) / 1000;
      Serial.printf("[PIR] %s  raw=%s  last edge: %lus ago\n",
                    motion ? "🚨 DETECTED" : "✓ Clear",
                    rawState ? "HIGH" : "LOW",
                    sinceEdge);
    } else {
      Serial.printf("[PIR] ✓ Clear  raw=%s  (no motion ever)\n",
                    rawState ? "HIGH" : "LOW");
    }
  }
}

// ─── MQ-2 Gas + Auto Safety ───────────────────────────────
void publishGas() {
  if (!mqReady) {
    long rem = (MQ_WARMUP - (millis() - bootTime)) / 1000;
    Serial.printf("[MQ-2] Warming up... %lds\n", rem > 0 ? rem : 0);
    return;
  }
  int  gas   = analogRead(MQ2_ANALOG);
  bool alert = (digitalRead(MQ2_DIGITAL) == LOW);
  char buf[10];
  itoa(gas, buf, 10);
  mqtt.publish(T_SMOKE,       buf, true);
  mqtt.publish(T_SMOKE_ALERT, alert ? "ON" : "OFF", true);
  Serial.printf("[MQ-2] ADC=%d  Alert=%s\n", gas, alert ? "⚠ SMOKE/GAS!" : "OK");

  if (alert != lastGasAlert) {
    lastGasAlert = alert;
    if (alert && doorState == DOOR_CLOSED) {
      Serial.println("[MQ-2] ⚠️  Gas alert → Auto open door!");
      openDoor();
    }
  }
}

// ─── LDR ──────────────────────────────────────────────────
void publishLDR() {
  int v = analogRead(LDR_PIN);
  char buf[10]; itoa(v, buf, 10);
  mqtt.publish(T_LDR, buf, true);
  Serial.printf("[LDR] ADC=%d (%s)\n", v, v > 2000 ? "มืด" : "สว่าง");
}

// ─── Actuators ────────────────────────────────────────────
void setGarageLight(bool on) {
  garageLight = on;
  digitalWrite(LED_GARAGE, on ? HIGH : LOW);
  pubBool(T_LIGHT_STATE, on);
  Serial.printf("[LED] ไฟโรงรถ → %s\n", on ? "ON" : "OFF");
}

void setPathway(bool on) {
  pathwayLight = on;
  digitalWrite(LED_PATHWAY, on ? HIGH : LOW);
  pubBool(T_PATHWAY_STATE, on);
  Serial.printf("[LED] ไฟทางเดิน → %s\n", on ? "ON" : "OFF");
}

// ─── Helpers ──────────────────────────────────────────────
void pubBool(const char* t, bool v) { mqtt.publish(t, v ? "ON" : "OFF", true); }

void pubDoorState() {
  const char* s = (doorState == DOOR_OPEN)    ? "OPEN"    :
                  (doorState == DOOR_CLOSED)  ? "CLOSED"  :
                  (doorState == DOOR_OPENING) ? "OPENING" :
                  (doorState == DOOR_CLOSING) ? "CLOSING" : "STOPPED";
  mqtt.publish(T_DOOR_STATE, s, true);
}


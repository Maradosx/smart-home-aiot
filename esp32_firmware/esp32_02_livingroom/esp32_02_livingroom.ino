/**
 * ============================================================
 * Smart Home AIoT — ESP32 #2 Living Room (WITH DOOR LOCK MOTOR)
 * ============================================================
 *
 * Board   : ESP32 DevKit V1 (ESP32-WROOM-32)
 * Zone    : ห้องนั่งเล่น (Living Room) — Sensors + Door Lock
 *
 * Wiring:
 *   GPIO4  → DHT22 DATA (10kΩ pull-up → 3.3V)      อุณหภูมิ/ความชื้น
 *   GPIO27 ← MQ-135 D0                              คุณภาพอากาศ (digital)
 *   GPIO35 ← MQ-135 A0                              คุณภาพอากาศ (analog, input-only)
 *   GPIO36 ← LDR: 3.3V→1kΩ→GPIO36→LDR→GND         แสงสว่าง
 *   GPIO13 → SG90 SIGNAL (สายเหลือง/ส้ม)            *** ใหม่: Door Lock Motor ***
 *
 * SG90 wiring (จ่ายไฟแยก แนะนำ):
 *   SG90 VCC (สายแดง) → 5V (USB / external 5V supply, ไม่ใช่ 3V3 ของ ESP32)
 *   SG90 GND (สายน้ำตาล) → GND ร่วมกับ ESP32
 *   SG90 SIG (สายเหลือง) → GPIO13
 *   ※ ถ้า servo สั่น/รีบูตบอร์ด ให้คั่น cap 470µF ระหว่าง 5V↔GND ใกล้ servo
 *
 * ── [TEST MODE] LED D2 แทน Zigbee ──────────────────────────────
 *   GPIO2  → 220Ω → LED(+) → LED(−) → GND   (ทดสอบแทน IKEA bulb)
 *
 * Zigbee (ผ่าน Pi + Z2M, ไม่ต่อ ESP32 โดยตรง):
 *   IKEA TRÅDFRI RGBW E27  → ไฟห้องนั่งเล่น (Zigbee)
 *   Sonoff S26R2ZB #1      → พัดลม (Zigbee Plug)
 *   Sonoff S26R2ZB #2      → เครื่องฟอกอากาศ (Zigbee Plug)
 *   Sonoff SNZB-03P        → PIR Motion Sensor (Zigbee)
 *   Sonoff SNZB-04         → Front Door Sensor (Zigbee) → คู่กับ servo lock นี้
 *
 * WiFi    : Primary=Maradosx / Fallback=SmartHome-Local
 * MQTT    : YOUR_EC2_IP:1883  Client=esp32-livingroom
 *
 * Publish:
 *   home/livingroom/temperature    float °C
 *   home/livingroom/humidity       float %
 *   home/livingroom/air_quality    int ADC 0-4095 (MQ-135 analog)
 *   home/livingroom/air_alert      ON/OFF (MQ-135 digital, LOW=แย่)
 *   home/livingroom/ldr            int ADC 0-4095
 *   home/livingroom/light/state    ON/OFF  (LED test)
 *   home/livingroom/door/state     LOCKED / UNLOCKED   *** ใหม่ ***
 *
 * Subscribe:
 *   home/livingroom/light/set      ON / OFF
 *   home/livingroom/door/set       LOCK / UNLOCK       *** ใหม่ ***
 *   home/broadcast/#               (broadcast bus)
 *
 * Door Lock behavior (SG90 — ใช้แค่ 90° ของ travel range):
 *   UNLOCK → angle 0°    (กดปิด/ปลดล็อค: หมุนกลับมาที่ 0°)
 *   LOCK   → angle 90°   (กดเปิด/ล็อค: หมุนไป 90° จากจุดเริ่ม)
 *   ※ servo เป็น SG90 180° แต่เราจำกัดใช้แค่ 0°–90° → travel รวม = 90°
 *   หลัง write servo เสร็จ จะ detach() เพื่อกัน servo สั่น/กินไฟ
 *   (servo จะค้างองศาเดิมไว้ด้วย gear แต่ไม่ฝืน — เหมาะกับ door lock)
 *
 * MQ-135 warm-up: 60 วินาทีหลัง boot
 * Libraries: PubSubClient, DHT sensor library, ESP32Servo
 * ============================================================
 */

#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <ESP32Servo.h>

// ─── WiFi ─────────────────────────────────────────────────
const char* WIFI_SSID_1 = "Chawee";
const char* WIFI_PASS_1 = "YOUR_WIFI_PASSWORD";
const char* WIFI_SSID_2 = "SmartHome-Local";
const char* WIFI_PASS_2 = "YOUR_AP_PASSWORD";

// ─── MQTT ─────────────────────────────────────────────────
const char* MQTT_SERVER = "YOUR_EC2_IP";   // EC2 t3.small
const int   MQTT_PORT   = 1883;
const char* MQTT_CLIENT = "esp32-livingroom";

// ─── Pins ─────────────────────────────────────────────────
#define LED_PIN        2    // [TEST] LED แทน IKEA Zigbee bulb
#define DHT_PIN        4
#define DHT_TYPE       DHT22
#define MQ135_DIGITAL  27   // D0: LOW = คุณภาพแย่
#define MQ135_ANALOG   35   // A0: ADC1 input-only
#define LDR_PIN        36   // GPIO36 input-only + R 1kΩ ภายนอก
#define DOOR_SERVO_PIN 13   // *** ใหม่: SG90 SIG ***

// ─── Door Lock travel range (ใช้แค่ 90° ไม่ใช่ full 180°) ─
// servo เริ่มที่ 0° (UNLOCK) → กดล็อค หมุน +90° → 90° (LOCK)
// กดปลดล็อค หมุนสวน -90° → 0° กลับ
const int DOOR_UNLOCK_ANGLE = 0;    // จุดเริ่ม / ปลดล็อค (สลักถอย)
const int DOOR_LOCK_ANGLE   = 90;   // หมุนไปอีก 90° / ล็อค (สลักดัน)
const int SERVO_MIN_ANGLE   = 0;    // safety: ไม่ให้เกิน range
const int SERVO_MAX_ANGLE   = 90;   // safety: ไม่ให้เกิน 90°
const unsigned long SERVO_HOLD_MS = 500;  // เวลาให้ servo เดินไปถึงองศา (0→90° ใช้ ~300-400ms)
                                          // หลังเวลานี้ servo ยังค้างองศา (ไม่ detach) เพื่อ hold position

// ─── MQTT Topics ──────────────────────────────────────────
#define T_TEMP         "home/livingroom/temperature"
#define T_HUM          "home/livingroom/humidity"
#define T_AIR          "home/livingroom/air_quality"
#define T_AIR_ALERT    "home/livingroom/air_alert"
#define T_LDR          "home/livingroom/ldr"
#define T_LIGHT_STATE  "home/livingroom/light/state"
#define T_LIGHT_SET    "home/livingroom/light/set"
#define T_DOOR_STATE   "home/livingroom/door/state"   // *** ใหม่ ***
#define T_DOOR_SET     "home/livingroom/door/set"     // *** ใหม่ ***
#define T_BROADCAST    "home/broadcast/#"

// ─── Timing ───────────────────────────────────────────────
const unsigned long SENSOR_MS  = 30000;
const unsigned long LDR_MS     = 10000;
const unsigned long MQ_WARMUP  = 60000;
const unsigned long RECONNECT  =  5000;

// ─── State ────────────────────────────────────────────────
WiFiClient   wifiClient;
PubSubClient mqtt(wifiClient);
DHT          dht(DHT_PIN, DHT_TYPE);
Servo        doorServo;

unsigned long tSensor = 0, tLDR = 0, tReconn = 0, bootTime = 0;
unsigned long tServoMove = 0;
bool servoMoving  = false;
bool mqReady      = false;
bool lightState   = false;
bool doorLocked   = true;     // boot state: ล็อคไว้เป็น default ปลอดภัย

// ─── Forward decls ────────────────────────────────────────
void pubBool(const char* t, bool v);
void pubStr (const char* t, const char* v);
void setLight(bool on);
void setDoorLock(bool lock);

// ══════════════════════════════════════════════════════════
void setup() {
  Serial.begin(115200);
  Serial.println("\n[BOOT] ESP32 #2 Living Room (with Door Lock)");

  pinMode(LED_PIN,       OUTPUT); digitalWrite(LED_PIN, LOW);
  pinMode(MQ135_DIGITAL, INPUT);
  pinMode(LDR_PIN,       INPUT);

  dht.begin();
  bootTime = millis();

  // ── SG90 servo timer allocation ──
  ESP32PWM::allocateTimer(0);
  Serial.printf("[SERVO] SG90 Door Lock on GPIO%d  travel=%d°  UNLOCK=%d°  LOCK=%d°\n",
                DOOR_SERVO_PIN,
                DOOR_LOCK_ANGLE - DOOR_UNLOCK_ANGLE,
                DOOR_UNLOCK_ANGLE, DOOR_LOCK_ANGLE);

  // boot ในสภาพล็อคไว้ (ไม่ขยับ servo จริงตอน boot กัน glitch
  //  — รอคำสั่งจาก HA หรือผู้ใช้ก่อนค่อย move)

  connectWiFi();
  mqtt.setServer(MQTT_SERVER, MQTT_PORT);
  mqtt.setCallback(mqttCallback);
  mqtt.setBufferSize(512);
  connectMQTT();

  Serial.println("[MQ-135] Warming up 60s...");
}

void loop() {
  if (!mqtt.connected()) {
    if (millis() - tReconn > RECONNECT) { tReconn = millis(); connectMQTT(); }
  } else {
    mqtt.loop();
  }

  // ── servo motion complete → KEEP ATTACHED to hold position (กันสปริงผลักกลับ) ──
  // ไม่ detach แล้ว — ปล่อย PWM ต่อเพื่อให้ servo ค้างองศาแน่นไม่ถอย
  if (servoMoving && millis() - tServoMove > SERVO_HOLD_MS) {
    // re-write target ครั้งสุดท้าย เพื่อ confirm position lock
    doorServo.write(doorLocked ? DOOR_LOCK_ANGLE : DOOR_UNLOCK_ANGLE);
    servoMoving = false;
    pubStr(T_DOOR_STATE, doorLocked ? "LOCKED" : "UNLOCKED");
    Serial.printf("[DOOR] ✓ %s (servo holding position, no detach)\n",
                  doorLocked ? "🔒 LOCKED" : "🔓 UNLOCKED");
  }

  // ── Periodic position refresh (ทุก 5 วินาที — กัน servo drift จาก noise) ──
  static unsigned long tHoldRefresh = 0;
  if (doorServo.attached() && !servoMoving && millis() - tHoldRefresh > 5000) {
    tHoldRefresh = millis();
    doorServo.write(doorLocked ? DOOR_LOCK_ANGLE : DOOR_UNLOCK_ANGLE);
  }

  if (!mqReady && millis() - bootTime > MQ_WARMUP) {
    mqReady = true;
    Serial.println("[MQ-135] Warm-up done ✓");
  }

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
    mqtt.subscribe(T_DOOR_SET);     // *** ใหม่ ***
    mqtt.subscribe(T_BROADCAST);
    pubBool(T_LIGHT_STATE, lightState);
    pubStr (T_DOOR_STATE,  doorLocked ? "LOCKED" : "UNLOCKED");
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

  if (t == T_LIGHT_SET) {
    setLight(msg == "ON");
  } else if (t == T_DOOR_SET) {                         // *** ใหม่ ***
    if (msg == "LOCK")   setDoorLock(true);
    if (msg == "UNLOCK") setDoorLock(false);
  } else if (t.startsWith("home/broadcast/")) {
    String sub = t.substring(15);
    if (sub == "livingroom/light/set") setLight(msg == "ON");
    if (sub == "livingroom/door/set") {                 // *** ใหม่ ***
      if (msg == "LOCK")   setDoorLock(true);
      if (msg == "UNLOCK") setDoorLock(false);
    }
  }
}

// ─── Sensors ──────────────────────────────────────────────
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

  // MQ-135 (หลัง warm-up)
  if (mqReady) {
    int  air      = analogRead(MQ135_ANALOG);
    bool airAlert = (digitalRead(MQ135_DIGITAL) == LOW);

    char buf[10];
    itoa(air, buf, 10);
    mqtt.publish(T_AIR, buf, true);
    mqtt.publish(T_AIR_ALERT, airAlert ? "ON" : "OFF", true);

    Serial.printf("[MQ-135] ADC=%d  Alert=%s\n", air, airAlert ? "⚠ ON" : "OK");
  } else {
    long rem = (MQ_WARMUP - (millis() - bootTime)) / 1000;
    Serial.printf("[MQ-135] Warming up... %lds\n", rem > 0 ? rem : 0);
  }
}

void publishLDR() {
  int v = analogRead(LDR_PIN);
  char buf[10]; itoa(v, buf, 10);
  mqtt.publish(T_LDR, buf, true);
  Serial.printf("[LDR] ADC=%d (%s)\n", v, v > 2000 ? "มืด" : "สว่าง");
}

// ─── [TEST] LED Actuator ──────────────────────────────────
void setLight(bool on) {
  lightState = on;
  digitalWrite(LED_PIN, on ? HIGH : LOW);
  pubBool(T_LIGHT_STATE, on);
  Serial.printf("[LED] ไฟห้องนั่งเล่น (TEST) → %s\n", on ? "ON" : "OFF");
}

// ─── Door Lock (SG90 180°) ────────────────────────────────
void setDoorLock(bool lock) {
  // ─── GUARD 1: servo กำลังหมุนอยู่ → block ทุกคำสั่ง (กัน reverse mid-motion = เด้งกลับ) ───
  if (servoMoving) {
    Serial.printf("[DOOR] ⚠ servo busy — ignore %s\n", lock ? "LOCK" : "UNLOCK");
    return;
  }

  // ─── GUARD 2: state เดียวกัน → skip + re-publish (เผื่อ HA หลุด state) ───
  if (lock == doorLocked) {
    Serial.printf("[DOOR] already %s — skip\n", lock ? "LOCKED" : "UNLOCKED");
    pubStr(T_DOOR_STATE, lock ? "LOCKED" : "UNLOCKED");
    return;
  }

  doorLocked  = lock;
  int target  = lock ? DOOR_LOCK_ANGLE : DOOR_UNLOCK_ANGLE;
  target = constrain(target, SERVO_MIN_ANGLE, SERVO_MAX_ANGLE);  // กัน servo เกิน 90°

  doorServo.attach(DOOR_SERVO_PIN, 500, 2400);  // pulse width 500–2400µs
  doorServo.write(target);

  servoMoving = true;
  tServoMove  = millis();

  pubStr(T_DOOR_STATE, lock ? "LOCKING" : "UNLOCKING");
  Serial.printf("[DOOR] %s → angle %d°\n",
                lock ? "🔒 LOCKING..." : "🔓 UNLOCKING...", target);
}

// ─── Helpers ──────────────────────────────────────────────
void pubBool(const char* t, bool v)        { mqtt.publish(t, v ? "ON" : "OFF", true); }
void pubStr (const char* t, const char* v) { mqtt.publish(t, v, true); }

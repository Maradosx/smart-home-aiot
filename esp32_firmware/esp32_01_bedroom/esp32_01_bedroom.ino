/**

* ============================================================

* Smart Home AIoT — ESP32 #1 Bedroom (mmWave Diagnostic Mode)

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

const char* MQTT_SERVER = "YOUR_EC2_IP";

const int   MQTT_PORT   = 1883;

const char* MQTT_CLIENT = "esp32-bedroom";
 
// ─── Pins ─────────────────────────────────────────────────

#define LED_PIN      2

#define DHT_PIN      4

#define DHT_TYPE     DHT22

#define LD2410_RX    16   // ESP32 RX  ← LD2410 TX

#define LD2410_TX    17   // ESP32 TX  → LD2410 RX

#define SERVO_PIN    25

#define RELAY_FAN    26

#define LDR_PIN      36
 
// ─── SG90 360° continuous servo — control by DIRECTION + TIME ───
// ไม่ใช่องศา — เป็น signal ส่ง servo หมุนคนละทิศ (timer-based)
// 🔧 FIXED 25 พ.ค. 2569 — swap ทิศ (ก่อนสลับเข้าใจผิด)
// ม่านพับเก็บบน = open (หน้าต่างโล่ง)
// มอเตอร์ดึงลง = close (ม่านคลุมหน้าต่าง — ปิด)
// มอเตอร์ดึงขึ้น = open (พับเก็บ — หน้าต่างโล่ง)
const int SERVO_DIR_OPEN  = 180;   // หมุนทิศดึงม่านขึ้น (พับเก็บ — open หน้าต่าง)
const int SERVO_DIR_CLOSE =   0;   // หมุนทิศดึงม่านลง (คลุมหน้าต่าง — close)

// เวลาที่ servo หมุน (วินาที) — กำหนดว่าม่านเลื่อนไกลแค่ไหน
const unsigned long CURTAIN_OPEN_MS  = 1150;   // 2 วินาที
const unsigned long CURTAIN_CLOSE_MS = 1150;   // 2 วินาที
 
#define T_TEMP          "home/bedroom/temperature"

#define T_HUM           "home/bedroom/humidity"

#define T_PRESENCE      "home/bedroom/presence"

#define T_FALL          "home/bedroom/fall_alert"

#define T_LDR           "home/bedroom/ldr"

#define T_LIGHT_STATE   "home/bedroom/light/state"

#define T_FAN_STATE     "home/bedroom/fan/state"

#define T_CURTAIN_STATE "home/bedroom/curtain/state"

#define T_LIGHT_SET     "home/bedroom/light/set"

#define T_FAN_SET       "home/bedroom/fan/set"

#define T_CURTAIN_SET   "home/bedroom/curtain/set"

#define T_BROADCAST     "home/broadcast/#"
 
const unsigned long SENSOR_MS  = 30000;

const unsigned long MMWAVE_MS  =   500;

const unsigned long LDR_MS     = 10000;

const unsigned long RECONNECT  =  5000;

const unsigned long MMWAVE_DIAG_MS = 3000;  // diag ทุก 3 วินาที
 
struct LD2410Data {

  uint8_t  state;

  uint16_t movingDist;

  uint8_t  movingEnergy;

  uint16_t stillDist;

  uint8_t  stillEnergy;

  uint16_t detectDist;

};
 
WiFiClient     wifiClient;

PubSubClient   mqtt(wifiClient);

DHT            dht(DHT_PIN, DHT_TYPE);

HardwareSerial LD2410Serial(2);

Servo          curtainServo;
 
unsigned long tSensor = 0, tMMWave = 0, tLDR = 0, tReconn = 0;

unsigned long tCurtainMove = 0;

unsigned long tMMWaveDiag = 0;

unsigned long mmwaveByteCount = 0;

unsigned long mmwaveFrameCount = 0;

unsigned long mmwaveLastFrame = 0;

bool curtainMoving = false;

bool curtainOpening = false;

bool lightState    = false;

bool fanState      = false;

bool curtainOpen   = false;

bool lastPresence  = false;

bool lastFall      = false;
 
void pubBool(const char* t, bool v);

void pubStr (const char* t, const char* v);

void setLight(bool on);

void setFan(bool on);

void setCurtain(bool open);
 
void setup() {

  Serial.begin(115200);

  delay(200);

  Serial.println("\n\n========================================");

  Serial.println("[BOOT] ESP32 #1 Bedroom (mmWave DIAG)");

  Serial.println("========================================");
 
  pinMode(LED_PIN,   OUTPUT); digitalWrite(LED_PIN, LOW);

  pinMode(RELAY_FAN, OUTPUT); digitalWrite(RELAY_FAN, LOW);   // boot: fan OFF (HIGH=ON, LOW=OFF)
 
  dht.begin();
 
  ESP32PWM::allocateTimer(0);

  curtainServo.setPeriodHertz(50);

  Serial.printf("[SERVO] dir_open=%d dir_close=%d  open=%lums close=%lums\n",

                SERVO_DIR_OPEN, SERVO_DIR_CLOSE, CURTAIN_OPEN_MS, CURTAIN_CLOSE_MS);
 
  // ── LD2410C UART ──

  LD2410Serial.begin(256000, SERIAL_8N1, LD2410_RX, LD2410_TX);

  Serial.println("[LD2410] UART2 @ 256000 baud");

  Serial.printf("[LD2410] Pins: RX=GPIO%d (ESP rx ← LD2410 TX)\n", LD2410_RX);

  Serial.printf("[LD2410]        TX=GPIO%d (ESP tx → LD2410 RX)\n", LD2410_TX);

  Serial.println("[LD2410] >>> รอ data 3 วิ...");

  delay(3000);

  int avail = LD2410Serial.available();

  Serial.printf("[LD2410] หลัง boot 3 วิ: มี %d bytes ใน buffer\n", avail);

  if (avail == 0) {

    Serial.println("[LD2410] ⚠️  ไม่มีข้อมูลเลย — เช็ค:");

    Serial.println("         1) LD2410 TX → ESP32 GPIO16 (RX) สลับขาไหม");

    Serial.println("         2) VCC 5V (ไม่ใช่ 3.3V)");

    Serial.println("         3) GND ร่วมกับ ESP32");

  } else {

    Serial.print("[LD2410] First bytes (hex): ");

    for (int i = 0; i < min(avail, 32); i++) {

      Serial.printf("%02X ", LD2410Serial.read());

    }

    Serial.println();

  }
 
  connectWiFi();

  mqtt.setServer(MQTT_SERVER, MQTT_PORT);

  mqtt.setCallback(mqttCallback);

  mqtt.setBufferSize(512);

  connectMQTT();

}
 
void loop() {

  if (!mqtt.connected()) {

    if (millis() - tReconn > RECONNECT) { tReconn = millis(); connectMQTT(); }

  } else {

    mqtt.loop();

  }
 
  // ─── Servo motion controller ───
  if (curtainMoving) {

    unsigned long moveMs = curtainOpening ? CURTAIN_OPEN_MS : CURTAIN_CLOSE_MS;
    unsigned long elapsed = millis() - tCurtainMove;

    if (elapsed > moveMs) {

      // ครบเวลาแล้ว → STOP servo
      // 1) เขียน 90 = stop pulse ก่อน (กัน servo ค้างหมุนต่อหลัง detach)
      curtainServo.write(90);

      // 2) Detach (หยุดส่ง PWM)
      curtainServo.detach();

      // Re-assert fan state right after servo detach (detach causes pin floating moment)
      digitalWrite(RELAY_FAN, fanState ? HIGH : LOW);

      curtainMoving = false;
      curtainOpen   = curtainOpening;

      pubStr(T_CURTAIN_STATE, curtainOpen ? "OPEN" : "CLOSED");
      Serial.printf("[SERVO] หยุด → ม่าน %s (เวลาจริง %lums)\n",
                    curtainOpen ? "เปิด 🪟" : "ปิด ▮", elapsed);

    } else {

      // ยังไม่ครบเวลา → re-write direction signal ทุก loop กัน library ตัด PWM
      curtainServo.write(curtainOpening ? SERVO_DIR_OPEN : SERVO_DIR_CLOSE);

      // log progress ทุก 500ms
      static unsigned long tLog = 0;
      if (millis() - tLog > 500) {
        tLog = millis();
        Serial.printf("[SERVO] %s: %lu/%lums (%.0f%%)\n",
                      curtainOpening ? "เปิด" : "ปิด",
                      elapsed, moveMs, (elapsed * 100.0) / moveMs);
      }

    }

  }
 
  if (millis() - tSensor > SENSOR_MS)         { tSensor     = millis(); publishSensors(); }

  if (millis() - tMMWave > MMWAVE_MS)         { tMMWave     = millis(); checkMMWave(); }

  if (millis() - tMMWaveDiag > MMWAVE_DIAG_MS){ tMMWaveDiag = millis(); diagMMWave(); }

  if (millis() - tLDR    > LDR_MS)            { tLDR        = millis(); publishLDR(); }

}
 
void connectWiFi() {

  WiFi.begin(WIFI_SSID_1, WIFI_PASS_1);

  Serial.print("[WiFi] " + String(WIFI_SSID_1));

  for (int i = 0; i < 20 && WiFi.status() != WL_CONNECTED; i++) { delay(500); Serial.print("."); }

  if (WiFi.status() == WL_CONNECTED) { Serial.println(" ✓ " + WiFi.localIP().toString()); return; }

  WiFi.begin(WIFI_SSID_2, WIFI_PASS_2);

  Serial.print("\n[WiFi] Fallback " + String(WIFI_SSID_2));

  for (int i = 0; i < 20 && WiFi.status() != WL_CONNECTED; i++) { delay(500); Serial.print("."); }

  if (WiFi.status() == WL_CONNECTED) { Serial.println(" ✓ " + WiFi.localIP().toString()); }

  else { Serial.println(" ✗ Restart"); ESP.restart(); }

}
 
void connectMQTT() {

  Serial.print("[MQTT] Connecting...");

  if (mqtt.connect(MQTT_CLIENT)) {

    Serial.println(" ✓");

    mqtt.subscribe(T_LIGHT_SET);

    mqtt.subscribe(T_FAN_SET);

    mqtt.subscribe(T_CURTAIN_SET);

    mqtt.subscribe(T_BROADCAST);

    pubBool(T_LIGHT_STATE,   lightState);

    pubBool(T_FAN_STATE,     fanState);

    pubStr (T_CURTAIN_STATE, curtainOpen ? "OPEN" : "CLOSED");

    pubBool(T_PRESENCE,      false);

    pubBool(T_FALL,          false);

  } else {

    Serial.println(" ✗ rc=" + String(mqtt.state()));

  }

}
 
void mqttCallback(char* topic, byte* payload, unsigned int length) {

  String t = String(topic);

  String msg = "";

  for (unsigned int i = 0; i < length; i++) msg += (char)payload[i];

  msg.toUpperCase();

  Serial.println("[MQTT IN] " + t + " → " + msg);

  if (t == T_LIGHT_SET) { setLight(msg == "ON"); }

  else if (t == T_FAN_SET) { setFan(msg == "ON"); }

  else if (t == T_CURTAIN_SET) {

    if (msg == "OPEN")  setCurtain(true);

    if (msg == "CLOSE") setCurtain(false);

  }

  else if (t.startsWith("home/broadcast/")) {

    String sub = t.substring(15);

    if (sub == "light/set")   setLight(msg == "ON");

    if (sub == "fan/set")     setFan(msg == "ON");

    if (sub == "curtain/set") {

      if (msg == "OPEN")  setCurtain(true);

      if (msg == "CLOSE") setCurtain(false);

    }

  }

}
 
void publishSensors() {

  float t = dht.readTemperature(), h = dht.readHumidity();

  if (!isnan(t) && !isnan(h)) {

    char buf[10];

    dtostrf(t, 4, 1, buf); mqtt.publish(T_TEMP, buf, true);

    dtostrf(h, 4, 1, buf); mqtt.publish(T_HUM,  buf, true);

    Serial.printf("[DHT22] %.1f°C  %.1f%%\n", t, h);

  } else {

    Serial.println("[DHT22] ✗ Read failed");

  }

}
 
// ═══════════════════════════════════════════════════════════

// mmWave DIAGNOSTIC: print every 3 sec — bytes count & status

// ═══════════════════════════════════════════════════════════

void diagMMWave() {

  unsigned long sinceFrame = millis() - mmwaveLastFrame;

  Serial.printf("[LD2410 DIAG] bytes=%lu  frames=%lu  ", mmwaveByteCount, mmwaveFrameCount);

  if (mmwaveFrameCount == 0) {

    Serial.printf("✗ ยังไม่มี valid frame  (bytes seen=%lu)\n", mmwaveByteCount);

    if (mmwaveByteCount == 0) {

      Serial.println("              → UART ไม่ได้รับข้อมูลเลย → เช็คสาย/power");

    } else {

      Serial.println("              → มี byte เข้ามาแต่ frame ผิดรูป → ตรวจ baud rate");

      // Dump some raw bytes

      Serial.print("              raw: ");

      int dumped = 0;

      while (LD2410Serial.available() && dumped < 24) {

        Serial.printf("%02X ", LD2410Serial.read());

        dumped++;

      }

      Serial.println();

    }

  } else {

    Serial.printf("✓ frame ล่าสุด %lums ที่แล้ว\n", sinceFrame);

  }

}
 
void checkMMWave() {

  // นับ bytes ที่เข้ามาก่อน parse

  int avail = LD2410Serial.available();

  if (avail > 0) mmwaveByteCount += avail;
 
  LD2410Data d;

  if (!parseLD2410(d)) return;
 
  mmwaveFrameCount++;

  mmwaveLastFrame = millis();
 
  const char* stateStr;

  switch (d.state) {

    case 0:  stateStr = "ไม่มีคน";    break;

    case 1:  stateStr = "ขยับ";        break;

    case 2:  stateStr = "นิ่ง";        break;

    case 3:  stateStr = "ขยับ+นิ่ง";   break;

    default: stateStr = "?";           break;

  }
 
  Serial.printf(

    "[LD2410] state=%u (%s) | moving %4ucm @%3u%% | still %4ucm @%3u%% | detect %4ucm\n",

    d.state, stateStr,

    d.movingDist, d.movingEnergy,

    d.stillDist,  d.stillEnergy,

    d.detectDist

  );
 
  bool presence = (d.state != 0);

  if (presence != lastPresence) {

    lastPresence = presence;

    pubBool(T_PRESENCE, presence);

    Serial.printf("[LD2410] >>> Presence CHANGE → %s\n",

                  presence ? "DETECTED ✓" : "Clear");

  }

}
 
bool parseLD2410(LD2410Data &d) {

  static uint8_t buf[32];

  static int idx = 0;

  static unsigned long lastByte = 0;
 
  if (idx > 0 && millis() - lastByte > 200) idx = 0;
 
  while (LD2410Serial.available()) {

    uint8_t b = LD2410Serial.read();

    lastByte = millis();

    // mmwaveByteCount nudged here too for accuracy

    // (we already counted in checkMMWave, skip to avoid double count)
 
    if (idx == 0 && b != 0xF4) continue;

    if (idx == 1 && b != 0xF3) { idx = 0; continue; }

    if (idx == 2 && b != 0xF2) { idx = 0; continue; }

    if (idx == 3 && b != 0xF1) { idx = 0; continue; }
 
    if (idx < (int)sizeof(buf)) buf[idx] = b;

    idx++;
 
    if (idx >= 23) {

      if (buf[19] == 0xF8 && buf[20] == 0xF7 && buf[21] == 0xF6 && buf[22] == 0xF5) {

        d.state        = buf[8];

        d.movingDist   = (uint16_t)buf[9]  | ((uint16_t)buf[10] << 8);

        d.movingEnergy = buf[11];

        d.stillDist    = (uint16_t)buf[12] | ((uint16_t)buf[13] << 8);

        d.stillEnergy  = buf[14];

        d.detectDist   = (uint16_t)buf[15] | ((uint16_t)buf[16] << 8);

        idx = 0;

        return true;

      }

      idx = 0;

    }

  }

  return false;

}
 
void publishLDR() {

  int v = analogRead(LDR_PIN);

  char buf[10]; itoa(v, buf, 10);

  mqtt.publish(T_LDR, buf, true);

  Serial.printf("[LDR] ADC=%d (%s)\n", v, v > 2000 ? "มืด" : "สว่าง");

}
 
void setLight(bool on) {

  lightState = on;

  digitalWrite(LED_PIN, on ? HIGH : LOW);

  pubBool(T_LIGHT_STATE, on);

  Serial.printf("[LED] ไฟห้องนอน → %s\n", on ? "ON" : "OFF");

}
 
void setFan(bool on) {

  fanState = on;

  digitalWrite(RELAY_FAN, on ? HIGH : LOW);

  pubBool(T_FAN_STATE, on);

  Serial.printf("[FAN] → %s\n", on ? "ON" : "OFF");

}
 
void setCurtain(bool open) {

  // ─── Guards: skip duplicate / overlapping commands ───
  if (curtainMoving) {
    Serial.printf("[CURTAIN] ⚠ กำลังหมุนอยู่ — skip คำสั่งใหม่ (%s)\n", open ? "OPEN" : "CLOSE");
    return;
  }
  if (open == curtainOpen) {
    Serial.printf("[CURTAIN] ⚠ ม่าน %s อยู่แล้ว — skip\n", open ? "เปิด" : "ปิด");
    pubStr(T_CURTAIN_STATE, open ? "OPEN" : "CLOSED");  // re-publish state
    return;
  }

  // Re-assert fan state BEFORE servo (servo current spike can glitch GPIO26 relay)
  digitalWrite(RELAY_FAN, fanState ? HIGH : LOW);

  curtainServo.attach(SERVO_PIN, 500, 2400);

  // ส่ง direction signal — OPEN/CLOSE หมุนคนละทิศ ควบคุมเวลาด้วย CURTAIN_OPEN_MS/CLOSE_MS
  curtainServo.write(open ? SERVO_DIR_OPEN : SERVO_DIR_CLOSE);

  // Re-assert fan AGAIN after servo write — overrides any noise on relay GPIO
  digitalWrite(RELAY_FAN, fanState ? HIGH : LOW);

  curtainOpening = open;

  curtainMoving  = true;

  tCurtainMove   = millis();

  pubStr(T_CURTAIN_STATE, open ? "OPENING" : "CLOSING");

  Serial.printf("[CURTAIN] → %s (dir=%d, %.1fs)\n", open ? "OPENING 🪟" : "CLOSING ▮",

                open ? SERVO_DIR_OPEN : SERVO_DIR_CLOSE,
                (open ? CURTAIN_OPEN_MS : CURTAIN_CLOSE_MS) / 1000.0);

}
 
void pubBool(const char* t, bool v) { mqtt.publish(t, v ? "ON" : "OFF", true); }

void pubStr (const char* t, const char* v) { mqtt.publish(t, v, true); }
 
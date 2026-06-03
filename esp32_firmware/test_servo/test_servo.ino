/**
 * ============================================================
 * Servo Test — ทดสอบ Servo 360° / 180° ผ่าน Serial
 * ============================================================
 *
 * Wiring:
 *   Servo VCC (สีแดง)    ─── 5V (ใช้ external 5V ดีกว่า)
 *   Servo GND (สีน้ำตาล)  ─── GND (shared กับ ESP32 GND!)
 *   Servo Signal (สีส้ม)  ─── GPIO25
 *
 * ⚠️ Servo SG90 กิน 600mA peak — ESP32 5V อาจไม่พอ!
 *
 * Commands (Serial 115200):
 *   o, open  → หมุนเปิด 1.5 วิ
 *   c, close → หมุนปิด 1.5 วิ
 *   s, stop  → หยุด (detach)
 *   1-9      → ทดสอบความเร็ว 1=ช้าสุด, 9=เร็วสุด
 *   r        → reverse direction (ทดสอบทิศทาง)
 *   t        → test sequence (เปิด→หยุด→ปิด→หยุด)
 *   a <num>  → write angle (180° servo: 0-180)
 * ============================================================
 */

#include <ESP32Servo.h>

#define SERVO_PIN  25

Servo myServo;

// Servo 360° speed values
int speedCW  = 110;  // หมุนตามเข็ม (เปิด)
int speedCCW = 70;   // หมุนทวนเข็ม (ปิด)

String inputBuffer = "";

void attachServo() {
  myServo.setPeriodHertz(50);
  myServo.attach(SERVO_PIN, 500, 2400);
}

void moveServo(int speed, int duration_ms, const char* label) {
  Serial.printf("[SERVO] %s — speed=%d, time=%dms\n", label, speed, duration_ms);
  attachServo();
  myServo.write(speed);
  delay(duration_ms);
  myServo.detach();
  Serial.println("[SERVO] หยุด (detached)");
}

void writeAngle(int angle) {
  Serial.printf("[SERVO] Write angle %d° (180° servo mode)\n", angle);
  attachServo();
  myServo.write(angle);
  delay(500);
}

void stopServo() {
  myServo.detach();
  Serial.println("[SERVO] หยุด ✋");
}

void testSequence() {
  Serial.println("\n=== TEST SEQUENCE ===");
  moveServo(speedCW, 1500, "เปิด CW");
  delay(1000);
  moveServo(speedCCW, 1500, "ปิด CCW");
  delay(1000);
  Serial.println("=== END ===\n");
}

void showHelp() {
  Serial.println("\n┌──────────────────────────────────┐");
  Serial.println("│  SERVO TEST Commands             │");
  Serial.println("├──────────────────────────────────┤");
  Serial.println("│ o, open  → หมุนเปิด 1.5s         │");
  Serial.println("│ c, close → หมุนปิด 1.5s          │");
  Serial.println("│ s, stop  → หยุด                  │");
  Serial.println("│ t        → test ครบรอบ           │");
  Serial.println("│ 1-9      → speed test (1=ช้า)    │");
  Serial.println("│ r        → reverse direction     │");
  Serial.println("│ a <n>    → write angle (180°)    │");
  Serial.println("│            ex: a 0, a 90, a 180  │");
  Serial.println("│ h        → help                  │");
  Serial.println("└──────────────────────────────────┘");
  Serial.printf("\nNow: speedCW=%d  speedCCW=%d  pin=GPIO%d\n\n",
                speedCW, speedCCW, SERVO_PIN);
}

void processCmd(String cmd) {
  cmd.trim();
  cmd.toLowerCase();
  if (cmd.length() == 0) return;
  Serial.printf("[CMD] '%s'\n", cmd.c_str());

  if      (cmd == "o" || cmd == "open")  moveServo(speedCW, 1500, "OPEN");
  else if (cmd == "c" || cmd == "close") moveServo(speedCCW, 1500, "CLOSE");
  else if (cmd == "s" || cmd == "stop")  stopServo();
  else if (cmd == "t")                   testSequence();
  else if (cmd == "r") {
    int t = speedCW; speedCW = speedCCW; speedCCW = t;
    Serial.printf("[REV] swapped → CW=%d CCW=%d\n", speedCW, speedCCW);
  }
  else if (cmd == "h" || cmd == "?") showHelp();
  else if (cmd.startsWith("a ")) {
    int angle = cmd.substring(2).toInt();
    angle = constrain(angle, 0, 180);
    writeAngle(angle);
  }
  else if (cmd.length() == 1 && cmd[0] >= '1' && cmd[0] <= '9') {
    // ทดสอบความเร็ว 1-9 (ห่าง 90 มากขึ้น = เร็วขึ้น)
    int n = cmd.toInt();
    int speed = 90 + (n * 10);  // 100, 110, 120, ..., 180
    moveServo(speed, 2000, "Speed test");
  }
  else Serial.printf("[?] Unknown '%s' — type h\n", cmd.c_str());
}

void setup() {
  Serial.begin(115200);
  delay(800);
  ESP32PWM::allocateTimer(0);
  Serial.println("\n[BOOT] Servo Test");
  showHelp();
  inputBuffer.reserve(64);
}

void loop() {
  while (Serial.available() > 0) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') {
      if (inputBuffer.length() > 0) {
        processCmd(inputBuffer);
        inputBuffer = "";
      }
    } else if (c >= 32 && c <= 126) {
      inputBuffer += c;
      if (inputBuffer.length() > 60) inputBuffer = "";
    }
  }
  static unsigned long t = 0;
  if (millis() - t > 5000) { t = millis(); Serial.print("."); }
}

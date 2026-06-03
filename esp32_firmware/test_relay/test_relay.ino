/**
 * ============================================================
 * Relay Test — เปิด/ปิด Relay ผ่าน Serial Monitor (V2 Robust)
 * ============================================================
 *
 * Wiring:
 *   GPIO26 → Relay IN
 *   ESP32 5V → Relay VCC
 *   ESP32 GND → Relay GND
 *
 *   Relay COM ─── 5V (+)
 *   Relay NO  ─── Fan (+) / LED Strip
 *   Load GND  ─── GND
 *
 * Usage (Serial Monitor 115200):
 *   พิมพ์: 1, on    → Relay ON
 *          0, off   → Relay OFF
 *          t        → toggle
 *          b        → blink 5 times
 *          s        → status
 *
 * V2 changes: ใช้ char-by-char parser แทน readStringUntil
 *             แก้ปัญหา Line ending / Serial buffer ค้าง
 * ============================================================
 */

#define RELAY_PIN  26
#define ACTIVE_LOW false  // ของคุณเป็น Active HIGH

bool relayState = false;
String inputBuffer = "";

void setRelay(bool on) {
  relayState = on;
  digitalWrite(RELAY_PIN, ACTIVE_LOW ? (on ? LOW : HIGH) : (on ? HIGH : LOW));
  Serial.printf("[RELAY] → %s  (GPIO%d=%s)\n",
                on ? "ON  🟢" : "OFF 🔴",
                RELAY_PIN,
                digitalRead(RELAY_PIN) == HIGH ? "HIGH" : "LOW");
}

void blinkRelay(int n) {
  Serial.printf("[BLINK] x%d\n", n);
  for (int i = 0; i < n; i++) {
    setRelay(true);  delay(500);
    setRelay(false); delay(500);
  }
}

void showStatus() {
  Serial.printf("[STATUS] Relay = %s  (GPIO%d=%s)\n",
                relayState ? "ON 🟢" : "OFF 🔴",
                RELAY_PIN,
                digitalRead(RELAY_PIN) == HIGH ? "HIGH" : "LOW");
}

void showHelp() {
  Serial.println("\n┌─────────────────────────────────┐");
  Serial.println("│  Commands:                      │");
  Serial.println("│    1 / on   → Relay ON          │");
  Serial.println("│    0 / off  → Relay OFF         │");
  Serial.println("│    t        → toggle            │");
  Serial.println("│    b        → blink 5x          │");
  Serial.println("│    s        → status            │");
  Serial.println("│    h / ?    → help              │");
  Serial.println("└─────────────────────────────────┘\n");
}

void processCommand(String cmd) {
  cmd.trim();
  cmd.toLowerCase();
  if (cmd.length() == 0) return;

  Serial.printf("[CMD] '%s'\n", cmd.c_str());

  if      (cmd == "1" || cmd == "on")            setRelay(true);
  else if (cmd == "0" || cmd == "off")           setRelay(false);
  else if (cmd == "t")                           setRelay(!relayState);
  else if (cmd == "b")                           blinkRelay(5);
  else if (cmd == "s")                           showStatus();
  else if (cmd == "h" || cmd == "?" || cmd == "help") showHelp();
  else Serial.printf("[?] Unknown '%s' — type 'h' for help\n", cmd.c_str());
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(RELAY_PIN, OUTPUT);
  setRelay(false);

  Serial.println("\n[BOOT] Relay Test V2");
  Serial.printf("GPIO=%d  ActiveLOW=%s\n", RELAY_PIN, ACTIVE_LOW ? "true" : "false");
  showHelp();
  inputBuffer.reserve(64);
}

void loop() {
  // อ่าน Serial ทีละ char — ไม่ block
  while (Serial.available() > 0) {
    char c = Serial.read();

    if (c == '\n' || c == '\r') {
      if (inputBuffer.length() > 0) {
        processCommand(inputBuffer);
        inputBuffer = "";
      }
    } else if (c >= 32 && c <= 126) {  // printable ASCII only
      inputBuffer += c;
      if (inputBuffer.length() > 60) inputBuffer = "";  // safety
    }
  }

  // Heartbeat ทุก 5 วินาที — บอกว่ายังไม่ตาย
  static unsigned long lastBeat = 0;
  if (millis() - lastBeat > 5000) {
    lastBeat = millis();
    Serial.printf(".");  // จุดเล็กๆ ทุก 5 วิ
  }
}

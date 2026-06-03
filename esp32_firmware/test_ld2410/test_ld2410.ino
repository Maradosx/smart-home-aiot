/**
 * ============================================================
 * LD2410C Test — Raw UART debug
 * ============================================================
 *
 * Wiring:
 *   LD2410 VCC  ─── 5V
 *   LD2410 GND  ─── GND
 *   LD2410 TX   ─── GPIO16 (UART2 RX)
 *   LD2410 RX   ─── GPIO17 (UART2 TX)
 *
 * จะ dump raw bytes จาก LD2410 มาแสดงทาง Serial
 * ถ้าเห็น 0xAA 0xFF... = LD ทำงาน
 * ถ้าไม่มีอะไรขึ้นเลย = สายเสีย/ไฟไม่พอ
 * ============================================================
 */

#define LD_RX  16
#define LD_TX  17

HardwareSerial LD2410Serial(2);

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("\n[BOOT] LD2410 Raw Test");

  // ลองหลาย baud rate
  Serial.println("Try baud 256000 first...");
  LD2410Serial.begin(256000, SERIAL_8N1, LD_RX, LD_TX);
  Serial.println("Listening for raw bytes (5 second)...");
}

void loop() {
  static unsigned long t = 0;
  static int byteCount = 0;
  static int aaffCount = 0;

  while (LD2410Serial.available()) {
    uint8_t b = LD2410Serial.read();
    Serial.printf("%02X ", b);
    byteCount++;
    if (b == 0xAA) aaffCount++;
    if (byteCount % 16 == 0) Serial.println();
  }

  // สรุปทุก 5 วิ
  if (millis() - t > 5000) {
    t = millis();
    Serial.printf("\n[STATS] bytes received: %d, 0xAA count: %d\n",
                  byteCount, aaffCount);
    if (byteCount == 0) {
      Serial.println("⚠️  ไม่ได้รับ byte เลย — เช็ค:");
      Serial.println("    1. TX/RX ต่อสลับกันไหม?");
      Serial.println("    2. VCC = 5V? GND shared?");
      Serial.println("    3. Baud rate (จะลอง 9600 ใน 10 วิ)");
    } else if (aaffCount == 0) {
      Serial.println("⚠️  ได้ bytes แต่ไม่มี 0xAA — baud rate ผิด");
    } else {
      Serial.println("✅ LD2410 ทำงาน! เห็น header 0xAA");
    }
  }

  // ถ้า 30 วิแล้วยังไม่ได้อะไร ลอง 9600
  static bool switched = false;
  if (!switched && millis() > 30000 && byteCount == 0) {
    switched = true;
    Serial.println("\n=== Switching to baud 9600... ===");
    LD2410Serial.end();
    delay(100);
    LD2410Serial.begin(9600, SERIAL_8N1, LD_RX, LD_TX);
    byteCount = 0; aaffCount = 0;
  }
}

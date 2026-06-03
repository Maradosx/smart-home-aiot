#define RELAY_PIN 23

void setup() {
  pinMode(RELAY_PIN, OUTPUT);

  // ปิดไว้ก่อน (สำคัญ)
  digitalWrite(RELAY_PIN, HIGH);  
}

void loop() {
  // เปิดพัดลม
  digitalWrite(RELAY_PIN, HIGH);
  delay(5000);

  // ปิดพัดลม
  digitalWrite(RELAY_PIN, LOW);
  delay(5000);
}
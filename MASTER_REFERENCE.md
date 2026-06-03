# 📖 Smart Home AIoT — MASTER REFERENCE (100% Complete + Deep Explanations)

> **The Single Source of Truth — รวมทุกอย่างของ Project + อธิบายละเอียดทุก tool**
>
> **Project:** Smart Home AIoT — Bangkok University Senior Project CE08
> **Owner:** Athit "Mos" Boonpinit
> **Defense:** ✅ **26 พ.ค. 2569 — PASSED "ไปได้ด้วยดีเลยครับ"** 🎓
> **Status:** Production-Ready + Battle-tested
> **Doc version:** 2.0 (27 พ.ค. 2569 — Deep explanations added)

---

## 📑 Table of Contents

1. [Project Overview](#1-project-overview)
2. [Timeline (เริ่ม → จบ)](#2-timeline-เริ่ม--จบ)
3. [Hardware — แต่ละชิ้นละเอียด + อธิบาย](#3-hardware--แต่ละชิ้นละเอียด--อธิบาย)
4. [Software Stack — Tools/Services ครบทุกตัว + อธิบาย](#4-software-stack--toolsservices-ครบทุกตัว--อธิบาย)
5. [Protocols & Concepts Explained](#5-protocols--concepts-explained)
6. [Architecture — 3-Layer End-to-End](#6-architecture--3-layer-end-to-end)
7. [Home Assistant System (Detail)](#7-home-assistant-system-detail)
8. [Per-Room Breakdown (5 zones)](#8-per-room-breakdown-5-zones)
9. [Communication & Protocols](#9-communication--protocols)
10. [AI System (ChAvee + Alexa) — Deep Dive](#10-ai-system-chavee--alexa--deep-dive)
11. [Automations (87 รายการ) แยกหมวด](#11-automations-87-รายการ-แยกหมวด)
12. [Modes + Scenes + Lighting](#12-modes--scenes--lighting)
13. [Notifications (3-Channel Resilient)](#13-notifications-3-channel-resilient)
14. [Safety Systems](#14-safety-systems)
15. [Cost Breakdown](#15-cost-breakdown)
16. [Bug-Fix Journey (12 bugs + ngrok bypass)](#16-bug-fix-journey-12-bugs--ngrok-bypass)
17. [Defense Verification](#17-defense-verification)
18. [Documentation Map](#18-documentation-map)
19. [Production Readiness + Future Improvements](#19-production-readiness--future-improvements)

---

## 1. Project Overview

### 🎯 Mission
สร้าง Smart Home ที่:
- **ใช้งานได้จริง** — ไม่ใช่ demo เฉพาะวันสอบ
- **AI-powered** — สั่งเป็นภาษาธรรมชาติได้ (ไทย/อังกฤษ)
- **Multi-modal** — LINE chat + Alexa voice + HA Dashboard + auto sensors
- **Resilient** — Self-healing, 3-channel notifications, safety cutoffs
- **Production-grade** — เก็บ data, มี analytics, สามารถ scale ได้

### 🏆 Final Result
- ✅ **Defense PASSED** — 26 พ.ค. 2569
- ✅ **Production-ready** — uptime 24/7 ตั้งแต่ deploy
- ✅ **Cool factor 10/10** — Engineering maturity ที่ committee ชม

### 📊 By the Numbers
| Metric | Value |
|---|---|
| Hardware components | **13** (5 ESP32 + 6 Zigbee + Pi + dongle) |
| HA entities | **~230** |
| HA automations | **87** (86 ON, 1 disabled) |
| HA scripts | **7** |
| n8n active workflows | **3** (daily report, alert, +ChAvee bypassed) |
| Lambda intents | **23** (Alexa) |
| Documentation files | **30+ MD files** |
| Total cost/month | **฿1,664-1,864** |

---

## 2. Timeline (เริ่ม → จบ)

```
มี.ค. 2569 ────────────────────────────────────────────────
│  • Docker + Mosquitto setup on EC2
│  • Mock MQTT publisher (test ก่อนซื้อ hardware)
│  • Project scaffolding
│
เม.ย. 2569 ────────────────────────────────────────────────
│  • ESP32 firmware drafts
│  • Thesis Chapter 1-4 draft
│  • Smart_home_esp32 first iterations
│
พ.ค. ต้นเดือน ─────────────────────────────────────────────
│  • Buy 5× ESP32 + sensors + relays + servos
│  • Wire each zone (bedroom/living/kitchen/bathroom-garden/garage)
│  • HA install on EC2 Docker
│  • Initial automations (lights, fans)
│
13-19 พ.ค. ─────────────────────────────────────────────────
│  • HARDWARE_GUIDE.md, WIRING_DETAILED.md
│  • Thesis Report Final
│  • Presentation scripts (43 pages)
│  • Mock UI screens
│
20-22 พ.ค. ─────────────────────────────────────────────────
│  • n8n workflow ชาวี v1-v3
│  • LINE Bot integration
│  • Lambda + Alexa skill development
│  • System flow documentation
│
23-25 พ.ค. ─────────────────────────────────────────────────
│  • ChAvee v3 → v4 SMART (JSON malform bug → 3 hotfix attempts)
│  • Echo Dot Alexa+ workaround (new Amazon account)
│  • Pi 3B+ + Zigbee2MQTT setup
│  • 6 Zigbee devices paired
│  • Comprehensive HOME_CONTEXT (16KB)
│
26 พ.ค. = DEFENSE DAY ──────────────────────────────────────
│  ☀️ Morning: 12-hour bug-fix sprint
│      • LINE quota exhausted → ฿1,284 Basic plan
│      • LDR direction fixed (3 iterations)
│      • Smart Light system rebuilt
│      • Pump state-based + safety cutoff
│      • Party loop + Alert blink + Scene smooth
│      • Mobile push 3-channel fan-out
│      • 24 new automations
│      • Dashboard cleanup (12+ Entity not found fixes)
│      • GPS zone 7m → 4m at current location
│      • ChAvee n8n update → blocked → **ngrok HTTPS bypass**
│
│  🎤 Afternoon: DEFENSE DEMO
│      • Alexa Echo Dot — all commands work
│      • ChAvee LINE — chat + control + AI reasoning
│      • HA Dashboard — 8 scenes, mode switching
│      • RESULT: ✅ PASSED "ไปได้ด้วยดี"
│
27 พ.ค. (today) ────────────────────────────────────────────
│  • POST_DEFENSE_FINAL.md retrospective
│  • Update all docs to post-defense version
│  • Archive old/duplicate files (51 files → _archive/)
│  • MASTER_REFERENCE.md created
└────────────────────────────────────────────────────────────
```

---

## 3. Hardware — แต่ละชิ้นละเอียด + อธิบาย

> 💡 **โปรดอ่านก่อน:** Section นี้อธิบายแต่ละชิ้น hardware ครบทุก aspect — *คืออะไร, ทำหน้าที่อะไรในโปรเจคนี้, ความสำคัญ, ทางเลือกที่พิจารณา*

---

### 🟦 3.1 ESP32 DevKit V1 (×5 zones)

#### **คืออะไร?**
**ESP32** เป็น microcontroller (MCU) จากบริษัท Espressif (จีน) ที่ผลิตในไต้หวัน — เป็นชิปประมวลผลตัวเล็กราคาถูก (~฿250) แต่ทรงพลัง มี **Wi-Fi + Bluetooth + Dual-core CPU** ในชิปเดียว

**Spec สำคัญ:**
- **CPU:** Tensilica Xtensa LX6 dual-core 240MHz (2 cores ทำงานพร้อมกัน — 1 core handle Wi-Fi, 1 core handle code)
- **RAM:** 520KB SRAM (เพียงพอสำหรับ logic ง่ายๆ + MQTT client)
- **Flash:** 4MB (เก็บ firmware + OTA backup)
- **Wi-Fi:** 2.4GHz 802.11 b/g/n (ไม่รองรับ 5GHz)
- **GPIO:** 38 pins (DevKit V1) — รองรับ digital I/O, ADC, PWM, I2C, SPI, UART
- **Power:** USB 5V หรือ VIN pin 5-12V
- **ราคา:** ฿200-300/ตัว

#### **ใช้ทำอะไรในโปรเจคนี้?**
- เป็น **"สมองท้องถิ่น" ของแต่ละห้อง** (Edge computing) — อ่าน sensor + ส่งข้อมูลไป cloud + รับคำสั่งกลับมา control actuator
- ห้องละ 1 ตัว รวม 5 ตัว: bedroom, living room, kitchen, bathroom+garden, garage
- เชื่อมต่อ sensor (DHT22, PIR, LDR, MQ-2, soil) → อ่านค่า → publish ผ่าน MQTT ไป Mosquitto broker บน EC2
- Subscribe MQTT topic → รับคำสั่งจาก HA → control relay/servo/LED

#### **ความสำคัญ:**
ESP32 เป็น **หัวใจของ Edge Layer** — ถ้าไม่มี ESP32:
- Sensor ทุกตัวจะอ่านค่าไม่ได้
- Actuator (relay, servo) จะ control ไม่ได้
- ระบบ smart home ทั้งหมดเป็นแค่ cloud — ไม่มีจุดเชื่อม physical
- ทุก zone ทำงานอิสระ — ถ้า Wi-Fi router พัง 1 ห้อง ห้องอื่นยังทำงานได้ (resilient by design)

#### **ทำไมเลือก ESP32 (ไม่ใช้ Arduino UNO/Nano)?**
- **Wi-Fi built-in** — Arduino UNO ต้องเพิ่ม ESP8266 shield (เปลือง + ซับซ้อน)
- **Dual-core** — handle Wi-Fi + sensor logic พร้อมกัน ไม่ block
- **ราคาถูก** — ฿250 vs UNO+Wi-Fi shield ฿700+
- **OTA update** — flash code ผ่าน Wi-Fi ได้ ไม่ต้องเสียบ USB ทุกครั้ง (สำคัญตอน deploy)
- **ADC ดีกว่า** — 12-bit ADC (UNO 10-bit) → อ่าน LDR/MQ-2/soil ละเอียดกว่า

---

### 🟦 3.2 DHT22 Temperature + Humidity Sensor (×4)

#### **คืออะไร?**
**DHT22** (ชื่ออื่น: AM2302) เป็น **digital sensor** ที่อ่านอุณหภูมิและความชื้นในตัวเดียว ใช้สาย data เพียง 1 เส้น (1-Wire protocol)

**Spec:**
- **Temperature:** -40 ถึง 80°C, ความแม่นยำ ±0.5°C
- **Humidity:** 0-100% RH, ±2%
- **Sampling rate:** ทุก 2 วินาที (อ่านบ่อยกว่านี้จะ inaccurate)
- **Power:** 3.3V หรือ 5V
- **ราคา:** ฿80/ตัว
- **มี pull-up resistor 10kΩ** ในตัว module

#### **ใช้ทำอะไรในโปรเจคนี้?**
- ติดตั้งใน **4 zones**: bedroom, living room, kitchen, bathroom
- เชื่อมกับ ESP32 ผ่าน GPIO 4 (1-Wire data pin)
- ESP32 อ่านทุก 30 วินาที → publish ไป MQTT topic `home/<room>/temperature` และ `home/<room>/humidity`
- HA สร้าง entity `sensor.bedroom_temperature` ฯลฯ
- Dashboard แสดง real-time + graph history
- Alexa: "what's the temperature in kitchen" → query entity

#### **ความสำคัญ:**
- **Comfort monitoring** — รู้ว่าบ้านร้อน/ชื้นไป → ปรับ AC/พัดลม
- **Sensor data for thesis** — สำหรับเก็บ data + วิเคราะห์
- **Trigger automations** — เช่น ถ้าครัวร้อน > 35°C + แสดงว่าทำกับข้าว → suggest พัดลม

#### **ทำไม DHT22 (ไม่ใช้ DHT11/SHT3x/BME280)?**
- **DHT11** ถูกกว่า (฿40) แต่ accuracy แย่ (±2°C) + range จำกัด (0-50°C) — ไม่เหมาะกับ thesis
- **SHT3x** แม่นยำกว่า (฿200+) แต่ใช้ I2C ซับซ้อนกว่าและไม่จำเป็นสำหรับ home use
- **BME280** เพิ่ม pressure ได้ (฿250) แต่ overkill — ไม่ใช้ pressure data
- **DHT22** = sweet spot ระหว่างราคา + accuracy + simplicity

#### **ห้องที่ไม่ได้ติด:**
- Garage, Garden, Bathroom (เฉพาะตัว bathroom+garden module ก็เพียงพอ)
- Alexa "what's temperature in garage" → friendly fallback "ห้องนี้ยังไม่ได้ติด sensor"

---

### 🟦 3.3 PIR HC-SR501 Motion Sensor (×5)

#### **คืออะไร?**
**PIR** = **P**assive **I**nfra**R**ed sensor — ตรวจจับความเคลื่อนไหวของวัตถุที่ปล่อยรังสีอินฟราเรด (เช่น คน, สัตว์เลี้ยง)

**หลักการทำงาน:**
- ทุกวัตถุที่อุณหภูมิ > 0K ปล่อย infrared radiation
- PIR มี pyroelectric crystal 2 ตัว — เมื่อมีคนเดินผ่าน → 1 ตัวรับ IR ก่อน → output spike
- **"Passive"** = ไม่ปล่อย IR เอง (ต่างจาก active sensor) → ประหยัดไฟ

**Spec HC-SR501:**
- **Range:** 7 เมตร, มุมตรวจจับ 110°
- **Adjustable:** Sensitivity + Hold time (potentiometer 2 ตัว)
- **Output:** Digital HIGH/LOW (3.3V compatible)
- **Power:** 5V (DC)
- **ราคา:** ฿50/ตัว

#### **ใช้ทำอะไรในโปรเจคนี้?**
- ตรวจจับ motion ใน 5 zones → trigger Smart Light automation
- Logic: PIR HIGH + LDR ≥ 2800 (มืด) + light off → turn light on
- เป็น input หลักของ "auto light" system

#### **ความสำคัญ:**
- **Smart Light = killer feature** — ไม่ต้องเปิดปิดเอง
- ประหยัดไฟ (ไฟไม่เปิดทิ้งไว้)
- Safety (มีคนเข้าห้องน้ำ → ไฟเปิดอัตโนมัติ → ไม่ตกบันได)

#### **Limitations + Workarounds:**
- **False positive จากแมว** → ปรับ sensitivity ลด
- **Detect ผ่านกระจก/ผ้าไม่ได้** (IR ผ่านไม่ได้)
- **ห้องนั่งเล่นใช้ SNZB-03P Zigbee แทน** เพราะ Zigbee เร็วกว่า + battery-powered (ไม่ต้องเสียบ USB ESP32 เพิ่ม)

---

### 🟦 3.4 LDR (Light-Dependent Resistor) ×5

#### **คืออะไร?**
**LDR** หรือ **photoresistor** — ความต้านทานเปลี่ยนตามแสง: สว่าง = ความต้านทานต่ำ, มืด = ความต้านทานสูง

**หลักการ:**
- ใช้สาร Cadmium Sulfide (CdS) — semiconductor ที่ resistance เปลี่ยนตามแสง
- ต่อเข้า ADC pin → อ่านค่า 0-4095 (12-bit)
- ต้องต่อ voltage divider กับ resistor 10kΩ

**Spec GL5528:**
- **Resistance:** ~1MΩ (มืด) → ~10kΩ (สว่าง)
- **Response time:** ~20ms
- **Spectral peak:** 540nm (green-yellow visible light)
- **ราคา:** ฿20/ตัว

#### **ใช้ทำอะไรในโปรเจคนี้?**
- ติดทุก 5 zones — อ่านความสว่างในห้อง
- ESP32 อ่าน ADC value → publish `sensor.<room>_ldr`
- เป็น **trigger หลักของ Smart Light** ร่วมกับ PIR
- ป้องกัน "เปิดไฟตอนสว่างอยู่แล้ว"

#### **ความสำคัญ:**
- **ทำให้ Smart Light ฉลาดขึ้น** — ไม่เปิดไฟตอนกลางวันที่สว่างอยู่แล้ว
- **ประหยัดไฟมาก** (เหตุผลหลักของ "Smart" Light)

#### **Bug Story (Iteration 3 ครั้ง):**
LDR direction ทำเข้าใจผิด 2 รอบก่อนหาคำตอบ:
- **Try 1:** `LDR > 2200 = DARK` → wrong (never triggered)
- **Try 2:** `LDR < 500 = DARK` → wrong (lamp itself dropped LDR)
- **Try 3 (FINAL):** ดู UI mapping ที่ user เห็น → `>= 2800 = มืด` ✅

**Lesson:** verify sensor direction ผ่าน user-observable UI ไม่ใช่ assumption

#### **Per-room thresholds:**
- Bedroom/Living/Garage: ≥ 2800 = trigger
- Bathroom: ≥ 2200 (sensitive — ห้องเล็ก แสงน้อยกว่า)

---

### 🟦 3.5 MQ-2 Gas/Smoke Sensor (×1, Kitchen)

#### **คืออะไร?**
**MQ-2** เป็น **chemical sensor** ตรวจจับก๊าซติดไฟ (combustible gas) — รวมถึง LPG, methane, butane, alcohol, smoke

**หลักการ:**
- ใช้ Tin Dioxide (SnO₂) semiconductor — เมื่อสัมผัสก๊าซติดไฟ → resistance ลด
- มี heater coil ภายใน (ต้อง preheat 24+ ชั่วโมงครั้งแรก)
- Output แบบ analog (ADC) + digital (threshold-based)

**Spec:**
- **Range:** 200-10000 ppm (parts per million)
- **Detection:** LPG, methane, butane, propane, smoke, hydrogen
- **Power:** 5V, ~150mA (กิน power เยอะเพราะมี heater)
- **Preheat:** 20+ วินาที (cold start), 24 ชม. (first time)
- **ราคา:** ฿180

#### **ใช้ทำอะไรในโปรเจคนี้?**
- ติดเฉพาะ **kitchen ESP32 #3** — ใกล้เตาก๊าซ
- Analog output → ADC GPIO 35
- ESP32 publish `sensor.kitchen_gas_lpg` (ppm value)
- HA สร้าง `binary_sensor.kitchen_gas_alert` (true if ppm > threshold)
- **เมื่อ alert ON:**
  1. Living room Tuya bulb → red blink (alert_flash_active = ON)
  2. Kitchen exhaust fan → auto-on
  3. 3-channel notify (CRITICAL on iPhone — bypass silent mode)

#### **ความสำคัญ:**
- **Real safety feature** — ไม่ใช่แค่ demo
- **Gas leak = fatal** — บ้านไทยใช้ LPG ในครัว, MQ-2 สามารถช่วยชีวิตได้
- **Defense talking point** — committee ชอบมาก (engineering for real-world)

#### **Calibration:**
- Baseline (no gas): ~500-800 ppm reading
- Threshold: baseline + 500 ppm (relative threshold ปลอดภัยกว่า absolute)
- Verify: ลอง LPG can ใกล้ ๆ → reading ขึ้นชัดเจน → confirm

#### **ทำไม MQ-2 (ไม่ใช่ MQ-5/MQ-9)?**
- **MQ-5** = LPG/methane specific
- **MQ-7** = CO เท่านั้น
- **MQ-9** = CO + flammable gas
- **MQ-2** = broad spectrum (LPG + smoke + methane) → best for general home safety

---

### 🟦 3.6 LD2410 mmWave Radar (×1, Bathroom)

#### **คืออะไร?**
**LD2410** (Hi-Link) เป็น **24GHz mmWave radar sensor** — ใช้คลื่นวิทยุความถี่สูง (millimeter wave) ตรวจจับการเคลื่อนไหว + presence + still detection

**ต่างจาก PIR:**
| Aspect | PIR | LD2410 mmWave |
|---|---|---|
| Technology | Passive IR | Active radar (24GHz) |
| Detect through curtain/wall | ❌ | ✅ thin wall |
| Still detection | ❌ (ต้องเคลื่อน) | ✅ detect breathing |
| Range | 7m | 6m + adjustable |
| Angle | 110° | 360° |
| Power | Low | Higher (~80mA) |
| Cost | ฿50 | ฿250 |

**Output:** UART (TX/RX) — ส่งข้อมูล presence + distance + duration

#### **ใช้ทำอะไรในโปรเจคนี้?**
- ติด **ห้องน้ำ** สำหรับ **fall detection (สำหรับผู้สูงอายุ)**
- Heuristic: presence ON + still ≥ 5 นาที = น่าจะล้ม
- Response: bathroom light flash + 3-channel notify + auto-restore prev state

#### **ความสำคัญ:**
- **Fall detection = killer feature** สำหรับ aging home
- **ห้องน้ำเป็นที่เกิดอุบัติเหตุล้มบ่อยที่สุด** ในผู้สูงอายุ
- **PIR ทำไม่ได้** เพราะคนล้มแล้วไม่เคลื่อนไหว — PIR จะ false negative
- Defense ชม "real-world value"

#### **Limitation (honest):**
- เป็น **heuristic 5-min still** ไม่ใช่ ML
- **False positive:** อาบน้ำนานๆ อาจ trigger
- **Future:** LD2451 sensor (รุ่นใหม่ — มี true ML-based fall detect)

---

### 🟦 3.7 Capacitive Soil Moisture Sensor (×1, Garden)

#### **คืออะไร?**
Sensor วัดความชื้นในดินโดยใช้ **capacitive principle** — ดินชื้น → dielectric constant สูง → capacitance สูง

**ต่างจาก resistive soil sensor (เก่า):**
| Aspect | Resistive | Capacitive ✅ |
|---|---|---|
| Method | วัด resistance ระหว่าง 2 probes | วัด capacitance ผ่าน PCB |
| Corrosion | สูง (electrolysis ในดิน) | ต่ำ (PCB เคลือบ epoxy) |
| Lifespan | ~6 เดือน | 2-3 ปี |
| ราคา | ฿80 | ฿150 |

**Output:** Analog 0-3.3V (or 0-5V) → ADC value 0-4095

#### **ใช้ทำอะไรในโปรเจคนี้?**
- ติดในกระถางต้นไม้ (garden zone — ESP32 #4)
- Publish `sensor.garden_soil_moisture` (% เปอร์เซ็นต์ที่ calibrate แล้ว)
- Trigger auto-pump:
  - soil < 45% + 06-20น + weather ≠ rainy → pump on
  - soil ≥ 60% → pump off
  - safety: max 10 min runtime

#### **ความสำคัญ:**
- **Auto-watering = practical feature** — ลืมรดน้ำไม่ได้
- **Weather-aware** — ฝนตกไม่รด (ประหยัดน้ำ + ไม่ทำให้รากเน่า)
- **Safety cutoff** — กันปั๊มทำงานตลอด (rare แต่ destructive ถ้าเกิด)

---

### 🟦 3.8 Servo SG90 (×2: front door + garage)

#### **คืออะไร?**
**SG90** เป็น **micro servo motor** — มอเตอร์เล็ก ๆ ที่หมุนได้ 0-180° แม่นยำผ่าน PWM signal

**Spec:**
- **Torque:** 1.8 kg·cm @ 4.8V
- **Speed:** 0.1s/60°
- **Power:** 5V, ~250mA (peak)
- **Weight:** 9 กรัม
- **Control:** PWM 50Hz (period 20ms), pulse width 1-2ms = 0-180°
- **ราคา:** ฿100

#### **ใช้ทำอะไรในโปรเจคนี้?**
- **Front door servo** (ESP32 #2 living room) — lock mechanism
  - 90° = ปลดล็อก, 0° = ล็อก
  - HA entity: `lock.front_door_lock`
- **Garage door servo** (ESP32 #5 garage)
  - 0° = ปิด, 180° = เปิด
  - HA entity: `cover.garage_door`

#### **ความสำคัญ:**
- **Physical security feature** — สามารถ ล็อค/ปลดล็อก ประตูได้
- **Demo talking point** — committee เห็นชัดเจน (visible action)
- Alexa: "lock the front door", "open garage" — verified work
- LINE: "ล็อคประตู", "เปิดประตูโรงรถ" — work

#### **Limitations:**
- **SG90 = micro servo** ไม่แข็งแรงพอสำหรับ ประตูจริง — เป็น demo mechanism
- Future: upgrade เป็น **MG996R metal gear** (฿250) สำหรับ production use
- หรือใช้ **electric strike lock** สำหรับ commercial-grade

---

### 🟦 3.9 Relay Module 5V 1-channel (×5)

#### **คืออะไร?**
**Relay** เป็น **electromechanical switch** — ใช้ low-voltage signal (3.3V/5V) ควบคุม high-voltage circuit (220V AC)

**หลักการ:**
- มี electromagnet ภายใน → เมื่อ energize → ดูด armature → close/open contact
- **Isolated** — ฝั่ง low-voltage และ high-voltage แยกขาดกัน (safety)

**Spec module ที่ใช้:**
- **Input:** 5V (HIGH=ON หรือ LOW=ON ขึ้นกับรุ่น)
- **Output:** 220V AC 10A
- **Type:** SPDT (Single Pole Double Throw) — NO + NC + COM
- **Opto-isolated** — มี optocoupler protect MCU จาก surge
- **ราคา:** ฿50/module

#### **ใช้ทำอะไรในโปรเจคนี้?**
- Bedroom: LED light relay
- Kitchen: Exhaust fan relay
- Garden: Water pump relay
- (Tuya plugs ใช้แทน relay สำหรับ fan/purifier ใน living room)

#### **ความสำคัญ:**
- **Bridge ระหว่าง low-voltage MCU กับ high-voltage AC**
- **Safety** — ไม่ต้องต่อ MCU เข้า 220V โดยตรง (อันตราย + จะพังทันที)
- **ราคาถูกมาก** ฿50 vs Tuya smart plug ฿350

#### **Caveat:**
- **เสียงคลิก** — แต่ละครั้งที่ switch มีเสียง (electromechanical)
- **SSR (Solid State Relay)** เงียบกว่าแต่แพง (฿200+) — ใช้ relay ธรรมดาก่อน

---

### 🟧 3.10 Tuya TS0505B (Zigbee RGB+CCT Bulb)

#### **คืออะไร?**
**Tuya** เป็นแบรนด์จีนที่ผลิต smart home devices มากที่สุดในโลก (white-label OEM ให้ brand อื่นใช้)
**TS0505B** คือรุ่น **RGB+CCT smart bulb** ที่ใช้ **Zigbee 3.0 protocol**

**Spec:**
- **Power:** 9W (เท่า incandescent ~60W)
- **Brightness:** 800 lumens
- **Color:** Full RGB (16.7M colors) + CCT 2700-6500K (warm to cool white)
- **Socket:** E27 (มาตรฐานไทย)
- **Protocol:** Zigbee 3.0 (mesh)
- **ราคา:** ฿450

#### **ใช้ทำอะไรในโปรเจคนี้?**
- ติดในห้องนั่งเล่น — เป็น **main color light** ของบ้าน
- HA entity: `light.living_room_light`
- Control:
  - Alexa: "set living room to blue" → blue
  - ChAvee LINE: "ปาร์ตี้" → 5-color cycle
  - Scene buttons: warm orange (welcome), amber (sunrise), soft blue (calm), dim red (bedtime)
  - Alert: red blink เมื่อ gas/leak/intrusion
- **เปลี่ยน LED ESP32 เดิม** — Tuya bulb ใช้แทน → GPIO ของ ESP32 free up สำหรับ servo

#### **ความสำคัญ:**
- **Hero device** — committee เห็นชัดเจนที่สุด (visible color change)
- **Defense storytelling** — "ChAvee เปลี่ยนสีไฟตามอารมณ์"
- **Mood/Scene engine** — เป็น output หลักของ scene system
- **Better than ESP32 + WS2812 LED strip** เพราะ:
  - White light quality ดีกว่ามาก (CCT ปรับได้)
  - Brightness สูงพอใช้งานจริง (LED strip มืดเกิน)
  - ติดในขั้วหลอดมาตรฐาน — ไม่ต้อง wire

#### **ทำไม Zigbee ไม่ Wi-Fi?**
- Tuya Wi-Fi bulb ก็มี (TS0505B-W) แต่:
  - **Zigbee mesh ทนทานกว่า** — ถ้า router restart, Zigbee ยังทำงาน
  - **ไม่กิน Wi-Fi bandwidth**
  - **ไม่ต้องตั้ง credentials** (paired ผ่าน Z2M)
  - **Local control** — ไม่ต้องผ่าน Tuya cloud (privacy + speed)

---

### 🟧 3.11 Tuya TS011F Smart Plug + Power Monitoring (×2)

#### **คืออะไร?**
**Smart plug** เสียบขั้วปลั๊กไทย/สากล — ควบคุม on/off + วัด watt + voltage + current

**Spec:**
- **Load:** 220V 10A (2200W max)
- **Protocol:** Zigbee 3.0
- **Power monitoring:** Watt, Voltage, Current, kWh accumulator
- **Indicator LED:** อยู่ที่ตัวปลั๊ก
- **ราคา:** ฿350/ตัว

#### **ใช้ทำอะไรในโปรเจคนี้?**
- **TS011F #1** — เสียบพัดลม living room
  - Entity: `switch.living_room_fan`
  - `sensor.fan_power` (watt real-time)
- **TS011F #2** — เสียบเครื่องฟอกอากาศ
  - Entity: `switch.air_purifier`
  - `sensor.purifier_power`

#### **ความสำคัญ:**
- **Energy monitoring** — รู้ว่าอุปกรณ์ไหนกินไฟเท่าไหร่
- **Defense talking point** — "บ้านนี้รู้ว่าตัวเองกินไฟเท่าไหร่"
- **Future analytics** — สร้าง dashboard energy consumption ตามเวลา
- **Detect anomaly** — ถ้าพัดลมกินไฟผิดปกติ → motor มีปัญหา → notify

#### **ทำไมไม่ใช้ relay ของ ESP32 แทน?**
- **Power monitoring** — relay ทำไม่ได้ (ต้องเพิ่ม CT clamp + ADC + library + math)
- **Modular** — เสียบใช้ที่ไหนก็ได้ ไม่ต้อง wire เข้า ESP32
- **Visual feedback** — มี LED indicator ที่ตัวปลั๊ก
- **Safety certified** — Tuya มี TIS/CE certification (relay DIY ไม่มี)

---

### 🟧 3.12 Sonoff SNZB-03P (Zigbee PIR, Living Room)

#### **คืออะไร?**
Sonoff (ITEAD) เป็นแบรนด์จีนที่ทำ smart home affordable
**SNZB-03P** = Zigbee PIR motion sensor (battery-powered)

**Spec:**
- **Range:** 4m, 100°
- **Power:** CR2450 coin battery (อายุ ~2 ปี)
- **Protocol:** Zigbee 3.0
- **Pairing:** ผ่าน Z2M (Pi)
- **ราคา:** ฿400

#### **ใช้ทำอะไรในโปรเจคนี้?**
- ติดในห้องนั่งเล่น — แทน PIR ESP32
- HA entity: `binary_sensor.living_room_presence`
- Trigger Smart Light + scene snapshot

#### **ความสำคัญ:**
- **Battery-powered** — ไม่ต้อง wire (ติดที่ไหนก็ได้)
- **Zigbee = fast** (latency ~100ms vs Wi-Fi ESP32 ~300ms)
- **Mesh** — ขยาย range ของ Zigbee network ผ่าน Tuya bulb (Zigbee router)

---

### 🟧 3.13 Sonoff SNZB-05P (Zigbee Water Leak, Bathroom)

#### **คืออะไร?**
Zigbee water leak sensor — detect ว่ามีน้ำที่ pin contact หรือไม่

**Spec:**
- **Detection:** 2-pin copper contact (short circuit = water)
- **Power:** CR2450 coin battery (~2 ปี)
- **Protocol:** Zigbee 3.0
- **ราคา:** ฿400

#### **ใช้ทำอะไรในโปรเจคนี้?**
- วางที่พื้นห้องน้ำ (จุดเสี่ยงน้ำท่วม)
- HA entity: `binary_sensor.bathroom_leak`
- เมื่อน้ำเปียก 2 pin → trigger:
  1. Living bulb → red blink
  2. 3-channel CRITICAL notify

#### **ความสำคัญ:**
- **Real safety** — น้ำท่วมห้องน้ำ = ข้าวของเสียหาย, ไฟฟ้าลัดวงจร
- **Demo-able** — เอาน้ำใส่นิ้วแตะ pin → notify ขึ้นทันที (committee เห็นชัด)

#### **Lesson learned:**
- **ตรวจจับเฉพาะ 2 copper pins** — ไม่ใช่ cable ทั้งสาย
- ตอนแรกเข้าใจผิดว่า cable เปียกก็ detect → ทดลองแล้วไม่ใช่ → fix logic

---

### 🟧 3.14 Sonoff SNZB-04 (Zigbee Door/Window Contact)

#### **คืออะไร?**
Zigbee door/window contact sensor — magnet + reed switch

**หลักการ:** มีแม่เหล็ก + reed switch — แยกห่างกัน = เปิด, ติดกัน = ปิด

**Spec:**
- **Detection:** Reed switch (boolean state)
- **Power:** CR1632 coin battery (~2 ปี)
- **Protocol:** Zigbee 3.0
- **ราคา:** ฿400

#### **ใช้ทำอะไรในโปรเจคนี้?**
- ติดที่ประตูหน้าบ้าน
- HA entity: `binary_sensor.front_door_contact`
- Trigger intrusion automation:
  - ประตูเปิด + house_mode = Away/Security → 
    - Servo auto-lock
    - Living bulb red flash
    - 3-channel CRITICAL notify

#### **ความสำคัญ:**
- **Intrusion detection** — ผู้บุกรุก (หรือ family forgot)
- **Combined with servo + Zigbee bulb** = full security response
- **Battery-powered** — ติดที่ประตูได้โดยไม่ต้อง wire

---

### 🟩 3.15 Raspberry Pi 3B+ (Zigbee Coordinator)

#### **คืออะไร?**
**Raspberry Pi** เป็น single-board computer (SBC) ขนาดเท่าบัตรเครดิต — full Linux computer

**Pi 3B+ Spec:**
- **CPU:** ARM Cortex-A53 quad-core 1.4GHz
- **RAM:** 1GB DDR2
- **Storage:** microSD card 16-64GB
- **Network:** Wi-Fi 2.4/5GHz + Ethernet
- **USB:** 4× USB 2.0
- **Power:** 5V 2.5A (USB-C)
- **ราคา:** ~฿1,500 (used in 2026)

#### **ใช้ทำอะไรในโปรเจคนี้?**
- **Zigbee coordinator** — รัน Zigbee2MQTT (Z2M) ใน Docker
- เสียบ Sonoff Zigbee dongle ผ่าน USB
- **Bridge ระหว่าง Zigbee mesh กับ MQTT broker**
- **IP:** 172.20.10.4 (Tailscale) / 192.168.1.155 (LAN)
- **Docker containers:**
  - `zigbee2mqtt` (Z2M v2.10.1)
  - `portainer` (management UI)

#### **ความสำคัญ:**
- **Without Pi → ไม่มี Zigbee** — Tuya bulb + plugs + Sonoff sensors ทุกตัวพังหมด
- **Pi อยู่ในบ้าน** — Zigbee range จำกัด → ต้องอยู่ใกล้ devices
- **Local processing** — Zigbee messages ไม่ออกอินเทอร์เน็ตจนกว่าจะ relay ผ่าน Pi → EC2 MQTT
- **Resilient** — ถ้า Internet down, Pi ยัง relay Zigbee messages กับ devices ในบ้านได้

#### **ทำไม Pi 3B+ ไม่ Pi 4/Pi Zero?**
- **Pi 4** = แพงกว่า (~฿2,800), overkill สำหรับ Z2M (ใช้ <10% CPU)
- **Pi Zero W** = ถูกกว่า (~฿800) แต่ Wi-Fi ช้ากว่า, USB ต้อง adapter, performance limited
- **Pi 3B+** = sweet spot — มีของอยู่แล้ว, performance พอ, 4× USB

---

### 🟩 3.16 Sonoff Zigbee 3.0 USB Dongle Plus

#### **คืออะไร?**
USB stick ที่ทำหน้าที่ **Zigbee coordinator** — เป็นจุดเริ่มต้นของ Zigbee mesh

**Spec:**
- **Chip:** Texas Instruments CC2652P (Zigbee 3.0 SoC)
- **Stack:** EmberZNet (Silicon Labs reference)
- **Antenna:** PCB antenna + external SMA option
- **Range:** ~30m indoor (ขยายผ่าน mesh)
- **Power:** 5V via USB
- **ราคา:** ฿800

#### **ใช้ทำอะไรในโปรเจคนี้?**
- เสียบ Pi 3B+ ผ่าน USB
- Z2M ใช้ dongle นี้คุย Zigbee
- Pair Zigbee devices ทั้ง 6 ตัวผ่าน dongle

#### **ความสำคัญ:**
- **หัวใจของ Zigbee network** — ถ้าไม่มี dongle ไม่มี Zigbee
- **Modular** — เอา dongle ไปเสียบเครื่องอื่นได้ (move setup)

#### **ทำไม Sonoff CC2652P (ไม่ใช้ Conbee II/Texas Instruments CC2531)?**
- **CC2531** = old chip (ZNet stack), จำกัด devices ~20 ตัว, ราคาถูกกว่าแต่ outdated
- **Conbee II** = Phoscon coordinator, แพงกว่า (~฿1,500), proprietary firmware
- **Sonoff CC2652P** = open firmware (EmberZNet), รองรับ 100+ devices, ราคาดี

---

### 🟩 3.17 AWS EC2 t3.small (Cloud Brain)

#### **คืออะไร?**
**EC2** = Amazon Elastic Compute Cloud — virtual server ที่เช่ารายชั่วโมง
**t3.small** = instance type — burstable performance, ราคาประหยัด

**Spec t3.small:**
- **vCPU:** 2 (burstable up to 100%)
- **RAM:** 2GB
- **Network:** Up to 5 Gbps
- **Storage:** EBS-backed (30GB SSD)
- **OS:** Ubuntu 22.04 LTS
- **Region:** us-east-1 (Virginia)
- **Cost:** $0/mo (Free Tier — $114.50 credits)

#### **ใช้ทำอะไรในโปรเจคนี้?**
- **Public IP:** YOUR_EC2_IP (elastic IP)
- **Docker containers:**
  - `homeassistant` (HA Core 2026.4.2)
  - `mosquitto` (MQTT broker)
- **Open ports:**
  - 8123 (HA HTTP)
  - 1883 (MQTT)
  - 22 (SSH)

#### **ความสำคัญ:**
- **The Brain** — HA รันที่นี่, ทุก automation/logic เกิดที่นี่
- **24/7 uptime** — บ้านไม่มี static IP → cloud server เท่านั้นที่ accessible จาก Internet (ESP32, Alexa, LINE Bot)
- **Public IP** — Alexa Lambda, n8n Railway, LINE webhook ทุกตัวต้องเข้าถึง HA → ต้อง public IP

#### **ทำไม EC2 (ไม่ใช้ Pi local server)?**
- **Pi at home** = ต้อง port forward + DDNS + ISP restrictions (Thai ISP block port 80/443 บางที)
- **EC2** = static IP + AWS security groups + 99.99% uptime
- **Free Tier 12 เดือน** = $0 ต้นทุน

#### **ทำไม t3.small (ไม่ใช้ t2.micro free)?**
- **t2.micro** = 1GB RAM ไม่พอสำหรับ HA + Mosquitto + buffer
- **t3.small 2GB RAM** = พอ + burstable CPU
- **Cost ใช้ AWS credits free**

---

### 🟩 3.18 Network Topology

```
🌐 Internet
   ├── AWS EC2 t3.small (YOUR_EC2_IP)
   │     ├── HA Core (port 8123)
   │     └── Mosquitto MQTT (port 1883)
   │
   ├── Railway (n8n workflows)
   ├── Supabase (Postgres)
   ├── AWS Lambda (Alexa backend)
   ├── Anthropic API
   └── LINE Platform API

🏠 Home LAN (192.168.1.x via router)
   ├── 📡 Wi-Fi Router (192.168.1.1)
   │
   ├── 🥧 Pi 3B+ (192.168.1.155, 172.20.10.4 Tailscale)
   │     ├── Z2M Docker container
   │     └── Sonoff Zigbee dongle (USB) ──┐
   │                                       │
   │                                       ▼
   │                              📶 Zigbee Mesh (channel 11)
   │                                  ├── Tuya bulb
   │                                  ├── Tuya plug × 2
   │                                  ├── SNZB-03P (motion)
   │                                  ├── SNZB-05P (leak)
   │                                  └── SNZB-04 (door)
   │
   ├── 🔧 ESP32 × 5 (DHCP from router)
   │     └── Wi-Fi 2.4GHz → router → Internet → EC2 MQTT
   │
   └── 📱 iPhone (HA Companion + GPS)
         └── Cellular/Wi-Fi → HA API direct
```

---

## 4. Software Stack — Tools/Services ครบทุกตัว + อธิบาย

> 💡 **โปรดอ่านก่อน:** Section นี้อธิบายแต่ละ tool/service ว่า *คืออะไร, ทำอะไรในโปรเจคนี้, ความสำคัญ, ทางเลือกที่พิจารณา*

---

### 🟦 4.1 Arduino IDE + ESP32 Framework

#### **คืออะไร?**
**Arduino IDE** = IDE (Integrated Development Environment) สำหรับเขียน firmware ของ microcontroller — ใช้ภาษา C++ แบบ simplified (มี framework helper)
**ESP32 Framework** = library extension ที่ทำให้ Arduino IDE ใช้ compile + flash ESP32 ได้

#### **ใช้ทำอะไรในโปรเจคนี้?**
- เขียน firmware ของ ESP32 ทั้ง 5 ตัว
- Compile + upload ผ่าน USB
- **Folder:** `/Project_1/esp32_firmware/เวอร์ชั่นมีmotor/` (authoritative)

#### **Libraries ที่ใช้:**

##### **WiFi.h**
- **Built-in** library ของ ESP32 Arduino core
- ทำหน้าที่: เชื่อม Wi-Fi 2.4GHz, จัดการ reconnect อัตโนมัติ
- ใน firmware: `WiFi.begin(ssid, password)` + `WiFi.status() == WL_CONNECTED`

##### **PubSubClient**
- **MQTT client library** สำหรับ Arduino (โดย Nick O'Leary)
- ทำหน้าที่: connect Mosquitto broker, publish/subscribe topics, parse messages
- **Critical** — ถ้าไม่มี library นี้ ESP32 คุย MQTT ไม่ได้

##### **ArduinoJson v6**
- Library สำหรับ parse/serialize JSON
- ใน firmware: รับคำสั่งจาก MQTT topic (JSON format) → parse → execute

##### **DHT.h (Adafruit)**
- Library อ่าน DHT22 sensor (1-Wire protocol)
- ใน firmware: `dht.readTemperature()` + `dht.readHumidity()`

##### **ESP32Servo**
- Library control servo บน ESP32 (ใช้ Servo.h ของ Arduino มาตรฐานไม่ได้ — ESP32 ใช้ PWM ต่างจาก AVR)
- ใน firmware: `servo.write(angle)` (0-180°)

#### **ความสำคัญ:**
- **Arduino IDE** เป็น tool ที่ใช้กันแพร่หลายที่สุด — มี community + documentation มาก
- เป็น standard ในการเรียน embedded systems

#### **ทางเลือก:**
- **PlatformIO** = professional alternative (better dependency mgmt + multi-MCU support) — แต่เรียนรู้ยากกว่า
- **ESP-IDF** = official Espressif framework (lower-level, faster) — overkill สำหรับ project นี้

---

### 🟦 4.2 Zigbee2MQTT (Z2M)

#### **คืออะไร?**
**Zigbee2MQTT (Z2M)** = open-source bridge ระหว่าง **Zigbee mesh network** กับ **MQTT broker**

**ทำงานยังไง:**
1. Z2M รันบน Pi (Docker)
2. Z2M ใช้ Sonoff Zigbee dongle (USB) คุย Zigbee mesh
3. เมื่อ Zigbee device ส่ง message → Z2M แปลเป็น MQTT message → publish ไป Mosquitto
4. เมื่อ HA สั่ง MQTT command → Z2M แปลเป็น Zigbee command → ส่งไป device

**Version:** 2.10.1 (2026)
**Web UI:** Port 8080 (Pi) — pair devices, view network map, debug

#### **ใช้ทำอะไรในโปรเจคนี้?**
- Pair ทุก Zigbee devices (6 ตัว):
  - Tuya bulb, 2× Tuya plugs, SNZB-03P, SNZB-05P, SNZB-04
- MQTT topics: `zigbee2mqtt/<friendly_name>/...`
- HA ใช้ MQTT integration อ่าน topics → สร้าง entity อัตโนมัติ

#### **ความสำคัญ:**
- **เปิด Zigbee ให้ HA เข้าถึงได้** — ถ้าไม่มี Z2M ต้องใช้ Tuya cloud หรือ Conbee Phoscon (proprietary, ช้า)
- **Open-source** — community-supported, รองรับ 4000+ Zigbee devices
- **Local processing** — Zigbee messages ไม่ต้องผ่าน cloud → fast + private

#### **ทำไม Z2M (ไม่ใช้ ZHA หรือ deconz)?**
- **ZHA** = HA built-in Zigbee integration — ใช้ dongle เดียวกันได้ แต่ device support น้อยกว่า, ปรับ config ยากกว่า
- **deconz** = Phoscon ecosystem (proprietary) — ต้องใช้ Conbee dongle เฉพาะ
- **Z2M** = most flexible + best device support

---

### 🟧 4.3 Home Assistant Core 2026.4.2

#### **คืออะไร?**
**Home Assistant (HA)** = open-source home automation platform — **เป็น central brain** ของ smart home

**Features:**
- **Entity model** — abstract every device as entity (sensor, switch, light, etc.)
- **Automation engine** — trigger + condition + action (YAML or UI-based)
- **Lovelace UI** — customizable dashboard
- **Integrations:** 3000+ (MQTT, Zigbee, Z-Wave, weather, smart speakers, etc.)
- **REST + WebSocket API** — เปิดให้ external tools เข้าถึง
- **Companion app** — iOS/Android (push, location)

**Version 2026.4.2:** ใหม่ที่สุดตอน defense

#### **ใช้ทำอะไรในโปรเจคนี้?**
- **Run on EC2 (Docker)** at `YOUR_EC2_IP:8123`
- **Centralize ทุก device + automation** — ESP32, Zigbee, Tuya, iOS, Alexa, LINE, Supabase, Met.no
- **87 automations** ทำงานที่นี่ — Smart Light, Pump, Scene, Mode, Notify, Safety
- **Dashboard** สำหรับ visualize + manual control
- **Webhook + API** เปิดให้ Lambda + n8n + ngrok เข้าถึง

#### **ความสำคัญ:**
- **THE BRAIN** — โดยไม่มี HA ไม่มี smart home logic
- **Single source of truth** — ทุก entity state, ทุก automation อยู่ที่นี่
- **Resilient** — ถ้า Anthropic API ดาวน์, HA ยังทำ automation ตามปกติ (เฉพาะ AI features หาย)
- **Production-grade** — used by millions, well-documented

#### **ทำไม HA (ไม่ใช้ OpenHAB / Domoticz)?**
- **OpenHAB** = Java-based, ใช้ DSL (Eclipse), community เล็กกว่า
- **Domoticz** = older, less integrations, basic UI
- **HA** = largest community + best UX + most integrations (3000+)

#### **เปรียบเทียบกับ commercial alternatives:**
- **Apple HomeKit** = closed ecosystem, requires HomePod/Apple TV, lock-in
- **Google Home** = Google account required, less customizable
- **Amazon Alexa** = ใช้คู่กันได้ (เรา bridge HA + Alexa) แต่ standalone limited
- **HA** = best for power users + DIY + privacy

---

### 🟧 4.4 Mosquitto MQTT Broker

#### **คืออะไร?**
**Mosquitto** = open-source MQTT broker (server-side)
**MQTT** = Message Queuing Telemetry Transport — lightweight pub/sub protocol สำหรับ IoT

**Pub/Sub model:**
```
Publisher → Topic → Subscriber(s)
ESP32 ──publishes "home/bedroom/temp"──> Mosquitto ──forwards──> HA
HA ──publishes "home/bedroom/light"────> Mosquitto ──forwards──> ESP32
```

**Features:**
- **Retained messages** — last message kept by broker → new subscriber gets immediately
- **QoS levels:** 0 (fire-forget), 1 (at least once), 2 (exactly once)
- **Persistent** — saves state to disk
- **Authentication** — username/password (เราใช้)

#### **ใช้ทำอะไรในโปรเจคนี้?**
- **Run on EC2** (Docker), port 1883
- **Central message bus** — ESP32 ทุกตัว + Pi (Z2M) + HA + n8n communicate ผ่าน Mosquitto
- **Retained messages ON** — เมื่อ ESP32 reboot, Mosquitto ส่ง last state ทันที (เห็นแสง ทันทีโดยไม่ต้องรอ sensor ใหม่)

#### **ความสำคัญ:**
- **Central nervous system** — ทุก message วิ่งผ่านที่นี่
- **Lightweight** — กิน RAM <50MB, handle thousands of messages/sec
- **Resilient** — ถ้า Mosquitto reboot, ESP32 auto-reconnect (PubSubClient handles)

#### **ทำไม MQTT (ไม่ใช้ HTTP/REST)?**
- **HTTP** = stateless, ต้อง poll → ไม่ efficient + ช้า
- **MQTT** = pub/sub → real-time push, ใช้ bandwidth น้อย
- **Designed for IoT** — มาตรฐาน de-facto สำหรับ device communication

#### **ทำไม Mosquitto (ไม่ใช้ EMQX / VerneMQ)?**
- **EMQX** = enterprise-grade, รองรับ million connections (overkill บ้าน)
- **VerneMQ** = clustered, Erlang-based (complex)
- **Mosquitto** = simple, light, perfect for home use + HA standard

---

### 🟧 4.5 n8n on Railway

#### **คืออะไร?**
**n8n** = open-source workflow automation tool (like Zapier/Make.com แต่ self-hosted)

**Features:**
- **Visual workflow editor** — drag-and-drop nodes
- **400+ integrations** — HTTP, MQTT, databases, Slack, email, etc.
- **Code nodes** — embed JavaScript/Python for custom logic
- **Webhook trigger** — receive HTTP POST → trigger workflow
- **Schedule trigger** — cron jobs

**Railway** = hosting platform (like Heroku) — deploy apps ด้วย git push หรือ template
- **Plan:** Hobby $5/mo = 500 hours compute + 1GB RAM

#### **ใช้ทำอะไรในโปรเจคนี้?**
- **3 active workflows:**
  1. **Daily Report (7 AM Bangkok)** — query HA → format report → push to LINE
  2. **HA Alert to LINE** — webhook from HA `rest_command.send_alert` → push to LINE
  3. ~~ChAvee main~~ — **bypassed** (Railway cookie bug → ngrok solution)

#### **ความสำคัญ:**
- **Cron + workflow without writing server code** — quick to prototype
- **Visual** — debug ง่ายมาก (เห็น flow + data flow)
- **Webhook receiver** — ทำให้ HA push event ไป LINE ได้แบบ simple

#### **ทำไม n8n (ไม่ใช้ Zapier/Make.com)?**
- **Zapier** = SaaS, ราคาแพง ($30+/mo), task quota จำกัด
- **Make.com** = SaaS, ดีกว่า Zapier แต่ยังเสีย $
- **n8n self-hosted** = $5/mo total + unlimited tasks + own data
- **Open-source** = no vendor lock-in

#### **ทำไม Railway (ไม่ใช้ deploy on EC2)?**
- **EC2 RAM 2GB เต็มเร็ว** = HA + Mosquitto + n8n ก็เกิน
- **Railway** = separate hosting + auto-restart + git-based deploy
- **$5/mo คุ้มมาก** (เทียบกับ DIY VPS ของอื่น)

#### **ChAvee Bypass Story:**
- เดิม: ChAvee main workflow รัน n8n
- 26 พ.ค.: ต้อง update HOME_CONTEXT (16KB context) → n8n update API คืน 401 (Railway cookie bug)
- ลอง: REST API, Pinia store inject, CodeMirror inject, manual UI → **ทุกอย่าง fail**
- Solution: **bypass n8n** → ใช้ ngrok HTTPS tunnel + HA-native webhook → call Anthropic API จาก HA โดยตรง
- **Result:** ดีกว่าเดิม — HA states fresh ทุก call, ไม่ต้องเก็บ context

---

### 🟧 4.6 Supabase (Postgres + Auth + Realtime)

#### **คืออะไร?**
**Supabase** = Open-source Firebase alternative — Postgres database + Auth + Realtime + Storage as a service

**Features:**
- **Postgres 15** — full SQL database
- **REST API auto-generated** จาก schema
- **Realtime subscriptions** — WebSocket pub/sub
- **Authentication** — email, OAuth, magic link
- **Storage** — S3-compatible
- **Plan:** Free tier — 500MB DB, 50,000 MAU

#### **ใช้ทำอะไรในโปรเจคนี้?**
- **Postgres tables:**
  - `chat_logs` — บันทึก LINE message + ChAvee reply (timestamp, user_id, text, sentiment, intent)
  - `mode_changes` — บันทึก house/comfort mode changes (mode_from, mode_to, source, timestamp)
- **Access:** HA via REST sensor, n8n via Postgres node

#### **ความสำคัญ:**
- **Conversation memory** — ChAvee สามารถ "จำ" ได้ว่าเคยคุยอะไร (ดู last 10 messages)
- **Behavior analytics** — รู้ว่า user เปลี่ยน mode บ่อยช่วงไหน → future predictive automation
- **Defense talking point** — "บ้านนี้มี database analytics"

#### **ทำไม Supabase (ไม่ใช้ self-hosted Postgres หรือ Firebase)?**
- **Self-hosted Postgres** = ต้อง manage backup + security + scaling
- **Firebase** = Google ecosystem lock-in, NoSQL only (Firestore)
- **Supabase** = full SQL + free tier + open-source
- **Free tier เพียงพอ** สำหรับ ~100k message/mo

---

### 🟧 4.7 AWS Lambda (Alexa Skill Backend)

#### **คืออะไร?**
**AWS Lambda** = serverless compute — รัน code โดยไม่ต้อง manage server
- Pay-per-execution — เสียเฉพาะตอน function ทำงาน
- Auto-scale — handle 1 หรือ 1 ล้าน requests พร้อมกัน

**Why "Lambda" for Alexa?**
- Alexa Skill Service ต้องการ HTTPS endpoint หรือ Lambda ARN เพื่อ invoke skill
- Lambda เป็น standard pattern ของ Alexa development (Amazon ecosystem)

#### **ใช้ทำอะไรในโปรเจคนี้?**
- **Function name:** `home-control`
- **Region:** us-east-1
- **Runtime:** Python 3.11
- **Memory:** 512MB
- **Timeout:** 8 sec (Alexa max)
- **Code:** `/Project_1/alexa/lambda_function.py` (32KB)
- **HOME_CONTEXT embedded:** 242 lines / 16KB
- **Flow:**
  1. Alexa skill invoked → Lambda receives intent JSON
  2. Lambda calls Anthropic Claude Haiku 4.5 with HOME_CONTEXT + user intent
  3. Claude returns structured action (service + entity_id + params)
  4. Lambda calls HA REST API → execute action
  5. Lambda returns speech response → Alexa speaks

#### **ความสำคัญ:**
- **Alexa skill ต้องการ backend** — Lambda เป็นที่ logic อยู่
- **HOME_CONTEXT comprehensive** — Claude รู้ทุก device + automation + threshold
- **AI-powered** — เข้าใจ natural language แม้คำสั่งไม่ตรง

#### **ทำไม Lambda (ไม่ใช้ HTTP endpoint บน EC2)?**
- **Lambda** = $0 cost (free tier 1M req/mo) + auto-scale
- **EC2 endpoint** = ต้อง expose port 443, manage SSL cert, write own server
- **Lambda + Alexa = standard pattern** — well-documented

---

### 🟧 4.8 Alexa Custom Skill

#### **คืออะไร?**
**Alexa Custom Skill** = ส่วนขยายของ Alexa ที่ developer สามารถสร้างเองได้
- **Invocation name:** "home control" → "Alexa, ask home control to ..."
- **Interaction Model:** JSON ที่อธิบาย intents + utterances + slots

#### **ใช้ทำอะไรในโปรเจคนี้?**
- **23 custom intents:**
  - TurnOn, TurnOff, SetColor, SetScene, GetTemperature, LockDoor, OpenGarage, SetMode, etc.
- **EN-US locale** (Thailand Amazon account)
- **Interaction Model:** `/Project_1/alexa/interaction_model.json`
- **Backend:** Lambda `home-control`

#### **ความสำคัญ:**
- **English voice control** ของบ้าน
- **Defense feature** — committee เห็น/ได้ยิน Alexa พูดตอบ → impressed
- **Bridge to Thai** — ผ่าน Claude AI ที่อยู่ใน Lambda → "Alexa, tell home control เปิดโหมดปาร์ตี้"

#### **Echo Dot Workaround:**
- Echo Dot บน user's primary Amazon account (Alexa+ Early Access) **บล็อก custom dev skills**
- **Workaround:** สร้าง Amazon account ใหม่ (`poy0929838482@gmail.com`) — Thailand location = no Alexa+ → custom skill ทำงานได้
- ใช้ **Beta Test invite** เพื่อแชร์ skill ระหว่าง accounts

#### **ทำไม Custom Skill (ไม่ใช้ Smart Home Skill)?**
- **Smart Home Skill** = ใช้ pattern fix (turn on/off, set level) — ไม่รองรับ AI reasoning
- **Custom Skill** = freedom เต็มที่ — ใช้ Claude AI ได้
- **Trade-off:** Custom Skill ต้องพูด "Alexa, ask home control to ..." (Smart Home Skill = "Alexa, turn on ...")

---

### 🟪 4.9 Anthropic Claude Haiku 4.5 (AI Brain) ⭐

#### **คืออะไร?**
**Claude** = LLM (Large Language Model) จาก **Anthropic** (founded by former OpenAI members)
**Haiku 4.5** = รุ่นใหม่+เร็ว+ราคาดี (2026 release)

**Comparison:**
- **Claude Opus 4** = สมาร์ทสุด, ช้า, แพง (~10x ของ Haiku)
- **Claude Sonnet 4** = balanced
- **Claude Haiku 4.5** ⭐ = fastest + cheapest + smart enough

**Spec Haiku 4.5:**
- **Speed:** ~1-2 sec response (vs Opus ~5-10 sec)
- **Cost:** ~$1/M input tokens, ~$5/M output (rough)
- **Context window:** 200,000 tokens
- **Multilingual:** Thai + English + 100+ languages

#### **ใช้ทำอะไรในโปรเจคนี้?**

**ChAvee LINE Bot (Thai):**
- User: "เปิดไฟห้องนั่งเล่นสีฟ้า"
- HA webhook (via ngrok) → call Anthropic API with:
  - System prompt: HOME_CONTEXT + persona ชาวี
  - User message
  - HA current states (fresh — read every call)
- Claude returns: `{ actions: [{ service: "light.turn_on", entity: "light.living_room_light", color: "blue" }] }`
- HA executes → light goes blue
- Claude returns Thai reply → LINE Push API

**Alexa Skill (English):**
- User: "Alexa, ask home control to set kitchen fan to high"
- Lambda → call Anthropic API with HOME_CONTEXT + intent
- Claude returns action JSON
- Lambda → HA REST API → execute
- Lambda returns voice response → Alexa speaks

#### **ความสำคัญ:**
- **AI Brain** ของระบบ — เข้าใจภาษาธรรมชาติ
- **Multi-action parsing** — "เปิดไฟห้องนอน + ปิดไฟห้องครัว" → 2 service calls
- **Out-of-scope handling** — ถามกลับสั้นๆ ไม่ crash
- **Personality** — ชาวีสุภาพ, polite, helpful
- **Defense killer feature** — committee impressed ที่บ้านเข้าใจคำสั่งซับซ้อน

#### **ทำไม Claude (ไม่ใช้ GPT-4 / Gemini)?**
- **GPT-4** = ดีกว่าในบาง task แต่แพงกว่ามาก (~10x)
- **Gemini Pro** = ราคาดีแต่ Thai language ยังไม่เท่า Claude
- **Claude Haiku 4.5** = Thai language ดีมาก + ราคาประหยัด + speed เร็ว → perfect for chat

#### **Cost:**
- ~฿0.05/turn (ค่าเฉลี่ย)
- ~฿200-400/mo (ขึ้นกับ usage)
- Free tier ไม่มี (Anthropic pay-per-use)

---

### 🟪 4.10 LINE Messaging API (Bot @457uecsu — ชาวี)

#### **คืออะไร?**
**LINE Messaging API** = API ของ LINE สำหรับ developer สร้าง chatbot
- **Webhook** — LINE ส่ง POST request ไป endpoint เมื่อมี user message
- **Push API** — bot ส่ง message กลับไป user
- **Plans:**
  - Free: 1,000 push msg/mo + unlimited reply
  - Basic ฿1,284/mo: 15,000 push msg ⭐ (เราใช้)
  - Pro ฿4,284/mo: 75,000 push msg

#### **ใช้ทำอะไรในโปรเจคนี้?**
- **Bot:** ชาวี (ChAvee) @457uecsu
- **Channel access token:** สำหรับ authentication
- **Webhook URL:** ngrok HTTPS tunnel → HA-native automation
- **Flow:**
  1. User chat → LINE → webhook to HA (via ngrok)
  2. HA automation parses event → calls Claude API
  3. Claude returns reply + actions
  4. HA executes actions + calls LINE Push API → reply to user

#### **ความสำคัญ:**
- **Primary UI** — Thai users ใช้ LINE ทุกคน
- **Push notifications + chat ในที่เดียว**
- **Defense feature** — chat with home in Thai ผ่าน LINE
- **15k msg/mo plan** = พอเหลือใช้

#### **Quota Crisis (26 พ.ค.):**
- ตอนเช้า defense day — quota หมด (300/300)
- ChAvee เงียบไม่ตอบ
- Diagnosis: `curl /v2/bot/message/quota/consumption` → 300/300
- **Fix:** Upgrade Basic plan ฿1,284 → 15,000 msg
- **Lesson:** monitor quota proactively, free tier ไม่พอจริงๆ

#### **ทำไม LINE (ไม่ใช้ Telegram / WhatsApp)?**
- **LINE** = #1 messaging app ในไทย (~50M MAU)
- **Telegram** = น้อยคน Thai users
- **WhatsApp** = Facebook-owned, API ซับซ้อนกว่า
- **LINE Bot integration well-documented**

---

### 🟪 4.11 ngrok (HTTPS Tunnel) ⭐

#### **คืออะไร?**
**ngrok** = tunneling service — exposes local server to Internet via secure HTTPS URL
- Local: `localhost:8123` → Public: `https://abc.ngrok.io`
- HTTPS cert + DDoS protection + custom domain (paid)

#### **ใช้ทำอะไรในโปรเจคนี้?**
- **Critical role on defense day** — bypass n8n cookie bug
- **Setup:**
  - ngrok ทำงานบน laptop/Pi (?) — tunnel ไปยัง HA on EC2 (YOUR_EC2_IP:8123)
  - LINE webhook URL = `https://<ngrok>/api/webhook/chavee`
  - HA webhook automation รับ event → trigger Claude call → execute

#### **ความสำคัญ:**
- **ChAvee LINE Bot ทำงานได้ 100% on defense** เพราะ ngrok bypass
- ทำให้ ChAvee LINE → HA โดยตรง — ไม่ต้องผ่าน n8n ที่มี bug
- **HTTPS required by LINE** — HA on EC2 ไม่มี SSL (port 8123 HTTP)
- ngrok ให้ HTTPS frontend ฟรี

#### **Trade-off:**
- **Free ngrok** = URL เปลี่ยนทุกครั้งที่ restart → ต้อง update LINE webhook URL ใหม่
- **Paid ngrok** ($8/mo) = custom static URL → no URL change
- **Future fix:** Cloudflare Tunnel (free, static URL, official) — production-grade replacement

#### **ทำไม ngrok (ไม่ใช้ Cloudflare Tunnel ตั้งแต่แรก)?**
- **ngrok = fast to set up** — `ngrok http 8123` → done in 30 sec
- **Cloudflare Tunnel** ต้องเชื่อม domain + DNS — มีขั้นตอนมากกว่า
- **Defense day = ngrok เพียงพอ** — Cloudflare migration เก็บไว้ post-defense

---

### 🟫 4.12 iOS HA Companion App

#### **คืออะไร?**
**Home Assistant Companion** = official iOS/Android app ของ HA
- **Native push notifications** (ไม่ใช่ web push)
- **Location tracking** (background GPS)
- **Sensors** — battery, network, audio output, etc.
- **Actions** — call HA service จาก app

#### **ใช้ทำอะไรในโปรเจคนี้?**
- **Device:** Maradosx iPhone (Mos's phone)
- **Notification channel:** `notify.mobile_app_maradosx`
- **Push priority:** Critical (bypass silent mode สำหรับ gas/leak/fall/intrusion)
- **GPS tracking:** `person.mos` — เปลี่ยน house mode อัตโนมัติ (4m zone radius)

#### **ความสำคัญ:**
- **One of 3 notification channels** — backup ถ้า LINE quota หมดหรือ phone silent
- **GPS auto-mode** — ออกจากบ้าน → Away mode auto
- **Critical alerts** — ดังแม้ phone silent (life safety)
- **Free + unlimited** — no quota limit

#### **Setup:**
- App Store → Home Assistant
- Login → YOUR_EC2_IP:8123
- Allow location access (always)
- Add `notify.mobile_app_maradosx` to HA scripts

---

### 🟫 4.13 Met.no Weather API

#### **คืออะไร?**
**Met.no** = Norwegian Meteorological Institute — official weather agency ของ Norway
- **Free + unlimited** API access
- Global forecast data (yr.no engine)
- No API key required (rate-limited)

#### **ใช้ทำอะไรในโปรเจคนี้?**
- HA integration: `weather.met_no_forecast_home`
- **Pump auto-skip on rainy** — ถ้า forecast = rainy → ไม่เปิด pump (ประหยัดน้ำ + ไม่ทำให้รากเน่า)
- **Outdoor light logic** — sunrise/sunset times → ปรับ scene

#### **ความสำคัญ:**
- **Weather-aware automations** — รดน้ำ + เปิดไฟภายนอกฉลาดขึ้น
- **Free** — no AccuWeather quota worry

---

### 🟫 4.14 Docker

#### **คืออะไร?**
**Docker** = containerization platform — รัน application ใน isolated environment

**Why Docker:**
- **Dependency hell ไม่มี** — ทุก library เก็บใน container
- **Portable** — ทำงานบน OS อะไรก็ได้
- **Easy updates** — `docker pull <image>:latest`
- **Isolated** — ถ้า container 1 crash, ตัวอื่นไม่กระทบ

#### **ใช้ทำอะไรในโปรเจคนี้?**
- **EC2:** docker-compose runs `homeassistant` + `mosquitto`
- **Pi:** docker runs `zigbee2mqtt` + `portainer`
- **`docker-compose.yml`** ใน Project_1 root

#### **ความสำคัญ:**
- **Production deployment standard** — modern way to run servers
- **Reproducible** — clone repo → docker-compose up → done
- **Auto-restart** — `restart: always` policy ทำให้ HA restart ถ้า crash

---

## 5. Protocols & Concepts Explained

> 💡 อธิบาย protocols + concepts สำคัญที่ใช้ในโปรเจค สำหรับคนที่ไม่คุ้น

---

### 🔌 5.1 MQTT (Message Queuing Telemetry Transport)

#### **คืออะไร?**
**MQTT** = lightweight pub/sub messaging protocol ที่ออกแบบมาสำหรับ IoT
- **Pub/Sub model** — sender + receiver ไม่รู้จักกัน, สื่อสารผ่าน topic
- **Topic-based** — `home/bedroom/temperature` (hierarchy)
- **Broker** = ตัวกลาง (Mosquitto) ที่จัดเก็บ + forward messages

#### **ทำไมใช้ MQTT (ไม่ใช้ HTTP)?**
- **HTTP** = request/response, stateless → ต้อง poll → wasteful
- **MQTT** = persistent connection, push → real-time + low bandwidth
- **Designed for IoT** — สำหรับ ESP32 ที่ RAM/Bandwidth จำกัด

#### **Key concepts:**
- **Topic:** เหมือน channel — `home/<room>/<sensor>`
- **Publish:** ส่ง message ไป topic
- **Subscribe:** ฟัง message จาก topic
- **Retained message:** broker เก็บ last message → new subscriber รับทันที
- **QoS:** Quality of Service (0/1/2)

---

### 📡 5.2 Zigbee Protocol

#### **คืออะไร?**
**Zigbee** = wireless mesh networking protocol สำหรับ home automation
- **Frequency:** 2.4GHz (เหมือน Wi-Fi แต่ใช้ channel ต่างกัน)
- **Low power** — battery devices ใช้ได้ 1-2 ปี
- **Mesh** — devices ขยาย range ของ network (mains-powered devices = router)

#### **ใน project นี้:**
- Tuya bulb + plugs = Zigbee router (mains-powered, repeat signal)
- SNZB-03P/05P/04 = end devices (battery, sleep most of time)
- Pi + Sonoff dongle = coordinator (สร้าง + manage network)

#### **ทำไม Zigbee (ไม่ Wi-Fi smart devices)?**
- **Battery-powered devices** ใช้ Zigbee อยู่ได้ 1-2 ปี (Wi-Fi กิน battery หมดใน 1 วัน)
- **Mesh = range ขยาย** — ใส่ device เยอะ → network ครอบคลุมบ้านดีขึ้น
- **ไม่กิน Wi-Fi bandwidth**

---

### 🌐 5.3 REST API

#### **คืออะไร?**
**REST (Representational State Transfer)** = architectural style ของ web API
- **HTTP methods:** GET (read), POST (create), PUT (update), DELETE
- **URL = resource:** `https://YOUR_EC2_IP:8123/api/states/light.bedroom_light`
- **JSON body** = data exchange format

#### **ใน project นี้:**
- Lambda → HA REST API → execute service
- n8n → HA REST API → query states
- HA REST API token (long-lived) → auth

---

### 🔌 5.4 WebSocket

#### **คืออะไร?**
**WebSocket** = persistent bi-directional connection (vs HTTP request/response)
- Open once → stays open → both sides push messages
- Better than HTTP polling

#### **ใน project นี้:**
- HA WebSocket API: `ws://YOUR_EC2_IP:8123/api/websocket`
- ใช้ตอน mutate dashboard config (lovelace/config/save) — bulk update
- Real-time entity state updates

---

### 🔐 5.5 HTTPS + SSL/TLS

#### **คืออะไร?**
- **HTTP** = unencrypted (anyone can read)
- **HTTPS** = HTTP + SSL/TLS (encrypted, authenticated)

#### **ใน project นี้:**
- LINE Bot **บังคับ HTTPS** สำหรับ webhook URL
- HA on EC2 = HTTP only (port 8123) — internal use
- **ngrok** = แปลง HTTP HA → HTTPS public URL (solve LINE requirement)

---

### 🪝 5.6 Webhook

#### **คืออะไร?**
**Webhook** = "reverse API" — server A ส่ง POST ไป server B's URL เมื่อมี event
- Push-based (ไม่ต้อง poll)
- เร็วกว่า + ประหยัด bandwidth

#### **ใน project นี้:**
- LINE → webhook → ngrok → HA (user message arrived)
- HA → webhook → n8n (alert event)

---

### 🔄 5.7 PWM (Pulse Width Modulation)

#### **คืออะไร?**
**PWM** = วิธี control analog output ผ่าน digital signal
- Switch HIGH/LOW เร็วๆ → duty cycle 50% = average voltage half
- Used for: LED brightness, servo position, motor speed

#### **ใน project นี้:**
- ESP32 GPIO 18 → servo SG90 (front door, garage)
- 50Hz period (20ms), pulse 1ms = 0°, 2ms = 180°

---

### 📍 5.8 ADC (Analog-to-Digital Converter)

#### **คืออะไร?**
**ADC** = แปลง analog voltage → digital number
- ESP32 ADC = 12-bit → 0-4095 range
- Voltage 0-3.3V → ADC 0-4095

#### **ใน project นี้:**
- LDR (light) → GPIO 34 → ADC 0-4095
- MQ-2 (gas) → GPIO 35 → ADC 0-4095
- Soil moisture → GPIO 35 → ADC 0-4095

---

## 6. Architecture — 3-Layer End-to-End

```
╔══════════════════════════════════════════════════════════════════════╗
║                      USERS / INTERFACES                              ║
║                                                                      ║
║   📱 LINE "ชาวี"      🎤 Echo Dot Alexa     💻 HA Dashboard          ║
║   (Thai chat)         (English voice)       (visual control)         ║
║                                                                      ║
║   📲 iOS HA Companion (push notify channel + GPS tracker)            ║
╚══════════════════════╤═══════════════╤════════════════╤══════════════╝
                       │               │                │
       (HTTPS via ngrok)│   (HTTPS)     │  (REST API)    │ (LAN)
                       │               │                │
                       ▼               ▼                ▼
       ┌───────────────────┐  ┌──────────────────┐
       │ ngrok HTTPS tunnel│  │ AWS Lambda      │
       │      ↓            │  │ "home-control"  │
       │ HA-native webhook │  │ (us-east-1)     │
       │      ↓            │  │   ↓             │
       │ Anthropic Claude │  │ Anthropic Claude│
       │ Haiku 4.5 (REST) │  │ Haiku 4.5       │
       │      ↓            │  │   ↓             │
       │ Execute HA svc    │  │ Execute HA svc  │
       └────────┬─────────┘   └────────┬────────┘
                │                       │
                └──────────┬────────────┘
                           ▼
╔══════════════════════════════════════════════════════════════════════╗
║                  CLOUD LAYER (EC2 t3.small)                          ║
║                  ─────────────────────────                           ║
║                                                                      ║
║   ┌──────────────────────────────────────────────────────────┐       ║
║   │  HOME ASSISTANT CORE 2026.4.2  (Docker)                  │       ║
║   │  IP: YOUR_EC2_IP:8123                                     │       ║
║   │                                                            │       ║
║   │  • 230 entities                                            │       ║
║   │  • 87 automations (86 ON)                                  │       ║
║   │  • 7 scripts                                               │       ║
║   │  • 9 Lovelace views                                        │       ║
║   │  • Met.no + Supabase + Companion integrations              │       ║
║   └────────────────────────┬─────────────────────────────────┘       ║
║                            │ (MQTT)                                   ║
║   ┌────────────────────────▼─────────────────────────────────┐       ║
║   │  MOSQUITTO MQTT BROKER  (Docker, port 1883)              │       ║
║   │  • Retained messages ON                                   │       ║
║   │  • Persistent storage                                     │       ║
║   │  • Username/password auth                                 │       ║
║   └────────────────────────┬─────────────────────────────────┘       ║
║                            │                                          ║
║   Side services:           │                                          ║
║   • n8n on Railway ($5/mo) — daily report + HA alert workflows       ║
║   • Supabase (Free) — chat_logs + mode_changes for analytics         ║
║   • Met.no Weather (Free) — pump auto-skip on rainy                  ║
╚════════════════════════════╤═════════════════════════════════════════╝
                             │ (MQTT over LAN/Tailscale)
                             ▼
╔══════════════════════════════════════════════════════════════════════╗
║                       EDGE LAYER                                     ║
║                                                                      ║
║  ┌────────────────────────────────────────────────────────────┐      ║
║  │   RASPBERRY PI 3B+                                         │      ║
║  │   IP: 172.20.10.4 / 192.168.1.155                          │      ║
║  │   ┌──────────────────────────────────────────┐             │      ║
║  │   │ Z2M v2.10.1 (Docker)                     │             │      ║
║  │   │   ↓ (Zigbee mesh via Sonoff dongle)      │             │      ║
║  │   │ • Tuya bulb (RGB+CCT)                    │             │      ║
║  │   │ • Tuya plug × 2 (fan, purifier)          │             │      ║
║  │   │ • SNZB-03P (motion living)               │             │      ║
║  │   │ • SNZB-05P (water leak bathroom)         │             │      ║
║  │   │ • SNZB-04 (front door contact)           │             │      ║
║  │   └──────────────────────────────────────────┘             │      ║
║  └────────────────────────────────────────────────────────────┘      ║
║                                                                      ║
║  ┌────────────────────────────────────────────────────────────┐      ║
║  │   ESP32 × 5 ZONES (MQTT direct to EC2 Mosquitto)           │      ║
║  │                                                             │      ║
║  │   #1 BEDROOM      PIR / DHT22 / LDR / LED relay            │      ║
║  │   #2 LIVING       PIR / DHT22 / LDR / Servo lock (door)    │      ║
║  │   #3 KITCHEN      PIR / DHT22 / LDR / MQ-2 / Fan relay     │      ║
║  │   #4 BATH/GARDEN  PIR / DHT22 / LDR / LD2410 / soil / pump │      ║
║  │   #5 GARAGE       PIR / LDR / Garage servo                  │      ║
║  └────────────────────────────────────────────────────────────┘      ║
╚══════════════════════════════════════════════════════════════════════╝
```

### 🔁 Data Flow Patterns

#### **Sensor → Automation Flow (Auto)**
```
Sensor (ESP32 or Zigbee)
   ↓ MQTT publish
Mosquitto (EC2)
   ↓ retained message
Home Assistant entity state
   ↓ trigger automation
HA Automation engine
   ↓ condition check + delay
HA service call (e.g., light.turn_on)
   ↓ MQTT publish or Zigbee command
Actuator (ESP32 relay OR Zigbee bulb/plug)
```

#### **LINE Bot Flow (ChAvee — POST-defense ngrok bypass)**
```
User types in LINE: "เปิดไฟห้องนั่งเล่นสีฟ้า"
   ↓ LINE webhook event
ngrok HTTPS tunnel
   ↓ POST to HA webhook endpoint
HA automation triggered
   ↓ Python script in automation reads HA states
   ↓ Build prompt with current device states + HOME_CONTEXT
   ↓ HTTP POST to Anthropic API (Claude Haiku 4.5)
   ↓ Claude returns: { actions: [{service: "light.turn_on", entity: "...", color: "blue"}] }
   ↓ HA service_call(...) for each action
   ↓ Side effect: Tuya bulb turns blue (via Zigbee mesh)
   ↓ HA automation calls LINE Push API to reply
LINE sends reply to user: "เปิดไฟสีฟ้าให้แล้วครับ ✅"
```

#### **Alexa Flow**
```
User says: "Alexa, ask home control to turn on bedroom light"
   ↓ Alexa wake word + ASR
Alexa Skill Service (custom skill "Home Control")
   ↓ Intent matched → Lambda invoke
AWS Lambda "home-control" (us-east-1)
   ↓ Read HOME_CONTEXT (embedded 16KB)
   ↓ Call Anthropic Claude Haiku 4.5
   ↓ Get structured action: light.turn_on(bedroom_light)
   ↓ HA REST API call (https://YOUR_EC2_IP:8123/api/services/light/turn_on)
   ↓ HA executes
   ↓ Lambda returns voice response
Alexa speaks: "Turning on bedroom light"
```

#### **Notification Fan-Out (3-channel resilient)**
```
HA Automation (e.g., gas alert)
   ↓
Parallel:
├── notify.mobile_app_maradosx → iOS push (instant, free, critical priority)
├── notify.persistent_notification → HA dashboard popup
└── rest_command.send_alert → n8n webhook → LINE Push
   ↓ User sees in 3 channels — if one fails, others still deliver
```

---

## 7. Home Assistant System (Detail)

### 🧱 Entities Breakdown (~230 total)

| Category | Count | Examples |
|---|---|---|
| `sensor.*` | ~80 | temperature, humidity, motion, ldr, gas, soil, battery |
| `binary_sensor.*` | ~30 | motion, door, leak, fall, gas_alert |
| `light.*` | ~6 | living_room_light, bedroom_light, etc. |
| `switch.*` | ~10 | fan, purifier, pump, relay |
| `lock.*` | 1 | front_door_lock |
| `cover.*` | 1 | garage_door |
| `weather.*` | 1 | met_no_forecast_home |
| `person.*` | 1 | mos (GPS-tracked) |
| `zone.*` | 1 | home (4m radius @ user current location) |
| `input_select.*` | 2 | house_mode (Home/Away/Security), comfort_mode (Normal/Warm/Sleep/Relax) |
| `input_boolean.*` | ~10 | party_loop_active, alert_flash_active, *_light_manual_override |
| `input_button.*` | 5 | scene_default/welcome/sunrise/calm/bedtime |
| `automation.*` | 87 | (see Section 11) |
| `script.*` | 7 | scene scripts (smooth lighting) |
| `notify.*` | 3 | mobile_app_maradosx, persistent_notification, send_alert (REST) |

### 🤖 Automations (87 total — 86 ON)

See Section 11 for complete breakdown by category.

### 🎬 Scripts (7 total)

1. `script.scene_default_smart` — warm 3000K @80%
2. `script.scene_welcome_smart` — rgb[255,180,100] @95% (warm orange)
3. `script.scene_sunrise_smart` — rgb[255,140,60] @70% (sunrise amber)
4. `script.scene_calm_smart` — rgb[60,140,255] @40% (soft blue)
5. `script.scene_bedtime_smart` — rgb[255,100,80] @15% (dim warm red)
6. `script.party_color_cycle_step` — single step of 5-color party loop
7. `script.alert_blink_step` — single blink red bright/dim

### 🎚️ Helpers

#### **input_select.house_mode**
- Options: `Home`, `Away`, `Security`
- Auto-trigger: GPS zone enter/leave (Mos's iPhone, 4m radius at user current location)
- Manual override: Alexa, LINE Bot, Dashboard button

#### **input_select.comfort_mode**
- Options: `Normal`, `Warm`, `Sleep`, `Relax`
- Affects: Light color/brightness/scene
- Combined with House mode for full control

#### **input_boolean (party + alert loops)**
- `party_loop_active` — ON → color cycle every 700ms forever
- `alert_flash_active` — ON → red blink every 400ms forever
- Toggle off → loop stops

#### **input_boolean (manual overrides)**
- `bedroom_light_manual_override` — 30-min override after user manually toggles
- Same for living_room, bathroom, garage

#### **input_button (scene triggers)**
- `scene_default` — press → fire scene_default_smart
- Same for welcome, sunrise, calm, bedtime

---

## 8. Per-Room Breakdown (5 zones)

### 🛏️ ห้องนอน (Bedroom) — ESP32 #1

| Component | Type | GPIO | Topic / Entity |
|---|---|---|---|
| DHT22 | Temp + Humidity | GPIO 4 | `sensor.bedroom_temperature`, `sensor.bedroom_humidity` |
| PIR HC-SR501 | Motion | GPIO 5 | `binary_sensor.bedroom_motion` (Note: replaced with mmWave logic) |
| LDR | Light level | GPIO 34 (ADC) | `sensor.bedroom_ldr` |
| Relay 5V 1ch | Light control | GPIO 23 | `light.bedroom_light` |

**Auto behavior:**
- Motion + LDR ≥ 2800 (dark) + no manual override + mode != Away/Security → light on
- No motion for 5 min OR LDR < threshold (becomes bright) → light off (with 30s delay)

### 🛋️ ห้องนั่งเล่น (Living Room) — ESP32 #2 + Zigbee Tuya bulb

| Component | Type | GPIO / Zigbee | Topic / Entity |
|---|---|---|---|
| DHT22 | Temp + Humidity | GPIO 4 | `sensor.living_room_temperature` |
| LDR | Light level | GPIO 34 | `sensor.living_room_ldr` |
| Servo SG90 | Front door lock | GPIO 18 (PWM) | `lock.front_door_lock` |
| Tuya TS0505B | RGB+CCT bulb | Zigbee | `light.living_room_light` |
| Tuya TS011F #1 | Fan plug + power | Zigbee | `switch.living_room_fan`, `sensor.fan_power` |
| Tuya TS011F #2 | Purifier plug + power | Zigbee | `switch.air_purifier`, `sensor.purifier_power` |
| SNZB-03P | Motion (Zigbee) | Zigbee | `binary_sensor.living_room_presence` |
| SNZB-04 | Door contact (Zigbee) | Zigbee | `binary_sensor.front_door_contact` |

**Auto behavior:**
- Motion → Tuya bulb on (current comfort_mode color/temp)
- Mode change → light color matches mode
- Party scene → 5-color loop on Tuya
- Alert flash → red blink on Tuya

### 🍳 ห้องครัว (Kitchen) — ESP32 #3

| Component | Type | GPIO | Topic / Entity |
|---|---|---|---|
| DHT22 | Temp + Humidity | GPIO 4 | `sensor.kitchen_temperature` |
| PIR | Motion | GPIO 5 | `binary_sensor.kitchen_motion` |
| LDR | Light level | GPIO 34 | `sensor.kitchen_ldr` |
| MQ-2 | Gas/Smoke | GPIO 35 (ADC) | `sensor.kitchen_gas_lpg`, `binary_sensor.kitchen_gas_alert` |
| Relay | Exhaust fan | GPIO 23 | `switch.kitchen_fan` |

**Auto behavior:**
- Gas ≥ threshold → red alert flash + fan auto-on + LINE/Mobile/Persistent notify
- Motion + dark → kitchen light on (if light entity exists)
- ASR fuzzy match: "kitchen" ↔ "chicken/kitten" (Alexa understands all)

### 🚿 ห้องน้ำ + สวน (Bathroom + Garden) — ESP32 #4

| Component | Type | GPIO / Zigbee | Topic / Entity |
|---|---|---|---|
| DHT22 | Temp + Humidity | GPIO 4 | `sensor.bathroom_temperature` |
| LDR | Light level | GPIO 34 | `sensor.bathroom_ldr` |
| LD2410 mmWave | Presence + still | GPIO 25/26 (UART) | `binary_sensor.bathroom_presence`, `bathroom_still_5min` |
| Capacitive soil | Soil moisture | GPIO 35 (ADC) | `sensor.garden_soil_moisture` |
| Relay | Water pump | GPIO 22 | `switch.garden_pump` |
| SNZB-05P | Water leak (Zigbee) | Zigbee | `binary_sensor.bathroom_leak` |

**Auto behavior:**
- Leak detected → red flash + LINE/Mobile/Persistent notify (critical)
- Fall: presence ON + still ≥ 5min → bathroom light flash + LINE alert + auto-restore prev state
- Pump: soil < 45% + 06-20น + weather != rainy → on
- Pump auto-off: soil ≥ 60% → off
- Pump safety: max 10 min runtime → force off + critical alert

### 🚗 โรงรถ (Garage) — ESP32 #5

| Component | Type | GPIO | Topic / Entity |
|---|---|---|---|
| PIR | Motion | GPIO 5 | `binary_sensor.garage_motion` |
| LDR | Light level | GPIO 34 | `sensor.garage_ldr` |
| Servo SG90 | Garage door | GPIO 18 (PWM) | `cover.garage_door` |

**Auto behavior:**
- Motion + dark + mode != Away/Security → garage light (if entity)
- Garage door open via Alexa/LINE/Dashboard → servo rotates
- House mode = Away → close garage if open

---

## 9. Communication & Protocols

### 📡 Network Map

```
Internet ──┬── EC2 t3.small (YOUR_EC2_IP)
           │      ├── port 8123 (HA HTTP)
           │      ├── port 1883 (MQTT)
           │      └── port 22 (SSH)
           │
           ├── Railway (n8n) — YOUR_N8N_INSTANCE.up.railway.app
           ├── Supabase (free tier project)
           ├── AWS Lambda (us-east-1)
           ├── Anthropic API (api.anthropic.com)
           └── LINE Platform (api.line.me)

Home LAN ──┬── Router (192.168.1.1)
           ├── Pi 3B+ (192.168.1.155)
           │      └── Sonoff Zigbee dongle (USB)
           │             └── Zigbee mesh (channel 11)
           │
           ├── 5× ESP32 (DHCP from router)
           │      └── Wi-Fi 2.4GHz → router → Internet → EC2 MQTT
           │
           └── iPhone (HA Companion + GPS)
                  └── Cellular/WiFi → HA API
```

### 📨 MQTT Topics (Mosquitto on EC2:1883)

```
home/bedroom/temperature       sensor publish (every 30s)
home/bedroom/humidity          sensor publish
home/bedroom/motion            sensor publish
home/bedroom/ldr               sensor publish
home/bedroom/light/cmd         actuator subscribe

home/livingroom/temperature    sensor publish
home/livingroom/humidity       sensor publish
home/livingroom/motion         sensor publish
home/livingroom/ldr            sensor publish
home/livingroom/lock/cmd       servo subscribe

home/kitchen/temperature       sensor publish
home/kitchen/humidity          sensor publish
home/kitchen/motion            sensor publish
home/kitchen/ldr               sensor publish
home/kitchen/gas               sensor publish (analog ppm)
home/kitchen/fan/cmd           actuator subscribe

home/bathroom/temperature      sensor publish
home/bathroom/ldr              sensor publish
home/bathroom/fall             LD2410 still-detection publish
home/garden/soil               soil moisture publish
home/garden/pump/cmd           pump relay subscribe

home/garage/motion             sensor publish
home/garage/ldr                sensor publish
home/garage/door/cmd           garage servo subscribe

zigbee2mqtt/<friendly>/...     Zigbee device topics (from Pi)
zigbee2mqtt/bridge/...         Z2M control topics
```

### 🔌 API Endpoints

| Service | Endpoint | Auth | Used by |
|---|---|---|---|
| HA REST API | `http://YOUR_EC2_IP:8123/api/...` | Long-lived token | Lambda |
| HA WebSocket | `ws://YOUR_EC2_IP:8123/api/websocket` | Token | Dashboard mutations |
| HA Webhook (ngrok) | `https://<ngrok>/api/webhook/chavee` | Public | LINE Bot |
| LINE Messaging API | `https://api.line.me/v2/bot/...` | Channel access token | HA, n8n |
| Anthropic API | `https://api.anthropic.com/v1/messages` | API key | HA (via ngrok), Lambda |
| Supabase REST | `https://<project>.supabase.co/rest/v1/...` | Anon key | HA, n8n |
| n8n Webhook | `https://YOUR_N8N_INSTANCE.up.railway.app/webhook/...` | None | HA alerts → LINE |

---

## 10. AI System (ChAvee + Alexa) — Deep Dive

### 🧠 Shared Brain — Claude Haiku 4.5

ทั้ง LINE และ Alexa ใช้ **Claude Haiku 4.5 ตัวเดียวกัน** → personality + knowledge consistent

### 💬 ChAvee LINE Bot (Thai)

**Persona:** "ชาวี" — สุภาพ, แสดงอารมณ์ได้, multi-action, รู้ทุก device

**Capabilities (verified during defense):**
1. **Direct control** — "เปิดไฟห้องนอน", "ล็อคประตู", "ปิดพัดลม"
2. **Color/scene** — "เปลี่ยนไฟเป็นสีฟ้า", "เปิดโหมดปาร์ตี้", "ต้อนรับ"
3. **Sensor queries** — "อุณหภูมิห้องนั่งเล่นเท่าไหร่", "ดินแห้งไหม"
4. **Mode change** — "Sleep mode", "เปิดโหมด away"
5. **Multi-action** — "เปิดไฟห้องนอน + ปิดไฟห้องครัว" (parallel execution)
6. **Reasoning** — "ปั๊มทำงานทุกกี่นาที?" → อธิบาย state-based logic
7. **Emergency** — "มีคนบุก" → intrusion mode response
8. **Out-of-scope** — "นัดหมอ" → กระชับถาม "นัดเรื่องอะไรครับ" (don't crash)

**Why ngrok bypass:** n8n on Railway มี 401 cookie bug ที่บล็อก HOME_CONTEXT update — bypass แทน → Claude อ่าน HA states fresh ทุกครั้ง (always knows latest)

### 🎤 Alexa Echo Dot (English)

**23 Custom Intents:** TurnOn, TurnOff, SetColor, SetScene, GetTemperature, etc.

**Capabilities (verified during defense):**
1. **Light control** — "turn on living room light", "set kitchen light to blue"
2. **Scene** — "turn on party mode", "activate sunrise scene"
3. **Sensor query** — "what's the temperature in bedroom", "is the soil dry"
4. **Lock + Garage** — "lock front door", "open the garage door"
5. **Mode** — "set mode to away", "switch to bedtime"
6. **Thai bridge** — "tell home control เปิดโหมดปาร์ตี้" → Claude bridges Thai

**Workaround:** Echo Dot บน Alexa+ Early Access (primary account) จะ block custom dev skills → Login ด้วย Amazon account ใหม่ (Thailand location = no Alexa+) แทน → custom skill ทำงานได้

### 🗂️ HOME_CONTEXT (16KB embedded in Lambda + bypass webhook)

Includes:
- 5 ESP32 zones with GPIO + topics
- 6 Zigbee devices
- 24 NEW automations (26 พ.ค.)
- 5 input_buttons, 6 input_booleans
- 5 Scene Smart scripts
- 3 notification channels
- Sensor thresholds (LDR per-room, gas, soil)
- Intent patterns + fuzzy ASR map
- Disabled/deprecated entity list

### 🎭 Why "Shared Brain"?

**ปัญหาเดิม:** ถ้า LINE Bot กับ Alexa ใช้ AI คนละตัว:
- Personality ต่างกัน → user สับสน
- Capabilities ต่างกัน → ต้อง maintain 2 logics
- Knowledge ต่างกัน → 1 ตัว update ได้แต่อีกตัวไม่รู้

**Solution:** ทั้งคู่ใช้ Anthropic Claude Haiku 4.5 + HOME_CONTEXT เดียวกัน:
- LINE → ngrok → HA webhook → Anthropic
- Alexa → Lambda → Anthropic
- HOME_CONTEXT update → ทั้งคู่ได้ผลพร้อมกัน
- Personality consistency = ชาวี (Thai) / professional assistant (English)

---

## 11. Automations (87 รายการ) แยกหมวด

### 💡 Smart Light (8 — per-room PIR + LDR)
- `{bedroom,living_room,bathroom,garage}_smart_light_auto_on`
- `{bedroom,living_room,bathroom,garage}_smart_light_auto_off`
- Conditions: motion + LDR ≥ threshold + light off + override off + mode allows
- Auto-off: no motion 5min OR LDR < threshold for 30s

### 🌱 Pump Automation (3 — state-based)
- `pump_auto_on_dry_daytime_not_raining` — soil < 45% + 06-20น + weather != rainy → on
- `pump_auto_off_moisture_target_reached` — soil ≥ 60% → off
- `pump_safety_max_runtime_10_min` — 10min → force off + critical alert

### 🎉 Party + Alert Loops (4)
- `party_button_start_loop` — toggle on input_boolean.party_loop_active
- `party_off_button_stop_loop` — toggle off
- `party_loop_executor_color_cycle` — cycle 5 colors every 700ms while ON
- `alert_flash_red_blink_loop` — blink red bright/dim every 400ms while ON

### 🎬 Scene Smart (5 — input_button.press triggered)
- `scene_default_smart` (warm 3000K @80%)
- `scene_welcome_smart` (warm orange rgb[255,180,100])
- `scene_sunrise_smart` (amber rgb[255,140,60])
- `scene_calm_smart` (soft blue rgb[60,140,255])
- `scene_bedtime_smart` (dim warm red rgb[255,100,80])
- All: turn off party+alert booleans → 300ms → light.turn_on with transition 1s

### 🏠 Mode System (4 — house mode)
- `house_mode_home_setup` — restore comfort settings
- `house_mode_away_lockdown` — lock door, turn off non-essential
- `house_mode_security_intrusion_response` — door open + away/security → alert + flash + lock
- `house_mode_gps_auto_change` — Mos enters/leaves 4m zone

### 🎛️ Comfort Mode (4)
- `comfort_mode_normal` — daylight white
- `comfort_mode_warm` — warm 2700K
- `comfort_mode_sleep` — dim 10%
- `comfort_mode_relax` — soft blue + low

### 🔔 Notifications (8 — fire 3 channels)
- `mobile_house_mode_change`
- `mobile_comfort_mode_change`
- `mobile_gas_alert_all_channels` (CRITICAL + LED red)
- `mobile_water_leak_all_channels` (CRITICAL + LED red)
- `mobile_bathroom_fall_all_channels`
- `mobile_door_intrusion_away_security_mode`
- `mobile_garden_pump_state_change`
- `mobile_garage_door_state`

### 🚨 Safety Response (6)
- Gas detect → red flash + fan auto-on + 3-channel notify + block stove
- Water leak → red flash + 3-channel notify
- Bathroom fall → flash + 3-channel notify + auto-restore prev state
- Front door open + away/security → lock + flash + 3-channel notify
- Smoke (MQ-2 secondary) → same as gas
- Pump leak heuristic (soil decreasing while pump off) → notify

### 🌅 Daily/Time-based (5)
- 7 AM daily report → n8n → LINE
- Sunset → outdoor lights logic
- Bedtime (22:00) → suggest bedtime scene
- Wake (07:00) → suggest sunrise scene
- Hourly health check (HA self-monitor)

### 📲 Misc (45+)
- Tuya power monitoring → log to Supabase
- Mode-based scene snapshot/restore
- Webhook guards (LINE Bot self-heal)
- Test/debug automations (some disabled)

---

## 12. Modes + Scenes + Lighting

### 🏠 House Mode (3 options)

| Mode | Trigger | Behavior |
|---|---|---|
| **Home** | GPS enter 4m zone OR manual | Light comfort, motion-aware, default scene |
| **Away** | GPS leave 4m zone OR manual | Lights off, doors lock, alerts active |
| **Security** | Manual | Like Away + door open = intrusion + flash |

### 🎛️ Comfort Mode (4 options)

| Mode | Light behavior | Use case |
|---|---|---|
| **Normal** | Daylight white 5000K @100% | Day activities |
| **Warm** | Warm 2700K @70% | Evening / dining |
| **Sleep** | Dim warm @10% | Pre-sleep |
| **Relax** | Soft blue @40% | Calm / meditation |

### 🎬 Scene System (5 + 2 = 7 effects)

**Static scenes (input_button.press):**
1. **Default** — warm 3000K @80% (reset)
2. **Welcome** — warm orange (greeting)
3. **Sunrise** — amber rising warmth
4. **Calm** — soft blue (meditation)
5. **Bedtime** — dim warm red (sleep prep)

**Dynamic loops (input_boolean toggle):**
6. **Party** — 5-color cycle every 700ms forever (until off)
7. **Alert** — red blink bright/dim every 400ms forever (safety alert)

### 💡 Smart Light Logic (per room)

```python
if motion_detected AND
   LDR_value >= dark_threshold AND
   light_currently_off AND
   manual_override_off AND
   house_mode != "Away" AND
   house_mode != "Security" AND
   party_loop_active = off AND
   alert_flash_active = off:
       turn light on

if no_motion_5min OR
   LDR_value < dark_threshold (became bright) AND wait_30s:
       turn light off
```

**LDR Thresholds (per user UI):**
- `< 800` = สว่างจัด (bright)
- `< 1800` = สว่าง
- `< 2800` = ปกติ
- `>= 2800` = มืด (DARK = trigger)
- `>= 3600` = มืดมาก

**Per-room threshold:**
- Bedroom/Living/Garage: ≥ 2800
- Bathroom: ≥ 2200 (more sensitive — small room)

---

## 13. Notifications (3-Channel Resilient)

```
HA Automation (e.g., gas alert)
   │
   ├─►  notify.mobile_app_maradosx
   │       → iOS push (instant, free, critical priority)
   │       → ใช้ HA Companion app on iPhone
   │
   ├─►  notify.persistent_notification
   │       → HA Dashboard popup (always visible until cleared)
   │
   └─►  rest_command.send_alert
           → POST to n8n webhook
           → n8n → LINE Push API
           → LINE Bot "ชาวี" sends message
           → User sees in LINE (15k msg/mo plan)
```

**Why 3 channels:**
- iPhone อาจปิดเสียง / battery dead → LINE backup
- LINE quota หมด → mobile push ยังทำงาน
- HA dashboard popup ทำให้ดูประวัติย้อนหลังได้

**Fan-out timing:** ทั้ง 3 ช่อง fire พร้อมกันใน automation เดียว — ไม่มี delay ระหว่างช่อง

---

## 14. Safety Systems

### 🔥 Gas/Smoke (Kitchen MQ-2)
- **Sensor:** MQ-2 → analog ppm reading
- **Threshold:** > calibrated baseline + margin
- **Response (parallel):**
  1. Living room Tuya bulb → red blink (alert_flash_active)
  2. Kitchen exhaust fan → auto-on
  3. 3-channel notify (CRITICAL priority on iOS)
  4. (TODO) stove relay block — not wired in hardware

### 💧 Water Leak (Bathroom SNZB-05P)
- **Sensor:** Zigbee 2-pin contact → boolean
- **Detection:** Only when 2 copper pins shorted (NOT just damp cable)
- **Response (parallel):**
  1. Living bulb → red blink
  2. 3-channel notify (CRITICAL)

### 🚶 Fall Detect (Bathroom LD2410)
- **Sensor:** LD2410 mmWave radar (UART)
- **Heuristic:** presence ON + still detected ≥ 5 min → assume fall
- **Response:**
  1. Bathroom light flash (snapshot prev state first)
  2. 3-channel notify
  3. After 30 sec OR motion resumes → restore prev light state
- **Limitation:** Heuristic (not ML) — may false-positive during long showers
- **Future:** LD2451 upgrade for true ML-based fall detection

### 🚪 Intrusion (Front Door + Away/Security mode)
- **Trigger:** SNZB-04 door contact opens WHILE house_mode in [Away, Security]
- **Response (parallel):**
  1. Servo auto-lock front door
  2. Living bulb red flash
  3. 3-channel notify (CRITICAL)

### 🌱 Pump Safety
- **Max runtime:** 10 minutes → force off
- **If exceeded:** CRITICAL alert (suspect: leak / stuck valve / sensor fault)

---

## 15. Cost Breakdown

### 💰 Monthly Recurring

| Service | Plan | Cost (THB) | Cost (USD) | Notes |
|---|---|---|---|---|
| AWS EC2 t3.small | Free Tier | ฿0 | $0 | $114.50 credits remaining |
| AWS Lambda | Free Tier | ฿0 | $0 | 1M req + 400K GB-sec |
| Railway n8n Hobby | Hobby | ฿180 | $5 | 500 hr compute |
| Supabase | Free | ฿0 | $0 | 500MB DB, 50k MAU |
| **LINE Bot Basic** | Basic | **฿1,284** | ~$36 | 15,000 msg/mo (upgraded 26 พ.ค.) |
| Anthropic Claude Haiku 4.5 | Pay-per-use | ฿200-400 | $6-12 | ~฿0.05/turn |
| Met.no Weather | Free | ฿0 | $0 | Unlimited |
| Z2M (Pi local) | N/A | ฿0 | $0 | Self-hosted |
| ngrok | Free | ฿0 | $0 | Static URL on paid plan TBD |
| **TOTAL** |  | **฿1,664-1,864** | **~$47-53** | Comfortable budget |

### 💸 One-time Hardware Cost (Approximate)

| Item | Qty | Cost (THB) |
|---|---|---|
| ESP32 DevKit V1 | 5 | ~฿1,250 (฿250 ea) |
| Pi 3B+ | 1 | ~฿1,500 (used) |
| Sonoff Zigbee 3.0 dongle | 1 | ~฿800 |
| Tuya RGB+CCT bulb | 1 | ~฿450 |
| Tuya smart plug | 2 | ~฿700 (฿350 ea) |
| Sonoff Zigbee sensors (SNZB-03P/05P/04) | 3 | ~฿1,200 (฿400 ea) |
| DHT22 | 4 | ~฿320 (฿80 ea) |
| PIR HC-SR501 | 5 | ~฿250 (฿50 ea) |
| LDR + 10kΩ resistor | 5 | ~฿100 |
| MQ-2 | 1 | ~฿180 |
| LD2410 mmWave | 1 | ~฿250 |
| Capacitive soil | 1 | ~฿150 |
| Relay 5V 1ch | 5 | ~฿250 |
| Servo SG90 | 2 | ~฿200 |
| Power supplies + jumpers + breadboards | various | ~฿1,000 |
| **TOTAL** | | **~฿8,600** |

---

## 16. Bug-Fix Journey (12 bugs + ngrok bypass)

### 26 พ.ค. Sprint — 12+ Hours

#### **Bug 1: LINE Quota 300/300 Exhausted**
- **Symptom:** ChAvee ไม่ตอบ, LINE quota consumed
- **Diagnosis:** `curl /v2/bot/message/quota/consumption` → 300/300
- **Fix:** Upgrade LINE Basic ฿1,284/mo = 15,000 msg
- **Lesson:** monitor quota proactively

#### **Bug 2: Single notification channel fragile**
- **Symptom:** Important alerts missed when LINE down or phone silenced
- **Fix:** 3-channel fan-out (Mobile + Persistent + LINE) on 8 critical automations
- **Lesson:** redundancy > efficiency for safety-critical paths

#### **Bug 3: LDR Direction Wrong (3 iterations)**
- **Attempt 1:** "LDR > 2200 = DARK" — wrong, never triggered
- **Attempt 2:** "LDR < 500 = DARK" — wrong, lamp drops LDR itself
- **Attempt 3 (FINAL):** Found user UI mapping `>= 2800 = มืด`, `< 800 = สว่างจัด`
- **Lesson:** verify sensor direction via user-observable UI, not assumption

#### **Bug 4: Override-Detect Misfiring**
- **Symptom:** Manual override fired on API-triggered light changes (Claude calling light.turn_on)
- **Fix:** Disabled 4 override automations
- **TODO:** Re-enable with context filter (user vs automation)

#### **Bug 5: Pump Time-Based → Not Triggering**
- **Symptom:** Old `garden_pump_soil_moisture_40` time-based ไม่ทำงาน
- **Fix:** Rewrite as 3 state-based automations:
  - on: soil < 45% + 06-20น + weather != rainy
  - off: soil ≥ 60%
  - safety: 10 min max runtime
- **Lesson:** state-based > time-based for sensor-driven actuators

#### **Bug 6: Party Loop Stopped on HA**
- **Symptom:** Party mode color cycle only fired once
- **Fix:** input_boolean + executor automation chain (while toggle ON, loop forever)
- **Lesson:** HA scripts don't loop natively — need state machine

#### **Bug 7: Alert Blink Stuck on White**
- **Symptom:** Red blink works briefly, then white solid (Smart Light auto-on overrode it)
- **Fix:** Smart Light conditions add `alert_flash_active = off` AND `party_loop_active = off`
- **Lesson:** automation conditions must consider concurrent states

#### **Bug 8: Dashboard Entity Not Found (12+)**
- **Symptom:** Lovelace dashboard showed broken refs after entity rename
- **Fix:** Bulk patch via WebSocket lovelace/config/save + SCP for YAML mode
- **Lesson:** rename entities → patch dashboard immediately

#### **Bug 9: Stale Entities (4)**
- **Symptom:** Old entities `sensor.kitchen_gas` (vs new `_lpg`), `binary_sensor.bedroom_motion` (vs new mmWave)
- **Fix:** Disable 4 stale entities

#### **Bug 10: Legacy Adaptive Light Broken (4)**
- **Symptom:** Old `*_adaptive_light_motion_ldr` automations had stale refs
- **Fix:** Disable 4 legacy → replaced with new Smart Light system

#### **Bug 11: GPS Zone Duplicate + 7m Radius Too Big**
- **Symptom:** 2 zones (old + new), 7m radius triggered too early
- **Fix:** Delete old zone, set 4m at user current GPS location
- **Lesson:** small radius needed for accurate "at home" detection

#### **Bug 12: Comfort Mode No Light Change**
- **Symptom:** Switching to Sleep/Warm/Relax didn't change light
- **Fix:** New automation `comfort_mode_*` (4 modes) → triggers light scene

### 💎 BIG Fix: ChAvee n8n → ngrok HTTPS Bypass

- **Symptom:** ChAvee replies but doesn't execute (AI hallucination)
- **Root cause:** n8n on Railway has 401 cookie bug blocking workflow updates
- **Attempts that failed:**
  - REST API: `/api/v1/workflows/{id}` → 401
  - Pinia store inject: not persistent
  - CodeMirror inject: doesn't trigger save
  - Manual edit in Railway UI: save fails
- **FINAL SOLUTION:** ngrok HTTPS tunnel → HA-native webhook automation → direct Anthropic API call → execute HA service
- **Result:** ChAvee works 100% during defense — Claude reads HA states fresh every call (better than static HOME_CONTEXT)
- **Lesson:** when service has bug, bypass beats fighting — direct is often simpler

---

## 17. Defense Verification

### 🎤 Live Demo (verified during defense)

**Alexa Echo Dot:**
1. ✅ "turn on living room light" → light on, warm white
2. ✅ "set living room to blue" → Tuya RGB → blue
3. ✅ "turn on party mode" → 5-color loop
4. ✅ "what's the temperature in kitchen" → DHT22 reading
5. ✅ "lock the front door" → servo locks
6. ✅ "open garage" → garage door servo opens
7. ✅ "if pump needs water" → "Pump activates when soil <45%"
8. ✅ "switch to bedtime mode" → scene_bedtime_smart fires

**ChAvee LINE:**
1. ✅ "เปิดไฟห้องนั่งเล่นสีฟ้า" → light blue
2. ✅ "อุณหภูมิห้องนอนเท่าไหร่" → reads DHT22, replies in Thai
3. ✅ "ปาร์ตี้" → input_boolean.party_loop_active on → loop
4. ✅ "Sleep mode" → comfort_mode = Sleep + lights adjust
5. ✅ "เปิดไฟห้องนอน + ปิดไฟห้องครัว" → parallel multi-action
6. ✅ "ปั๊มทำงานทุกกี่นาที" → AI reasoning replies with state-based logic
7. ✅ Out-of-scope test → ChAvee asks back politely (no crash)

### 🏆 Committee Feedback

User direct quote: **"ผมไปสอบโปรเจคมาแล้ว ผ่านแบบไปได้สวย ไปได้ด้วยดีเลยครับ"**

---

## 18. Documentation Map

```
/Users/path_mos/Desktop/Mos/Project_1/
│
├── 📖 MASTER_REFERENCE.md           ⭐⭐⭐ THIS FILE — start here
│
├── PROJECT_STATE.md                  Live state snapshot
├── PROJECT_PROGRESS.md               12-module tracker
├── PROJECT_STATUS_REPORT.md          Formal status + stack tables + cost
├── HOME_FLOW_FINAL.md                Architecture + flows (concise)
├── SYSTEM_FLOW.md                    490-line comprehensive walkthrough
├── V4_FIX_LOG.md                     Bug-fix journey (PART 1 + PART 2)
├── PROJECT_JOURNEY.md                Build history (74KB)
├── HARDWARE_GUIDE.md                 Hardware reference (50KB)
├── WIRING_DETAILED.md                Wiring details (41KB)
├── THESIS_REPORT_FINAL.md            Thesis (79KB)
├── SmartHome_Thesis_Ch1-4.docx       Thesis chapter 1-4
├── SmartHome_Thesis_v2.docx          Thesis v2
├── HA_configuration.yaml             Latest local HA config
├── HA_ui-lovelace.yaml               Latest local dashboard config
│
├── DEFENSE_HANDOFF/
│   ├── 🏆 POST_DEFENSE_FINAL.md      Comprehensive retrospective
│   ├── README.md                     Entry point
│   ├── DEFENSE_DAY_FINAL_STATUS.md   Defense day status
│   ├── ALEXA_COMMANDS_DEFENSE.md     60+ Alexa commands
│   ├── CHAVEE_COMMANDS_DEFENSE.md    Thai ChAvee commands
│   ├── *.pdf (4)                     Defense slides
│   ├── 01_docs/
│   │   ├── ALEXA_LAMBDA_VERIFY.md    Alexa verification
│   │   └── CHAVEE_ALEXA_UPDATE_STATUS.md   Status update
│   ├── 02_test_sheets/               130-test PDFs (10 files)
│   └── 03_scripts/
│       └── generate_test_sheets.py   PDF generator
│
├── alexa/
│   ├── lambda_function.py            Lambda code (32KB)
│   ├── lambda.zip                    Deployed package (40KB)
│   ├── interaction_model.json        Skill model (23 intents)
│   ├── ALEXA_DEVICE_SETUP_GUIDE.md   Echo Dot setup
│   ├── ALEXA_TEST_CASES_FULL.md      100+ test cases
│   ├── DEPLOY_GUIDE.md               Lambda deployment
│   ├── VOICE_AUDIT.md                Audit + fix log
│   └── VOICE_COMMANDS_FULL.md        Voice command reference
│
├── esp32_firmware/
│   └── เวอร์ชั่นมีmotor/             Authoritative firmware
│       └── README.md                  Firmware notes
│
├── pi_setup/
│   ├── DEPLOY_GUIDE.md               Pi WiFi + Z2M setup
│   └── README.md                     Pi notes
│
├── n8n_workflows/
│   ├── 01_chavee_main_v5_EXPERT.json    Latest workflow (bypassed but archived)
│   ├── 02_daily_report.json             7 AM report → LINE
│   ├── 03_auto_water.json               Legacy (replaced by HA pump auto)
│   └── 04_ha_alert_to_line.json         HA alert → LINE webhook
│
├── test_sheets/                      Test sheet generators
├── mockup/                           HTML mockup experiments
├── agile_report/                     Project agile reports
├── thesisตัวอย่างรุ่นพี่/             Thesis examples (reference)
├── volumes/                          Docker volumes
├── __pycache__/                      Python cache
├── smart_home_esp32/                 Old ESP32 folder
│
└── _archive/26may_cleanup/           51 archived files (post-cleanup)
    ├── README.md                     Archive manifest
    ├── root_old_docs/                12 pre-defense plans
    ├── root_old_paste/               4 paste snippets
    ├── root_old_presentations/       2 old scripts
    ├── HA_old_yaml/                  8 old HA backups
    ├── n8n_old_versions/             7 ChAvee v1-v4
    ├── defense_handoff_obsolete/     10 obsolete files (incl 6 sync duplicates)
    └── HA_LATEST_snapshot/HA_LATEST/ 7 HA snapshot files
```

---

## 19. Production Readiness + Future Improvements

### ✅ Production Status

| Criteria | Status | Notes |
|---|:-:|---|
| Hardware uptime | ✅ | All 5 ESP32 + Pi + EC2 running 24/7 |
| Software stability | ✅ | 100% test pass + self-healing |
| Notification reliability | ✅ | 3-channel fan-out (mobile + persistent + LINE) |
| Safety automations | ✅ | gas/water/fall/intrusion all live |
| AI quality | ✅ | Claude Haiku 4.5 — multi-action, Thai/English |
| Documentation | ✅ | 30+ MD files including this master ref |
| Cost | ✅ | ~฿1,664-1,864/mo within budget |
| Defense | ✅ | PASSED 26 พ.ค. 2569 |

### 🔮 Future Improvements (Optional)

#### **Short-term (next 1-3 months)**
- [ ] Replace ngrok with Cloudflare Tunnel (no URL change on restart)
- [ ] Re-enable manual override with user-vs-automation context filter
- [ ] SSL/HTTPS for HA (Let's Encrypt via Caddy)
- [ ] Music integration (Spotify or Jellyfin local) — currently graceful fallback
- [ ] Reduce ChAvee API cost by caching common queries

#### **Medium-term (3-6 months)**
- [ ] LD2451 sensor for true ML-based fall detection
- [ ] Camera integration (Frigate NVR + person detection)
- [ ] Energy monitoring + auto-reports
- [ ] Voice clone for ชาวี (Thai TTS via XTTS or similar)
- [ ] Local LLM option (Llama 3 on edge GPU for privacy + cost)

#### **Long-term (6-12 months)**
- [ ] Multi-user support (member-aware automations)
- [ ] Predictive automations (ML on chat_logs + mode_changes)
- [ ] Mobile app (Flutter) instead of LINE-only
- [ ] Integration with Apple HomeKit / Google Home
- [ ] Continuous testing — automated nightly test suite via PDF sheets

---

## 🎓 Final Note

This Smart Home AIoT system is **NOT a demo** — it's a **working production system** that the user (Mos) plans to use long-term in his own home.

**Defense outcome:** ✅ PASSED "ไปได้ด้วยดี" — committee impressed by:
- Engineering maturity (self-healing, fuzzy ASR, retained MQTT)
- AI sophistication (multi-action, Thai bridge, out-of-scope handling)
- Real-world value (fall detect, gas/leak/intrusion)
- Multi-modal UX (LINE + Alexa + Dashboard + Auto)
- Production architecture (3-layer, conversation memory, analytics)

**Architect/Owner:** Athit "Mos" Boonpinit — Bangkok University CE08
**Built over:** ~3 months (Mar-May 2569)
**Bug-fix sprint:** 12+ hours on defense day morning — emerged victorious

---

**📌 This document is the single source of truth.**

For specific deep-dives, see:
- Bug history → `V4_FIX_LOG.md`
- Defense retrospective → `DEFENSE_HANDOFF/POST_DEFENSE_FINAL.md`
- Per-room flow → `SYSTEM_FLOW.md`
- Live state → `PROJECT_STATE.md`

**Updated:** 27 พ.ค. 2569 (post-defense)
**Doc version:** 2.0 — 100% Complete + Deep Explanations

🏠✨🎓

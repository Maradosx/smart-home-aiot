# 🏠 Smart Home AIoT — Full System Walkthrough

> **Project:** Smart Home AIoT — Bangkok University Senior Project CE08
> **Owner:** Athit Boonpinit (Mos)
> **Defense:** ✅ **26 May 2026 — PASSED "ไปได้ด้วยดีเลยครับ"** 🎓
> **Last updated:** 25 พ.ค. 2569 (body) — header refreshed 27 พ.ค. post-defense

> ⚠️ **POST-DEFENSE NOTE (27 พ.ค. 2569):**
> Body ของไฟล์นี้สรุปสถาปัตยกรรมตอน 25 พ.ค. (pre-defense). 26 พ.ค. มี bug-fix sprint แก้ 12 จุด + เพิ่ม **24 automations ใหม่** + เปลี่ยน ChAvee LINE Bot จาก n8n เป็น **ngrok HTTPS bypass**
>
> **เลขปัจจุบัน:** 230 entities, 41 automations in HA UI (87 total incl. YAML + test/debug), 7 scripts
>
> **อ่านอัปเดตล่าสุด:**
> - [`DEFENSE_HANDOFF/POST_DEFENSE_FINAL.md`](DEFENSE_HANDOFF/POST_DEFENSE_FINAL.md) — Full retrospective
> - [`V4_FIX_LOG.md`](V4_FIX_LOG.md) PART 2 — 26 พ.ค. bug-fix sprint
> - [`HOME_FLOW_FINAL.md`](HOME_FLOW_FINAL.md) — Latest concise flow
> - [`PROJECT_STATE.md`](PROJECT_STATE.md) — Live state snapshot

ทวนระบบทั้งบ้านครับ ตั้งแต่ภาพรวม → แต่ละห้อง → sensor thresholds → flows → modes ครบทุกมิติ

---

## 📑 Table of Contents

1. [ภาพรวมสถาปัตยกรรม (Architecture)](#1-ภาพรวมสถาปัตยกรรม-architecture)
2. [ห้องและอุปกรณ์ละเอียด (Per-Room Breakdown)](#2-ห้องและอุปกรณ์ละเอียด-per-room-breakdown)
3. [Sensor Thresholds — ทำไมเป็นเลขนี้](#3-sensor-thresholds--ทำไมเป็นเลขนี้)
4. [Auto Flows (ทำงานเอง)](#4-auto-flows-ทำงานเอง--ไม่ต้องสั่ง)
5. [Manual Flows (สั่งเอง)](#5-manual-flows-สั่งเอง)
6. [Mode System](#6-mode-system)
7. [Scene System](#7-scene-system-6-ฉาก)
8. [Emergency Response Priority](#8-emergency-response-priority)
9. [Communication Layers](#9-communication-layers)
10. [Defense Pitch](#10-defense-pitch-3-ประโยค)
11. [Summary Numbers](#11-summary-numbers)

---

## 1. ภาพรวมสถาปัตยกรรม (Architecture)

```
┌─────────────────────────────────────────────────────────────────┐
│  USERS / INTERFACES                                              │
│  📱 LINE Bot "ชาวี"  🎤 Alexa Echo Dot  💻 HA Dashboard         │
└──────────────┬─────────────────┬──────────────────┬──────────────┘
               │                 │                  │
               ▼                 ▼                  ▼
       ┌──────────────┐  ┌──────────────┐  ┌──────────────┐
       │ n8n + Claude │  │ AWS Lambda   │  │ Home Assist. │
       │ Haiku 4.5    │  │ + HOME_CTX   │  │  Native UI   │
       │ (Railway)    │  │ (us-east-1)  │  │              │
       └──────┬───────┘  └──────┬───────┘  └──────┬───────┘
              │                 │                  │
              └────────┬────────┴──────────────────┘
                       ▼
         ╔══════════════════════════════════╗
         ║  HOME ASSISTANT — Brain (EC2)   ║
         ║  IP: YOUR_EC2_IP:8123           ║
         ║  200 entities, 41 automations   ║
         ║  7 scripts (scene-like)         ║
         ╚════════════════╤═════════════════╝
                          │
              ┌───────────┼───────────┐
              ▼           ▼           ▼
         ┌────────┐  ┌────────┐  ┌────────┐
         │ MQTT   │  │ Zigbee │  │ REST   │
         │Mosquit.│  │ 2 MQTT │  │  API   │
         └───┬────┘  └───┬────┘  └───┬────┘
             │           │           │
       ┌─────┴────┐  ┌───┴────┐  ┌──┴───┐
       │ ESP32×5  │  │ Pi 3B+ │  │ LINE │
       │ wired    │  │ + ZB   │  │Notify│
       │ sensors  │  │ stick  │  │      │
       └──────────┘  └────────┘  └──────┘
```

**Brain คือ Home Assistant** — ทุก decision ผ่านมันทั้งหมด ESP32 = แค่ sensor relay + actuator, ไม่มี logic เอง

### 🌐 Network & Endpoints

| Service | Address | Port |
|---|---|---|
| Home Assistant | `YOUR_EC2_IP` | `8123` |
| Mosquitto MQTT | `YOUR_EC2_IP` | `1883` |
| Pi (Zigbee2MQTT) | `172.20.10.4` | `8080` |
| n8n | `YOUR_N8N_INSTANCE.up.railway.app` | `443` |
| Supabase | `YOUR_PROJECT.supabase.co` | `443` |
| AWS Lambda | `arn:aws:lambda:us-east-1:233151234126:function:smart-home-alexa` | — |

---

## 2. ห้องและอุปกรณ์ละเอียด (Per-Room Breakdown)

### 🛏️ ห้องนอน — ESP32 #1 (`esp32_01_bedroom`)

| อุปกรณ์ | รุ่น | GPIO | MQTT Topic | หน้าที่ |
|---|---|---|---|---|
| DHT22 | DHT22 | 4 | `home/bedroom/temperature`, `/humidity` | วัดอุณหภูมิ + ความชื้น ทุก 30s |
| SG90 servo | 360° continuous | 25 | `home/bedroom/curtain/set` + `/state` | หมุนเปิด/ปิดม่าน (timer 1150ms = ระยะม่าน) |
| Relay พัดลม | 5V module | 26 | `home/bedroom/fan/set` + `/state` | Active HIGH = พัดลม ON |
| LED ห้องนอน | ESP32 LED pin | 27 | `home/bedroom/light/set` + `/state` | Active HIGH = ไฟ ON |

**ทำไมต้องมีในห้องนอน:**
- DHT22 → ใช้กับ comfort mode (วัดอากาศนอนสบายไหม)
- Servo curtain → automate ตอน sleep mode (ปิดม่าน) / sunrise (เปิด)
- พัดลม + ไฟ → control พื้นฐาน (เปิด/ปิด เสียง + Alexa)

---

### 🛋️ ห้องนั่งเล่น — ESP32 #2 (`esp32_02_livingroom`)

| อุปกรณ์ | รุ่น | ที่ไหน | หน้าที่ |
|---|---|---|---|
| DHT22 | DHT22 | ESP32 GPIO4 | วัดอากาศห้องรับแขก |
| **Tuya RGB+CCT bulb** | TS0505B | **Zigbee** (via Pi+Z2M) | หลอด RGB หลัก — เปลี่ยนสี + พัดไฟ + scene |
| **Tuya Plug** | TS011F | Zigbee | เสียบพัดลม → ควบคุมผ่าน HA |
| **Tuya Plug** | TS011F | Zigbee | เสียบเครื่องฟอกอากาศ |
| **PIR SNZB-03P** | Sonoff Zigbee | Zigbee | ตรวจการเคลื่อนไหว → motion sensor หลักของบ้าน |
| **Door SNZB-04** | Sonoff Zigbee | Zigbee | sensor magnet ประตูหน้า → เปิด/ปิด/นานเกิน |
| **SG90 180°** | servo | ESP32 GPIO13 | กลอนประตูหน้า (0° = unlock, 90° = lock) |

**ทำไมต้องมีในห้องนั่งเล่น:**
- เป็น **hub กลางบ้าน** — sensor เยอะสุดเพื่อจับ activity
- Tuya RGB = ตัวแสดงสถานะบ้าน (เปลี่ยนสีเมื่อเกิด event)
- PIR + Door sensor = **หัวใจของ Security Mode** (intrusion detection)
- SG90 = สมาร์ทล็อค

---

### 🍳 ห้องครัว — ESP32 #3 (`esp32_03_kitchen`)

| อุปกรณ์ | รุ่น | GPIO | MQTT | หน้าที่ |
|---|---|---|---|---|
| DHT22 | DHT22 | 4 | `home/kitchen/temperature`, `/humidity` | วัดอากาศครัว |
| **MQ-2 gas sensor** | analog + digital | A0=34, D0=13 | `home/kitchen/gas_lpg` (0-4095), `/gas_alert` (ON/OFF) | ตรวจจับ LPG + ก๊าซไวไฟ |
| LDR | analog | 35 | `home/kitchen/ldr` | วัดแสง (ใช้ judge ครัวมืดไหม) |
| Relay LED ครัว | 26 | `home/kitchen/light/set` | ไฟห้องครัว |
| Relay พัดลมระบาย | 27 | `home/kitchen/fan/set` | vent fan |
| Relay เตา | 25 | `home/kitchen/stove/set` | **block อัตโนมัติเมื่อ gas alert** |

**ทำไมต้องมีในห้องครัว:**
- MQ-2 + เตา **อัตโนมัติตัดไฟเตา + เปิดพัดลม** เมื่อก๊าซรั่ว = safety feature เด่นที่สุดของระบบ
- LDR ครัว → ตอน Away mode คนกลับมาเปิดไฟอัตโนมัติ
- พัดลมระบาย → ช่วย vent หลังทำอาหาร

---

### 🚿 ห้องน้ำ + สวน — ESP32 #4 (`esp32_04_bathroom_garden`)

| อุปกรณ์ | รุ่น | GPIO | MQTT | หน้าที่ |
|---|---|---|---|---|
| **LD2410** mmWave radar | UART2 | RX=16, TX=17 | `home/bathroom/fall_alert` | **ตรวจคนล้ม** (5 นาที นิ่งเฉย → alert) |
| **SNZB-05P** water leak | Zigbee | — | `binary_sensor.bathroom_water_leak` | ตรวจน้ำรั่ว/ท่วม |
| LDR ห้องน้ำ | analog | 36 (VP) | `home/bathroom/ldr` | วัดแสง |
| Relay ไฟห้องน้ำ | 26 | `home/bathroom/light/set` | ไฟเพดาน |
| Soil moisture | analog | 32 | `home/garden/soil_moisture` (0-100%), `/soil_raw` (0-4095) | ความชื้นดิน |
| LDR สวน | analog | 39 (VN) | `home/garden/ldr` | วัดแสงสวน |
| Relay ปั๊มน้ำ | 27 | `home/garden/water_pump/set` | ปั๊มรดน้ำ + auto-on ถ้า soil < 30% |
| Relay ไฟสวน × 2 | 25, 33 | `home/garden/light/set`, `/pathway/set` | ไฟทางเดิน + ต้นไม้ |

**ทำไมต้องมีในห้องน้ำ + สวน:**
- LD2410 = **คุณสมบัติเด่นสำหรับผู้สูงอายุ** (ถ้าล้มในห้องน้ำ → แจ้ง LINE)
- SNZB-05P = ป้องกันน้ำท่วม (รั่วจากชักโครก/อ่าง)
- Soil + Pump = ระบบรดน้ำอัตโนมัติ + manual + ตั้งเวลา 07:00 ทุกวัน

---

### 🚗 โรงรถ — ESP32 #5 (`esp32_05_garage`)

| อุปกรณ์ | รุ่น | GPIO | MQTT | หน้าที่ |
|---|---|---|---|---|
| **PIR HC-SR501** | analog edge-detect | 23 | `home/garage/motion` (smart 30s timeout) | ตรวจคน/รถ |
| **MQ-2 smoke** | digital | 32 | `home/garage/smoke_lpg`, `/smoke_alert` | ตรวจควัน/ไฟไหม้ |
| LDR | analog | 34 | `home/garage/ldr` | วัดแสง |
| Relay ไฟโรงรถ × 2 | 25, 26 | `/light/set`, `/pathway/set` | ไฟหลัก + ไฟทางเดิน |
| SG90 garage door | servo | 27 | `cover.garage_door` | ประตูโรงรถ (open/close) |

**ทำไมต้องมีในโรงรถ:**
- PIR + ไฟอัตโนมัติ ตอนกลางคืน (มืดมาก + มีคน → เปิดไฟ)
- MQ-2 smoke = **ป้องกันไฟไหม้รถ/ของในโรงรถ**
- SG90 garage door = ควบคุมเปิด/ปิดด้วยเสียง

---

## 3. Sensor Thresholds — ทำไมเป็นเลขนี้

### 🔥 MQ-2 Gas (ครัว + โรงรถ) — ADC 0-4095

| ค่า ADC | ระดับ | หมายความว่า | ทำไม |
|---|---|---|---|
| **0 – 599** | 🟢 **SAFE** | อากาศปกติ | ค่าฐานปกติ ~300-400 ในห้อง |
| **600 – 999** | 🟡 **WARN** | มีกลิ่นอาหาร / ควันบางๆ | ตอนทำอาหารปกติแตะ ~500-700 |
| **1000 – 1799** | 🟠 **HIGH** | ก๊าซเกินกว่าปกติชัดเจน | ใกล้ขีดอันตราย — แจ้ง LINE |
| **1800 – 4095** | 🔴 **DANGER** | ก๊าซรั่วจริง! | **Auto: ตัดเตา + เปิดพัดลมระบาย + LINE alert + red flash** |

**Digital D0:** module มี potentiometer ปรับ threshold เอง — ตั้งไว้ที่ ~1800 ADC → ถ้าเกินจะส่ง LOW = `gas_alert ON`

**ทำไมเลือกเลขนี้:** MQ-2 sensitivity curve (datasheet) บอกว่า ADC > 1500 = LPG concentration > 200 ppm (ปลอดภัย < 1000 ppm)

---

### 🌱 Soil Moisture (สวน) — แปลง ADC → %

```python
SOIL_DRY = 3300   # ADC ตอนดินแห้งสนิท   → 0%
SOIL_WET = 1100   # ADC ตอนแช่น้ำ        → 100%
moisture% = map(rawADC, 3300, 1100, 0, 100)
```

| Moisture % | สถานะ | Action |
|---|---|---|
| **< 30%** | 🟠 ดินแห้ง | **Auto: pump ON** (ถ้า pump ยังไม่เปิด) + LINE alert |
| 30 – 70% | 🟢 พอดี | ไม่ทำอะไร |
| **> 70%** | 🔵 ชื้นพอ | **Auto: pump OFF** (กันรดน้ำเกิน) |

**ทำไมเลือกเลขนี้:** Hysteresis 30/70 — ป้องกันปั๊มเปิด-ปิดถี่ ๆ ตอนค่าผันผวน (anti-chatter)

---

### 🚶 LD2410 mmWave Fall Detection (ห้องน้ำ)

- Sensor ส่ง `state` 3 ค่า: `0=ว่าง`, `1=มีคนขยับ`, `2=มีคนอยู่แต่นิ่ง`
- ถ้า state == 2 ต่อเนื่อง **5 นาที (300,000ms)** → **fall_alert = ON**
- ถ้า state เปลี่ยนเป็น 1 หรือ 0 → reset timer

```cpp
const unsigned long FALL_THRESHOLD_MS = 5UL * 60UL * 1000UL;  // 5 นาที
```

**ทำไม 5 นาที:** คนอาบน้ำนิ่งใต้ฝักบัวบางจังหวะอาจถึง 2-3 นาที — 5 นาที = แน่ใจว่าผิดปกติ (ล้ม/หมดสติ)

---

### 🚶‍♂️ PIR Motion (โรงรถ + ห้องน้ำ-สวน) — Smart Edge Detection

```cpp
MOTION_TIMEOUT = 30000   // 30s
```

**Logic:**
1. PIR ส่ง HIGH = "เห็นคน" → publish `motion ON`
2. ถ้าไม่มี rising edge ใหม่ใน 30s → publish `motion OFF`
3. **ป้องกัน PIR ค้าง HIGH** (stuck): force clear แม้ GPIO ยังเป็น HIGH

**ทำไม 30s:** PIR HC-SR501 มีปัญหาค้างบ่อย (ทั้งที่ไม่มีคน) — edge detection แก้ได้ และ 30s นานพอที่กิจกรรมปกติจะ trigger ใหม่ทัน

---

### 🌡️ DHT22 (3 ห้อง: bedroom, livingroom, kitchen)

| ค่า | ความหมาย |
|---|---|
| temperature | °C (อัตราอ่าน ทุก 30s) |
| humidity | % RH |
| **Range ปกติ** | 20-35°C, 40-80% RH |
| ใช้ตรง | Comfort mode (Warm/Relax อ้างอิงค่า), Dashboard graph, AI Agent ตอบ |

**ไม่มี auto-trigger** จากค่า DHT22 ตรงๆ — ใช้แค่อ่านค่า (เพราะเป็น personal preference)

---

### 💧 SNZB-05P Water Leak (ห้องน้ำ)

- Binary: `OFF` = แห้ง, `ON` = ตรวจเจอน้ำ
- เซ็นเซอร์ทำงานเมื่อ **น้ำแตะขา copper ของอุปกรณ์ตรง ๆ** (ไม่ใช่สาย — ขาทองแดงสั้น ๆ ใต้ตัว)
- Auto: เปิด blue pulse + LINE alert ทันที

---

### 🚪 SNZB-04 Door Contact (ประตูหน้า)

- Binary: `OFF` = ปิด (แม่เหล็กแตะกัน), `ON` = เปิด (แม่เหล็กแยก)
- Auto rules:
  1. ถ้า door=ON ตอน Security mode → **intrusion alert + flash + lock + LINE**
  2. ถ้า door=ON เกิน 10 นาที → แจ้งเตือน "ลืมปิดประตู"

---

## 4. Auto Flows (ทำงานเอง — ไม่ต้องสั่ง)

> **Updated 25 พ.ค. 2569:** เพิ่ม **Adaptive Lighting** (motion+LDR) + **GPS Welcome Home** + **Sleep Path Light** + **Relax Adaptive Fan** = รวม **18 auto flows** (เดิม 8)

### 💡 Flow A1-A4: Adaptive Motion Lighting (NEW)
**Logic:** มีคนเดิน + ห้องมืดพอ → เปิดไฟอัตโนมัติ; ห้องสว่างพอ → ไม่เปิด

| Room | Motion Trigger | LDR Threshold | Action |
|---|---|---|---|
| **Bedroom** | `binary_sensor.bedroom_motion` (PIR) | > 2200 | `switch.bedroom_light` ON |
| **Living Room** | `binary_sensor.living_room_motion` (Zigbee PIR) | > 2200 | `light.living_room_light` **brightness ตาม LDR** (Tuya bulb) |
| **Bathroom** | `binary_sensor.bathroom_presence` (LD2410) | > 2000 | `switch.bathroom_light` ON |
| **Garage** | `binary_sensor.garage_motion` (PIR) | > 2400 | `switch.garage_light` + `garage_pathway_light` ON |

**Disabled ใน mode:** Away, Sleep (Sleep มี path-light แยก — flow A6)

### 🌅 Flow A5: Auto-Off (NEW)
ปิดไฟอัตโนมัติเมื่อไม่มี motion 5 นาที (bedroom, garage, bathroom)

### 🌙 Flow A6: Sleep Path Light (NEW)
ตอน **Sleep mode** + เดินผ่าน PIR bedroom → เปิด Tuya bulb dim warm 10% (color_temp 2200K) เป็นไฟทางเดิน → auto-off ใน 2 นาที (ปลอดภัยเดินไปห้องน้ำ ไม่รบกวนการนอน)

### 🏠 Flow A7: GPS Welcome Home (NEW)
```
person.athit_boonpinit เปลี่ยน: not_home → home
   ↓ Actions:
      1. cover.garage_door → open (เปิดประตูโรงรถ)
      2. ถ้า garden_ldr > 2200 → switch.garden_light + pathway ON
      3. ถ้า garage_ldr > 2400 → switch.garage_light + pathway ON
      4. ถ้า house_mode = Away → switch ไป Home
      5. LINE: "ยินดีต้อนรับกลับบ้าน — เปิดโรงรถและไฟภายนอกตามแสงให้แล้ว"
```

### 🛋 Flow A8: Relax Mode — Adaptive Fan (NEW)
```
sensor.living_room_temperature > 28°C + comfort_mode = Relax
   ↓ Actions:
      1. switch.living_room_fan → ON
      2. light.living_room_light → dim 60% warm (2700K)
```

### 🚪 Flow A9: GPS Goodbye — Auto Away (NEW v2)
```
person → not_home for 2 min grace period
   ↓ ถ้า house_mode ≠ Away:
      1. input_select.house_mode → Away (cascades to automation #1)
      2. LINE: "ออกจากบ้านเกิน 2 นาที — ตั้ง Away mode ให้อัตโนมัติ"
```
**Symmetric กับ GPS Welcome** — มีกลับมาก็ต้องมีออกไป + กันลืม set Away

### 🔥 Flow A10-A11: Stove Safety Extension (NEW v2)
```
A10 (Warning): switch.kitchen_stove ON for 10 min
   AND bedroom motion = ON (proxy ว่าคนอยู่ห้องอื่น ไม่ใช่ครัว)
   ↓ LINE warning "⚠️ เตาเปิดทิ้งไว้ 10 นาทีแล้ว!"

A11 (Auto-OFF): switch.kitchen_stove ON for 15 min + house_mode = Away
   ↓ switch.kitchen_stove OFF + LINE notify
```
**Real-world safety** beyond gas leak detection

### 🌅 Flow A12-A13: Auto Curtain by Sun (NEW v2)
```
A12 (Sunset Close): sun.sun → below_horizon
   AND house_mode = Home AND curtain = open
   ↓ cover.bedroom_curtain close + LINE notify

A13 (Sunrise +30min Open): sun event sunrise + 30 min
   AND house_mode = Home AND comfort_mode ≠ Sleep AND curtain = closed
   ↓ cover.bedroom_curtain open
```
**ใช้ HA built-in `sun` entity** — auto-calculated for current location

### 💨 Flow A14-A15: Sleep Bedroom Auto-Fan (NEW v2)
```
A14 (Hot): sensor.bedroom_temperature > 27°C + comfort_mode = Sleep
   ↓ switch.bedroom_fan ON

A15 (Cool): sensor.bedroom_temperature < 25°C + comfort_mode = Sleep
   ↓ switch.bedroom_fan OFF
```
**Hysteresis 25/27°C** — ป้องกัน fan เปิด-ปิดถี่ (anti-chatter)

### 🔥 Flow 1: Gas Alert (ครัว)
```
MQ-2 ADC > 1800 (digital LOW)
   ↓ ESP32 ส่ง MQTT: home/kitchen/gas_alert ON
   ↓ HA trigger: binary_sensor.kitchen_gas_alert
   ↓ Actions (ทำพร้อมกัน):
      1. switch.kitchen_stove → turn_off (ESP32 ก็ block เอง 2 ชั้น)
      2. switch.kitchen_fan → turn_on (vent)
      3. script.living_room_red_flash → ไฟแดงพริบ 30s
      4. rest_command.send_alert → LINE "🔥 ก๊าซรั่วครัว!"
```

### 💧 Flow 2: Water Leak (ห้องน้ำ)
```
SNZB-05P detect water
   ↓ binary_sensor.bathroom_water_leak = ON
   ↓ HA trigger
      1. script.living_room_blue_pulse → ไฟฟ้าพริบ
      2. LINE alert "💧 น้ำรั่วห้องน้ำ!"
```

### 🚶 Flow 3: Bathroom Fall (ห้องน้ำ)
```
LD2410 state=2 ต่อเนื่อง 5 นาที
   ↓ MQTT: home/bathroom/fall_alert ON
   ↓ HA trigger:
      1. scene.create snapshot (เก็บสถานะไฟทั้งหมดไว้)
      2. ไฟแดง flash 30 วินาที (10 รอบ on/off)
      3. scene.turn_on restore (กลับสถานะเดิม)
      4. LINE alert "🚨 คนล้มห้องน้ำ!"
   ↓ Delay 5 นาที (กัน trigger ซ้ำ)
```

### 🔥 Flow 4: Garage Smoke
```
MQ-2 ADC > threshold
   ↓ binary_sensor.garage_smoke_alert ON
   ↓ HA trigger:
      1. script.living_room_red_flash
      2. LINE alert "🚨 ควันโรงรถ!"
```

### 🚨 Flow 5: Intrusion (Security mode + ประตูเปิด)
```
input_select.house_mode = Security
   AND binary_sensor.front_door = ON (เปิด)
   ↓ HA trigger:
      1. lock.front_door_lock → lock (ล็อคทันที)
      2. script.living_room_red_flash
      3. LINE alert "🚨 มีคนบุก!"
```

### 🌱 Flow 6: Soil Auto-Water
```
soil_moisture < 30%
   ↓ ESP32 ส่ง pump ON อัตโนมัติ + HA trigger LINE "🌱 ดินแห้ง รดน้ำอัตโนมัติ"
   หลังจาก soil > 70%
   ↓ ESP32 ส่ง pump OFF อัตโนมัติ
```

### 🌅 Flow 7: Daily Watering 07:00
```
time = 07:00:00
   ↓ HA trigger:
      switch.garden_water_pump → ON 10 นาที → OFF
      LINE "🌅 อรุณสวัสดิ์ — รดน้ำเช้าเรียบร้อย"
```

### 🌙 Flow 8: Door Open Too Long
```
binary_sensor.front_door = ON ต่อเนื่อง 10 นาที
   ↓ HA trigger:
      LINE alert "🚪 ลืมปิดประตูหน้า!"
```

---

## 5. Manual Flows (สั่งเอง)

### 📱 ทาง LINE → ชาวี (n8n + Claude Haiku 4.5)
```
User: "เปิดไฟห้องนอน"
   ↓ LINE webhook → n8n
   ↓ "Save Memory & Build Command" (parser)
   ↓ "Call Claude" → Anthropic API (Haiku 4.5 + HOME_CONTEXT)
   ↓ Claude reply: { intent: "control", commands: [{entity: "switch.bedroom_light", action: "on"}] }
   ↓ "Execute via HA" → REST: /api/services/switch/turn_on
   ↓ "LINE Reply" → "💡 เปิดไฟห้องนอนแล้วครับ"
```

### 🎤 ทาง Alexa → Echo Dot
```
User: "Alexa, ask home control to turn on kitchen fan"
   ↓ Alexa ASR → match intent (ControlFanIntent, room=kitchen, action=on)
   ↓ AWS Lambda smart-home-alexa
   ↓ handle_control_fan(slots) → _fuzzy_room → "kitchen"
   ↓ ha_service("homeassistant", "turn_on", "switch.kitchen_fan")
   ↓ Alexa TTS: "Okay, turned on the kitchen fan"
```

**ถ้า intent หายาก (เช่น "I'm cold"):**
```
   ↓ FallbackIntent → SmartHomeChatIntent → AI Agent (เดียวกับชาวี!)
   ↓ Claude วิเคราะห์ + ตัดสินใจ + execute via HA
```

### 💻 ทาง HA Dashboard
```
คลิก switch บน Lovelace UI → HA service call → MQTT/Zigbee → device
```

---

## 6. Mode System (Redesigned 25 พ.ค. 2569)

> **ปรับใหม่:** แต่ละ mode ใช้ **sensor + actuator** จริงในบ้าน ไม่ใช่แค่เปลี่ยนสีไฟ

### 🏛 House Mode (3 โหมดหลัก — `input_select.house_mode`)

| Mode | Sensor ที่ใช้ | Actuator ที่ใช้ | Effect |
|---|---|---|---|
| **🏠 Home** | LDR ทุกห้อง, PIR, LD2410 | ไฟทุกห้อง, ม่าน, lock, ไฟสวน | Welcome scene + **adaptive lighting active** + GPS welcome enabled |
| **🚪 Away** | door sensor, motion, MQ-2 | lock, ม่าน, ไฟทุกห้อง, pump, vent fan | **ปิดทุกอย่าง + ล็อคประตู + ปิดม่าน** + adaptive light DISABLED + security listener ON |
| **🛡 Security** | door + motion + MQ-2 | lock, Tuya RGB flash, vent fan | Active monitor — door open = **intrusion auto-lock + red flash + LINE** |

### 😌 Comfort Mode (4 โหมดย่อย — `input_select.comfort_mode`)

| Mode | Sensor ที่ใช้ | Actuator ที่ใช้ | Effect |
|---|---|---|---|
| **🌙 Sleep** | bedroom PIR, LD2410 bath, LDR | bedroom curtain, ไฟทุกห้อง, Tuya bulb (path light) | ปิดไฟทุกห้อง + ปิดม่าน + **Path Light** (PIR bedroom → Tuya dim 10% warm 2 นาที) + fall monitor active |
| **🛋 Relax** | DHT22 livingroom, LDR | Tuya bulb warm dim 60%, พัดลม | **Adaptive Fan** (>28°C เปิดพัดลม) + ไฟ warm 2700K |
| **🔥 Warm** | DHT22 | Tuya bulb warm 2700K + ปิดพัดลม | Cozy ambience (ใช้ตอนหนาว <22°C) |
| **🔄 Normal** | LDR, motion | ทุกอย่าง | Default — **adaptive lighting active**, sensor auto-triggers ทุกอย่าง |

### 🎯 ทำไมแบ่ง 2 ชั้น (House × Comfort)

| Layer | Role | Trigger |
|---|---|---|
| **House Mode** | Security/Presence | Set manually หรือ GPS auto |
| **Comfort Mode** | Ambience/Sensor automation | Set manually หรือ time-based |

**ใช้ซ้อนกันได้** เช่น:
- `Home + Sleep` = อยู่บ้านแต่จะนอน → adaptive light OFF, path light ON
- `Home + Relax` = อยู่บ้านชิล → adaptive fan + Tuya warm
- `Away + Normal` = ออกบ้าน → ปิดทุกอย่าง

### 🔁 Mode → Effect (Cascading Automations)

```
input_select.house_mode = "Away"  
   → automation #1: turn_off all switches + lights + close curtain
   → automation #11: LINE "ออกจากบ้านเรียบร้อย"
   → adaptive lighting flows = DISABLED (condition check)

input_select.comfort_mode = "Sleep"
   → automation #3: turn_off all lights + close bedroom curtain
   → Sleep Path Light flow = ENABLED (motion → dim path)
   → adaptive lighting = DISABLED

input_select.comfort_mode = "Relax"
   → Relax Adaptive Fan flow = ENABLED (>28°C trigger)
   → Mode Scene automation: Tuya warm 60%
```

---

## 7. Scene System (6 ฉาก)

| Scene | Script ที่ทำ | ใช้ตอนไหน |
|---|---|---|
| **🎉 Party** | colorloop ที่ Tuya bulb + ปิดพัดลม | สั่งเอง "party mode" |
| **🌅 Sunrise** | ไฟค่อย ๆ สว่างจาก dim → bright ใน 5 นาที | wake up auto/manual |
| **🏠 Welcome** | ไฟเปิดทุกห้อง dim 40% + เปิดม่าน | trigger หลัง Away → Home |
| **🚨 Alert** | ไฟแดงพริบ + LINE notify | gas/smoke/fall/intrusion |
| **🧘 Calm** | ไฟ warm 30% + ฟังเพลงสงบ | meditation / stress |
| **🔄 Default** | reset ทุก scene → state ปกติ | clear emergency |

---

## 8. Emergency Response Priority

```
PRIORITY 1 (LIFE) — Fall, Gas, Smoke, Water leak
   → flash + LINE + auto-mitigation (ตัดเตา, เปิดพัดลม, ปิด/เปิด actuator)

PRIORITY 2 (SECURITY) — Intrusion (door open in Security mode)
   → flash + LINE + auto-lock

PRIORITY 3 (CONVENIENCE) — Door left open >10min, Soil dry
   → LINE notify only (ไม่ trigger ไฟ)
```

---

## 9. Communication Layers

| Layer | Protocol | Where |
|---|---|---|
| **ESP32 ↔ HA** | MQTT (Mosquitto on EC2 :1883) | wired sensors |
| **Zigbee devices ↔ HA** | Zigbee → Pi (Z2M 2.10.1 in Docker) → MQTT → HA | Tuya, Sonoff |
| **LINE ↔ HA** | LINE webhook → n8n (Railway) → HA REST API | natural language |
| **Alexa ↔ HA** | Alexa → AWS Lambda → HA REST API | voice EN |
| **HA ↔ LINE notify** | rest_command + LINE Messaging API | outgoing alerts |
| **AI Agent ↔ Claude** | n8n + Lambda → Anthropic API (Haiku 4.5) | smart fallback |

---

## 10. Defense Pitch (3 ประโยค)

> "ระบบ Smart Home AIoT ที่ผมสร้างประกอบด้วย ESP32 5 ตัว + Zigbee 6 ตัว เชื่อมเข้า Home Assistant ผ่าน MQTT ทุก sensor มี threshold ที่อ้างอิงจาก datasheet จริงและทดสอบจริง (เช่น MQ-2 ที่ ADC>1800 = LPG > 200 ppm) ส่วน AI Agent ใช้ Claude Haiku 4.5 บริดจ์ทั้ง LINE และ Alexa เข้าด้วยกัน — สั่งภาษาไทย-อังกฤษได้ และตอบ edge case ด้วย context ของบ้านจริง"

---

## 11. Summary Numbers

| Metric | Value |
|---|--:|
| ESP32 zones | 5 |
| Zigbee devices | 6 (Tuya × 3 + Sonoff × 3) |
| HA entities | 177 |
| **HA automations** | **41 (40 ON)** ⬆ cleanup 51→41 (lossy decimation of duplicates) |
| HA scripts | 17 |
| HA scenes | 6 |
| **Auto-safety flows** | **10** (gas, leak, fall × 2, smoke, intrusion, soil, door-long, **stove warning, stove auto-off**) |
| **Adaptive/UX flows** | **15** (motion+LDR × 4, auto-off × 3, sleep path, GPS welcome, GPS goodbye, relax fan, sunset/sunrise curtain × 2, sleep auto-fan × 2) |
| **GPS zone radius** | **7m** (tuned for defense demo) |
| House modes | 3 |
| Comfort modes | 4 |
| Scenes | 6 |
| Alexa intents | 19 custom (with garage door) + 4 built-in = **23** |
| Languages supported | TH + EN (bridge via Claude) |
| Safety threshold sources | MQ-2 datasheet, LD2410 spec, soil ADC calibration, **LDR universal scale** |
| Notification channels | LINE Bot + Alexa TTS + HA Dashboard |

## 12. LDR Adaptive Lighting Reference

LDR scale (ESP32 photoresistor 0-4095, voltage divider 10kΩ):
- **ค่าสูง = มืด** (resistance สูง → voltage drop เยอะ)
- **ค่าต่ำ = สว่าง** (resistance ต่ำ)

### Per-Room Thresholds (calibrated 25 พ.ค.)

| Room | LDR sensor | Baseline (เช้า) | Threshold เปิดไฟ | Tuya brightness scaling |
|---|---|--:|--:|---|
| Bedroom | `sensor.bedroom_ldr` | 2288 | > 2200 | — (relay) |
| Living Room | `sensor.living_room_ldr` | 2851 | > 2200 | **brightness = map(LDR, 2200, 3500, 80, 255)** |
| Kitchen | `sensor.kitchen_ldr` | 1665 | (no motion sensor) | — |
| Bathroom | `sensor.bathroom_ldr` | 1425 | > 2000 | — (relay) |
| Garden | `sensor.garden_ldr` | 1520 | > 2200 (GPS welcome) | — (relay) |
| Garage | `sensor.garage_ldr` | 2406 | > 2400 | — (relay) |

### Universal Brightness Scale (Tuya bulb)

```
LDR < 2200    → ไม่เปิด (สว่างพอ)
LDR 2200      → brightness 80/255  (~30%)
LDR 2900      → brightness 168/255 (~65%)
LDR 3500+     → brightness 255/255 (100%)
```

---

## 📚 Reference Files

| File | Purpose |
|---|---|
| `alexa/lambda_function.py` | AWS Lambda — Alexa intent handlers + AI Agent bridge |
| `alexa/interaction_model.json` | Alexa Skill schema (23 intents) |
| `alexa/ALEXA_TEST_CASES_FULL.md` | 100+ voice command test cases |
| `alexa/VOICE_AUDIT.md` | Test audit report |
| `HA_LATEST/configuration.yaml` | HA automations + scripts (1300+ lines) |
| `HA_LATEST/scenes.yaml` | 6 scenes definition |
| `esp32_firmware/เวอร์ชั่นมีmotor/` | **Authoritative** ESP32 firmware (5 zones) |
| `n8n_workflows/` | Workflow exports — LINE/Claude/HA bridge |
| `pi_setup/` | Pi 3B+ Z2M + Docker setup |

---

**🎓 Ready for defense — 26 พ.ค. 2569**

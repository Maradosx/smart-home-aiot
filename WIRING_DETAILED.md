# 🔌 WIRING DETAILED — Smart Home AIoT v2 (Final)

> **Version:** 2.0 — Updated 6 พ.ค. 2026
> **Boards:** 5 × ESP32 DevKit V1 (38-pin) + 1 × Raspberry Pi 3 Model B+
> **Cloud:** AWS EC2 t3.small (YOUR_EC2_IP) — Mosquitto MQTT :1883
> **Reference:** Latest firmware in `esp32_firmware/esp32_0X_*.ino`

ครอบคลุมทุก GPIO · ทุก device · ทุก layer · ทุก pin

---

## 📑 สารบัญ

1. [Layer Overview](#-layer-overview)
2. [Power Layer (Always-On)](#-1-power-layer)
3. [Network Layer](#-2-network-layer)
4. [ESP32 #1 — Bedroom](#-esp32-1--bedroom)
5. [ESP32 #2 — Living Room](#-esp32-2--living-room)
6. [ESP32 #3 — Kitchen](#-esp32-3--kitchen)
7. [ESP32 #4 — Bathroom + Garden](#-esp32-4--bathroom--garden)
8. [ESP32 #5 — Garage](#-esp32-5--garage)
9. [Pi 3B+ Wiring](#-pi-3-model-b-wiring)
10. [GPIO Master Map](#-gpio-master-map)
11. [Common Wiring Patterns](#-common-wiring-patterns)
12. [Known Quirks](#-known-quirks)

---

## 🏗️ Layer Overview

```
╔══════════════════════════════════════════════════════════════╗
║  LAYER 1 — POWER (Always-On 24/7 ผ่าน Power Bank passthrough) ║
║  AC 220V → USB-C 65W → Power Bank 20Ah → USB Hub → Devices   ║
╠══════════════════════════════════════════════════════════════╣
║  LAYER 2 — NETWORK                                           ║
║  Wi-Fi 2.4 GHz (Maradosx) → Pi AP fallback (SmartHome-Local) ║
║  Zigbee 2.4 GHz mesh ผ่าน ZBDongle-P → Pi → MQTT             ║
╠══════════════════════════════════════════════════════════════╣
║  LAYER 3 — HARDWARE (5 ESP32 zones + Pi 3B+ + Zigbee mesh)   ║
║  ESP32 ×5 → Wi-Fi → MQTT  · Pi → Zigbee2MQTT → MQTT          ║
╠══════════════════════════════════════════════════════════════╣
║  LAYER 4 — CLOUD                                             ║
║  EC2 (HA + Mosquitto) · Railway (n8n) · Groq · Lambda · Supabase ║
╚══════════════════════════════════════════════════════════════╝
```

---

## ⚡ 1. POWER LAYER

### 1.1 Power Distribution Diagram

```
┌─────────────────────────────────────────────────────────────────┐
│                                                                 │
│   AC 220V Wall Outlet                                          │
│         │                                                       │
│         ▼                                                       │
│   ┌──────────────┐                                             │
│   │  USB-C 65W   │  Output: 5V/3A · 9V/3A · 15V/3A · 20V/3.25A│
│   │   Adapter    │  PD 3.0 Standard                           │
│   └──────┬───────┘                                             │
│          │ USB-C cable                                         │
│          ▼                                                      │
│   ┌──────────────┐  Pass-through charging mode                 │
│   │ Power Bank   │  Input:  USB-C PD 18W                       │
│   │ 20,000 mAh   │  Output: 2× USB-A (5V/2.4A each)            │
│   │              │  Capacity: 20,000 mAh × 5V = 100 Wh         │
│   └─────┬────┬───┘                                             │
│         │    │                                                  │
│         ▼    ▼                                                  │
│   ┌──────────────┐                                             │
│   │  USB Hub     │  4× USB-A 2.0 ports (powered)               │
│   │   4-Port     │                                             │
│   └─┬──┬──┬──┬───┘                                             │
│     │  │  │  │                                                  │
│     ▼  ▼  ▼  ▼                                                  │
│   ESP32 ESP32 ESP32 Pi 3B+                                      │
│   #1-#3  #4-#5      (1 ช่อง)                                    │
│                                                                 │
│   หมายเหตุ: ESP32 มี 5 บอร์ด → ใช้ USB Hub 4-port ไม่พอ        │
│            ต่อ USB Hub 7-port หรือ daisy-chain 2 hubs           │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### 1.2 Power Budget Calculation

| Device | Idle | Active | Notes |
|--------|------|--------|-------|
| ESP32 ×5 | 5 × 0.3W = 1.5W | 5 × 1.2W = **6W** | Wi-Fi TX peak |
| Pi 3B+ | 3W | **5-7W** | + 1W เมื่อมี ZBDongle |
| ZBDongle-P | 0.5W | 1W | RX/TX active |
| LD2410C ×2 | 1W | 1W | always on |
| DC Fan 5V ×2 | 0W | 5W | กระตุ้น on/off |
| Mini Pump 5V | 0W | 3W | < 5 นาที/ครั้ง |
| Servo SG90 ×2 | 0.1W | 5-6W | peak 1-2 วิ |
| LEDs ×10 + R | 0W | 0.6W | ทุกตัว ON |
| MQ-2 ×2 + MQ-135 | 1.5W | 1.5W | heater always on |
| **รวม** | **~7W** | **~30W peak** | กำลัง 65W เผื่อ ~2× |

### 1.3 Wiring จาก Power Bank ไปแต่ละ Device

```
Power Bank USB-A Port 1 → USB Hub (Powered) → 5V/2.4A shared
                            ├─► Cable Micro-USB → ESP32 #1 (Bedroom)
                            ├─► Cable Micro-USB → ESP32 #2 (Living Room)
                            ├─► Cable Micro-USB → ESP32 #3 (Kitchen)
                            └─► Cable Micro-USB → ESP32 #4 (Bath+Garden)

Power Bank USB-A Port 2 → ESP32 #5 (Garage) — Direct cable
                          + Pi 3B+ Power (Micro-USB 2.5A)
```

**⚠️ สำคัญ:**
- **Relay VCC ห้ามต่อจาก ESP32 5V pin** — กระแสไม่พอ ทำให้ relay ค้าง/ไม่ดับ
- ต่อตรงจาก Power Bank/USB Hub 5V rail (ผ่าน breadboard rail)
- Servo SG90 VCC ก็ต่อ 5V external เหมือน relay (ป้องกัน brown-out ESP32)

---

## 🌐 2. NETWORK LAYER

### 2.1 Wi-Fi Configuration

```
ทุก ESP32 มี Dual-WiFi config:

Primary:    SSID = "Maradosx"          (Home Router)
            Pass = "YOUR_WIFI_PASSWORD"
            Timeout: 10 วินาที

Fallback:   SSID = "SmartHome-Local"   (Pi 3B+ AP mode — ถ้า router ตาย)
            Pass = "YOUR_AP_PASSWORD"
            กิน DHCP จาก Pi (192.168.4.x)
```

### 2.2 MQTT Topology

```
                         ┌─────────────────────┐
                         │  AWS EC2 t3.small   │
                         │  YOUR_EC2_IP:1883   │
                         │  Mosquitto MQTT     │
                         └──────────┬──────────┘
                                    │
       ┌────────────────────────────┼────────────────────────────┐
       │                            │                            │
       ▼                            ▼                            ▼
  ESP32 ×5 Wi-Fi          Pi 3B+ Wi-Fi              Mobile App / LINE
  (publish/sub)          (Zigbee2MQTT bridge)        (via webhook)
       │                            │
       │                       Zigbee 2.4GHz
       │                            │
       │                            ▼
       │                  ZBDongle-P (CC2652P)
       │                            │
       │                       Zigbee Mesh
       │                            │
       └─ Wi-Fi 2.4GHz       ┌──────┴────────────────┐
                             ▼      ▼      ▼      ▼
                          IKEA   Plug1  Plug2  SNZB-05
                          Bulb           Motion
```

### 2.3 MQTT Topic Structure (รวมทุกห้อง)

| Topic Pattern | Direction | Description |
|---|---|---|
| `home/<zone>/<sensor>` | Publish | sensor data จาก ESP32 |
| `home/<zone>/<actuator>/state` | Publish | สถานะปัจจุบัน |
| `home/<zone>/<actuator>/set` | Subscribe | คำสั่งจาก HA |
| `home/broadcast/#` | Subscribe | คำสั่ง global (ทุก zone รับ) |
| `zigbee2mqtt/<friendly_name>` | Publish | Zigbee device state |
| `zigbee2mqtt/<friendly_name>/set` | Subscribe | Zigbee control |
| `homeassistant/<entity>` | Publish/Sub | HA MQTT discovery |

---

## 🛏️ ESP32 #1 — Bedroom

### Sensors & Actuators

| Component | GPIO | Type | Purpose |
|-----------|:---:|:---:|---|
| LED ไฟห้องนอน | **2** | OUT | Direct (220Ω) |
| DHT22 | **4** | IN/OUT | อุณหภูมิ + ความชื้น |
| LD2410C TX | **16** | IN (UART2 RX) | mmWave Presence |
| LD2410C RX | **17** | OUT (UART2 TX) | คำสั่ง config |
| Servo SG90 | **25** | PWM | ม่านห้องนอน |
| Relay Fan | **26** | OUT (Active LOW) | พัดลมห้องนอน |
| LDR | **36** (VP) | ADC1_CH0 | แสงสว่าง |

### Wiring Diagram

```
═══════════════════════════════════════════════════════════════════
                    ESP32 #1 BEDROOM
═══════════════════════════════════════════════════════════════════

ESP32 Pin                Component                     Wiring
─────────                ──────────                    ──────
3.3V ────────────────► DHT22 VCC (red)               
GND  ────────────────► DHT22 GND (black)             10kΩ pull-up
GPIO4 ──────────────► DHT22 DATA (yellow) ──┐       3.3V → 10kΩ → GPIO4
                                              └────────► to DHT22 DATA

5V (ext) ───────────► LD2410C VCC                    ⚠️ ต้อง 5V ไม่ใช่ 3.3V
GND      ───────────► LD2410C GND                    
GPIO16 (RX2) ◄──────── LD2410C TX                    UART2 256000 baud
GPIO17 (TX2) ────────► LD2410C RX                    Frame: F4 F3 F2 F1

5V (ext) ───────────► Servo SG90 VCC (red)           ⚠️ 5V ภายนอก
GND      ───────────► Servo SG90 GND (brown)          GND ร่วม ESP32
GPIO25 ─────────────► Servo SG90 Signal (orange)     PWM 50Hz
                                                     SERVO_OPEN  = 1500us (90°)
                                                     SERVO_CLOSE = 600us  (0°)

GPIO26 ─────────────► Relay IN                       Active LOW
                                                     LOW = ON, HIGH = OFF
5V (Power Hub)──────► Relay VCC                      ⚠️ ห้ามต่อ ESP32 5V
GND ────────────────► Relay GND                      
Relay COM ──────────► DC Fan 5V (+)                  
Relay NO ───────────► 5V Power line                  
DC Fan GND ─────────► GND                            

GPIO2 ──────────────► [220Ω] ► LED(+) ► LED(-) ► GND  ไฟห้องนอน
                                                     I = (3.3-2.0)/220 = 5.9mA

3.3V ─── [10kΩ] ─┐
                 ├──► GPIO36 (VP, input-only) ◄─── LDR ─── GND
                 │                                  Voltage divider
                 │                                  bright = 3000+
                 │                                  dark   = < 1000

USB Power ──────────► Micro-USB (ESP32)              From USB Hub
═══════════════════════════════════════════════════════════════════
```

### MQTT Topics

```
PUBLISH:
  home/bedroom/temperature    float °C        (DHT22)
  home/bedroom/humidity       float %         (DHT22)
  home/bedroom/presence       ON/OFF          (LD2410C)
  home/bedroom/fall_alert     ON/OFF          (LD2410C 5min stationary)
  home/bedroom/ldr            int 0-4095      (LDR ADC)
  home/bedroom/light/state    ON/OFF
  home/bedroom/fan/state      ON/OFF
  home/bedroom/curtain/state  OPEN/CLOSED

SUBSCRIBE:
  home/bedroom/light/set      ON/OFF
  home/bedroom/fan/set        ON/OFF
  home/bedroom/curtain/set    OPEN/CLOSE
  home/broadcast/#
```

---

## 🛋️ ESP32 #2 — Living Room

> **โหมด:** Sensor-only (ไม่มี actuator ต่อ ESP32 — ของจริงใช้ Zigbee)
> **Zigbee:** IKEA TRÅDFRI E27 (ไฟ) + S26R2ZB ×2 (พัดลม + เครื่องฟอกอากาศ) + SNZB-03P (PIR)

### Sensors

| Component | GPIO | Type | Purpose |
|-----------|:---:|:---:|---|
| LED [TEST] | **2** | OUT | แทน IKEA bulb ตอนทดสอบ |
| DHT22 | **4** | IN/OUT | อุณหภูมิ + ความชื้น |
| MQ-135 D0 | **27** | IN (digital) | Air alert (LOW=แย่) |
| MQ-135 A0 | **35** | ADC1_CH7 | Air quality raw |
| LDR | **36** (VP) | ADC1_CH0 | แสงสว่าง |

### Wiring Diagram

```
═══════════════════════════════════════════════════════════════════
                  ESP32 #2 LIVING ROOM (Sensor-Only)
═══════════════════════════════════════════════════════════════════

ESP32 Pin                Component                     Wiring
─────────                ──────────                    ──────
3.3V ────────────────► DHT22 VCC                      
GND  ────────────────► DHT22 GND                      10kΩ pull-up
GPIO4 ──────────────► DHT22 DATA                       3.3V → 10kΩ → GPIO4

5V (ext) ───────────► MQ-135 VCC                      ⚠️ ต้อง 5V (heater)
GND      ───────────► MQ-135 GND                      Warm-up 60 วิ
GPIO27 ◄────────────── MQ-135 D0                      LOW = AQI แย่
GPIO35 ◄────────────── MQ-135 A0                      Analog raw

3.3V ─── [1kΩ] ──┐                                   ⚠️ Living Room ใช้ 1kΩ
                 ├──► GPIO36 (VP) ◄─── LDR ─── GND     (บอร์ดอื่นใช้ 10kΩ)
                 │                                    เพราะ LDR ตัวนี้ดู
                 │                                    range ต่างกัน

GPIO2 ──────────────► [220Ω] ► LED(+) ► LED(-) ► GND  [TEST] แทน IKEA bulb

USB Power ──────────► Micro-USB                        From USB Hub
═══════════════════════════════════════════════════════════════════

ZIGBEE DEVICES (ผ่าน Pi — ไม่ต่อ ESP32):
  IKEA TRÅDFRI E27 RGBW  → ขั้วโคม E27 ไฟ 220V บ้าน
  Sonoff S26R2ZB #1      → ปลั๊กผนัง 220V (พัดลม)
  Sonoff S26R2ZB #2      → ปลั๊กผนัง 220V (เครื่องฟอกอากาศ)
  Sonoff SNZB-03P        → battery CR2450 (PIR motion)
═══════════════════════════════════════════════════════════════════
```

### MQTT Topics

```
PUBLISH:
  home/livingroom/temperature    float °C
  home/livingroom/humidity       float %
  home/livingroom/air_quality    int 0-4095   (MQ-135 analog)
  home/livingroom/air_alert      ON/OFF       (MQ-135 digital)
  home/livingroom/ldr            int 0-4095

SUBSCRIBE:
  home/broadcast/#               (รับ broadcast เฉยๆ)
```

---

## 🍳 ESP32 #3 — Kitchen

### Sensors & Actuators

| Component | GPIO | Type | Purpose |
|-----------|:---:|:---:|---|
| LED ไฟครัว | **2** | OUT | Direct (220Ω) |
| DHT22 | **4** | IN/OUT | อุณหภูมิ + ความชื้น |
| MQ-2 D0 | **13** | IN (digital) | Gas alert (LOW=แก๊ส) |
| Relay Fan | **23** | OUT ⚠️ inverted | พัดลมระบาย (HIGH=ON) |
| Relay Stove | **25** | OUT (Active LOW) | จำลองเตาแก๊ส (LOW=ON) |
| MQ-2 A0 | **34** | ADC1_CH6 | Gas raw |
| LDR | **36** (VP) | ADC1_CH0 | แสงสว่าง |

> ⚠️ **GPIO23 ไม่ตรงกับ comment header** — code ใช้ 23 จริง · header เขียน 26 (เก่า)

### Wiring Diagram

```
═══════════════════════════════════════════════════════════════════
                    ESP32 #3 KITCHEN
═══════════════════════════════════════════════════════════════════

ESP32 Pin                Component                     Wiring
─────────                ──────────                    ──────
3.3V ────────────────► DHT22 VCC                      
GND  ────────────────► DHT22 GND                      10kΩ pull-up
GPIO4 ──────────────► DHT22 DATA                       3.3V → 10kΩ → GPIO4

5V (ext) ───────────► MQ-2 VCC                        ⚠️ Heater 5V
GND      ───────────► MQ-2 GND                        Warm-up 60 วิ
GPIO13 ◄────────────── MQ-2 D0                        LOW = แก๊สรั่ว
GPIO34 ◄────────────── MQ-2 A0                        Analog raw

GPIO23 ─────────────► Relay #1 IN  (Fan)              ⚠️ INVERTED
                                                     HIGH=ON, LOW=OFF
5V (Hub) ───────────► Relay #1 VCC                    
GND ────────────────► Relay #1 GND                    
Relay COM ──────────► DC Fan 5V (+)                   
Relay NO ───────────► 5V                              

GPIO25 ─────────────► Relay #2 IN  (Stove)            Active LOW
                                                     LOW=ON, HIGH=OFF
5V (Hub) ───────────► Relay #2 VCC                    
Relay #2 COM ───────► [Heater Sim / LED]              จำลองเตา

GPIO2 ──────────────► [220Ω] ► LED ► GND              ไฟครัว

3.3V ─── [10kΩ] ──┐
                  ├──► GPIO36 (VP) ◄─── LDR ─── GND
═══════════════════════════════════════════════════════════════════

ℹ️ AUTO-SAFETY (สถานะตาม code ปัจจุบัน — DISABLED):
   // setStove(false);  // ถูก comment ไว้
   // setFan(true);     // ถูก comment ไว้
   → ตอนนี้ระบบจะ "publish gas_alert" อย่างเดียว
     ไม่สั่ง stove/fan อัตโนมัติ
   → ใช้ HA Automation แทน (ผ่าน MQTT subscribe gas_alert)
═══════════════════════════════════════════════════════════════════
```

### MQTT Topics

```
PUBLISH:
  home/kitchen/temperature    float °C
  home/kitchen/humidity       float %
  home/kitchen/gas_lpg        int 0-4095     (MQ-2 analog)
  home/kitchen/gas_alert      ON/OFF         (MQ-2 digital)
  home/kitchen/ldr            int 0-4095
  home/kitchen/light/state    ON/OFF
  home/kitchen/fan/state      ON/OFF
  home/kitchen/stove/state    ON/OFF

SUBSCRIBE:
  home/kitchen/light/set      ON/OFF
  home/kitchen/fan/set        ON/OFF
  home/kitchen/stove/set      ON/OFF  (block ถ้า gas_alert ON)
  home/broadcast/#
```

---

## 🚿 ESP32 #4 — Bathroom + Garden

> **โซนรวม 2 ห้อง** (ห้องน้ำ + สวน) บนบอร์ดเดียว

### Sensors & Actuators

| Component | GPIO | Type | Purpose |
|-----------|:---:|:---:|---|
| LED ห้องน้ำ | **2** | OUT | Direct (220Ω) |
| LD2410C TX | **16** | IN (UART2 RX) | Fall detection |
| LD2410C RX | **17** | OUT (UART2 TX) | Config |
| LED ทางเดินสวน | **25** | OUT | 2 LEDs parallel |
| Relay Pump | **26** | OUT (Active LOW) | ปั๊มน้ำสวน |
| LED สวน | **27** | OUT | Direct (220Ω) |
| Soil Moisture | **32** | ADC1_CH4 | ความชื้นดิน |
| **LDR ห้องน้ำ** | **36** (VP) | ADC1_CH0 | แสงห้องน้ำ |
| **LDR สวน** ⭐ | **39** (VN) | ADC1_CH3 | แสงสวน (ใหม่) |

### Wiring Diagram

```
═══════════════════════════════════════════════════════════════════
                ESP32 #4 BATHROOM + GARDEN
═══════════════════════════════════════════════════════════════════

ESP32 Pin                Component                     Wiring
─────────                ──────────                    ──────

5V (ext) ───────────► LD2410C VCC                     ⚠️ 5V ห้ามใช้ 3.3V
GND      ───────────► LD2410C GND
GPIO16 (RX2) ◄──────── LD2410C TX                     UART2 256000
GPIO17 (TX2) ────────► LD2410C RX                     Fall = stationary > 5min

3.3V ───────────────► Soil VCC                        Capacitive v1.2
GND  ───────────────► Soil GND                        ⚠️ ใช้ 3.3V (ไม่ใช่ 5V)
GPIO32 ◄────────────── Soil AOUT                      dry≈3300, wet≈1100

GPIO26 ─────────────► Relay Pump IN                   Active LOW
5V (Hub) ───────────► Relay Pump VCC                  ⚠️ External 5V
GND ────────────────► Relay Pump GND
Relay COM ──────────► Mini Pump 5V (+)
Relay NO ───────────► 5V
Pump GND ───────────► GND

GPIO2 ──────────────► [220Ω] ► LED ► GND              ไฟห้องน้ำ
GPIO27 ─────────────► [220Ω] ► LED ► GND              ไฟสวน
GPIO25 ─┬─► [220Ω] ► LED1 ► GND                       ไฟทางเดิน 1
        └─► [220Ω] ► LED2 ► GND   (parallel)          ไฟทางเดิน 2
                                                     I = 5.9 mA × 2 = 12 mA ✅

3.3V ─── [10kΩ] ──┐
                  ├──► GPIO36 (VP) ◄─── LDR1 ─── GND  แสงห้องน้ำ
3.3V ─── [10kΩ] ──┐
                  ├──► GPIO39 (VN) ◄─── LDR2 ─── GND  แสงสวน ⭐ ใหม่
═══════════════════════════════════════════════════════════════════

ZIGBEE (ผ่าน Pi):
  Sonoff SNZB-05 Water Leak  → ใต้อ่างห้องน้ำ (battery CR2032)
═══════════════════════════════════════════════════════════════════
```

### MQTT Topics

```
PUBLISH:
  home/bathroom/presence       ON/OFF       (LD2410C)
  home/bathroom/fall_alert     ON/OFF       (5min stationary)
  home/bathroom/light/state    ON/OFF
  home/bathroom/ldr            int 0-4095   (LDR GPIO36)
  home/garden/ldr              int 0-4095   (LDR GPIO39 ⭐ ใหม่)
  home/garden/soil_moisture    int 0-100%
  home/garden/soil_raw         int 0-4095
  home/garden/pump_state       ON/OFF
  home/garden/light/state      ON/OFF
  home/garden/pathway/state    ON/OFF

SUBSCRIBE:
  home/bathroom/light/set      ON/OFF
  home/garden/pump/set         ON/OFF
  home/garden/light/set        ON/OFF
  home/garden/pathway/set      ON/OFF
  home/broadcast/#
```

---

## 🚗 ESP32 #5 — Garage

### Sensors & Actuators

| Component | GPIO | Type | Purpose |
|-----------|:---:|:---:|---|
| LED ไฟโรงรถ | **2** | OUT | Direct (220Ω) |
| MQ-2 D0 | **13** | IN (digital) | Smoke alert |
| LED ทางเดิน | **25** | OUT | Direct (220Ω) |
| Servo SG90 | **26** | PWM | ประตูโรงรถ |
| PIR HC-SR501 | **27** | IN | Motion (HIGH=detected) |
| MQ-2 A0 | **34** | ADC1_CH6 | Smoke raw |
| LDR | **36** (VP) | ADC1_CH0 | แสงสว่าง |

### Wiring Diagram

```
═══════════════════════════════════════════════════════════════════
                       ESP32 #5 GARAGE
═══════════════════════════════════════════════════════════════════

ESP32 Pin                Component                     Wiring
─────────                ──────────                    ──────

5V (ext) ───────────► PIR HC-SR501 VCC                ⚠️ 4.5-20V
GND      ───────────► PIR HC-SR501 GND
GPIO27 ◄────────────── PIR HC-SR501 OUT               HIGH = motion
                                                     ปรับ Tx delay + Sx sensitivity
                                                     Edge detection + 30s clear

5V (ext) ───────────► MQ-2 VCC                        Heater 5V
GND      ───────────► MQ-2 GND                        Warm-up 60 วิ
GPIO13 ◄────────────── MQ-2 D0                        LOW = ควัน
GPIO34 ◄────────────── MQ-2 A0                        Analog raw

5V (ext) ───────────► Servo SG90 VCC (red)            ⚠️ External 5V
GND      ───────────► Servo SG90 GND (brown)
GPIO26 ─────────────► Servo SG90 Signal (orange)      PWM 50Hz
                                                     OPEN  = 180°
                                                     CLOSE =   0°
                                                     Detach pattern (ลด jitter)

GPIO2 ──────────────► [220Ω] ► LED ► GND              ไฟโรงรถ
GPIO25 ─────────────► [220Ω] ► LED ► GND              ไฟทางเดิน

3.3V ─── [10kΩ] ──┐
                  ├──► GPIO36 (VP) ◄─── LDR ─── GND
═══════════════════════════════════════════════════════════════════

AUTO-SAFETY:
  if (smoke_alert) → openDoor()     (เปิดประตูระบายอากาศ)
  if (motion + dark) → light ON     (ไฟทางเดินอัตโนมัติตอนกลางคืน)
═══════════════════════════════════════════════════════════════════
```

### MQTT Topics

```
PUBLISH:
  home/garage/smoke_lpg        int 0-4095
  home/garage/smoke_alert      ON/OFF
  home/garage/motion           ON/OFF       (PIR edge-detect)
  home/garage/door/state       OPEN/CLOSED/STOPPED
  home/garage/light/state      ON/OFF
  home/garage/pathway/state    ON/OFF
  home/garage/ldr              int 0-4095

SUBSCRIBE:
  home/garage/door/set         OPEN/CLOSE/STOP
  home/garage/light/set        ON/OFF
  home/garage/pathway/set      ON/OFF
  home/broadcast/#
```

---

## 🟦 Pi 3 Model B+ Wiring

> **บทบาท:** Zigbee Bridge + (optional) Wi-Fi AP backup + Local Mosquitto

### 1. Physical Connections

```
═══════════════════════════════════════════════════════════════════
                    Pi 3 MODEL B+ — Connections
═══════════════════════════════════════════════════════════════════

ด้านบนของบอร์ด:
┌──────────────────────────────────────────────┐
│  ●●●●●●●●●●●●●●●●●●●● 40-pin GPIO header     │  ← ไม่ใช้
│                                               │
│   [ชิป BCM2837B0]                  [USB-A]   │  ← ZBDongle-P
│                                    [USB-A]   │     เสียบรูบนสุด (อันไหนก็ได้)
│                                    [USB-A]   │
│                                    [USB-A]   │
│                                    [LAN]     │  ← (Optional Ethernet)
│  HDMI                                         │
│  3.5mm                                        │
│  Camera CSI / Display DSI                    │
└──────────[Micro-USB Power]───────────────────┘
                  ↑
            สาย Micro-USB จาก USB Hub (5V/2.5A)

ด้านล่าง (พลิกบอร์ด):
┌──────────────────────────────────────────────┐
│         [microSD slot]  ← MicroSD 16GB+      │
└──────────────────────────────────────────────┘
═══════════════════════════════════════════════════════════════════
```

### 2. Software Stack (รัน 3 services)

```
Pi 3 Model B+ (Raspberry Pi OS Lite 64-bit)
├── 🔌 Zigbee2MQTT
│   ├── อ่าน Zigbee จาก /dev/serial/by-id/usb-Sonoff_*
│   ├── Web UI :8080
│   └── Publish ไป EC2 Mosquitto
│
├── 📡 Mosquitto (local) — สำรองตอน internet ตาย
│   └── port 1883 (LAN only)
│
└── 📶 hostapd + dnsmasq — Wi-Fi AP fallback (เปิดเฉพาะตอนไม่มี internet)
    └── SSID "SmartHome-Local" / 192.168.4.1
```

### 3. Boot & Service Flow

```
เสียบไฟ → Pi boot 30-45 วิ → systemd auto-start
                                ├─► mosquitto (local)
                                ├─► zigbee2mqtt
                                └─► (hostapd ถ้าตรวจ uplink ไม่ได้)
                                
Zigbee2MQTT พร้อม → Web UI ที่ http://smarthome-pi.local:8080
                  → Pair devices
                  → MQTT bridge ไป EC2

HA บน EC2 → MQTT discovery → เห็น Zigbee devices อัตโนมัติ
```

---

## 🗺️ GPIO Master Map

> ตารางสรุป GPIO ที่ใช้ในแต่ละบอร์ด — ขาที่เหมือนกันทำหน้าที่ต่างกันได้

| GPIO | Bedroom | Living Room | Kitchen | Bath+Garden | Garage | Type |
|:----:|:--------|:------------|:--------|:------------|:-------|:-----|
| **2** | LED light | LED [test] | LED light | LED bath | LED garage | OUT |
| **4** | DHT22 | DHT22 | DHT22 | — | — | I/O |
| **13** | — | — | MQ-2 D0 | — | MQ-2 D0 | IN |
| **16** | LD2410C TX | — | — | LD2410C TX | — | UART2 RX |
| **17** | LD2410C RX | — | — | LD2410C RX | — | UART2 TX |
| **23** | — | — | **Relay Fan** ⚠️ | — | — | OUT inverted |
| **25** | Servo curtain | — | Relay Stove | LED pathway ×2 | LED pathway | OUT/PWM |
| **26** | Relay Fan | — | — | Relay Pump | Servo door | OUT/PWM |
| **27** | — | MQ-135 D0 | — | LED garden | PIR | I/O |
| **32** | — | — | — | Soil A0 | — | ADC1_CH4 |
| **34** | — | — | MQ-2 A0 | — | MQ-2 A0 | ADC1_CH6 |
| **35** | — | MQ-135 A0 | — | — | — | ADC1_CH7 |
| **36** (VP) | LDR | LDR | LDR | LDR bath | LDR | ADC1_CH0 input-only |
| **39** (VN) | — | — | — | LDR garden ⭐ | — | ADC1_CH3 input-only |

### GPIO ที่ห้ามใช้ (สำคัญ)

| GPIO | สาเหตุ |
|:---:|---|
| 0 | Boot mode (อย่าใช้ — ทำให้บอร์ดไม่บูท) |
| 1, 3 | UART0 (Serial Monitor) |
| 6-11 | Flash SPI (built-in flash) |
| 12 | Boot strap (HIGH ตอน boot อาจมีปัญหา) |
| 15 | Boot strap (debug log) |

### ADC2 ห้ามใช้ตอนเปิด Wi-Fi

GPIO 0, 2, 4, 12, 13, 14, 15, 25, 26, 27 = ADC2 → **ห้ามใช้ analogRead เมื่อ Wi-Fi active**

> ใช้ ADC1 เท่านั้น: GPIO 32, 33, 34, 35, 36, 39

---

## 🔧 Common Wiring Patterns

### A. Relay Module Active LOW (มาตรฐาน)

```
ESP32 GPIO ──► Relay IN
5V (Power Hub) ──► Relay VCC      ⚠️ ห้าม ESP32 5V
GND (shared) ──► Relay GND

digitalWrite(RELAY_PIN, LOW);  // ON
digitalWrite(RELAY_PIN, HIGH); // OFF
```

### B. Relay สำหรับ Load 5V (Pump/Fan)

```
Relay COM ──► Load (+)
Relay NO  ──► 5V Power line
Load (-) ──► GND

(NO = Normally Open — ปิดเมื่อ relay OFF, เปิดเมื่อ relay ON)
```

### C. LDR Voltage Divider

```
3.3V
  │
 [10kΩ]
  │
  ├──► GPIO36 (VP) ADC input
  │
 [LDR GL5528]
  │
 GND

ADC ค่ามาก (>3000) = สว่าง (LDR resistance ต่ำ)
ADC ค่าน้อย (<1000) = มืด (LDR resistance สูง)
```

### D. DHT22 with Pull-up

```
3.3V ──┬── DHT22 VCC
       │
     [10kΩ]
       │
ESP32 GPIO4 ──┴── DHT22 DATA
       
GND ────── DHT22 GND
```

### E. UART2 LD2410C

```
ESP32 GPIO16 (UART2 RX) ◄──── LD2410C TX
ESP32 GPIO17 (UART2 TX) ────► LD2410C RX
5V external ────► LD2410C VCC  (3.3V ไม่พอ — ต้อง 5V)
GND ────► LD2410C GND

Code:
  Serial2.begin(256000, SERIAL_8N1, 16, 17);
  Frame format: F4 F3 F2 F1 [data] F8 F7 F6 F5
  State byte at index 8: 0=none, 1=move, 2=stationary, 3=both
```

### F. Servo SG90 PWM

```
5V external ────► Servo VCC  (red)
GND (shared) ────► Servo GND  (brown)
ESP32 GPIO ────► Servo Signal (orange)

Code:
  servo.attach(SERVO_PIN);
  servo.writeMicroseconds(1500);  // 90° (middle)
  servo.writeMicroseconds(600);   // 0°
  servo.writeMicroseconds(2400);  // 180°
  servo.detach();                 // ลด jitter หลังหยุด
```

---

## ⚠️ Known Quirks

### 1. Kitchen Fan ใช้ GPIO23 ไม่ใช่ GPIO26

```cpp
// File: esp32_03_kitchen.ino
// Header comment เขียน:  GPIO26 → Relay Fan (เก่า)
// แต่ #define จริง:        GPIO23 ← ใช้จริง

#define RELAY_FAN    23   // <-- ของจริง
```

→ ต่อสายตามตัวเลข `#define` ใน code ไม่ใช่ comment

### 2. Kitchen Fan Inverted Logic

```cpp
void setFan(bool on) {
  digitalWrite(RELAY_FAN, on ? HIGH : LOW);  // <-- INVERTED
}

// แต่ Stove ใช้ Active LOW ปกติ:
void setStove(bool on) {
  digitalWrite(RELAY_STOVE, on ? LOW : HIGH);
}
```

→ Relay Fan ใน Kitchen เป็น Active HIGH (น่าจะเพราะใช้ relay module คนละแบบ)

### 3. Living Room ใช้ R = 1kΩ สำหรับ LDR

บอร์ดอื่นใช้ 10kΩ — Living Room ใช้ 1kΩ เพราะ characteristic ต่างกัน (อาจเป็น LDR คนละรุ่น)

### 4. Auto-Safety in Kitchen (DISABLED — by design)

```cpp
// File: esp32_03_kitchen.ino, line ~210
// ปิดไว้ — ใช้ HA Automation แทน (ดู #6 ใน HA_automations.yaml):
// setStove(false);
// setFan(true);
```

→ Code ปัจจุบันไม่ตัดสินใจ auto-safety บน ESP32
→ ส่ง `gas_alert = ON` ผ่าน MQTT → HA รับ → trigger automation

### 5. ESP32 #5 Garage มี SERVO บน GPIO26

> Bedroom servo อยู่ GPIO25 · Garage servo อยู่ GPIO26 — ไม่เหมือนกัน
> เพราะ Garage GPIO25 = LED pathway, GPIO27 = PIR (กิน slot ปกติของ servo)

---

## 📊 BOM Summary (ปริมาณรวม)

| Component | จำนวน | ห้องที่มี |
|---|:---:|---|
| ESP32 DevKit V1 38-pin | **5** | ทุกห้อง |
| DHT22 | **3** | Bedroom, Living, Kitchen |
| MQ-2 | **2** | Kitchen, Garage |
| MQ-135 | **1** | Living Room |
| HLK-LD2410C | **2** | Bedroom, Bathroom |
| PIR HC-SR501 | **1** | Garage |
| LDR GL5528 | **6** | Bedroom, Living, Kitchen, Bath, Garden ⭐, Garage |
| Soil Moisture v1.2 | **1** | Garden |
| Servo SG90 | **2** | Bedroom (curtain), Garage (door) |
| Relay 1-Channel 5V | **4** | Bedroom (fan), Kitchen ×2 (fan + stove), Bath+Garden (pump) |
| DC Fan 5V | **2** | Bedroom, Kitchen |
| Mini Pump 5V | **1** | Garden |
| LED 5mm + 220Ω | **~12** | LED ทุกห้อง + ทางเดินสวน 2 + parallel etc. |
| Resistor 10kΩ | **~10** | DHT22 pull-ups + LDR dividers |
| Resistor 1kΩ | **1** | LDR Living Room |

---

*Last Updated: 6 พ.ค. 2026 — Hardware ติดตั้งครบ 100% (เหลือแค่ Pi + Zigbee)*
*เอกสารนี้สะท้อน firmware code ปัจจุบัน 1:1 — ไม่มีคำแนะนำให้แก้ code*

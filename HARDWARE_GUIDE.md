# HARDWARE GUIDE — Smart Home AIoT Project (Model House Version)
> Last Updated: **May 5, 2026** — Model House (โมเดลบ้านจำลอง) — **v2 Final** (5 zones, EC2 t3.small, Pi 3B+)
>
> **What's new in v2:**
> - Expanded from 4 → **5 ESP32 zones** (added Kitchen as standalone)
> - EC2 upgraded `t3.micro` → `t3.small` (1.9 GB RAM) — new IP: **YOUR_EC2_IP**
> - Pi: switched from Pi 3 Model B+ → **Pi 3 Model B+** (more RAM + native USB-A)
> - Added Servo 360° (continuous rotation) for curtain + garage
> - Added LDR GL5528 (×3) for ambient light sensing
> - LD2410C used in 2 zones (Bedroom presence + Bathroom fall detection)

---

## Architecture Overview — Always-On Layers

```
╔══════════════════════════════════════════════════════════════════════════╗
║  POWER LAYER                                                             ║
║                                                                          ║
║  Wall 220V AC ──► USB-C 65W Adapter ──► Power Bank 20,000 mAh          ║
║                                              │                           ║
║                              (passthrough — charges while powering)      ║
║                                              │                           ║
║                                         USB Hub 4-Port                  ║
║                                   ┌──────────┼──────────┐               ║
║                                   │          │          │               ║
║                              ESP32 ×5    Pi 3 Model B+ (spare)          ║
║                                                                          ║
║  When wall power FAILS → Power Bank auto-continues → ~20 hrs runtime    ║
╠══════════════════════════════════════════════════════════════════════════╣
║  NETWORK LAYER — Normal Operation                                        ║
║                                                                          ║
║  Home Router (Wi-Fi) ◄──► ESP32 #1, #2, #3, #4, #5                     ║
║                      ◄──► Pi 3 Model B+                                  ║
║                      ◄──► Amazon Echo Dot 4th Gen                        ║
║                            │                                             ║
║                            └──► Internet ──► AWS EC2 t3.small           ║
║                                              (YOUR_EC2_IP)               ║
║                                              HA + Mosquitto MQTT         ║
║                                                                          ║
║  NETWORK LAYER — Backup (when router/internet fails)                     ║
║                                                                          ║
║  Pi 3 Model B+ (AP mode) ──► "SmartHome-Local" Wi-Fi hotspot            ║
║                    ◄──► ESP32 #1, #2, #3, #4, #5 (auto-fallback)        ║
║                    ◄──► Local Mosquitto MQTT (on Pi)                     ║
║                         (basic automations keep running offline)          ║
╠══════════════════════════════════════════════════════════════════════════╣
║  HARDWARE ZONES (โมเดลบ้านจำลอง — 5 zones)                              ║
║                                                                          ║
║  ESP32 #1 — Bedroom    │ DHT22 + LD2410C + Servo 360° + LDR + LED       ║
║                          (presence detect + curtain control)             ║
║  ESP32 #2 — Living Rm  │ DHT22 + MQ-135 + LDR + LED (sensor-only)       ║
║                          (air-quality monitoring)                        ║
║  ESP32 #3 — Kitchen    │ DHT22 + MQ-2 + Relay→DC Fan 5V + LED           ║
║                          (gas/smoke detection + auto-vent)               ║
║  ESP32 #4 — Bath+Garden│ DHT22 + LD2410C + Soil + Relay→Pump + LED      ║
║                          (fall detection + plant watering)               ║
║  ESP32 #5 — Garage     │ PIR + MQ-2 + Servo 360° + LED                  ║
║                          (motion edge-detect + garage door)              ║
║                                                                          ║
║  Pi 3 Model B+         │ Zigbee Bridge + Local MQTT + Wi-Fi AP Backup   ║
║  Zigbee Network        │ IKEA TRÅDFRI + S26R2ZB ×2 + SNZB-03P + 05      ║
╠══════════════════════════════════════════════════════════════════════════╣
║  CLOUD LAYER                                                             ║
║                                                                          ║
║  AWS EC2       ── Home Assistant + (Cloud MQTT bridge)                   ║
║  Railway       ── n8n Workflow + LINE Bot Chawi                          ║
║  Groq          ── LLaMA 3.3 70B AI Engine                               ║
║  Supabase      ── PostgreSQL (chat logs, sensor logs)                    ║
║  AWS Lambda    ── Alexa Skill Backend                                    ║
╚══════════════════════════════════════════════════════════════════════════╝
```

**Always-On Resilience Flow:**

```
Normal:    ESP32 ──► Home Router ──► Internet ──► AWS HA ──► Automations
Backup:    ESP32 ──► Pi AP "SmartHome-Local" ──► Pi Mosquitto ──► Local Automations
```

ESP32 dual Wi-Fi config: tries home router first (timeout 10s) → falls back to Pi AP automatically.

---

## Complete Hardware List (BOM) — 27 Items

---

### 1. Microcontroller — ESP32 DevKit V1 (38-pin)

| Detail | Spec |
|---|---|
| Model | Espressif ESP32 DevKit V1 (38-pin) |
| Chip | ESP32-WROOM-32 / ESP32-WROOM-32D |
| CPU | Xtensa dual-core LX6, 240 MHz |
| RAM | 520 KB SRAM |
| Flash | 4 MB |
| Wi-Fi | 802.11 b/g/n 2.4 GHz (built-in dual-config: home router + Pi AP fallback) |
| GPIO | 34 pins (ADC 12-bit, DAC, UART, I2C, SPI, PWM) |
| Supply | 5V via Micro-USB |
| Quantity | **5 units** (Bedroom, Living Room, Kitchen, Bathroom+Garden, Garage) |
| Price | ~200 THB/unit → **total ~1,000 THB** |
| Shopee | [ESP32 DevKit V1 38pin](https://shopee.co.th/search?keyword=ESP32+DevKit+V1+38pin) |

> Buy the 38-pin version (not 30-pin) for full GPIO availability. Brands: AZ-Delivery or DOIT.

---

### 2. Temperature & Humidity Sensor — DHT22 AM2302

| Detail | Spec |
|---|---|
| Model | DHT22 / AM2302 (Aosong Electronics) |
| Range | Temperature -40°C to +80°C, Humidity 0-100% RH |
| Accuracy | ±0.5°C, ±2-5% RH |
| Protocol | Single-wire digital |
| Supply | 3.3V-5V DC |
| Quantity | **4 units** (Bedroom, Living Room, Kitchen, Bathroom) |
| Price | ~100 THB/unit → **total ~400 THB** |
| Shopee | [DHT22 sensor module](https://shopee.co.th/search?keyword=DHT22+sensor+module) |

---

### 3. Motion Sensor — PIR HC-SR501

| Detail | Spec |
|---|---|
| Model | HC-SR501 Passive Infrared Motion Sensor |
| Detection angle | 120° (adjustable via potentiometer) |
| Range | 3-7 meters (adjustable) |
| Supply | 4.5V-20V DC |
| Output | Digital HIGH (3.3V) on detection |
| Quantity | **2 units** (PIR #1: Living Room, PIR #2: Garage) |
| Price | ~60 THB/unit → **total ~120 THB** |
| Shopee | [PIR HC-SR501](https://shopee.co.th/search?keyword=PIR+HC-SR501) |

---

### 4. Presence Sensor — mmWave HLK-LD2410

| Detail | Spec |
|---|---|
| Model | Hi-Link HLK-LD2410 / HLK-LD2410C |
| Technology | 24 GHz FMCW Millimeter Wave Radar |
| Key feature | Detects stationary people (sleeping, reading) — not just motion |
| Range (moving) | Up to 6 meters |
| Range (stationary) | Up to 3 meters |
| Angle | ±60° horizontal |
| Interface | UART TX/RX (3.3V) |
| Supply | 5V DC |
| Quantity | **2 units** (Bedroom — sleep presence; Bathroom — fall detection 5min stationary) |
| Price | ~280 THB/unit → **total ~560 THB** |
| Shopee | [HLK-LD2410 mmwave](https://shopee.co.th/search?keyword=HLK-LD2410+mmwave) |
| Frame format | `F4 F3 F2 F1 [data] F8 F7 F6 F5` (header/footer) — UART 256000 baud |

---

### 5. Gas Sensor — MQ-2 (LPG/Smoke)

| Detail | Spec |
|---|---|
| Model | MQ-2 Gas Sensor Module (Hanwei Electronics) |
| Detects | LPG, Propane, Butane, Hydrogen, Methane, Smoke |
| Range | 300-10,000 ppm |
| Output | Analog (A0) + Digital (D0) |
| Supply | 5V DC |
| Warm-up | ~60 seconds after first power-on |
| Quantity | **2 units** (Kitchen — gas/smoke; Garage — smoke alert) |
| Price | ~75 THB/unit → **total ~150 THB** |
| Shopee | [MQ-2 gas sensor module](https://shopee.co.th/search?keyword=MQ-2+gas+sensor+module) |

---

### 6. Air Quality Sensor — MQ-135

| Detail | Spec |
|---|---|
| Model | MQ-135 Air Quality Sensor Module (Hanwei Electronics) |
| Detects | NH3, CO2, CO, Benzene, Alcohol, Smoke |
| Range | 10-300 ppm |
| Output | Analog (A0) + Digital (D0) |
| Supply | 5V DC |
| Warm-up | ~60 seconds |
| Quantity | **1 unit** (Living Room) |
| Price | ~75 THB |
| Shopee | [MQ-135 air quality sensor](https://shopee.co.th/search?keyword=MQ-135+air+quality+sensor) |

---

### 7. Soil Moisture Sensor — Capacitive v1.2

| Detail | Spec |
|---|---|
| Model | Capacitive Soil Moisture Sensor v1.2 |
| Type | Capacitive (corrosion-resistant, more durable than resistive) |
| Output | Analog (0-3.3V) |
| Supply | 3.3V-5V |
| Raw values | Dry ~3300 / Wet ~1100 (soil-dependent) |
| Quantity | **1 unit** (Garden / plant pot in model) |
| Price | ~80 THB |
| Shopee | [capacitive soil moisture sensor v1.2](https://shopee.co.th/search?keyword=capacitive+soil+moisture+sensor+v1.2) |

---

### 8. Water Leak Sensor — Sonoff SNZB-05 (Zigbee)

| Detail | Spec |
|---|---|
| Model | Sonoff SNZB-05 Zigbee Water Sensor |
| Protocol | Zigbee 3.0 (no Wi-Fi needed) |
| Detection | Probe contact with water |
| Battery | CR2032 |
| Alert path | Zigbee2MQTT → Pi Mosquitto → Home Assistant → LINE |
| Quantity | **1 unit** (Bathroom / under sink) |
| Price | ~380 THB |
| Shopee | [Sonoff SNZB-05 water sensor](https://shopee.co.th/search?keyword=Sonoff+SNZB-05+water+sensor) |

---

### 9. Relay Module — 4-Channel 5V (×4 units)

| Detail | Spec |
|---|---|
| Model | 4-Channel 5V Relay Module (Active LOW) |
| Contact rating | 10A / 250VAC or 10A / 30VDC |
| Control signal | Active LOW — GPIO LOW = Relay ON |
| Coil supply | 5V DC (connect directly to power, not ESP32 5V pin) |
| Control voltage | 3.3V-5V GPIO compatible |
| Quantity | **4 units** (Kitchen, Bath+Garden, +2 spare) — Living Rm is sensor-only, Garage uses servo (no relay) |
| Price | ~100 THB/unit → **total ~400 THB** |
| Shopee | [relay module 1 channel 5v](https://shopee.co.th/search?keyword=relay+module+1+channel+5v) |

Relay assignments (v2 — 5 zones):
- ESP32 #1 (Bedroom): NO relay — uses Servo 360° on GPIO25 for curtain
- ESP32 #2 (Living Room): NO relay — sensor-only zone (DHT22 + MQ-135 + LDR)
- ESP32 #3 (Kitchen): Relay GPIO26 → DC Fan 5V (vent for gas/smoke alert)
- ESP32 #4 (Bath+Garden): Relay → Mini Water Pump 5V (auto-watering)
- ESP32 #5 (Garage): NO relay — uses Servo 360° on GPIO25 for garage door

> WARNING: Connect Relay VCC directly to 5V from Power Hub — never from the ESP32 5V pin (insufficient current). Share GND with ESP32.

---

### 10. Mini Water Pump — 5V DC Submersible

| Detail | Spec |
|---|---|
| Model | Mini Submersible Water Pump 5V DC |
| Supply | 5V DC (model-scale — replaces 12V real pump) |
| Current | ~500-800 mA at 5V |
| Flow rate | ~80-120 L/h |
| Control | Relay #3 → ESP32 #3 → MQTT |
| Quantity | **1 unit** (garden/plant watering simulation) |
| Price | ~50 THB |
| Shopee | [mini water pump 5v submersible](https://shopee.co.th/search?keyword=mini+water+pump+5v+submersible) |

---

### 11. DC Fan 5V Mini — ×2

| Detail | Spec |
|---|---|
| Model | DC Fan 5V Mini Cooling Fan |
| Supply | 5V DC |
| Purpose | Simulates AC/real fan in model house (replaces real AC unit) |
| Control | Relay #1 (Bedroom), Relay #2 (Living Room) |
| Quantity | **2 units** |
| Price | ~50 THB/unit → **total ~100 THB** |
| Shopee | [dc fan 5v mini cooling](https://shopee.co.th/search?keyword=dc+fan+5v+mini+cooling) |

---

### 12. LED 5mm + Resistor 220Ohm — Set

| Detail | Spec |
|---|---|
| Type | LED 5mm white/warm white + 220Ohm resistor |
| Purpose | Simulates room lights in model house (replaces real bulbs) |
| Wiring | GPIO → 220Ohm resistor → LED anode → LED cathode → GND |
| GPIO | GPIO2 on each ESP32 (one LED per zone) |
| Quantity | **1 set** (multiple LEDs + resistors pack) |
| Price | ~50 THB |
| Shopee | [LED 5mm white resistor pack](https://shopee.co.th/search?keyword=LED+5mm+white+resistor+pack) |

---

### 13. Servo Motor — SG90

| Detail | Spec |
|---|---|
| Model | SG90 Continuous Rotation (360°) — modified version |
| Torque | 1.8 kg·cm |
| Signal | PWM (50Hz) — speed-based, NOT position-based |
| Supply | 5V DC (use external 5V, not ESP32 5V pin) |
| Control | CW (~135), STOP (~90), CCW (~45) — write microseconds to spin |
| PWM pin | GPIO25 on ESP32 #1 (Bedroom curtain) and ESP32 #5 (Garage door) |
| Open timing | `CURTAIN_OPEN_MS = 1500ms` for full open/close motion |
| Quantity | **2 units** (Bedroom curtain + Garage door) |
| Price | ~80 THB/unit → **total ~160 THB** |
| Shopee | [servo motor 360 continuous rotation](https://shopee.co.th/search?keyword=servo+motor+360+continuous+rotation) |

> **v2 update:** Switched from standard SG90 (180° position-based) → **360° continuous rotation** because curtain/garage door need controlled timed motion, not absolute angles. Use `servo.write(SERVO_CW)` then `delay(1500)` then `servo.write(STOP)` and detach to reduce jitter.

---

### 14. Zigbee Smart Plug — Sonoff S26R2ZB (×2, demo only)

| Detail | Spec |
|---|---|
| Model | Sonoff S26R2ZB Zigbee Smart Plug |
| Protocol | Zigbee 3.0 (no Wi-Fi needed) |
| Max load | 2300W / 10A |
| Voltage | 100-240V AC |
| Energy monitoring | Yes (Watt/Volt/Ampere) |
| Zigbee role | Router (extends mesh) |
| Quantity | **2 units** (demo units placed outside model, not embedded) |
| Price | ~300 THB/unit → **total ~600 THB** |
| Shopee | [Sonoff S26R2ZB](https://shopee.co.th/search?keyword=Sonoff+S26R2ZB) |

> Note: Only 2 Smart Plugs for this model version (demo/display outside model house). Used to demonstrate Zigbee integration and energy monitoring in presentation.

---

### 15. Zigbee Coordinator — Sonoff ZBDongle-E (V2)

| Detail | Spec |
|---|---|
| Model | Sonoff Zigbee 3.0 USB Dongle Plus V2 (ZBDongle-E variant) |
| Chip | Silicon Labs EFR32MG21 (NOT CC2652P as previously assumed) |
| Protocol | Zigbee 3.0 |
| Interface | USB-A (connected directly to Pi 3 Model B+ — no OTG needed) |
| Built-in PA | Yes (extended range) |
| Firmware | **EmberZNet 7.4.4 [GA]** (pre-flashed, confirmed via Z2M boot log) |
| Z2M adapter type | **`ember`** (NOT `zstack` — verified 2026-05-13 during install) |
| USB chip | Silicon Labs CP210x UART Bridge (USB ID `10c4:ea60`) |
| Connected to | Pi 3 Model B+ via USB-A port |
| Quantity | **1 unit** |
| Price | ~700 THB |
| Shopee | [Sonoff ZBDongle-E zigbee dongle](https://shopee.co.th/search?keyword=Sonoff+ZBDongle-E+zigbee+dongle) |

> ⚠️ **Important:** ตอนซื้อมาทีแรกคิดว่าเป็น ZBDongle-E (CC2652P chip) แต่พอตอนติดตั้งจริง 13 พ.ค. 2026 Z2M ระบุว่า dongle ใช้ **EmberZNet** firmware = ZBDongle-E variant ต้อง config `adapter: ember` ไม่ใช่ `adapter: zstack` — ลอง zstack จะติด `Failed to connect to the adapter (SRSP - SYS - ping after 6000ms)` แล้ว container restart loop

---

### 16. Local Hub — Raspberry Pi 3 Model B+ (v2)

| Detail | Spec |
|---|---|
| Model | **Raspberry Pi 3 Model B+** (upgraded from Pi 3 Model B+ in v2) |
| CPU | Broadcom BCM2837B0 Cortex-A53 Quad-core 1.4 GHz |
| RAM | **1 GB LPDDR2** (2× more than Pi 3 Model B+) |
| Wi-Fi | 802.11 b/g/n/ac 2.4 & 5 GHz (built-in) |
| Ethernet | Gigabit Ethernet (300 Mbps actual) |
| USB | **4× USB-A 2.0** (no OTG adapter needed for ZBDongle) |
| OS | Raspberry Pi OS Lite 64-bit |
| Supply | 5V 2.5A via Micro-USB |
| Quantity | **1 unit** |
| Price | ~1,400 THB (มีของอยู่แล้ว — หาได้แค่ตัวนี้ในท้องตลาด) |
| Shopee | [Raspberry Pi 3 Model B+](https://shopee.co.th/search?keyword=Raspberry+Pi+3+Model+B+plus) |

> **v2 reasoning:** Pi 3 Model B+ (512 MB RAM) ตึงเกินไปเมื่อรัน Zigbee2MQTT + Mosquitto + hostapd + dhcpcd พร้อมกัน → Pi 3B+ (1GB RAM) เผื่อขยายและมี Ethernet สำรองได้

**Pi 3 Model B+ runs 3 roles simultaneously:**

```
Role 1: Zigbee Bridge
  └── Zigbee2MQTT → manages SNZB-05 + Smart Plug ×2
  └── Bridges Zigbee device state → MQTT topics

Role 2: Local MQTT Broker (Mosquitto)
  └── Runs Mosquitto MQTT broker on Pi
  └── ESP32s publish/subscribe to Pi's broker
  └── Keeps automations alive when internet is down
  └── Bridges to AWS HA when internet is available

Role 3: Wi-Fi AP Backup (hostapd)
  └── Creates "SmartHome-Local" hotspot when home router fails
  └── SSID: SmartHome-Local | Password: YOUR_AP_PASSWORD
  └── All ESP32s auto-connect to Pi AP (second SSID in firmware)
  └── Local Mosquitto continues serving MQTT over Pi AP
```

---

### 17. MicroSD Card — 16GB SanDisk

| Detail | Spec |
|---|---|
| Model | SanDisk Ultra microSD 16GB |
| Class | Class 10 / UHS-I |
| Purpose | OS storage for Pi 3 Model B+ |
| Quantity | **1 unit** |
| Price | ~150 THB |
| Shopee | [microsd 16gb sandisk](https://shopee.co.th/search?keyword=microsd+16gb+sandisk) |

---

### 18. Voice Control — Amazon Echo Dot 4th Gen

| Detail | Spec |
|---|---|
| Model | Amazon Echo Dot 4th Generation |
| Wi-Fi | 802.11 a/b/g/n/ac (2.4 & 5 GHz) |
| Wake word | "Alexa" |
| Supply | 15W USB-C (included) |
| Function | Voice commands → Alexa Custom Skill "Chawi" → n8n → Groq AI → HA |
| Quantity | **1 unit** (Living Room, optional — see budget) |
| Price | ~2,300 THB |
| Shopee | [Amazon Echo Dot 4th](https://shopee.co.th/search?keyword=Amazon+Echo+Dot+4th) |

---

### 19. Power Bank — 20,000 mAh

| Detail | Spec |
|---|---|
| Capacity | 20,000 mAh |
| Key feature | Passthrough charging (charges via USB-C while providing power) |
| Output | USB-A ports (to USB Hub) |
| Input | USB-C (from 65W adapter) |
| Runtime estimate | ~20 hours powering 4× ESP32 + Pi 3 Model B+ |
| Purpose | Always-on power backup when wall power fails |
| Quantity | **1 unit** |
| Price | ~600 THB |
| Shopee | [power bank 20000mah](https://shopee.co.th/search?keyword=power+bank+20000mah) |

> IMPORTANT: Verify the Power Bank supports passthrough mode (some brands disable output while charging). Check product description or reviews before purchase.

---

### 20. USB Hub — 4-Port Powered

| Detail | Spec |
|---|---|
| Model | USB Hub 4-Port Powered |
| Ports | 4× USB-A |
| Purpose | Distributes power from Power Bank to ESP32 ×4 + Pi 3 Model B+ |
| Quantity | **1 unit** |
| Price | ~150 THB |
| Shopee | [usb hub 4 port powered](https://shopee.co.th/search?keyword=usb+hub+4+port+powered) |

---

### 21. USB-C Adapter — 65W

| Detail | Spec |
|---|---|
| Output | 65W USB-C |
| Purpose | Charges Power Bank via USB-C while system is running |
| Quantity | **1 unit** |
| Price | ~150 THB |
| Shopee | [usb c adapter 65w](https://shopee.co.th/search?keyword=usb+c+adapter+65w) |

---

### 22. Jumper Wire M-F 40pcs — ×2 sets

| Detail | Spec |
|---|---|
| Type | Male-to-Female, 20cm |
| Quantity | **2 sets (80 wires total)** |
| Price | ~50 THB/set → **total ~100 THB** |
| Shopee | [jumper wire male female 40pcs](https://shopee.co.th/search?keyword=jumper+wire+male+female+40pcs) |

---

### 23. Breadboard 830 MB-102 — ×4

| Detail | Spec |
|---|---|
| Model | MB-102 830-point Breadboard |
| Quantity | **4 units** (one per ESP32 zone) |
| Price | ~40 THB/unit → **total ~160 THB** |
| Shopee | [breadboard 830 mb102](https://shopee.co.th/search?keyword=breadboard+830+mb102) |

---

### 24. Micro-USB OTG Adapter — ×1

| Detail | Spec |
|---|---|
| Type | Micro-USB OTG (USB-A female → Micro-USB male) |
| Purpose | Connect ZBDongle-E (USB-A) to Pi 3 Model B+ (Micro-USB) |
| Quantity | **1 unit** |
| Price | ~30 THB |
| Shopee | [micro usb otg adapter](https://shopee.co.th/search?keyword=micro+usb+otg+adapter) |

---

### 25. Silicone Tube 6mm + Accessories

| Detail | Spec |
|---|---|
| Size | 6mm ID silicone tube |
| Purpose | Water routing from mini pump to plant pots in model |
| Included | Tube + connectors + clamps |
| Quantity | **1 set** |
| Price | ~230 THB |
| Shopee | [silicone tube 6mm](https://shopee.co.th/search?keyword=silicone+tube+6mm) |

---

### 26. Transistor NPN 2N2222 — Pack

| Detail | Spec |
|---|---|
| Model | 2N2222 NPN Transistor |
| Purpose | Drive relays or motors that need more current than GPIO can supply |
| Quantity | **1 pack** |
| Price | ~20 THB |
| Shopee | [transistor 2N2222 NPN pack](https://shopee.co.th/search?keyword=transistor+2N2222+NPN+pack) |

---

### 27. Micro-USB Cable 1m — ×5

| Detail | Spec |
|---|---|
| Type | Micro-USB cable 1 meter |
| Purpose | Power cables for ESP32 ×5 + Pi 3 Model B+ |
| Quantity | **6 units** |
| Price | ~30 THB/unit → **total ~180 THB** |
| Shopee | [micro usb cable 1m](https://shopee.co.th/search?keyword=micro+usb+cable+1m) |

---

## Room-by-Room Wiring — All 5 ESP32 Zones (v2)

> Firmware files: `esp32_firmware/esp32_0X_<zone>.ino` — 5 standalone sketches, one per zone.
> All zones publish initial state on MQTT connect (กัน HA แสดง "ไม่ทราบสถานะ").
> All zones use `MQTT_SERVER = "YOUR_EC2_IP"` (EC2 t3.small) port 1883.

---

### ESP32 #1 — Bedroom Zone (`esp32_01_bedroom.ino`)

**Sensors:** DHT22 + LD2410C mmWave + LDR GL5528
**Actuators:** LED (light) + Servo 360° (curtain)
**v2 changes:** Removed PIR (mmWave is better for bedroom), added LDR + Servo curtain

```
ESP32 #1 — BEDROOM WIRING
══════════════════════════════════════════════════════════
  3.3V ─────────────────────────────────── DHT22 VCC
  GND  ─────────────────────────────────── DHT22 GND
  GPIO4 ────────────────────────────────── DHT22 DATA
                                           (+ 10kOhm pull-up to 3.3V)

  5V   ─────────────────────────────────── mmWave LD2410C VCC  (must be 5V!)
  GND  ─────────────────────────────────── mmWave LD2410C GND
  GPIO16 (UART2 RX) ────────────────────── mmWave TX
  GPIO17 (UART2 TX) ────────────────────── mmWave RX
                                           (UART 256000 baud, frame F4 F3 F2 F1)

  3.3V ─── 10kOhm ─── GPIO36 (VP, ADC1_CH0, input-only) ─── LDR ─── GND
                      (voltage divider — bright = high voltage)

  GPIO2 ─── [220Ohm] ─── LED(+) ─── LED(-) ─── GND
                          (bedroom light simulation)

  5V (ext) ─────────────────────────────── Servo 360° VCC (red)
  GND (shared) ─────────────────────────── Servo 360° GND (brown)
  GPIO25 ───────────────────────────────── Servo 360° Signal (orange)
                                           SERVO_CW=135, STOP=90, CCW=45
                                           CURTAIN_OPEN_MS = 1500ms

  Micro-USB ────────────────────────────── USB Hub (from Power Bank)
══════════════════════════════════════════════════════════
```

**MQTT Topics (ESP32 #1):**
```
home/bedroom/temperature   → float (°C)         [DHT22]
home/bedroom/humidity      → float (%)          [DHT22]
home/bedroom/lux           → int (raw ADC 0-4095) [LDR]
home/bedroom/presence      → "ON" / "OFF"       [LD2410C — detects stationary too]
home/bedroom/light         → "ON" / "OFF"       [LED via GPIO2]
home/bedroom/curtain       → "open" / "closed"  [Servo 360° timing]
```

---

### ESP32 #2 — Living Room Zone (`esp32_02_livingroom.ino`)

**Sensors:** DHT22 + MQ-135 Air Quality + LDR GL5528
**Actuators:** LED (test indicator only — no relay)
**v2 changes:** **Sensor-only zone** (removed MQ-2 + Relay → moved to Kitchen). Added LDR.

```
ESP32 #2 — LIVING ROOM WIRING
══════════════════════════════════════════════════════════
  3.3V ─────────────────────────────────── DHT22 VCC
  GND  ─────────────────────────────────── DHT22 GND
  GPIO4 ────────────────────────────────── DHT22 DATA
                                           (+ 10kOhm pull-up to 3.3V)

  5V   ─────────────────────────────────── MQ-135 VCC
  GND  ─────────────────────────────────── MQ-135 GND
  GPIO34 (ADC1_CH6, input-only) ────────── MQ-135 A0
                                           (60s warm-up before reading)

  3.3V ─── 10kOhm ─── GPIO36 (VP, input-only) ─── LDR ─── GND
                      (voltage divider — ambient light)

  GPIO2 ─── [220Ohm] ─── LED(+) ─── LED(-) ─── GND
                          (living room test indicator)

  Micro-USB ────────────────────────────── USB Hub (from Power Bank)
══════════════════════════════════════════════════════════

NOTE: GPIO34 and GPIO36 are input-only with no internal pull-up.
NOTE: MQ-135 requires 60-second warm-up after power-on.
```

**MQTT Topics (ESP32 #2):**
```
home/livingroom/temperature  → float (°C)
home/livingroom/humidity     → float (%)
home/livingroom/air_quality  → int (raw ADC) [MQ-135 — CO2/NH3/etc]
home/livingroom/lux          → int (raw ADC) [LDR]
home/livingroom/light        → "ON" / "OFF"  [LED state]
```

---

### ESP32 #3 — Kitchen Zone (`esp32_03_kitchen.ino`) — NEW in v2

**Sensors:** DHT22 + MQ-2 Gas/Smoke
**Actuators:** LED + Relay (Active LOW) → DC Fan 5V (vent)

```
ESP32 #3 — KITCHEN WIRING
══════════════════════════════════════════════════════════
  3.3V ─────────────────────────────────── DHT22 VCC
  GND  ─────────────────────────────────── DHT22 GND
  GPIO4 ────────────────────────────────── DHT22 DATA
                                           (+ 10kOhm pull-up to 3.3V)

  5V   ─────────────────────────────────── MQ-2 VCC
  GND  ─────────────────────────────────── MQ-2 GND
  GPIO34 (ADC1_CH6, input-only) ────────── MQ-2 A0
                                           (60s warm-up before reading)

  GPIO2 ─── [220Ohm] ─── LED(+) ─── LED(-) ─── GND
                          (kitchen light)

  GPIO26 ───────────────────────────────── Relay IN (Active LOW)
  5V (DIRECT to Power Hub) ─────────────── Relay VCC  ⚠️ NOT from ESP32 5V pin!
  GND (shared) ─────────────────────────── Relay GND
  Relay COM ─── DC Fan 5V (+)
  Relay NO  ─── 5V power line
  DC Fan GND ── GND

  Micro-USB ────────────────────────────── USB Hub (from Power Bank)
══════════════════════════════════════════════════════════

⚠️ KNOWN ISSUE (fixed): หลีกเลี่ยงการต่อ Relay VCC เข้าที่ ESP32 5V pin โดยตรง
   เคยเจอปัญหา: solder joint บน ESP32 board มีรอยแยก → Relay ไม่ดับสนิท
   → ต่อตรงจาก Power Hub 5V เสมอ

AUTO-SAFETY (currently DISABLED for testing):
  if (gasAlert) {
      // setStove(false);   // ปิดเตา
      // setFan(true);      // เปิดพัดลมระบาย
  }
  Re-enable for production demo.
```

**MQTT Topics (ESP32 #3):**
```
home/kitchen/temperature  → float (°C)
home/kitchen/humidity     → float (%)
home/kitchen/gas_lpg      → int (raw ADC)   [MQ-2]
home/kitchen/gas_alert    → "ON" / "OFF"
home/kitchen/light        → "ON" / "OFF"
home/kitchen/fan          → "ON" / "OFF"    [DC Fan via Relay]
```

---

### ESP32 #4 — Bathroom + Garden Zone (`esp32_04_bathroom_garden.ino`)

**Sensors:** DHT22 + LD2410C mmWave (fall detection) + Capacitive Soil Moisture
**Actuators:** LED + Relay → Mini Pump 5V
**v2 changes:** Added LD2410C mmWave for **fall detection** (5 min stationary threshold)

```
ESP32 #4 — BATHROOM + GARDEN WIRING
══════════════════════════════════════════════════════════
  3.3V ─────────────────────────────────── DHT22 VCC
  GND  ─────────────────────────────────── DHT22 GND
  GPIO4 ────────────────────────────────── DHT22 DATA
                                           (+ 10kOhm pull-up to 3.3V)

  5V   ─────────────────────────────────── mmWave LD2410C VCC  (must be 5V)
  GND  ─────────────────────────────────── mmWave LD2410C GND
  GPIO16 (UART2 RX) ────────────────────── mmWave TX
  GPIO17 (UART2 TX) ────────────────────── mmWave RX
                                           Frame: F4 F3 F2 F1 [data] F8 F7 F6 F5

  3.3V ─────────────────────────────────── Soil Moisture VCC
  GND  ─────────────────────────────────── Soil Moisture GND
  GPIO35 (ADC1_CH7) ────────────────────── Soil Moisture AOUT
                                           (analog: dry~3300, wet~1100)

  GPIO2 ─── [220Ohm] ─── LED(+) ─── LED(-) ─── GND
                          (bathroom light)

  GPIO26 ───────────────────────────────── Relay IN (Active LOW)
  5V (Power Hub direct) ────────────────── Relay VCC
  GND (shared) ─────────────────────────── Relay GND
  Relay COM ─── Mini Pump 5V (+)
  Relay NO  ─── 5V power line
  Pump GND  ─── GND

  Micro-USB ────────────────────────────── USB Hub (from Power Bank)
══════════════════════════════════════════════════════════

FALL DETECTION LOGIC:
  - LD2410C raw byte 8 = state (0=no presence, 1=moving, 2=stationary, 3=both)
  - If presence stationary > 5 minutes → publish home/bathroom/fall = ON
  - HA automation → LINE alert via n8n workflow

WATER LEAK (Zigbee — no wiring to ESP32):
  Sonoff SNZB-05 ──[Zigbee]──► ZBDongle-E on Pi 3 Model B+
  Pi Zigbee2MQTT publishes: zigbee2mqtt/water_leak → {"water_leak": true/false}
```

**MQTT Topics (ESP32 #4):**
```
home/bathroom/temperature   → float (°C)
home/bathroom/humidity      → float (%)
home/bathroom/presence      → "ON" / "OFF"   [LD2410C]
home/bathroom/fall          → "ON" / "OFF"   [LD2410C — 5min stationary]
home/bathroom/light         → "ON" / "OFF"
home/garden/soil_raw        → int (raw ADC)
home/garden/soil_moisture   → int (% mapped)
home/garden/pump_state      → "ON" / "OFF"   [Relay]
[Water leak via Zigbee2MQTT — not ESP32 MQTT]
```

---

### ESP32 #5 — Garage Zone (`esp32_05_garage.ino`)

**Sensors:** PIR HC-SR501 (edge detection) + MQ-2 (smoke alert)
**Actuators:** LED + Servo 360° (garage door)
**v2 changes:** Added MQ-2, switched relay+strip → Servo 360° for door, smart edge detection

```
ESP32 #5 — GARAGE WIRING
══════════════════════════════════════════════════════════
  5V   ─────────────────────────────────── PIR HC-SR501 VCC
  GND  ─────────────────────────────────── PIR HC-SR501 GND
  GPIO27 ───────────────────────────────── PIR HC-SR501 OUT
                                           (Tx delay + Sx sensitivity pots)

  5V   ─────────────────────────────────── MQ-2 VCC
  GND  ─────────────────────────────────── MQ-2 GND
  GPIO34 (ADC1_CH6, input-only) ────────── MQ-2 A0

  GPIO2 ─── [220Ohm] ─── LED(+) ─── LED(-) ─── GND
                          (garage indicator light)

  5V (ext) ─────────────────────────────── Servo 360° VCC (red)
  GND (shared) ─────────────────────────── Servo 360° GND (brown)
  GPIO25 ───────────────────────────────── Servo 360° Signal (orange)
                                           Detach pattern (reduce jitter)
                                           CW=135 (open), STOP=90, CCW=45 (close)

  Micro-USB ────────────────────────────── USB Hub (from Power Bank)
══════════════════════════════════════════════════════════

SMART PIR EDGE DETECTION (firmware):
  const unsigned long MOTION_TIMEOUT = 30000;  // 30s auto-clear
  const unsigned long DEBOUNCE_MS    = 200;
  - Tracks LOW→HIGH transitions (rising edges)
  - Each edge resets timeout
  - After 30s without new edge → auto-clear motion = OFF
  - Solves "PIR always detects" problem (without rotating sensor)
```

**MQTT Topics (ESP32 #5):**
```
home/garage/motion          → "ON" / "OFF"   [PIR edge-detected]
home/garage/smoke_alert     → "ON" / "OFF"   [MQ-2 threshold]
home/garage/light           → "ON" / "OFF"
home/garage/door            → "open" / "closed"  [Servo 360°]
```

---

## Pi 3 Model B+ — Triple-Role Setup (v2)

```
Pi 3 Model B+ — HARDWARE CONNECTIONS
══════════════════════════════════════════════════════════
  USB-A port 1 ──► Sonoff ZBDongle-E (direct, no OTG needed)
  Micro-USB PWR ──► USB Hub port (from Power Bank, 5V 2.5A)
  Wi-Fi (built-in dual-band) ──► Home Router (normal)
                              ──► AP mode "SmartHome-Local" (backup)
  Ethernet (Gigabit) ──► (optional fallback)
══════════════════════════════════════════════════════════

SOFTWARE STACK:
  /etc/zigbee2mqtt/        → Zigbee2MQTT config
  /etc/mosquitto/          → Mosquitto MQTT broker
  /etc/hostapd/hostapd.conf → Wi-Fi AP "SmartHome-Local"
  /etc/dnsmasq.conf        → DHCP for AP clients
  /etc/systemd/system/     → All services auto-start on boot

ROLE 1 — Zigbee Bridge:
  Zigbee2MQTT reads ZBDongle-E serial
  Publishes device states → local Mosquitto
  Topics:
    zigbee2mqtt/smart_plug_1   → {"state": "ON/OFF", "power": 42.3}
    zigbee2mqtt/smart_plug_2   → {"state": "ON/OFF", "power": 15.1}
    zigbee2mqtt/water_leak     → {"water_leak": false}

ROLE 2 — Local MQTT Broker (Mosquitto):
  Listens on port 1883 (LAN only)
  ESP32s connect to: 192.168.1.x:1883 (home network)
                  or 192.168.4.1:1883 (Pi AP network)
  Bridges to AWS HA MQTT when internet available
  Falls back to local-only when internet down
  Retains last-known values for all topics

ROLE 3 — Wi-Fi AP Backup (hostapd):
  Triggered: when uplink check fails (ping 8.8.8.8 fails 3x)
  Creates hotspot: SSID=SmartHome-Local, Pass=YOUR_AP_PASSWORD
  IP range: 192.168.4.x
  Pi IP on AP: 192.168.4.1
  ESP32 firmware: tries home router → timeout 10s → tries Pi AP
  Recovery: when router comes back → ESP32s reconnect automatically

ZIGBEE NETWORK (Pi manages):
  ZBDongle-E (Coordinator)
  ├── Sonoff S26R2ZB #1  ── switch.demo_plug_1  [demo outside model]
  ├── Sonoff S26R2ZB #2  ── switch.demo_plug_2  [demo outside model]
  └── Sonoff SNZB-05     ── binary_sensor.water_leak  [bathroom]
```

---

## Always-On Resilience Architecture

```
NORMAL OPERATION:
─────────────────────────────────────────────────────────
Wall 220V ─► USB-C 65W ─► Power Bank ─► USB Hub ─► Devices
                           (charging)
Home Router ─► Internet ─► AWS EC2 (Home Assistant)
ESP32s ──[Wi-Fi]──► Home Router ──[MQTT]──► AWS HA
Pi ──────[Wi-Fi]──► Home Router ──[Zigbee2MQTT]──► AWS HA

WHEN WALL POWER FAILS:
─────────────────────────────────────────────────────────
Power Bank (20,000mAh) ─► USB Hub ─► all devices continue
Duration: ~20 hours
Internet: still works (router on its own UPS or battery)
System: transparent failover, no interruption

WHEN ROUTER/INTERNET FAILS (but power OK):
─────────────────────────────────────────────────────────
Pi detects: ping 8.8.8.8 fails 3 consecutive times
Pi action:  activates hostapd → creates "SmartHome-Local" AP
ESP32 action: home router connection drops → tries Pi AP → connects
MQTT:  ESP32s now connect to Pi Mosquitto at 192.168.4.1:1883
Automations that continue locally:
  - PIR-triggered lights (Automation #4, #14)
  - Fan on temperature threshold (Automation #3)
  - Pump on soil moisture (Automation #8, #9)
  - Garage door via PIR (Automation #14, #15)
  - Gas alert logic (Automation #6, #7)
Automations that need cloud (suspended until internet returns):
  - LINE notifications (#6, #10)
  - GPS-based modes (#1, #2)
  - Alexa voice control
  - AI responses via Groq

WHEN POWER BANK DEPLETES (>20hrs outage):
─────────────────────────────────────────────────────────
System shuts down gracefully
On power restore: Pi boots → all services auto-start
ESP32s reconnect within 30 seconds of Pi being ready
```

---

## 15 Automations

| # | Trigger | Condition | Action |
|---|---|---|---|
| 1 | GPS → arrives home | house_mode = away | house_mode=home, LED welcome ON, Servo CW (garage opens) |
| 2 | GPS → leaves home | house_mode = home | house_mode=away, all devices OFF, Servo CCW (garage closes) |
| 3 | home/kitchen/temperature > 30°C | any | Kitchen Relay ON → DC Fan vent ON |
| 4 | home/bedroom/presence = ON | nighttime 22:00-06:00 | LED bedroom ON dim, Servo curtain CCW (close) |
| 5 | home/garage/motion = ON | any | LED garage ON, sticky 30s edge timeout |
| 6 | home/kitchen/gas_lpg > 500 (ADC) | any | LINE alert + Auto-safety: stove off, vent on |
| 7 | home/livingroom/air_quality HIGH | any | LINE notice + suggest open window |
| 8 | home/garden/soil_moisture < 30% | pump is OFF | Bath/Garden Relay ON → Mini Pump ON |
| 9 | home/garden/soil_moisture > 70% | pump is ON | Bath/Garden Relay OFF → Mini Pump OFF |
| 10 | binary_sensor.water_leak = ON | any | LINE alert immediately + Bath/Garden Relay OFF |
| 11 | home/bathroom/fall = ON | any | LINE emergency alert (5 min stationary detected) |
| 12 | Time = 06:00 | any | Morning: bedroom curtain open (Servo CW), Daily Report → LINE |
| 13 | Time = 22:00 | any | Night: all LEDs OFF, bedroom curtain close |
| 14 | home/garage/smoke_alert = ON | any | LINE alert + Garage door close (Servo CCW) |
| 15 | LINE chat with ชาวี | any | Groq AI parses → HA service call |

---

## Complete Budget Table

| # | Item | Qty | Price (THB) |
|---|---|---|---|
| 1 | ESP32 DevKit V1 38-pin | 5 | ~1,000 |
| 2 | DHT22 AM2302 Sensor | 4 | ~400 |
| 3 | PIR HC-SR501 | 1 | ~60 |
| 4 | mmWave HLK-LD2410C | 2 | ~560 |
| 5 | MQ-2 Gas Sensor Module | 2 | ~150 |
| 6 | MQ-135 Air Quality Sensor | 1 | ~75 |
| 7 | Capacitive Soil Moisture v1.2 | 1 | ~80 |
| 8 | LDR GL5528 + 10kOhm Resistor | 3 | ~30 |
| 9 | Sonoff SNZB-05 Water Leak | 1 | ~380 |
| 10 | Sonoff SNZB-03P Motion (Zigbee) | 1 | ~300 |
| 11 | Relay Module 1-Channel 5V (Active LOW) | 4 | ~400 |
| 12 | Mini Water Pump 5V DC | 1 | ~50 |
| 13 | DC Fan 5V Mini | 2 | ~100 |
| 14 | LED 5mm + Resistor 220Ohm set | 1 | ~50 |
| 15 | Servo Motor 360° (continuous rotation) | 2 | ~160 |
| 16 | Sonoff S26R2ZB Smart Plug | 2 | ~600 |
| 17 | IKEA TRÅDFRI E27 LED bulb | 1 | ~600 |
| 18 | Sonoff ZBDongle-E | 1 | ~700 |
| 19 | Raspberry Pi 3 Model B+ | 1 | ~1,400 |
| 20 | MicroSD 16GB SanDisk | 1 | ~150 |
| 21 | Power Bank 20,000 mAh | 1 | ~600 |
| 22 | USB Hub 4-Port Powered | 1 | ~150 |
| 23 | USB-C Adapter 65W | 1 | ~150 |
| 24 | Jumper Wire M-F 40pcs | 2 sets | ~100 |
| 25 | Breadboard 830 MB-102 | 5 | ~200 |
| 26 | Silicone Tube 6mm + Accessories | 1 | ~230 |
| 27 | Transistor NPN 2N2222 pack | 1 | ~20 |
| 28 | Micro-USB Cable 1m | 6 | ~180 |
| 29 | Amazon Echo Dot 4th Gen | 1 | ~2,300 |
| | **TOTAL without Echo Dot** | | **~8,575 THB** |
| | **TOTAL with Echo Dot** | | **~10,875 THB** |

> **Cloud costs (monthly):**
> - AWS EC2 t3.small: ~$15/month (~525 THB) after Free Tier ends
> - Railway (n8n) Pro plan: $20/month (~700 THB) — supports 24/7 webhooks
> - Groq API: Free tier (rate-limited but enough for thesis)
> - Supabase: Free tier (500MB DB)
> - Total recurring: ~1,225 THB/month after Free Tiers expire

> Cloud costs (monthly, after project): AWS EC2 ~280-420 THB/month, Railway (n8n) ~200-400 THB/month. AWS Free Tier covers first 12 months.

---

## Important Notes

### 1. Relay Wiring — External 5V Required

The Relay Module coil needs more current than the ESP32 can safely supply through its 5V or 3.3V pins. Always connect:
- Relay VCC → USB Hub 5V directly (or Power Bank output)
- Relay GND → shared GND with ESP32
- Relay IN → GPIO26 (Active LOW — GPIO LOW turns relay ON)

### 2. Servo SG90 PWM — Correct Frequency

SG90 requires exactly 50Hz PWM. Use ESP32 LEDC peripheral:
- `ledcSetup(channel, 50, 16)` — 50Hz, 16-bit resolution
- CLOSED (0°): duty ~3276 (~1ms pulse width)
- OPEN (90°): duty ~7864 (~2ms pulse width)
- Do not use `analogWrite()` — frequency will be wrong.

### 3. Power Bank Passthrough Mode

Not all Power Banks support simultaneous charging and output (passthrough). Verify the specific model supports this before purchase. Look for labels like "Pass-through charging" or check that the device does not cut output when USB-C input is connected. Recommended brands: Anker, Aukey, Baseus (verify specific model).

### 4. ESP32 Dual Wi-Fi Firmware Logic

```cpp
// Pseudocode for dual Wi-Fi fallback
WiFi.begin(HOME_SSID, HOME_PASS);
delay(10000);  // wait 10 seconds
if (WiFi.status() != WL_CONNECTED) {
    WiFi.disconnect();
    WiFi.begin("SmartHome-Local", "YOUR_AP_PASSWORD");
    // connect to Pi AP and use 192.168.4.1 as MQTT broker
    mqtt_broker = "192.168.4.1";
} else {
    mqtt_broker = HOME_ROUTER_IP;  // Pi's IP on home network
}
```

### 5. MQ Sensor 60-Second Warm-up

MQ-2 and MQ-135 require a warm-up period. Firmware must wait 60 seconds after boot before reading and publishing ADC values, otherwise readings will be inaccurate and may trigger false alerts.

### 6. GPIO Input-Only Pins

ESP32 GPIO 34, 35, 36, 39 are input-only (no pull-up, no output). Use them exclusively for ADC input (MQ-2, MQ-135). Do not attempt to use them for output.

### 7. Pi 3 Model B+ Boot Time

Pi 3 Model B+ takes approximately 30-45 seconds to boot and start all services. ESP32s should implement MQTT reconnect logic with a retry interval (5-10 seconds) so they reconnect automatically once Pi is ready after a power cycle.

### 10. EC2 IP migration (Lessons learned — v2)

When upgrading EC2 instance type (t3.micro → t3.small), the public IP changes (54.226.244.180 → YOUR_EC2_IP in our case). **All these places must be updated:**
- ESP32 firmware: `MQTT_SERVER` constant in every `.ino` file (5 zones)
- HA `configuration.yaml` (if it references EC2 IP for other services)
- n8n workflows: every HTTP Request node calling HA — use n8n REST API to bulk-edit:
  ```bash
  curl -X PUT -H "X-N8N-API-KEY: $KEY" \
    -H "Content-Type: application/json" \
    --data @workflow.json \
    https://YOUR_N8N_INSTANCE.up.railway.app/api/v1/workflows/<id>
  ```
- Zigbee2MQTT `configuration.yaml`: `mqtt: server: mqtt://YOUR_EC2_IP:1883`
- Mobile HA Companion app: Server URL field

> Use Elastic IP next time to avoid this — but it has cost ~$3.6/month.

### 11. LD2410C frame format (NOT 0xAA 0xFF)

LD2410C uses Hi-Link's binary protocol with header `F4 F3 F2 F1` and footer `F8 F7 F6 F5`, NOT the generic `0xAA 0xFF` that some tutorials show. Frame size is 23 bytes. State byte is at index 8: 0=no presence, 1=moving, 2=stationary, 3=both. Use UART2 at 256000 baud.

### 12. Servo 360° vs Servo 180°

Standard SG90 (180°) is **position-based** — write angle 0-180. But for curtain/garage door we need **timed motion** — that requires **continuous-rotation 360° servo** which is **speed-based**:
- Write CW (~135) → motor spins clockwise
- Write STOP (~90) → motor stops
- Write CCW (~45) → motor spins counter-clockwise
- Use `delay(CURTAIN_OPEN_MS)` for travel time, then write STOP

Detach servo signal pin between operations to reduce jitter and current draw.

### 8. Zigbee Pairing Procedure

To pair a Smart Plug or SNZB-05 for the first time:
1. Open Zigbee2MQTT web UI on Pi (port 8080)
2. Click "Permit Join" (all devices)
3. Press and hold the button on the device until LED blinks
4. Device appears in Zigbee2MQTT within 30 seconds
5. Rename device entity in Home Assistant

### 9. Model House Scale Considerations

- DC Fans 5V simulate real fans/AC — they produce airflow but no cooling load calculation needed
- LEDs simulate room lighting — brightness is not proportional to real bulbs
- Mini Pump 5V operates at lower pressure — use short silicone tube runs (<30cm) for reliable operation
- Servo SG90 simulates garage door — attach a small cardboard flap to the servo arm

---

*Last Updated: April 21, 2026 — Model House (โมเดลบ้านจำลอง) Final Version*

# 🏠 Smart Home AIoT — บันทึกการพัฒนาโครงการตั้งแต่ต้นจนปัจจุบัน

**ชื่อโครงการ:** Smart Home AIoT (Senior Thesis Project)
**เจ้าของ:** CE08
**ชื่อบอท:** ชาวี (Chawi) — ผู้ช่วยบ้านอัจฉริยะ AI เพศชาย ภาษาไทย
**วันที่อัปเดต:** 5 พฤษภาคม 2026 (v2 — multi-zone hardware deployed)
**Platform:** macOS → AWS EC2 t3.small (Ubuntu 22.04, YOUR_EC2_IP), Railway Cloud (Pro plan), AWS Lambda
**Status:** Software 100% ✅ | Hardware Wiring 100% ✅ | Zigbee/Echo รอติดตั้ง

---

## 📋 สารบัญ

1. [ที่มาและเป้าหมาย](#1-ที่มาและเป้าหมาย)
2. [Technology Stack และเหตุผล](#2-technology-stack-และเหตุผล)
3. [Phase 1 — Infrastructure Setup (Docker)](#3-phase-1--infrastructure-setup-docker)
4. [Phase 2 — Home Assistant Configuration](#4-phase-2--home-assistant-configuration)
5. [Phase 3 — ESP32 Firmware](#5-phase-3--esp32-firmware)
6. [Phase 4 — n8n Workflow + LINE Bot ชาวี](#6-phase-4--n8n-workflow--line-bot-ชาวี)
7. [Phase 5 — LINE Rich Menu](#7-phase-5--line-rich-menu)
8. [Phase 6 — Remote Access (ngrok + Tailscale)](#8-phase-6--remote-access-ngrok--tailscale)
9. [Phase 7 — Alexa Custom Skill](#9-phase-7--alexa-custom-skill)
10. [Phase 8 — Cloud Migration (AWS EC2)](#10-phase-8--cloud-migration-aws-ec2)
11. [ปัญหาและวิธีแก้ทั้งหมด](#11-ปัญหาและวิธีแก้ทั้งหมด)
12. [Architecture ปัจจุบัน](#12-architecture-ปัจจุบัน)
13. [Hardware Plan](#13-hardware-plan)
14. [Roadmap ที่เหลือ](#14-roadmap-ที่เหลือ)
15. [สิ่งที่ได้เรียนรู้](#15-สิ่งที่ได้เรียนรู้)
16. [Phase 9 — v2 Multi-Zone Hardware Deployment (Apr-May 2026)](#16-phase-9--v2-multi-zone-hardware-deployment)
17. [Phase 10 — EC2 Upgrade & RAM Crisis (May 2026)](#17-phase-10--ec2-upgrade--ram-crisis)
18. [Phase 11 — Wiring & Hardware Debug Session (May 2026)](#18-phase-11--wiring--hardware-debug-session)
19. [Phase 12 — n8n Workflow IP Migration via REST API (May 5, 2026)](#19-phase-12--n8n-workflow-ip-migration-via-rest-api)

---

## 1. ที่มาและเป้าหมาย

### ที่มาของโครงการ

โครงการนี้เริ่มจากความต้องการสร้างระบบบ้านอัจฉริยะสำหรับโปรเจคจบ โดยโจทย์หลักคือต้องการระบบที่:

1. **ใช้งานจริงได้** — ไม่ใช่แค่ demo สั้นๆ แต่ทำงานจริงแบบ 24/7
2. **ควบคุมผ่าน LINE** — เพราะคนไทยใช้ LINE เป็น primary messaging app
3. **มี AI** — บอทตอบสนองภาษาธรรมชาติ ไม่ใช่แค่ keyword matching
4. **รองรับ Voice Control** — ผ่าน Amazon Alexa สำหรับ hands-free control
5. **มีระบบ IoT จริง** — ESP32 + Sensors วัดค่าจริง
6. **ใช้ Zigbee** — protocol ประหยัดพลังงาน แสดงความเข้าใจ wireless IoT
7. **ทำงานได้จากทุกที่** — ผ่าน cloud ไม่ต้องเปิดคอมทิ้งไว้

### เป้าหมายสุดท้าย

```
ผู้ใช้พูดหรือพิมพ์ภาษาไทย → ระบบเข้าใจ → ควบคุมอุปกรณ์ในบ้านได้จริง
ทุกอย่างทำงานบน Cloud → ปิด Mac ได้ → ระบบยังทำงาน 24/7
```

### ชื่อบอท "ชาวี"

ชาวี (Chawi) เป็นผู้ช่วยบ้านอัจฉริยะ AI เพศชาย อารมณ์ดี เป็นกันเอง พูดภาษาไทย ตอบสนองคำสั่งและคำถามเกี่ยวกับบ้านด้วยภาษาธรรมชาติ

---

## 2. Technology Stack และเหตุผล

| เครื่องมือ | หน้าที่ | เหตุผลที่เลือก |
|-----------|---------|--------------|
| **Docker** | Container orchestration | Portable, isolate services, ย้าย server ได้ง่าย |
| **Home Assistant** | Smart home controller | Open source, รองรับ MQTT+Zigbee, automation ดี |
| **Mosquitto MQTT** | IoT message broker | Lightweight, pub/sub pattern, มาตรฐาน IoT |
| **Zigbee2MQTT** | Zigbee bridge | แปลง Zigbee → MQTT, รองรับ device หลายพัน |
| **n8n (Railway)** | Workflow automation | Visual workflow, public webhook URL, ฟรี |
| **Groq LLaMA 3.3 70B** | AI language model | ฟรี API, เร็วมาก, JSON mode, ภาษาไทยดี |
| **LINE Messaging API** | Chat bot platform | คนไทยใช้ LINE เป็นหลัก, rich features |
| **AWS EC2** | Cloud server | Free tier 1 ปี, public IP, reliable |
| **AWS Lambda** | Alexa backend | Serverless, integrate Alexa ง่าย |
| **Alexa Custom Skill** | Voice control | รองรับ complex intent, natural language |
| **ESP32** | IoT microcontroller | WiFi built-in, ราคาถูก, community ใหญ่ |
| **DHT22** | Temp/Humidity sensor | แม่นยำกว่า DHT11, digital output |
| **MQ-2** | Gas/Smoke sensor | ราคาถูก, ตรวจจับ LPG/smoke/CO ได้ |
| **Zigbee Smart Plug** | Smart power control | ประหยัดพลังงาน, IEEE 802.15.4 |
| **Tailscale** | VPN | Peer-to-peer, ง่ายกว่า OpenVPN |
| **ngrok** | Tunnel (ใช้ช่วงแรก) | Public URL สำหรับ HA ตอนยังใช้ local |

### เหตุผลที่เลือก Zigbee

Zigbee ใช้ IEEE 802.15.4 low-power protocol ซึ่ง:
- ประหยัดพลังงานกว่า WiFi มาก (สำคัญสำหรับ IoT device)
- สร้าง mesh network ได้ — device แต่ละตัวทำหน้าที่ relay signal ให้กัน
- Latency ต่ำ เหมาะกับ home automation
- ช่วง frequency 2.4GHz ไม่รบกวน WiFi มากนัก

---

## 3. Phase 1 — Infrastructure Setup (Docker)

### เครื่องมือที่ใช้
- Docker Desktop (macOS)
- Docker Compose
- Terminal / bash scripting

### สิ่งที่ทำ

สร้าง `docker-compose.yml` กำหนด 4 services ทำงานร่วมกัน:

```yaml
services:
  homeassistant:   # ระบบควบคุมบ้านหลัก
  mosquitto:       # MQTT broker สำหรับ IoT
  zigbee2mqtt:     # แปลง Zigbee → MQTT
  n8n:             # Automation engine (ใช้ช่วงแรก)
```

**ไฟล์ที่สร้างทั้งหมด:**
- `docker-compose.yml` — กำหนด services, volumes, networks
- `.env` — environment variables (API keys, passwords)
- `.env.example` — template สำหรับ reference
- `mosquitto.conf` — config MQTT (anonymous login, persistence, port 1883+9001)
- `setup.sh` — script ติดตั้งอัตโนมัติ
- `.gitignore` — ป้องกัน push credential ขึ้น git

**คำสั่งที่ใช้บ่อย:**
```bash
docker compose up -d          # Start all services
docker compose ps             # Check status
docker compose logs -f homeassistant  # ดู HA logs
docker compose restart homeassistant  # Restart HA
```

### เหตุผลที่ใช้ Docker

แทนที่จะติดตั้ง HA โดยตรง Docker ทำให้:
1. ติดตั้งง่ายด้วยคำสั่งเดียว
2. Services แยกกันอย่างชัดเจน ไม่ conflict กัน
3. ย้าย server ในอนาคต (Mac → EC2) ทำได้สะดวก — copy config แล้วรันใหม่ได้เลย
4. ลบ/สร้างใหม่ได้ง่ายถ้ามีปัญหา

### ปัญหาที่เจอ

| ปัญหา | วิธีแก้ |
|-------|---------|
| Docker network conflict | สร้าง custom bridge network "smarthome" |
| HA volume permission denied | `sudo chmod 777 volumes/homeassistant` |
| MQTT container ไม่ start | ตรวจสอบ mosquitto.conf syntax |

---

## 4. Phase 2 — Home Assistant Configuration

### เครื่องมือที่ใช้
- Home Assistant YAML configuration
- MQTT integration
- Lovelace UI (yaml mode)

### สิ่งที่ทำ

#### 4.1 MQTT Switches (8 ตัว)

แต่ละ switch มี command_topic (สั่งงาน) และ state_topic (รับสถานะ):

```yaml
mqtt:
  switch:
    - name: "Living Room Light"
      command_topic: "home/livingroom/light/set"
      state_topic: "home/livingroom/light/state"
      payload_on: "ON"
      payload_off: "OFF"
```

Switches ทั้งหมด: living_room_light, living_room_fan, bedroom_light, bedroom_fan, kitchen_vent_fan, garden_water_pump, garden_light, front_door_lock

#### 4.2 MQTT Sensors (5 ตัว)

```yaml
mqtt:
  sensor:
    - name: "Living Room Temperature"
      state_topic: "home/livingroom/temperature"
      unit_of_measurement: "°C"
      device_class: temperature
```

Sensors: อุณหภูมิห้องนั่งเล่น, ความชื้นห้องนั่งเล่น, อุณหภูมิห้องนอน, แก๊สห้องครัว, ความชื้นดินสวน

#### 4.3 Input Selects (Modes)

```yaml
input_select:
  house_mode:
    options: [Home, Away, Security]
    initial: Home
  comfort_mode:
    options: [Normal, Relax, Warm, Sleep]
    initial: Normal
```

#### 4.4 Automations (13 รายการ)

Automation สำคัญ:
- **Auto Fan Temperature** — temp > 30°C → เปิดพัดลมทุกตัว
- **Motion Light Night** — ตรวจพบการเคลื่อนไหว 20:00-06:00 → เปิดไฟ 5 นาที
- **Gas Alert** — gas > 400 ppm → ปิดไฟฟ้า + แจ้งเตือน
- **Welcome Home GPS** — ใกล้บ้าน < 200m → เปิดไฟ + ปลดล็อค
- **Leave Home GPS** — ออกห่างบ้าน > 500m → Away mode
- **Good Morning/Night** — schedule เปิด/ปิดอัตโนมัติ

#### 4.5 Emulated Hue (สำหรับ Alexa)

```yaml
emulated_hue:
  host_ip: 0.0.0.0
  listen_port: 80
  expose_by_default: false
  entities:
    switch.living_room_light:
      name: "Living Room Light"
      hidden: false
    # ... อีก 7 device
```

ทำให้ Alexa มองเห็น HA device เหมือน Philips Hue bridge สั่งได้: "Alexa, turn on living room light"

### ปัญหาที่เจอ

| ปัญหา | สาเหตุ | วิธีแก้ |
|-------|--------|---------|
| HA returning 400 via ngrok | trusted_proxies ไม่รวม ngrok IPs | เพิ่ม `0.0.0.0/0` ใน trusted_proxies |
| Switch state ค้างที่ "unknown" | ยังไม่มี ESP32 publish state กลับ | ปกติ — รอ hardware มาต่อ |

---

## 5. Phase 3 — ESP32 Firmware

### เครื่องมือที่ใช้
- Arduino IDE
- C++ / Arduino framework
- PubSubClient library (MQTT)
- DHT library

### สิ่งที่ทำ

สร้างไฟล์ `smart_home_esp32.ino` — firmware สำหรับ ESP32 แต่ละตัว:

```cpp
// โครงสร้างหลัก
void setup() {
  WiFi.begin(ssid, password);      // เชื่อม WiFi
  client.connect(mqtt_broker);     // เชื่อม MQTT broker
  client.subscribe("home/livingroom/light/set");  // รับคำสั่ง
}

void loop() {
  // อ่านค่า sensor
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  // ส่งค่าไป MQTT
  client.publish("home/livingroom/temperature", String(temp).c_str());
  client.publish("home/livingroom/humidity", String(hum).c_str());

  // รับคำสั่งและควบคุม relay
  client.loop();
}
```

**ฟังก์ชันหลัก:**
- เชื่อม WiFi → MQTT broker (EC2)
- อ่านค่า DHT22 ทุก 30 วินาที → publish ไป MQTT
- Subscribe topic คำสั่ง → toggle relay on/off
- Auto reconnect ถ้า connection ขาด

**สถานะ:** Code พร้อมแล้ว รอแค่ hardware มาถึงแล้ว flash ได้เลย

---

## 6. Phase 4 — n8n Workflow + LINE Bot ชาวี

### เครื่องมือที่ใช้
- n8n (Railway Cloud)
- Groq API (LLaMA 3.3 70B)
- LINE Messaging API
- JavaScript (n8n code nodes)

### เหตุผลที่ย้ายจาก n8n local → Railway

n8n ต้องการ public URL สำหรับ LINE webhook ตอนแรกใช้ ngrok ต่อผ่าน Mac แต่มีปัญหา:
- ngrok URL เปลี่ยนทุกครั้งที่ restart (free tier)
- Mac ต้องเปิดตลอด
- ถ้าอินเทอร์เน็ตหลุด LINE ส่ง webhook ไม่ได้

Railway แก้ปัญหาทั้งหมด:
- URL คงที่: `https://YOUR_N8N_INSTANCE.up.railway.app`
- ไม่ต้องเปิด Mac
- Reliable uptime

### Workflow Architecture (10 nodes)

```
[1] LINE Webhook
     │ รับ POST จาก LINE API
     │ ข้อมูล: replyToken, userId, messageText, timestamp
     ↓
[2] Parse LINE Message (Code Node)
     │ แยก event type, กรอง non-message events
     │ Output: replyToken, userId, message, timestamp
     ↓
[3] Get HA States (HTTP Request)
     │ GET http://YOUR_EC2_IP:8123/api/states
     │ Header: Authorization Bearer [token]
     │ รับ: state ของ entities ทั้งหมด (switches, sensors, modes)
     ↓
[4] Build Context (Code Node)
     │ สร้าง context string สำหรับบอก AI สถานะบ้าน
     │ Format: "[บ้าน] โหมด Home | ความสะดวก Normal | ไฟห้องนั่งเล่น=ปิด | อุณหภูมิ:28°C"
     ↓
[5] Get Conversation Memory (Code Node)
     │ ดึง history 2 turns จาก $getWorkflowStaticData('global')
     │ เก็บต่อ userId แยกกัน
     ↓
[6] AI AGENT API (HTTP Request → Groq)
     │ POST https://api.groq.com/openai/v1/chat/completions
     │ Model: llama-3.3-70b-versatile
     │ max_tokens: 500
     │ response_format: {"type": "json_object"}
     │ System prompt: ชาวี, ภาษาไทย, วิเคราะห์ intent
     │ Output JSON: {intent, reply_th, room, device, action, mode}
     ↓
[7] Save Memory & Build Command (Code Node)
     │ บันทึก conversation history (เก็บ 4 messages = 2 turns)
     │ Map intent → haService + haBody
     │ intent=control → switch/turn_on หรือ switch/turn_off
     │ intent=set_mode → input_select/select_option
     ↓
     ├──[8] LINE Reply (HTTP Request)
     │       POST https://api.line.me/v2/bot/message/reply
     │       Body: replyToken + message text + Quick Reply 6 ปุ่ม
     │
     ├──[9] HTTP Response 200
     │       ตอบ LINE server ทันทีเพื่อป้องกัน timeout
     │
     └──[10] HA Service Call (HTTP Request)
             POST http://YOUR_EC2_IP:8123/api/services/{haService}
             Body: {entity_id: [...]}
             ควบคุมอุปกรณ์จริงใน Home Assistant
```

### AI System Prompt (ชาวี)

```
ชื่อ "ชาวี" ผู้ช่วยสมาร์ทโฮมส่วนตัว เพศชาย อารมณ์ดี เป็นกันเอง พูดภาษาไทย
- ตอบกลับเสมอเป็น JSON: {intent, reply_th, room, device, action, mode}
- intent: "control" | "set_mode" | "query" | "chat"
- ห้ามพิมพ์ "ครับ" ซ้ำในประโยคเดียว
- รายงานสถานะเป็น "House Mode: [ค่า] | Comfort Mode: [ค่า]"
- ถ้าทักทายทั่วไป ตอบธรรมดา ห้ามพูดถึง house mode โดยไม่จำเป็น
- เว้นวรรคระหว่างภาษาไทยและอังกฤษ
```

### Quick Reply 6 ปุ่ม

```json
"quickReply": {
  "items": [
    {"type":"action","action":{"type":"message","label":"🏠 สถานะบ้าน","text":"สถานะบ้านครับ"}},
    {"type":"action","action":{"type":"message","label":"🌙 Sleep Mode","text":"เปลี่ยนเป็น Sleep mode ครับ"}},
    {"type":"action","action":{"type":"message","label":"🚗 Away Mode","text":"ออกบ้านแล้วครับ"}},
    {"type":"action","action":{"type":"message","label":"🏡 Home Mode","text":"กลับบ้านแล้วครับ"}},
    {"type":"action","action":{"type":"message","label":"💡 เปิดไฟทั้งหมด","text":"เปิดไฟทั้งหมดครับ"}},
    {"type":"action","action":{"type":"message","label":"🌑 ปิดไฟทั้งหมด","text":"ปิดไฟทั้งหมดครับ"}}
  ]
}
```

### ปัญหาที่เจอและแก้

| ปัญหา | สาเหตุ | วิธีแก้ |
|-------|--------|---------|
| บอทตอบ "ขออภัยครับ เกิดข้อผิดพลาด" | max_tokens=280 ทำให้ JSON ถูก truncate | เพิ่ม max_tokens เป็น 500 |
| AI ตอบไม่ใช่ JSON บางครั้ง | ไม่ได้กำหนด response format | เพิ่ม `response_format: {"type":"json_object"}` |
| บอทพูด "ครับ" ซ้ำ ("...ครับครับ") | System prompt ไม่ชัดเจน | เพิ่มกฎ "ห้ามพิมพ์ครับซ้ำ" ใน prompt |
| บอททักทายว่า "อยู่บ้านแบบไหน" | AI ดึง house_mode มาใช้ทุกครั้ง | เพิ่มกฎ "ถ้าทักทาย ตอบธรรมดา ห้ามพูดถึง house mode" |
| Railway n8n PUT API reject | workflow JSON มี fields เกิน (continueOnFail) + ต้องไม่ใส่ versionId | ส่งเฉพาะ name/nodes/connections/settings |
| MQTT ใช้ไม่ได้จาก Railway | Railway ออก internet ไม่ถึง local MQTT broker | แทนด้วย HA REST API (HA Service Call node) |
| LINE bot ไม่ตอบหลัง migrate EC2 | LINE Webhook URL ยังชี้ไป ngrok เก่า | แก้ URL ใน LINE Developers Console |

---

## 7. Phase 5 — LINE Rich Menu

### เครื่องมือที่ใช้
- LINE Messaging API
- Python (Pillow library) — สร้างรูป
- curl — เรียก LINE API

### เหตุผลที่ทำ Rich Menu

Quick Reply แสดงเฉพาะมือถือ (iOS/Android) ไม่แสดงบน iPad และ LINE Desktop
Rich Menu แก้ปัญหานี้ — แสดงได้ทั้งมือถือและ iPad (LINE Desktop ไม่รองรับทั้งคู่ เป็น LINE limitation)

### ขั้นตอนสร้าง Rich Menu

1. **ออกแบบ Layout** — 4 คอลัมน์ × 3 แถว = 12 ปุ่ม (1200×810 px)
2. **สร้างรูปภาพ** — Python Pillow วาด grid + text บน canvas
3. **สร้าง Rich Menu** — POST /v2/bot/richmenu (กำหนด area coordinates)
4. **Upload รูป** — POST /v2/bot/richmenu/{id}/content
5. **Set เป็น default** — POST /v2/bot/user/all/richmenu/{id}

```
richMenuId: richmenu-4e8ffd5d9acc81da5374da1a4ef9fe7e
```

**12 ปุ่ม:**
| แถว 1 | แถว 2 | แถว 3 |
|-------|-------|-------|
| 🏠 HOME | 😌 RELAX | 💡 LIGHTS ON |
| 🚗 AWAY | 🔥 WARM | 🌑 LIGHTS OFF |
| 🔒 SECURITY | 📊 NORMAL | 🔐 LOCK |
| 🌙 SLEEP | 📱 STATUS | 🔓 UNLOCK |

### ปัญหาที่เจอ

| ปัญหา | วิธีแก้ |
|-------|---------|
| LINE Flex Message ตัดข้อความ "Sleep M..." บน mobile | เปลี่ยนมาใช้ Rich Menu แทน Flex Message |
| LINE Desktop ไม่รองรับ Quick Reply และ Rich Menu | ไม่มีวิธีแก้ — เป็น LINE limitation ที่ LINE ยังไม่รองรับ |

---

## 8. Phase 6 — Remote Access (ngrok + Tailscale)

### ngrok

**หน้าที่:** สร้าง tunnel จาก Home Assistant ที่รันบน Mac (local) → public HTTPS URL
**เหตุผล:** HA รันบน Mac ไม่มี public IP → n8n และ LINE ไม่สามารถเรียกได้จาก internet

```bash
# start_tunnel.sh
ngrok http 8123 --domain=YOUR_NGROK_DOMAIN.ngrok-free.dev
```

Static domain: `YOUR_NGROK_DOMAIN.ngrok-free.dev`

**ปัญหา:** start_tunnel.sh ตั้ง port 5678 (n8n) แทน 8123 (HA) → แก้เป็น 8123

**หมายเหตุ:** หลัง migrate ขึ้น EC2 แล้ว ngrok ไม่จำเป็นอีกต่อไป HA มี public IP โดยตรง

### Tailscale VPN

**หน้าที่:** สร้าง private network เชื่อม Mac + iPhone
**เหตุผล:** เข้า HA จาก iPhone โดยไม่ต้องเปิด public port — ปลอดภัยกว่า

---

## 9. Phase 7 — Alexa Custom Skill

### เครื่องมือที่ใช้
- Amazon Alexa Developer Console
- AWS Lambda (Python 3.12, us-east-1)
- Home Assistant REST API

### เหตุผลที่เลือก Custom Skill (ไม่ใช้ Smart Home Skill)

Alexa Smart Home Skill รองรับแค่ basic commands (turn on/off, dim)
Custom Skill รองรับคำสั่งซับซ้อน เช่น "set mode to away" หรือ "home status"
ทำให้ demo ได้น่าประทับใจกว่ามาก

### Skill Configuration

```
Skill Name: Smart Home
Invocation: "smart home"
Skill ID: amzn1.ask.skill.cdae6967-df64-4416-a821-a9747f84064f
```

### Interaction Model (interaction_model.json)

```json
{
  "interactionModel": {
    "languageModel": {
      "invocationName": "smart home",
      "intents": [
        {
          "name": "ControlLightIntent",
          "slots": [
            {"name": "action", "type": "ActionType"},
            {"name": "room", "type": "RoomType"}
          ],
          "samples": ["turn {action} the {room} light"]
        },
        {
          "name": "SetModeIntent",
          "slots": [{"name": "mode", "type": "ModeType"}],
          "samples": ["set mode to {mode}"]
        }
        // ...8 intents รวม
      ]
    }
  }
}
```

### Lambda Function (lambda_function.py)

```python
HA_URL   = "http://YOUR_EC2_IP:8123"
HA_TOKEN = "[Long-Lived Token]"

def lambda_handler(event, context):
    req_type = event["request"]["type"]

    if req_type == "LaunchRequest":
        return speak("Welcome to Smart Home...")

    if req_type == "IntentRequest":
        name = event["request"]["intent"]["name"]
        slots = event["request"]["intent"].get("slots", {})

        if name == "SetModeIntent":
            return handle_set_mode(slots)
        if name == "GetStatusIntent":
            return handle_get_status()
        # ...
```

### การเชื่อม Lambda กับ Alexa

1. สร้าง Lambda function `smart-home-alexa` ใน us-east-1
2. Upload `lambda.zip` (lambda_function.py)
3. Add trigger: Alexa Skills Kit → ใส่ Skill ID
4. ใน Alexa Console → Endpoint → ใส่ Lambda ARN

### ผลการทดสอบ (Alexa Simulator)

```
User: "open smart home"
Alexa: "Welcome to Smart Home. You can say: turn on the lights, set mode to away, or check home status."

User: "set mode to away"
Alexa: "House mode set to Away." ✅
```

### ปัญหาที่เจอและแก้

| ปัญหา | สาเหตุ | วิธีแก้ |
|-------|--------|---------|
| Build error "Slot cannot reference more than one slot type" | LockDoorIntent ใช้ slot "action" ซึ่งชนกับ ActionType ของ intent อื่น | เปลี่ยนเป็น slot "lockAction" กับ LockActionType |
| AWS Lambda region ผิด (Sydney) | เปิดผิด region | เปลี่ยน region เป็น us-east-1 (ต้องตรงกับ Alexa) |
| Amazon Developer ID verification fail | ชื่อภาษาไทยในบัตรไม่ตรงกับ account | พบว่า verification ต้องการเฉพาะตอน publish skill สู่สาธารณะ — development ใช้ได้ทันที |
| Lambda trigger error "SkillManifestError" | Lambda ไม่มี Alexa Skills Kit trigger | เพิ่ม trigger ใน Lambda console ใส่ Skill ID |

---

## 10. Phase 8 — Cloud Migration (AWS EC2)

### ทำไมต้อง migrate

ระบบเดิมทั้งหมดรันบน Mac → ถ้าปิด Mac ระบบดับทั้งหมด
เป้าหมายคือ **Full Cloud** — ปิด Mac ได้ ระบบทำงาน 24/7

### เครื่องมือที่ใช้
- AWS EC2 Console
- SSH + SCP (Terminal)
- Docker + Docker Compose บน EC2
- curl (อัปเดต Railway workflow ผ่าน API)

### ขั้นตอนทำ EC2 Setup

**1. สร้าง EC2 Instance:**
```
Name: smart-home-ha
AMI: Ubuntu Server 22.04 LTS
Instance Type: t3.micro (Free Tier)
Storage: 20 GB (Free Tier ให้ 30 GB)
Key Pair: smart-home-key.pem
```

**2. Security Group (Inbound Rules):**
| Type | Port | Source |
|------|------|--------|
| SSH | 22 | 0.0.0.0/0 |
| Custom TCP | 8123 | 0.0.0.0/0 |
| Custom TCP | 1883 | 0.0.0.0/0 |

**3. ติดตั้ง Docker บน EC2:**
```bash
curl -fsSL https://get.docker.com -o get-docker.sh
sudo sh get-docker.sh
sudo usermod -aG docker ubuntu
sudo curl -L "https://github.com/docker/compose/releases/latest/download/docker-compose-$(uname -s)-$(uname -m)" -o /usr/local/bin/docker-compose
```

**4. Copy Config จาก Mac ไป EC2:**
```bash
scp -i smart-home-key.pem \
  volumes/homeassistant/configuration.yaml \
  volumes/homeassistant/automations.yaml \
  volumes/homeassistant/ui-lovelace.yaml \
  ubuntu@YOUR_EC2_IP:~/smart-home/volumes/homeassistant/
```

**5. Start Services บน EC2:**
```bash
cd ~/smart-home && docker compose up -d
```

**6. Setup MQTT Integration ใน HA:**
- Settings → Integrations → MQTT
- Broker: `localhost` | Port: `1883`
- (Mosquitto อยู่บน EC2 เครื่องเดียวกัน)

**7. สร้าง Long-Lived Token ใหม่:**
- Profile → Long-Lived Access Tokens → Create Token → "n8n-api"

**8. อัปเดต Railway n8n:**
- URL: `https://YOUR_NGROK_DOMAIN.ngrok-free.dev` → `http://YOUR_EC2_IP:8123`
- Token: เก่า → ใหม่
- ลบ `ngrok-skip-browser-warning` header (ไม่ต้องแล้ว)
- PUT ผ่าน Railway n8n API

**9. อัปเดต Alexa Lambda:**
- แก้ `lambda_function.py`: HA_URL + HA_TOKEN ใหม่
- Repackage `lambda.zip`
- Upload ผ่าน AWS Lambda Console

**10. อัปเดต LINE Webhook:**
- LINE Developers Console → Messaging API → Webhook URL
- เปลี่ยนจาก: `https://YOUR_NGROK_DOMAIN.ngrok-free.dev/webhook/line-webhook`
- เป็น: `https://YOUR_N8N_INSTANCE.up.railway.app/webhook/line-webhook`

### ผลลัพธ์

```
EC2 Public IP: YOUR_EC2_IP
HA URL: http://YOUR_EC2_IP:8123
MQTT: YOUR_EC2_IP:1883
ปิด Mac ได้ ระบบทำงาน 24/7 ✅
```

### ปัญหาที่เจอและแก้

| ปัญหา | สาเหตุ | วิธีแก้ |
|-------|--------|---------|
| SCP permission denied | HA volume สร้างโดย Docker (root) | `sudo chmod 777 ~/smart-home/volumes/homeassistant` |
| Railway n8n PUT API reject "additional properties" | node มี `continueOnFail` field ที่ API ไม่รับ + ใส่ versionId ไม่ได้ | ส่งเฉพาะ name/nodes/connections/settings โดยไม่ใส่ versionId |
| LINE bot ไม่ตอบ | LINE Webhook URL ยังชี้ไป ngrok เก่า | เปลี่ยน URL ใน LINE Developers Console |
| n8n workflow inactive หลัง update | PUT อัปเดต workflow แต่ไม่ re-activate webhook | POST /api/v1/workflows/{id}/activate |

---

## 11. ปัญหาและวิธีแก้ทั้งหมด (รวมทุก Phase)

| # | ปัญหา | Phase | สาเหตุ | วิธีแก้ |
|---|-------|-------|--------|---------|
| 1 | HA returning 400 via ngrok | 2 | trusted_proxies ไม่รวม ngrok IPs | เพิ่ม `0.0.0.0/0` ใน trusted_proxies |
| 2 | บอทตอบ "ขออภัย เกิดข้อผิดพลาด" | 4 | max_tokens=280 ทำให้ JSON truncate | เพิ่มเป็น 500 |
| 3 | AI ตอบไม่ใช่ JSON | 4 | ไม่ได้กำหนด format | เพิ่ม response_format json_object |
| 4 | บอทพูด "ครับๆ" ซ้ำ | 4 | System prompt ไม่ชัด | เพิ่มกฎห้ามพูดซ้ำใน prompt |
| 5 | บอททักทายว่า "อยู่บ้านแบบไหน" | 4 | AI ดึง house_mode มาใช้ทุกครั้ง | เพิ่มกฎ "ถ้าทักทาย ตอบธรรมดา" |
| 6 | LINE Flex Message ตัดข้อความ | 5 | Button label ยาวเกินบน mobile | เปลี่ยนเป็น Rich Menu |
| 7 | LINE Desktop ไม่รองรับ Rich Menu | 5 | LINE limitation | ยอมรับ — ไม่มีทางแก้ |
| 8 | start_tunnel.sh port ผิด | 6 | script ตั้ง 5678 แทน 8123 | แก้เป็น 8123 |
| 9 | Alexa build error slot conflict | 7 | "action" slot ใช้ type ซ้ำกัน | เปลี่ยนเป็น "lockAction" slot |
| 10 | Lambda region ผิด | 7 | เลือก Sydney แทน us-east-1 | เปลี่ยน region |
| 11 | Amazon Developer verification fail | 7 | ชื่อภาษาไทยไม่ตรง | ไม่ต้อง verify สำหรับ development |
| 12 | Lambda trigger error | 7 | ไม่มี Alexa Skills Kit trigger | เพิ่ม trigger + Skill ID |
| 13 | MQTT ใช้ไม่ได้จาก Railway | 4 | Railway ออก internet ไม่ถึง local | ใช้ HA REST API แทน |
| 14 | Railway PUT API reject | 4,8 | Extra fields (continueOnFail) + versionId | Strip fields, ไม่ใส่ versionId |
| 15 | SCP permission denied (EC2) | 8 | Docker สร้าง folder เป็น root | chmod 777 |
| 16 | LINE bot ไม่ตอบหลัง migrate | 8 | Webhook URL ยังเป็น ngrok เก่า | เปลี่ยน URL ใน LINE Console |
| 17 | n8n workflow inactive | 8 | PUT ไม่ re-activate | POST /activate endpoint |

---

## 12. Architecture ปัจจุบัน

```
╔══════════════════════════════════════════════════════════════╗
║               FULL CLOUD ARCHITECTURE (Apr 2026)             ║
╠══════════════════════════════════════════════════════════════╣
║                                                              ║
║  📱 USER                                                     ║
║   │ พิมพ์/พูดคำสั่งภาษาไทย                                    ║
║   │                                                          ║
║   ├──► LINE App ──► LINE Messaging API                       ║
║   │                      │ Webhook POST                      ║
║   │                      ▼                                   ║
║   │              Railway n8n (Cloud)                        ║
║   │              workflow: jl4Fp487lsnfagne                  ║
║   │                      │                                   ║
║   │              ┌───────┴────────┐                         ║
║   │              ▼                ▼                          ║
║   │         Groq API          AWS EC2                        ║
║   │         LLaMA 3.3 70B     YOUR_EC2_IP                ║
║   │         Intent Analysis   ├── HA :8123                  ║
║   │              │            └── MQTT :1883                 ║
║   │              └────────────────┘                         ║
║   │              วิเคราะห์ + สั่งงาน HA                       ║
║   │                                                          ║
║   └──► Alexa ──► AWS Lambda ──► AWS EC2 HA :8123            ║
║         Echo 4th gen  us-east-1                             ║
╚══════════════════════════════════════════════════════════════╝

╔══════════════════════════════════════════════════════════════╗
║              HARDWARE LAYER (รอซื้อ)                        ║
║                                                              ║
║  ห้องนั่งเล่น: ESP32 + DHT22 + Relay(ไฟ+พัดลม)               ║
║  ห้องนอน:    ESP32 + DHT22 + Relay(ไฟ+พัดลม)                ║
║  ห้องครัว:   ESP32 + MQ-2 + Relay(พัดลมดูด)                 ║
║  สวน:        ESP32 + Soil + Relay(ปั๊ม+ไฟ)                  ║
║  ประตูหน้า:  ESP32 + Servo(ล็อค)                            ║
║  ทุกห้อง:    Zigbee Smart Plug → Sonoff Dongle              ║
║               → Zigbee2MQTT → MQTT → HA                     ║
╚══════════════════════════════════════════════════════════════╝
```

---

## 13. Hardware Plan

### Shopping List (~5,450 บาท)

| อุปกรณ์ | จำนวน | ราคาประมาณ | ซื้อที่ไหน |
|---------|-------|-----------|-----------|
| ESP32 DevKit V1 (38-pin) | 5 | ~750 บาท | Shopee |
| DHT22 Temperature/Humidity | 3 | ~240 บาท | Shopee/ThaiEasyElec |
| MQ-2 Gas Sensor | 1 | ~60 บาท | Shopee |
| Capacitive Soil Moisture v1.2 | 1 | ~80 บาท | Shopee |
| Servo SG90 | 1 | ~80 บาท | Shopee/ThaiEasyElec |
| Mini Water Pump 5V | 1 | ~80 บาท | Shopee |
| Relay 2-Channel 5V | 3 | ~180 บาท | Shopee |
| Relay 1-Channel 5V | 1 | ~40 บาท | Shopee |
| Sonoff ZBDongle-P (Zigbee Coordinator) | 1 | ~650 บาท | Shopee/itead.cc |
| Sonoff S26R2ZB Zigbee Smart Plug | 8 | ~2,000 บาท | Shopee/Lazada |
| Zigbee Water Leak Sensor | 1 | ~400 บาท | Shopee |
| Power Supply 5V 2A USB | 5 | ~400 บาท | Shopee |
| Jumper Wire + Breadboard + สาย | 1 ชุด | ~490 บาท | Shopee |
| **รวม** | | **~5,450 บาท** | |

### Wiring แต่ละห้อง

**ห้องนั่งเล่น — ESP32 #1:**
```
ESP32
├── GPIO4  → DHT22 Data (อุณหภูมิ/ความชื้น + 10kΩ pull-up)
├── GPIO26 → Relay CH1 IN (ไฟ — backup)
├── GPIO27 → Relay CH2 IN (พัดลม — backup)
├── 3.3V   → DHT22 VCC
├── 5V     → Relay VCC
└── GND    → GND ทุกตัว
Zigbee: Smart Plug A (ไฟ), Smart Plug B (พัดลม)
```

**ห้องนอน — ESP32 #2:**
```
ESP32
├── GPIO4  → DHT22 Data (อุณหภูมิ/ความชื้น + 10kΩ pull-up)
├── GPIO26 → Relay CH1 IN (ไฟ — backup)
├── GPIO27 → Relay CH2 IN (พัดลม — backup)
├── 3.3V   → DHT22 VCC
├── 5V     → Relay VCC
└── GND    → GND ทุกตัว
Zigbee: Smart Plug C (ไฟ), Smart Plug D (พัดลม)
```

**ห้องครัว — ESP32 #3:**
```
ESP32
├── GPIO34 → MQ-2 A0 (Analog gas value 0-4095)
├── GPIO35 → MQ-2 D0 (Digital threshold alert)
├── GPIO26 → Relay IN (พัดลมดูดอากาศ — backup)
├── 5V     → MQ-2 VCC + Relay VCC
└── GND    → GND ทุกตัว
Zigbee: Smart Plug E (พัดลมดูดอากาศ)
```

**สวน — ESP32 #4:**
```
ESP32
├── GPIO32 → Soil Moisture AOUT (Analog 0-3.3V)
├── GPIO26 → Relay CH1 IN (ปั๊มน้ำ)
├── GPIO27 → Relay CH2 IN (ไฟสวน — backup)
├── 3.3V   → Soil Sensor VCC
├── 5V     → Relay VCC
└── GND    → GND ทุกตัว
Relay COM → 5V | Relay NO → Water Pump +
Zigbee: Smart Plug F (ปั๊ม), Smart Plug G (ไฟสวน)
```

**ประตูหน้า — ESP32 #5:**
```
ESP32
├── GPIO18 → Servo SG90 Signal (PWM 50Hz)
├── 5V     → Servo VCC (Red Wire)
└── GND    → Servo GND (Brown Wire)

Servo 0°  = ปลดล็อค
Servo 90° = ล็อค
```

**ห้องน้ำ — Zigbee Only (ไม่มี ESP32):**
```
Zigbee Water Leak Sensor
└── วางบนพื้นใต้อ่างล้างหน้า / ใกล้โถส้วม
└── จับคู่ผ่าน Zigbee2MQTT อัตโนมัติ
└── ส่ง MQTT: zigbee2mqtt/leak_bathroom → {"water_leak": true}

Zigbee Smart Plug H → เสียบพัดลมระบายอากาศห้องน้ำ
```

### Zigbee Architecture

```
Zigbee Smart Plug × 8 + Water Leak Sensor × 1
        │ Zigbee Protocol (2.4GHz, mesh network)
        ▼
Sonoff ZBDongle-P (CC2652P Coordinator)
        │ USB
        ▼
Raspberry Pi หรือ EC2 (ถ้าต่อ USB ได้)
Zigbee2MQTT (Docker)
        │ MQTT
        ▼
Mosquitto Broker (EC2: YOUR_EC2_IP:1883)
        │
        ▼
Home Assistant (EC2: YOUR_EC2_IP:8123)
```

**เหตุผลที่ใช้ Zigbee:**
- IEEE 802.15.4 low-power protocol — ประหยัดพลังงานกว่า WiFi มาก
- Mesh network — device แต่ละตัวเป็น repeater ให้กัน
- เหมาะกับ Smart Plug ที่เปิดตลอด (ประหยัดค่าไฟ)
- รองรับ device ได้หลายพัน ตัวในเครือข่ายเดียว

---

## 14. Roadmap ที่เหลือ

### ระยะสั้น (ก่อน Present)

```
สัปดาห์ที่ 1:
☐ ซื้อ hardware ทั้งหมด
☐ Flash ESP32 firmware ทุกตัว
☐ ทดสอบ MQTT จาก ESP32 → EC2

สัปดาห์ที่ 2:
☐ ต่อ circuit แต่ละห้อง
☐ Pair Zigbee Smart Plug กับ Dongle
☐ ทดสอบ Automation ทั้งหมดกับ hardware จริง

สัปดาห์ที่ 3:
☐ ทดสอบ Alexa กับ Echo 4th Gen จริง
☐ Fine-tune AI responses
☐ ทำ Energy Monitoring Dashboard
☐ ซ้อม present

วันก่อน Present:
☐ Full system test ทุก feature
☐ ทำ backup config ทั้งหมด
☐ เตรียม demo script
```

### ลำดับความสำคัญ

1. 🔴 **ESP32 + Sensor** — core IoT feature ที่ขาดไม่ได้
2. 🟡 **Zigbee Smart Plug** — energy efficiency feature
3. 🟢 **Echo 4th Gen** — Alexa voice test
4. ⬜ **Energy Dashboard** — nice to have

---

## 15. สิ่งที่ได้เรียนรู้

### Technical Lessons

1. **MQTT pub/sub pattern** — เหมาะมากกับ IoT หลาย device ไม่ต้องรู้จักกันโดยตรง
2. **Docker volume permissions** — Docker สร้าง folder เป็น root → ต้อง chmod ก่อน mount
3. **n8n API limitations** — PUT workflow API ยอมรับเฉพาะ fields ที่กำหนด ต้องไม่ใส่ extra fields
4. **Alexa slot type conflict** — intent หลายตัวที่ใช้ชื่อ slot เหมือนกันต้องมี type เดียวกัน
5. **trusted_proxies ใน HA** — ต้องรวม IP ของ proxy (ngrok) ไม่งั้น HA reject 400
6. **AWS Region** — Alexa Skills ต้องใช้ Lambda ใน us-east-1 เท่านั้น
7. **LINE limitation** — Desktop app ไม่รองรับ Quick Reply และ Rich Menu เลย
8. **Cloud vs Local** — Local development ง่ายกว่าแต่ production ต้องใช้ Cloud

### Architecture Lessons

1. **Separation of concerns** — n8n ทำ workflow, HA ทำ device control, Groq ทำ AI แยกกันชัดเจน
2. **Stateless webhook** — n8n ต้องตอบ LINE ภายใน 5 วินาที → ทำ HTTP Response 200 ก่อน แล้วค่อย process
3. **Static data สำหรับ memory** — เก็บ conversation history ใน n8n staticData แทน database (ง่ายกว่า)
4. **Cloud-first** — ออกแบบให้ทำงานบน cloud ตั้งแต่แรก ย้ายง่ายกว่า

### Project Management Lessons

1. **Document ทุกอย่าง** — ปัญหาและวิธีแก้ที่จดไว้ช่วยประหยัดเวลามาก
2. **Test ทีละ component** — test n8n แยก, test HA แยก ก่อน integrate รวมกัน
3. **Backup credentials** — API key, token, .pem file ต้องเก็บไว้ปลอดภัย
4. **Free tier limitations** — รู้จัก limitation ของแต่ละ service ก่อนเลือกใช้

---

## 16. Phase 9 — v2 Multi-Zone Hardware Deployment

**ช่วงเวลา:** ปลายเมษายน 2026 → ต้นพฤษภาคม 2026

### ที่มา
หลัง Phase 1-8 ระบบ Cloud + AI + LINE bot พร้อมใช้งานแล้ว **แต่ยังไม่มี hardware จริง** ทั้งหมดเป็น MQTT topic ปลายเปิดที่ HA listen รออยู่ ต้องลงมือต่อจริงเพื่อ:
1. Demo ได้แบบจับต้องได้
2. ทดสอบ end-to-end ทุก sensor + actuator
3. แสดงให้เห็นว่าโปรเจคใช้งานได้จริงไม่ใช่แค่ simulation

### การขยายจาก 4 → 5 zones

แผนเดิม (v1) แบ่งเป็น 4 ESP32:
- ESP32 #1 Bedroom (DHT22 + PIR + mmWave + Relay→Fan)
- ESP32 #2 Living Room (DHT22 + MQ-2 + MQ-135 + Relay→Fan)
- ESP32 #3 Bathroom + Garden (DHT22 + Soil + Relay→Pump)
- ESP32 #4 Garage (PIR + Relay→LED Strip + Servo)

**ปัญหา:** Living Room ใหญ่เกินสำหรับบอร์ดเดียว — มีทั้ง gas detection (สำหรับห้องครัว) + air quality + relay fan + light control → GPIO เริ่มแน่น และ logic ปนกัน

**v2 plan — แยก Kitchen ออกมาเป็น zone ของตัวเอง:**
- ESP32 #1 Bedroom: DHT22 + **LD2410C mmWave** + **LDR** + **Servo 360° (curtain)** + LED — *no relay*
- ESP32 #2 Living Room: DHT22 + **MQ-135** + **LDR** + LED — **sensor-only**
- ESP32 #3 Kitchen: DHT22 + **MQ-2** + **Relay→DC Fan** + LED — *gas safety zone*
- ESP32 #4 Bathroom + Garden: DHT22 + **LD2410C (fall detection)** + Soil + Relay→Pump + LED
- ESP32 #5 Garage: **PIR (edge detection)** + **MQ-2** + **Servo 360° (door)** + LED

### Component additions in v2

| Component | จำนวน | เหตุผลที่เพิ่ม |
|---|---|---|
| **LD2410C mmWave** | 2 | ตรวจคนนิ่ง (sleep, fall) ดีกว่า PIR ที่ตรวจแค่ motion |
| **MQ-135 Air Quality** | 1 | คู่กับ MQ-2 (Kitchen) — Living Room AQI |
| **LDR GL5528** | 3 | ambient light sensing (3 ห้อง) |
| **Servo 360° (continuous)** | 2 | curtain (Bedroom) + garage door (Garage) — timing-based control |
| **PIR HC-SR501** | 1 (เหลือ 1) | Garage motion (ใช้ edge detection ลด false positive) |
| **Pi 3 Model B+** | 1 | Zigbee bridge (เปลี่ยนจาก Pi Zero 2W เพราะหาได้แค่ตัวนี้ และ RAM มากกว่า) |

### ESP32 Firmware Architecture (5 sketches)

แทนที่จะใช้ firmware ตัวเดียวที่ branch ตาม `BOARD_ID` แล้วรันได้หลายห้อง — เลือก **5 ไฟล์แยก** เพราะ:
1. แต่ละ zone GPIO ต่างกันมาก, sensor ต่างกัน → branching logic ซับซ้อน
2. แต่ละไฟล์อ่านง่าย maintenance ง่าย
3. compile + flash แยก zone — debug ปัญหา zone นึงไม่กระทบอื่น

```
esp32_firmware/
├── esp32_01_bedroom/esp32_01_bedroom.ino
├── esp32_02_livingroom/esp32_02_livingroom.ino
├── esp32_03_kitchen/esp32_03_kitchen.ino
├── esp32_04_bathroom_garden/esp32_04_bathroom_garden.ino
├── esp32_05_garage/esp32_05_garage.ino
├── test_relay/test_relay.ino       # standalone Serial relay test
├── test_servo/test_servo.ino       # standalone servo 360° test
└── test_ld2410/test_ld2410.ino     # standalone LD2410C raw byte dumper
```

**Common patterns ใน firmware ทุก zone:**
- WiFi multi-SSID (home + Pi AP fallback ready)
- MQTT_SERVER = "YOUR_EC2_IP" (EC2 t3.small)
- Initial publish ทุก state ตอน MQTT connect (กัน HA แสดง "ไม่ทราบสถานะ")
- 60-second warm-up delay สำหรับ MQ-2 / MQ-135
- Reconnect logic + exponential backoff

---

## 17. Phase 10 — EC2 Upgrade & RAM Crisis

**ช่วงเวลา:** ต้นพฤษภาคม 2026

### ปัญหา
EC2 t3.micro (1 GB RAM) เริ่มไม่พอ — เพราะ:
- HA + Mosquitto + n8n + Zigbee2MQTT (เผื่อรัน) + container overhead
- HA ตัวเดียวก็กิน 700+ MB อยู่แล้ว
- Add memory pressure → swap → ระบบช้า → SSH banner exchange timeout
- Public IP ติดต่อไม่ได้บางครั้ง (instance ใช้ CPU credit หมด)

### Symptoms
- `ssh ubuntu@54.226.244.180` → timeout
- HA web UI โหลดช้ามาก หรือ 502 Bad Gateway
- Mosquitto disconnect บ่อย → ESP32 ต้อง reconnect ทุกๆ 1-2 นาที
- n8n workflow execution stuck → memory pressure

### การแก้ไข — Upgrade เป็น t3.small

**กระบวนการ (5 พ.ค. 2026):**
1. SSH เข้า EC2 → `docker compose down` → save state
2. AWS Console → EC2 → Instance → Stop
3. Actions → Instance Settings → Change Instance Type → **t3.small** (2 vCPU, 2 GB RAM)
4. Start instance
5. **⚠️ Public IP เปลี่ยน:** `54.226.244.180` → **`YOUR_EC2_IP`** (เพราะไม่ได้ใช้ Elastic IP)
6. SSH เข้า → `docker compose up -d` → ระบบกลับมา

**ค่าใช้จ่าย:** ใช้ AWS credits $114.50 ที่มีอยู่ครอบคลุม ~6 เดือน

### ผลกระทบจาก IP ใหม่ — ต้องอัปเดตทุกที่

| ที่ | จากเก่า | เปลี่ยนเป็น |
|---|---|---|
| ESP32 #1-5 firmware | `MQTT_SERVER = "54.226.244.180"` | `"YOUR_EC2_IP"` |
| HA Mobile App | ตั้ง URL ใหม่ | http://YOUR_EC2_IP:8123 |
| Zigbee2MQTT config | mqtt://54.226...:1883 | mqtt://YOUR_EC2_IP:1883 |
| n8n workflows | HTTP Request URLs | (มาแก้ทีหลังใน Phase 12) |
| README / docs | ทุกที่ที่อ้าง IP | ใหม่ |

**Lesson:** ครั้งหน้าควรใช้ **Elastic IP** ($3.6/mo) เพื่อไม่ต้องตามแก้ทุกที่เวลา upgrade

---

## 18. Phase 11 — Wiring & Hardware Debug Session

**ช่วงเวลา:** 4-5 พฤษภาคม 2026 (มาราธอน wiring ทั้งคืน)

### ลำดับการต่อ + ปัญหาที่เจอ

**ESP32 #1 Bedroom (1 ชั่วโมง):**
- ✅ DHT22 ต่อง่าย (10kOhm pull-up)
- ✅ LDR voltage divider — ใช้ GPIO36 (VP, input-only) ไม่มีปัญหา
- ❌ Servo SG90 standard ไม่หมุน → ปรากฏว่าผมสั่ง 180° แต่ user มี servo 360° (continuous rotation)
- 🔧 แก้ firmware: `SERVO_CW=135 / STOP=90 / CCW=45` + `CURTAIN_OPEN_MS=1500`
- ❌ Servo ไม่หมุนเลย → user: "อ่อ ผมต่อผิดเฉยๆครับ" (สลับ + กับ -)

**ESP32 #3 Kitchen (3 ชั่วโมง — งงเป็นไก่ตาแตก):**
- ❌ Relay พัดลมไม่ดับเลย — ปิดจาก HA ก็ยังหมุน
- 🔍 เปลี่ยน relay ที่ดี → ก็ยังเป็น
- 🔍 เปลี่ยน GPIO 26 → 33 → 26 → ยังเป็น
- 🔍 อ่าน Serial: log บอก "Relay OFF" ตามคำสั่ง แต่ device ยังเปิด
- 🔍 ปรับ Active LOW vs HIGH สลับไปสลับมา → user: "low high สลับกัน" → "ไม่คือปุ่มกด Low Relay แสดง High มันสลับกันอยู่เฉยๆ"
- **🎯 Root cause:** ESP32 บอร์ด 5V pin มี **solder joint ที่มีรอยแยก** — สาย 5V ไป relay หลุดบ้างไม่หลุดบ้าง
- ✅ user แก้: "ช่องไฟ 5V ในบอร์ด 1 สายมีปัญหา แก้แล้ว ขอบคุณครับ หายละ"
- 📝 **Lesson:** เวลา relay บัค ตรวจแหล่งจ่าย 5V ก่อน software!

**ESP32 #4 Bathroom + Garden (2 ชั่วโมง):**
- ✅ DHT22 + Soil + Relay+Pump ต่อปกติ
- ❌ LD2410C mmWave detect ไม่ได้ — Serial ไม่ขึ้น state เลย
- 🔍 ผม assume frame format `0xAA 0xFF` (จาก tutorial เก่า) — **ผิด!**
- 🎯 **Real format:** LD2410C ใช้ `F4 F3 F2 F1 [data] F8 F7 F6 F5` (23 bytes)
- 🔧 เขียน parser ใหม่:
  ```cpp
  bool parseLD2410(uint8_t &state) {
      // header check: F4 F3 F2 F1
      // state byte at index 8
      // footer: F8 F7 F6 F5
  }
  ```
- ✅ ใช้ได้! state = byte[8] (0=ไม่มี, 1=movement, 2=stationary, 3=both)
- 🔧 เพิ่ม fall detection: `presence stationary > 5 minutes` → publish `home/bathroom/fall = ON`

**ESP32 #5 Garage (1.5 ชั่วโมง):**
- ❌ PIR ตรวจเจอตลอดเวลา ทั้งที่ผม("user")ไม่ได้อยู่หน้า sensor
- 🔍 user: "ผมหันมันไปทางอื่นทำไมมันยังตรวจเจออยู่ครับ 555555"
- 🔍 user: "หมุนยังไงนะครับ มีแค่สามสาย vcc vin gnd"
- 🎯 PIR HC-SR501 มี 2 potentiometer (Tx delay + Sx sensitivity) — ปรับให้ "ขาวโพน" (sensitivity ลดลง)
- 🔧 แต่ก็ยังตรวจเจอบ่อยไป → **Smart Edge Detection logic:**
  ```cpp
  const unsigned long MOTION_TIMEOUT = 30000;  // 30s
  const unsigned long DEBOUNCE_MS    = 200;
  // Track LOW→HIGH transitions only
  // Auto-clear after 30s without new edge
  ```
- ✅ user: "ขอบคุณครับใช้ได้แล้ว"

### Sensor warm-up gotcha

MQ-2 และ MQ-135 ต้อง warm up 60 วินาทีหลัง power-on — ค่าระหว่าง warm-up ไม่ valid ถ้าเอาไป trigger automation ทันทีจะ alert ผิดบ่อย

**Firmware:** publish "warming_up" ระหว่างต้น แล้วเริ่ม publish ค่าจริงเมื่อเลย 60s

### Hardware-related lessons learned

1. **5V pin บน ESP32 dev board เปราะ** — ถ้าใช้ relay/servo ที่กิน current เยอะ ใช้ Power Hub ภายนอก
2. **Servo 360° ≠ Servo 180°** — ตัวแรก speed-based, ตัวสอง position-based (โค้ดต่างกัน)
3. **PIR ปรับยาก** — เปลี่ยน logic เป็น edge detection ดีกว่าพึ่ง sensor calibration
4. **LD2410C frame format ใช้ F4F3F2F1** — ไม่ใช่ generic 0xAAFF
5. **GPIO 36 (VP) ใช้กับ LDR ดี** — input-only, ADC, ไม่มี internal pull-up พอดีสำหรับ voltage divider
6. **GPIO 34, 35, 36, 39 input-only** — ใช้กับ MQ-2/MQ-135 ADC ได้ ไม่กินพินอื่น

---

## 19. Phase 12 — n8n Workflow IP Migration via REST API

**ช่วงเวลา:** 5 พฤษภาคม 2026 (ตอนเย็น — เกือบให้ shipping)

### Symptom
หลัง EC2 IP เปลี่ยนเป็น YOUR_EC2_IP + ESP32 5 zones ต่อเสร็จ → **ทักชาวีใน LINE แล้วไม่ตอบ!**

### Investigation (ใช้ external probe)

ผมเทสจาก curl โดยส่ง mock LINE event ไปที่ webhook:
```bash
# Empty events
curl -X POST .../webhook/line-webhook -d '{"events":[]}'
→ HTTP 200 ใน 0.7s ✅

# Follow event
curl -X POST -d '{"events":[{"type":"follow",...}]}'
→ HTTP 200 ใน 0.9s ✅

# Sticker message
curl -X POST -d '{"events":[{"type":"message","message":{"type":"sticker"},...}]}'
→ HTTP 200 ใน 1.0s ✅

# Text message — เจอแล้ว!
curl -X POST -d '{"events":[{"type":"message","message":{"type":"text","text":"hello"},...}]}'
→ HTTP 000 ใน 35s ❌ TIMEOUT
```

### Hypothesis ladder

1. ❌ Railway บิลหมด? → เช็ค dashboard: Pro plan active, Postgres + n8n online ✅
2. ❌ Groq API ล่ม? → curl https://api.groq.com → 200 ✅
3. ❌ HA ล่ม? → curl http://YOUR_EC2_IP:8123 → 200 ✅
4. ✅ **Workflow node hang** — text branch มี node ที่เรียก endpoint ตาย/ค้าง

### Diagnosis ผ่าน n8n REST API

ขอ user สร้าง n8n API key (Settings → n8n API → Create) แล้ว fetch workflow JSON:

```bash
curl -H "X-N8N-API-KEY: $KEY" \
  https://YOUR_N8N_INSTANCE.up.railway.app/api/v1/workflows/jl4Fp487lsnfagne \
  | jq '.nodes[] | {name, url: .parameters.url}'
```

**ผลลัพธ์:**
| Node | URL |
|---|---|
| ✅ Get HA States | `http://YOUR_EC2_IP:8123/api/states` (user แก้แล้ว) |
| 🚨 **HA Service Call** | `http://54.226.244.180:8123/api/services/{{...}}` (IP เก่า!) |
| (อื่นๆ) | OK |

**Root cause:** workflow มี **2 nodes** เรียก HA — user แก้ใน UI ได้แค่ node แรก, node สองยังเป็น IP เดิมที่ตายแล้ว → request ไป IP ที่ instance termination แล้ว → **TCP timeout 30+ วินาที** ทำให้ webhook ค้าง 35s

### การแก้ไข — Bulk fix ผ่าน REST API

```bash
# 1. Fetch workflow
curl ... > /tmp/wf.json

# 2. Replace IP in all node URLs (Python)
python3 -c "
import json
wf = json.load(open('/tmp/wf.json'))
for n in wf['nodes']:
    if 'url' in n.get('parameters',{}):
        n['parameters']['url'] = n['parameters']['url'].replace(
            '54.226.244.180', 'YOUR_EC2_IP')
clean = {'name': wf['name'], 'nodes': wf['nodes'], 
         'connections': wf['connections'], 'settings': {...}}
json.dump(clean, open('/tmp/wf_new.json','w'))
"

# 3. PUT back to n8n
curl -X PUT -H "X-N8N-API-KEY: $KEY" \
  -H "Content-Type: application/json" \
  --data @/tmp/wf_new.json \
  https://YOUR_N8N_INSTANCE.up.railway.app/api/v1/workflows/jl4Fp487lsnfagne
→ HTTP 200 ✅
```

### Verification — 3 รอบติดกัน

```
Test 1: HTTP 200 | 3.18s ✅
Test 2: HTTP 200 | 2.36s ✅
Test 3: HTTP 200 | 2.06s ✅
```

จาก timeout 35s → **2-3 วินาที** (ลดลง ~12 เท่า)

### Bonus — เจออีก workflow ที่มี IP เก่า

เช็ค 2 workflows อื่น:
- `Smart Home - HA Alert to LINE` → ✅ clean
- `Smart Home - Daily Morning Report` → 🚨 มี **3 nodes** ใช้ IP เก่า (Get HA States, HA Service Call, Get HA Sensors)
- → fix เหมือนกันผ่าน REST API → ✅

### Tooling lesson

**n8n REST API > Chrome MCP** สำหรับ bulk edit:
- Chrome MCP เคยใช้ได้ แต่ session pair หายตอน compact
- n8n REST API ใช้แค่ API key → bulletproof
- API endpoint สำคัญ:
  - `GET /api/v1/workflows` → list
  - `GET /api/v1/workflows/:id` → fetch JSON
  - `PUT /api/v1/workflows/:id` → update (ระวัง settings ต้องไม่มี extra props)
  - `GET /api/v1/executions?workflowId=:id` → debug history

**Caveat ที่ตามมา:** ถ้า curl test ใช้ replyToken ปลอม → execution จะ error ที่ LINE Reply node เพราะ LINE refuse fake token — แต่ webhook **return 200 ก่อน** เพราะมี HTTP Response 200 node ทำงานก่อน LINE Reply (n8n parallel branches) → ทดสอบจาก LINE จริงต้องไม่มี issue

---

*อัปเดตล่าสุด: 5 พฤษภาคม 2026*
*สถานะ: Software 100% ✅ | Cloud 100% ✅ | ESP32 Hardware 100% ✅ | Zigbee 0% ⏳ | Echo 80% ⏳*
*Demo: 11 พฤษภาคม 2026*

*อัปเดตล่าสุด: 16 เมษายน 2026*
*สถานะ: Software 100% ✅ | Cloud 100% ✅ | Hardware 0% ⏳*

---

## 20. Phase 13 — Pi 3B+ Zigbee Bridge Setup (13 พ.ค. 2026)

### บริบท

หลัง EC2 t3.small + ESP32 5 zones + LINE bot ชาวี ทุกอย่างเสถียร 24/7 มาเป็นเดือน เหลือชิ้นเดียวที่ยังไม่ติดตั้ง: **Zigbee** ผ่าน Raspberry Pi 3B+ + Sonoff ZBDongle เพื่อรองรับ IKEA TRÅDFRI + Sonoff smart plugs/sensors

Defense เลื่อนจาก 11 พ.ค. → **26 พ.ค. 2026** ทำให้มีเวลาเก็บงาน Zigbee ก่อน demo

### Steps + ปัญหาที่เจอ

#### Step 1: Flash SD card
- โหลด `raspios-lite-arm64.img` (Debian 13 Trixie)
- `sudo dd if=raspios-lite-arm64.img of=/dev/rdisk11 bs=1m conv=fsync status=progress`
- ใส่ SD card → Pi → boot สำเร็จ → SSH ได้ที่ `pi@172.20.10.4` (iPhone hotspot "Maradosx")

#### Step 2: เขียน setup-pi.sh — One-shot script
สคริปต์ทำ 7 ขั้น: apt update → install deps → Node 20 → ตรวจ ZBDongle → clone Z2M → config → systemd

**❌ Bug 1:** `npm ci` fail (`The 'npm ci' command can only install with an existing package-lock.json`)
→ Z2M repo ใหม่ใช้ pnpm ไม่มี `package-lock.json` ใน git
→ **แก้:** เปลี่ยนเป็น `npm install --omit=dev --no-audit --no-fund`

**❌ Bug 2:** SSH disconnect ฆ่า npm install
→ `Connection reset by peer` ระหว่าง `npm install`
→ **แก้:** ใช้ `systemd-run --unit=z2m-install` (fully detached, SSH หลุดไม่กระทบ)

**❌ Bug 3:** `dphys-swapfile: command not found`
→ Debian Trixie ไม่ใช้ dphys-swapfile แล้ว (มี zram-swap built-in 904Mi)
→ **แก้:** สร้าง swapfile manual: `fallocate -l 2G /swapfile && mkswap && swapon`

**❌ Bug 4:** Pi reboot ระหว่าง npm install (uptime 1 min)
→ ทีแรกคิดว่า OOM (RAM 905Mi, swap 904Mi) → เพิ่ม swap เป็น 2.9Gi + `NODE_OPTIONS=--max-old-space-size=512` → ยัง reboot อีก

**❌ Bug 5:** `npm EINTEGRITY` — ดาวน์โหลด package ได้ 0 bytes
→ Wi-Fi hotspot iPhone หลุดชั่วคราว → corrupt download
→ **แก้:** `npm cache clean --force` + `--fetch-retries=10 --fetch-retry-maxtimeout=120000`

**❌❌ Root cause:** Pi reboot ครั้งที่ 3 — `vcgencmd get_throttled` = **`0x50005`** = **Under-voltage**

| Bit | ความหมาย | สถานะ |
|---|---|---|
| 0 | Under-voltage NOW | ✅ |
| 2 | Currently throttled | ✅ |
| 16 | Under-voltage occurred | ✅ |
| 18 | Throttling occurred | ✅ |

→ Pi 3B+ ต้อง 5V/2.5A แต่ Pi ตอนนั้นเสียบ USB จาก Mac ที่จ่ายแค่ 0.5-0.9A → ไฟตก → kernel panic → reboot
→ **แก้:** เสียบ Pi ตรง wall adapter จริง → `throttled=0x0` → เสถียร

### Step 3: เปลี่ยน strategy → Docker
ถึงไฟดีแล้ว Pi 3B+ ก็ยัง compile Z2M จาก source ยาก (RAM 1GB น้อย, build time นาน) จึงเปลี่ยนเป็น **Docker image** prebuilt

```bash
sudo apt install -y docker.io
sudo systemctl enable --now docker
sudo systemd-run --unit=z2m-pull /usr/bin/docker pull koenkk/zigbee2mqtt:latest
```

- Pull image: ~146MB, 4 นาทีบน Wi-Fi hotspot
- ไม่มี npm install / build / compile → ไม่กระทบ RAM/CPU มาก

### Step 4: systemd unit สำหรับ Z2M container

```ini
[Unit]
Description=Zigbee2MQTT (Docker)
After=docker.service network-online.target
Requires=docker.service

[Service]
Type=simple
Restart=always
RestartSec=10s
ExecStartPre=-/usr/bin/docker rm -f zigbee2mqtt
ExecStart=/usr/bin/docker run --rm --name zigbee2mqtt \
  --network host \
  -v /opt/zigbee2mqtt-docker/data:/app/data \
  --device=/dev/serial/by-id/usb-Itead_Sonoff_Zigbee_3.0_USB_Dongle_Plus_V2_...:/dev/ttyACM0 \
  -e TZ=Asia/Bangkok \
  koenkk/zigbee2mqtt:latest
ExecStop=/usr/bin/docker stop zigbee2mqtt

[Install]
WantedBy=multi-user.target
```

### Step 5: ❌ Bug 6 — Adapter type ผิด

ครั้งแรกใส่ `adapter: zstack` ใน config (เพราะ HARDWARE_GUIDE บอกว่าซื้อ ZBDongle-P / CC2652P) → Z2M ติดที่:
```
Error: Failed to connect to the adapter (Error: SRSP - SYS - ping after 6000ms)
```
Container restart loop ทุก 30 วินาที

**Debug:** USB reset (`echo 0 > authorized` → `echo 1`) ไม่ช่วย → ตัว dongle จริงๆ น่าจะเป็น **ZBDongle-E** (EmberZNet) ไม่ใช่ -P (Z-Stack)

→ **แก้:** เปลี่ยน `adapter: ember` → boot ผ่าน ทันที:
```
zh:ember:ezsp: ======== EZSP started ========
zh:ember: Adapter version info: {"ezsp":13,"revision":"7.4.4 [GA]","build":0,"major":7,"minor":4,"patch":4,"special":0,"type":170}
zh:ember: [INIT FORM] New network formed!
z2m: zigbee-herdsman started
z2m: Connected to MQTT server (mqtt://YOUR_EC2_IP:1883)
z2m: Zigbee2MQTT started!
```

**สรุป:** dongle ที่ซื้อจริงๆ คือ **Sonoff ZBDongle-E** (Silicon Labs EFR32MG21 + EmberZNet firmware) ต้อง update HARDWARE_GUIDE.md ให้ตรงกับของจริง

### บทเรียนหลัก

1. **เช็ค `vcgencmd get_throttled` เป็นอันดับแรกเมื่อ Pi reboot บ่อย** — ก่อนเสีย 30 นาทีไปกับ debug OOM/network
2. **อย่าจ่ายไฟ Pi 3B+ ผ่าน Mac USB / USB hub / powerbank** — ต้อง wall adapter 5V/2.5A ขึ้นตรง
3. **Docker > npm install บน Pi RAM น้อย** — เลี่ยง build time + memory pressure
4. **`adapter: ember` ≠ `adapter: zstack`** — Sonoff ZBDongle V2 มี 2 variants (E/P) firmware ต่างกัน เช็คจาก Z2M boot log ทุกครั้ง
5. **`systemd-run --unit=` > `nohup &`** — process detach สมบูรณ์ ไม่ตายเมื่อ SSH หลุด
6. **SSH key auth + NOPASSWD sudo** ลด friction มหาศาลเวลา debug remotely

### ผลลัพธ์

- ✅ Pi 3B+ smarthome-pi 172.20.10.4 — Z2M Docker container active
- ✅ Web UI: http://172.20.10.4:8080
- ✅ MQTT bridge ไปที่ EC2 Mosquitto (YOUR_EC2_IP:1883)
- ✅ HA auto-discovery topics published — HA จะเห็น "Zigbee2MQTT Bridge" device อัตโนมัติ
- ⏳ ขั้นถัดไป: Pair IKEA TRÅDFRI + Sonoff S26R2ZB ×2 + SNZB-03P + SNZB-05

---

*อัปเดตล่าสุด: 13 พฤษภาคม 2026 — Phase 13 Pi Zigbee Bridge setup สำเร็จ*
*สถานะ: Software 100% ✅ | Cloud 100% ✅ | Hardware 95% ✅ (เหลือ pair Zigbee devices + Echo)*
*Defense: 26 พฤษภาคม 2026*

---

## 21. Phase 14 — Light Effects + Mood Scenes + LINE Bot Expansion (15 พ.ค. 2026)

### บริบท
หลัง Pi Zigbee Bridge live แล้ว, Zigbee Tuya RGB+CCT bulb (`light.living_room_light`) สามารถเปลี่ยนสี/ความสว่างได้เต็มที่. โอกาสนี้เลย add layer ของ "lighting personality" เพิ่ม WOW factor สำหรับ defense + ใช้งานจริง

### Step 1: Mood Scenes ตาม comfort_mode + house_mode
สร้าง HA automation "🎨 Living Room Light - Mode Scene" ที่ trigger เมื่อ
`input_select.comfort_mode` หรือ `input_select.house_mode` เปลี่ยน → ตั้งสี/brightness ตามโหมด

**❌ Bug 1:** HA 2024.x+ ใช้ `color_temp_kelvin` แทน `color_temp` (mired)
- Error: "extra keys not allowed @ data['color_temp']"
- **แก้:** เปลี่ยนทั้งหมดเป็น `color_temp_kelvin` (Kelvin)
- Conversion: 454 mired = 2200K, 420 = 2400K, 370 = 2700K

**❌ Bug 2:** Sleep Mode automation เก่าปิด `light.living_room_light` ก่อน → mood scene ใหม่ทับไม่ทัน
- ตัด `light.living_room_light` ออกจาก kill-switch ของ Sleep Mode
- ให้ Mode Scene อย่างเดียวจัดการไฟห้องนั่งเล่นใน Sleep

### Step 2: Effect Scripts (6 scripts)
ใช้ Zigbee bulb effects + custom sequences:

```yaml
script:
  living_room_red_flash:       # 🚨 6× กระพริบแดง (gas alert)
  living_room_blue_pulse:       # 💧 8× หายใจฟ้า (water leak)
  living_room_sunrise:          # 🌅 15-min fade ตื่นนอน
  living_room_party_on/off:     # 🎉 colorloop + fan
  living_room_welcome_home:     # 🏠 colorloop 8s → warm
```

### Step 3: Auto-hooks → automation เดิม
- "Gas Alert" + `script.living_room_red_flash` → ไฟกระพริบแดงทุกครั้งที่เจอแก๊ส
- "Water Leak" + `script.living_room_blue_pulse` → ไฟหายใจฟ้า
- "Garage Smoke" + `script.living_room_red_flash` → flash จากห้องนั่งเล่น (เห็นง่ายกว่าโรงรถ)
- "GPS Return" + `script.living_room_welcome_home` → colorloop ทักทาย

### Step 4: Daily Sunrise (06:00)
- New automation: time trigger 06:00 + condition `house_mode = Home`
- Initial state: enabled
- Call `script.living_room_sunrise` (15 นาที fade dawn → cool morning white)

### Step 5: LINE Bot ชาวี — เพิ่ม device coverage + scene intent
อัปเดต n8n workflow บน Railway ผ่าน REST API:

**5.1 Entity map expansion (10 → 18 endpoints):**
- เดิม: livingroom light/fan, bedroom light/fan, kitchen fan, garden light/pump, frontdoor lock
- เพิ่ม: kitchen light/stove, bathroom light, garden_light_1, garage light/pathway, bedroom curtain, garage door

**5.2 Service domain → `homeassistant/turn_on` (universal)** — ใช้ได้กับ light/switch/cover

**5.3 Scene intent:**
- เพิ่ม intent enum: `control|query|set_mode|scene|chat|history|unknown`
- เพิ่ม sceneMap: party / sunrise / welcome_home / red_flash / blue_pulse
- AI prompt examples: "เปิด party mode" → intent=scene, scene=party

### Step 6: Tech win — iPhone hotspot 2.4GHz fix
- Pi 3B+ ไม่รองรับ Wi-Fi 5GHz → iPhone hotspot default 5GHz → Pi connect ไม่ได้
- **แก้:** iPhone Settings → Personal Hotspot → **"Maximize Compatibility" ON** → broadcast 2.4GHz

### Step 7: HA entity_registry cleanup
- เจอ ghost: `switch.living_room_fan` (จาก placeholder เก่า `unique_id: switch_livingroom_fan`)
- HA สร้าง `switch.living_room_fan_2` ให้ Zigbee plug — เพราะ name conflict
- **แก้:** ลบ ghost จาก entity_registry + rename `_fan_2` → `_fan` (canonical)

### ผลลัพธ์ Phase 14
- ✅ 6 light effect scripts
- ✅ 4 automation hooks (gas/water/smoke/GPS-arrive)
- ✅ 7 mood scenes (Sleep/Relax/Warm/Normal/Away/Security/Home)
- ✅ LINE bot 18 device endpoints + scene intent
- ✅ Daily Sunrise auto 06:00 enabled
- ✅ HA entity registry cleaned
- ✅ Pi Wi-Fi reliable (iPhone hotspot compatibility mode)

### บทเรียน
1. **HA schema migration** — `color_temp` → `color_temp_kelvin` (HA 2024+)
2. **HA service.homeassistant.turn_on** — universal (light/switch/cover) ดีกว่า `switch/turn_on`
3. **iPhone hotspot "Maximize Compatibility"** — บังคับ 2.4GHz สำหรับ Pi 3B+
4. **Automation conflicts** — ใหม่+เก่าอาจ trigger พร้อมกัน, อันที่ใช้ entity เดียวกันต้องปรับให้ไม่ทับ
5. **n8n REST API > UI** สำหรับ bulk JS update — รวดเร็ว + version control ได้

---

*อัปเดตล่าสุด: 15 พฤษภาคม 2026 — Phase 14 Light Effects + Mood Scenes + LINE Bot Expansion สำเร็จ*
*สถานะ: Software 100% ✅ | Cloud 100% ✅ | Hardware 95% ✅ | Light Effects 100% ✅ | LINE Bot 100% (18 endpoints + scene intent)*
*Defense: 26 พฤษภาคม 2026 — เหลือ 11 วัน*

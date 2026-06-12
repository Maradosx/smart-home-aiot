# 🏠 Smart Home AIoT

> **Production-ready bilingual smart home system** — Voice (English via Alexa) + Chat (Thai via LINE) + Dashboard control
>
> 🎓 **Bangkok University Senior Project CE08** · ✅ **Defense PASSED — 26 May 2026**

![Status](https://img.shields.io/badge/Defense-PASSED-success?style=for-the-badge) ![Production](https://img.shields.io/badge/Production-Ready-blue?style=for-the-badge) ![Tests](https://img.shields.io/badge/Tests-130%20cases-orange?style=for-the-badge)

![ESP32](https://img.shields.io/badge/ESP32-DevKit_V1-E7352C?logo=espressif&logoColor=white) ![Home Assistant](https://img.shields.io/badge/Home_Assistant-2026.4.2-41BDF5?logo=home-assistant&logoColor=white) ![Claude](https://img.shields.io/badge/Anthropic-Claude_Haiku_4.5-D4A27F) ![MQTT](https://img.shields.io/badge/MQTT-Mosquitto-660066) ![Zigbee](https://img.shields.io/badge/Zigbee-Z2M_2.10.1-EB0928) ![AWS](https://img.shields.io/badge/AWS-Lambda_+_EC2-FF9900?logo=amazonaws&logoColor=white)

---

## ✨ Highlights

- 🤖 **Shared AI Brain** — Anthropic Claude Haiku 4.5 powers both LINE Bot "ชาวี" (Thai) and Alexa Echo Dot (English)
- 🏘 **5 Hardware Zones** — Bedroom · Living Room · Kitchen · Bathroom+Garden · Garage
- 🛡 **Real Safety** — Gas/Water Leak/Fall/Intrusion detection with 3-channel resilient notifications (Mobile + LINE + HA Popup)
- 💡 **Smart Lighting** — Motion + LDR adaptive system with 7 scenes (Welcome, Sunrise, Calm, Bedtime, Party Loop, Alert Flash)
- 🌱 **Auto Watering** — State-based pump (soil < 45% + weather-aware + 10-min safety cutoff)
- 📍 **GPS Auto-mode** — House mode switches automatically (Home/Away/Security) at 4m zone radius
- 🔄 **Self-healing** — Webhook auto-recovery, fuzzy ASR matching, retained MQTT state

---

## 📊 By the Numbers

| Metric | Value |
|---|---|
| **Hardware components** | 13 (5× ESP32 + Pi 3B+ + 6 Zigbee) |
| **HA entities** | **235** (verified live 12 Jun 2026) |
| **HA automations** | **71** (65 active) — UI-managed + YAML packages |
| **HA scripts** | 7 |
| **Alexa Custom Intents** | 23 |
| **MQTT topics** | 30+ |
| **Test cases** | 130 (10 trials each) |
| **Documentation files** | 30+ MD files |
| **Lines of MASTER_REFERENCE** | 2,349 |
| **Monthly running cost** | ~฿200–600 steady-state (~฿1,664–1,864 during public demo w/ LINE Basic) |
| **End-to-end response** | ~1.7 sec (LINE) / ~1.5 sec (Alexa) |

---

## 🏗 3-Layer Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│  USERS / INTERFACES                                              │
│  📱 LINE "ชาวี"   🎤 Alexa Echo Dot   💻 HA Dashboard   📲 iOS   │
└──────────────────────────┬──────────────────────────────────────┘
                           │
            (ngrok HTTPS)  │  (AWS Lambda)
                           ▼
╔══════════════════════════════════════════════════════════════════╗
║  ☁️ CLOUD LAYER — AWS EC2 t3.small (us-east-1)                  ║
║  • Home Assistant 2026.4.2 (Docker)                              ║
║  • Mosquitto MQTT Broker (port 1883)                             ║
║  • n8n Railway (Daily Report + Alert workflows)                  ║
║  • Supabase Postgres (chat_logs + mode_changes)                  ║
║  • Anthropic Claude Haiku 4.5 (shared AI brain)                  ║
╚══════════════════════════╤═══════════════════════════════════════╝
                           │ (MQTT over Wi-Fi)
                           ▼
╔══════════════════════════════════════════════════════════════════╗
║  🏠 EDGE LAYER — Local Devices                                   ║
║  • Raspberry Pi 3B+ + Sonoff ZBDongle-P (Zigbee2MQTT)            ║
║  • 5× ESP32 zones (DHT22 + PIR + LDR + zone-specific sensors)    ║
║  • 6× Zigbee devices (Tuya bulb + plugs + Sonoff SNZB sensors)   ║
╚══════════════════════════════════════════════════════════════════╝
```

📖 **Full architecture diagram + 18 detailed sections:** [`MASTER_REFERENCE.md`](MASTER_REFERENCE.md)

---

## 🛠 Tech Stack

### 🔌 Hardware
- **5× ESP32 DevKit V1** (Wi-Fi MCU, edge zones)
- **Raspberry Pi 3B+** (Zigbee coordinator hub)
- **Sonoff ZBDongle-P** (Zigbee 3.0 USB, CC2652P chip)
- **6× Zigbee devices** — Tuya TS0505B (RGB bulb), TS011F ×2 (smart plugs), SNZB-03P (motion), SNZB-04 (door), SNZB-05P (water leak)
- **Sensors** — DHT22 ×4, MQ-2 (gas), PIR HC-SR501 ×4, LDR ×5, HLK-LD2410 (mmWave fall), Capacitive soil
- **Actuators** — Servo SG90 ×2 (door locks), Relay 5V (5 zones)

### ☁️ Cloud Services
- **AWS EC2** t3.small — runs HA + Mosquitto in Docker
- **AWS Lambda** — Alexa custom skill backend (Python 3.11)
- **Railway** — n8n self-hosted workflows ($5/mo Hobby)
- **Supabase** — Postgres + Realtime (Free tier)
- **Anthropic** — Claude Haiku 4.5 API (~฿0.05/turn, 200K context)
- **LINE Messaging API** — Basic plan ฿1,284/mo (15k msg)
- **ngrok** — HTTPS tunnel for LINE webhook bypass
- **Met.no** — Weather API (free, unlimited)

### 💻 Software
- **Home Assistant Core 2026.4.2** — automation engine
- **Mosquitto MQTT Broker** — central message bus
- **Zigbee2MQTT v2.10.1** — Zigbee ↔ MQTT bridge
- **Arduino IDE** — ESP32 firmware (PubSubClient + DHT + ArduinoJson + ESP32Servo)
- **Docker + docker-compose** — containerized deployment

---

## 📂 Project Structure

```
smart-home-aiot/
├── 📖 README.md                 You are here
├── 📖 MASTER_REFERENCE.md       ⭐ Single source of truth (2,349 lines)
│
├── 📚 docs/                     Detailed documentation
│   ├── SYSTEM_FLOW.md           490-line architecture walkthrough
│   ├── HARDWARE_GUIDE.md        Hardware reference
│   ├── WIRING_DETAILED.md       Wiring details + GPIO map
│   ├── V4_FIX_LOG.md            Bug-fix journey (12 bugs + ngrok bypass)
│   ├── RECOVERY.md              Disaster recovery procedures
│   ├── THESIS_REPORT_FINAL.md   Thesis document
│   ├── PROJECT_STATE.md         Live state snapshot
│   ├── PROJECT_STATUS_REPORT.md Formal status + cost breakdown
│   ├── PROJECT_PROGRESS.md      12-module progress tracker
│   ├── PROJECT_JOURNEY.md       Build history
│   └── HOME_FLOW_FINAL.md       Architecture + flows
│
├── 🎓 DEFENSE_HANDOFF/          Defense bundle
│   ├── POST_DEFENSE_FINAL.md    Comprehensive retrospective
│   ├── ALEXA_COMMANDS_DEFENSE.md   60+ Alexa commands
│   ├── CHAVEE_COMMANDS_DEFENSE.md  Thai chat commands
│   ├── DEFENSE_DAY_FINAL_STATUS.md
│   ├── *.pdf                    Defense presentation slides (4 files)
│   ├── 01_docs/                 Technical verification docs
│   ├── 02_test_sheets/          130-test PDFs for thesis data collection
│   └── 03_scripts/              PDF generator script
│
├── 🎤 alexa/                    AWS Lambda + Alexa skill
│   ├── lambda_function.py       Lambda code with HOME_CONTEXT (16KB)
│   ├── interaction_model.json   23 custom intents
│   └── *.md                     Setup + test docs
│
├── 🔌 esp32_firmware/           5 zones + test sketches
│   ├── esp32_01_bedroom/
│   ├── esp32_02_livingroom/
│   ├── esp32_03_kitchen/
│   ├── esp32_04_bathroom_garden/
│   ├── esp32_05_garage/
│   └── test_*/                  Test sketches (LD2410, relay, servo)
│
├── 🥧 pi_setup/                 Raspberry Pi 3B+ deployment
│   ├── DEPLOY_GUIDE.md
│   └── wpa_supplicant.conf
│
├── 📊 test_sheets/              Thesis data collection PDFs
│   └── 130-test PDFs (A–I phases) + generator
│
└── docker-compose.yml, setup.sh, start_tunnel.sh, mosquitto.conf
```

> 💡 `_archive/`, `thesisตัวอย่างรุ่นพี่/`, `volumes/`, `n8n_workflows/` are **gitignored** (local only)

---

## 🚀 Quick Start

### Prerequisites
- Hardware: 5× ESP32, Raspberry Pi 3B+, Sonoff Zigbee dongle, sensors listed above
- AWS account (Free tier OK)
- LINE Developer account
- Anthropic API key
- Local Wi-Fi 2.4GHz

### Deploy
```bash
# 1. Clone
git clone https://github.com/Maradosx/smart-home-aiot.git
cd smart-home-aiot

# 2. Copy env template + fill in values
cp .env.example .env
nano .env

# 3. Start EC2 stack (HA + Mosquitto)
docker-compose up -d

# 4. Setup Pi (Zigbee2MQTT)
# See pi_setup/DEPLOY_GUIDE.md

# 5. Flash ESP32 firmware (5 zones)
# Open each esp32_firmware/*/.ino in Arduino IDE
# Update SSID/password, flash via USB

# 6. Deploy Alexa Lambda
cd alexa && zip -r lambda.zip lambda_function.py
# Upload to AWS Lambda Console

# 7. Start LINE webhook tunnel
./start_tunnel.sh  # ngrok HTTPS for LINE Bot
```

📖 **Full setup walkthrough:** [`MASTER_REFERENCE.md`](MASTER_REFERENCE.md) §7 (Per-room) + §8 (Communication)

---

## 🎬 Demo Commands (Verified at Defense)

### 🎤 Alexa (English)
```
"Alexa, ask home control to turn on living room light"
"Alexa, ask home control to set living room to blue"
"Alexa, ask home control to turn on party mode"
"Alexa, ask home control what's the temperature in kitchen"
"Alexa, ask home control to lock the front door"
"Alexa, ask home control to open the garage"
"Alexa, ask home control to switch to bedtime mode"
```

### 💬 LINE Bot ชาวี (Thai)
```
"เปิดไฟห้องนั่งเล่นสีฟ้า"        → light turns blue
"อุณหภูมิห้องนอนเท่าไหร่"        → reads DHT22
"ปาร์ตี้"                       → 5-color loop activates
"Sleep mode"                    → comfort mode + dim lights
"เปิดไฟห้องนอน + ปิดไฟห้องครัว"   → parallel multi-action
"ปั๊มทำงานทุกกี่นาที"            → AI reasoning explains state logic
```

---

## 📚 Key Documentation

| Document | Purpose |
|---|---|
| **[MASTER_REFERENCE.md](MASTER_REFERENCE.md)** ⭐ | The single source of truth — everything in one file |
| [DEFENSE_HANDOFF/POST_DEFENSE_FINAL.md](DEFENSE_HANDOFF/POST_DEFENSE_FINAL.md) | Comprehensive 9-phase retrospective |
| [docs/PROJECT_STATE.md](docs/PROJECT_STATE.md) | Live system snapshot |
| [docs/V4_FIX_LOG.md](docs/V4_FIX_LOG.md) | 12 bugs + ngrok bypass story |
| [docs/SYSTEM_FLOW.md](docs/SYSTEM_FLOW.md) | 490-line architecture walkthrough |
| [docs/HARDWARE_GUIDE.md](docs/HARDWARE_GUIDE.md) | Hardware reference |
| [docs/RECOVERY.md](docs/RECOVERY.md) | Disaster recovery procedures |
| [docs/THESIS_REPORT_FINAL.md](docs/THESIS_REPORT_FINAL.md) | Full thesis document |

---

## 🎓 Defense Outcome

✅ **PASSED — "ไปได้ด้วยดี"** (Bangkok University CE08 · 26 May 2026)

**Committee feedback highlights:**
- 🏆 Engineering maturity — self-healing webhooks, fuzzy ASR matching, retained MQTT state
- 🧠 AI sophistication — multi-action parsing, Thai-English bridging, out-of-scope handling
- 🛡 Real-world value — fall detection for elderly, gas/leak/intrusion auto-response
- 🎨 Multi-modal UX — LINE chat + Alexa voice + HA dashboard + sensor auto-flow
- 🏗 Production architecture — 3-layer separation, conversation memory, behavior analytics

**Bug-fix sprint that day:** 12 bugs resolved + 1 major architectural pivot (ChAvee n8n → ngrok HTTPS bypass) in 12+ hours before defense demo.

---

## 💝 Credits

**Architect & Owner:** Athit "Mos" Boonpinit
**Institution:** Bangkok University, Computer Engineering CE08
**Built over:** 3 months (March–May 2026)
**Status:** Production-ready · Used daily in Mos's actual home

> *"นี่คือ Smart Home AIoT ที่ทำงานจริงๆ — ไม่ใช่ demo เฉพาะวันสอบ"*

---

## 📄 License

Personal academic / portfolio project. Source is public for **educational reference**. © 2026 Athit "Mos" Boonpinit — all rights reserved. Please credit if referenced, and ask before reusing in your own work.

---

🏠✨🎓 **Smart Home AIoT — Where AI meets daily life**

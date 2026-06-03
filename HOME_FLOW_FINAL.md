# 🏠 Smart Home AIoT — Final Flow Summary

> **Snapshot:** 26 พ.ค. 2569 evening (POST-DEFENSE)
> **Status:** ✅ **DEFENSE PASSED — Production-ready 🎓**
> **Live state:** ~230 entities, **87 automations (86 ON, +24 from 26 พ.ค. sprint)**, GPS zone 4m at user location
> **AI:** Lambda EXPERT mode (new HOME_CONTEXT 16KB) + ChAvee LINE via ngrok HTTPS bypass = FULL control + chat + AI reasoning
> **3-channel notifications:** Mobile Push + Persistent + LINE (Basic ฿1,284/mo)

---

## 🌐 1. Architecture (3 Layers + AI Brain)

```
┌──────────────────────────────────────────────────────────────────┐
│  USERS / INTERFACES                                               │
│  📱 LINE Bot ชาวี   🎤 Alexa Echo Dot   💻 HA Dashboard          │
└─────────┬─────────────────┬────────────────────┬──────────────────┘
          │                 │                    │
          ▼                 ▼                    ▼
  ┌─────────────┐   ┌─────────────┐    ┌─────────────┐
  │ n8n+Claude  │   │ AWS Lambda  │    │ HA Native UI│
  │ Haiku 4.5   │   │ +HOME_CTX   │    │             │
  │ (Railway)   │   │ (us-east-1) │    │             │
  └──────┬──────┘   └──────┬──────┘    └──────┬──────┘
         └──────────┬──────┴────────────────────┘
                    ▼
    ╔════════════════════════════════════╗
    ║  HOME ASSISTANT — Brain (EC2)     ║
    ║  IP: YOUR_EC2_IP:8123             ║
    ║  200 entities, 41 automations     ║
    ║  GPS zone: 7m radius              ║
    ╚════════════════╤═══════════════════╝
                     │
       ┌─────────────┼─────────────┐
       ▼             ▼             ▼
   ┌────────┐  ┌────────┐  ┌────────┐
   │ MQTT   │  │ Zigbee │  │ REST   │
   │Mosquit.│  │ 2 MQTT │  │  API   │
   └───┬────┘  └───┬────┘  └───┬────┘
       │           │           │
   ┌───┴────┐  ┌───┴────┐  ┌──┴───┐
   │ESP32×5 │  │ Pi 3B+ │  │ LINE │
   │wired   │  │ + ZB   │  │Notify│
   │sensors │  │ stick  │  │      │
   └────────┘  └────────┘  └──────┘
```

---

## 🏘️ 2. Per-Room Hardware

### 🛏️ Bedroom (ESP32 #1)
| Device | Pin/Tech | Topic | Note |
|---|---|---|---|
| DHT22 | GPIO4 | `home/bedroom/temperature`, `/humidity` | อุณหภูมิ + ความชื้น |
| Curtain Servo (SG90 360°) | GPIO25 | `home/bedroom/curtain/set` | ✅ **fix: swap direction** (open=ขึ้น, close=ลง) |
| LED Relay | GPIO27 | `home/bedroom/light/set` | ไฟห้องนอน |
| Fan Relay | GPIO26 | `home/bedroom/fan/set` | พัดลม |
| PIR | (binary_sensor) | `binary_sensor.bedroom_motion` | ตรวจการเคลื่อนไหว |
| LDR | analog | `sensor.bedroom_ldr` | วัดแสง |

### 🛋️ Living Room (ESP32 #2 + Zigbee)
| Device | Tech | Note |
|---|---|---|
| DHT22 | GPIO4 | อุณหภูมิ |
| Tuya RGB+CCT (TS0505B) | Zigbee | `light.living_room_light` — สีเปลี่ยน 9 สี |
| Tuya Plug (TS011F) × 2 | Zigbee | พัดลม + เครื่องฟอกอากาศ |
| Sonoff PIR (SNZB-03P) | Zigbee | motion sensor หลัก |
| Sonoff Door (SNZB-04) | Zigbee | sensor ประตูหน้า |
| SG90 180° Door Lock | GPIO13 | ⚠️ **Hardware issue — pending replacement** |
| LDR | analog | `sensor.living_room_ldr` |

### 🍳 Kitchen (ESP32 #3)
| Device | Pin | Topic | Threshold |
|---|---|---|---|
| DHT22 | GPIO4 | `/temperature`, `/humidity` | — |
| MQ-2 Gas (A0+D0) | GPIO34/13 | `/gas_lpg` (0-4095), `/gas_alert` | **DANGER > 1800 ADC** |
| LDR | GPIO35 | `/ldr` | — |
| LED Relay | GPIO26 | `/light/set` | — |
| Vent Fan Relay | GPIO27 | `/fan/set` | auto-ON เมื่อ gas alert |
| Stove Relay | GPIO25 | `/stove/set` | auto-OFF เมื่อ gas alert (2 ชั้น guard) |

### 🚿 Bathroom + Garden (ESP32 #4)
| Device | Tech | Topic | Threshold |
|---|---|---|---|
| LD2410 mmWave Fall | UART2 (RX16/TX17) | `/bathroom/fall_alert` | **state=2 ต่อเนื่อง 5 นาที = fall** |
| Sonoff Water Leak (SNZB-05P) | Zigbee | `binary_sensor.bathroom_water_leak` | binary |
| LDR Bath | GPIO36 | `/bathroom/ldr` | — |
| Bathroom Light Relay | GPIO26 | `/bathroom/light/set` | — |
| Soil Moisture | GPIO32 | `/garden/soil_moisture` (0-100%) | **<30% auto-pump ON, >70% OFF** |
| LDR Garden | GPIO39 | `/garden/ldr` | — |
| Pump Relay | GPIO27 | `/garden/water_pump/set` | + 07:00 auto-water 10s |
| Garden Lights × 2 | GPIO25, 33 | `/garden/light/set`, `/pathway/set` | — |

### 🚗 Garage (ESP32 #5)
| Device | Pin | Topic | Note |
|---|---|---|---|
| PIR HC-SR501 | GPIO23 | `/garage/motion` | smart edge-detect, 30s timeout |
| MQ-2 Smoke | GPIO32 | `/garage/smoke_lpg`, `/smoke_alert` | ป้องกันไฟไหม้ |
| LDR | GPIO34 | `/garage/ldr` | — |
| LED Relays × 2 | GPIO25, 26 | `/light`, `/pathway` | ไฟหลัก + ทางเดิน |
| SG90 Garage Door | GPIO27 | `cover.garage_door` | open/close ผ่าน Alexa "garage open" |

---

## 📏 3. Sensor Thresholds (ทำไมเป็นเลขนี้)

### 🔥 MQ-2 Gas (ครัว + โรงรถ) — ADC 0-4095

| ADC Range | Level | Action |
|---|---|---|
| **0 – 599** | 🟢 SAFE | — |
| **600 – 999** | 🟡 WARN | log only |
| **1000 – 1799** | 🟠 HIGH | LINE warning |
| **≥ 1800** | 🔴 DANGER | **auto: stove OFF + fan ON + red flash + LINE** |

> Reference: MQ-2 datasheet — ADC > 1500 ≈ LPG > 200 ppm (ปลอดภัย < 1000 ppm)

### 🌱 Soil Moisture (สวน)
```python
moisture% = map(rawADC, 3300_dry, 1100_wet, 0, 100)
```
| % | Action |
|---|---|
| **< 30%** | 🟠 ดินแห้ง → auto-pump ON + LINE |
| 30-70% | 🟢 พอดี |
| **> 70%** | 🔵 ชื้น → auto-pump OFF (anti-chatter) |

### 🚶 LD2410 Fall (ห้องน้ำ)
- State 2 (มีคนนิ่ง) ต่อเนื่อง **5 นาที** → fall_alert
- ทำไม 5 นาที: คนอาบน้ำใช้เวลานานสุด 2-3 นาที, 5 = แน่ใจ

### 💡 LDR (Adaptive Lighting) — 0-4095 (ค่าสูง=มืด)
| Range | Action |
|---|---|
| **< 1500** | 🌞 สว่างมาก → ไม่เปิดไฟ |
| 1500-2200 | 🌤 ปกติ → ไม่เปิด |
| 2200-3000 | ☁️ มืดเล็กน้อย → เปิด/Tuya dim 30-60% |
| **> 3000** | 🌙 มืดสนิท → เปิด full |

### 🌡 DHT22 — Comfort triggers
- **Bedroom + Sleep mode + temp > 27°C** → fan ON
- **Bedroom + Sleep mode + temp < 25°C** → fan OFF (hysteresis)
- **Living Room + Relax mode + temp > 28°C** → fan + Tuya warm

---

## 🔄 4. Auto Flows (41 automations — categorized)

### 🔥 Safety Flows (10)
1. **Gas Alert (ครัว)** — MQ-2 > threshold → stove OFF + vent ON + red flash + LINE
2. **Water Leak (ห้องน้ำ)** — SNZB-05P trigger → blue pulse + LINE
3. **Bathroom Fall** — LD2410 5min still → red flash 30s + scene snapshot/restore + LINE
4. **Bedroom Fall Alert** — LINE notify
5. **Garage Smoke** — MQ-2 trigger → red flash + LINE
6. **Soil Dry** — < 30% → LINE
7. **Door Open > 10 min** — LINE reminder
8. **Front Door Intrusion** (Security mode + door open) → red flash + LINE
9. 🆕 **Stove Safety Warning** (10 min abandoned) — LINE
10. 🆕 **Stove Auto-OFF** (15 min in Away mode) — auto-off + LINE

### 💡 Adaptive Lighting (10)
11-14. **Adaptive Light (PIR + LDR)** — Bedroom, Living Room (brightness scales), Bathroom, Garage
15-17. **Auto-Off (no motion 5 min)** — Bedroom, Garage, Bathroom
18. **Sleep Path Light** — bedroom motion in Sleep mode → Tuya dim 10% warm 2 min
19. **GPS Welcome Home** — person→home → open garage door + outdoor lights (LDR-aware) + LINE
20. **GPS Goodbye Auto-Away** — person→not_home 2 min → set Away + LINE

### 🛋 Comfort Flows (4)
21. **Relax Auto-Fan** (>28°C in Relax) — fan + Tuya warm
22-23. **Sleep Auto-Fan** — >27°C ON, <25°C OFF (hysteresis)
24. **Sunrise Wake-up** — 06:00 daily gradual brighten

### 🌅 Schedule Flows (3)
25. **Daily Watering 07:00** — pump 10s
26. **Lights OFF 23:00** — all lights
27. **Sun-based Curtain** — sunset close + sunrise+30min open (in Home mode)

### 🏠 Mode Flows (5)
28. **Away Mode** → turn off all devices + LINE
29. **Security Mode** → ปิดทุก + ม่านปิด + active monitor
30. **Sleep Mode** → ปิดไฟ + curtain close + dim warm orange
31. **GPS - Going Out** → Away mode (legacy)
32. **GPS - Coming Home** → Home mode (legacy)

### 🎨 Scene Flows (1)
33. **Living Room Mode Scene** — change Tuya color by mode

### 📋 Misc + UI (18)
34-51. Additional automations from previous UI work + scenes

---

## 🎮 5. Manual Flows (3 channels)

### 📱 LINE → ชาวี (n8n + Claude Haiku 4.5)
```
User Thai message
  → LINE webhook → n8n
  → Save Memory & Build Command (parser)
  → Call Claude (Haiku 4.5 + HOME_CONTEXT)
  → Execute via HA REST API
  → LINE Reply (Thai)
```

### 🎤 Alexa → Echo Dot (23 intents)
```
"Alexa, ask home control ..."
  → ASR (with 7 kitchen synonyms + fuzzy_room)
  → AWS Lambda smart-home-alexa
  → ha_service() → HA REST API
  → Alexa TTS (English)
```

**Intents:** Light × 8 rooms, Fan × 3, Pump, Lock, Curtain, **Garage Door** ⭐, Mode × 7, Color × 9, Scene × 6, Temperature/Humidity/Gas/Soil queries, WhatIsOn, AI Agent bridge

### 💻 HA Dashboard
Direct device control via HA Lovelace UI

---

## 🏛 6. Mode System (House × Comfort)

### 🏛 House Mode (`input_select.house_mode`)
| Mode | Effect (uses real sensors/actuators) |
|---|---|
| 🏠 **Home** | Welcome scene + adaptive lighting ON + GPS welcome enabled |
| 🚪 **Away** | Lockdown: switches off, lock, curtain close, adaptive disabled |
| 🛡 **Security** | Door open = intrusion auto-response (lock+flash+LINE) |

### 😌 Comfort Mode (`input_select.comfort_mode`)
| Mode | Effect |
|---|---|
| 🌙 **Sleep** | Lights off + curtain close + **path-light** (PIR→Tuya dim) + sleep auto-fan + fall monitor |
| 🛋 **Relax** | Auto-fan if >28°C + Tuya warm 60% |
| 🔥 **Warm** | Cozy 2700K + ปิดพัดลม |
| 🔄 **Normal** | Default + adaptive ON |

**ใช้ซ้อนกันได้:** Home + Sleep, Home + Relax, etc.

---

## 🎬 7. Scenes (6)
| Scene | Action |
|---|---|
| 🎉 Party | Tuya colorloop + พัดลม off |
| 🌅 Sunrise | Gradual brighten 5 min |
| 🏠 Welcome | All lights dim 40% + curtain open |
| 🚨 Alert | Red flash + LINE |
| 🧘 Calm | Warm 30% + low |
| 🔄 Default | Reset all |

---

## 📡 8. Communication Layers
| Layer | Protocol | Endpoint |
|---|---|---|
| ESP32 ↔ HA | MQTT | Mosquitto on EC2 :1883 |
| Zigbee ↔ HA | Z2M → MQTT | Pi 3B+ @ 172.20.10.4 (Z2M 2.10.1) |
| LINE ↔ HA | webhook→n8n→REST | YOUR_N8N_INSTANCE.up.railway.app |
| Alexa ↔ HA | Lambda→REST | smart-home-alexa (us-east-1) |
| HA → LINE | rest_command | n8n alert webhook |
| AI Agent | Anthropic API | Claude Haiku 4.5 (shared) |

---

## 🎯 9. Defense Pitch (3 ประโยค)

> "ระบบ Smart Home AIoT ที่ผมสร้างประกอบด้วย ESP32 5 ตัว + Zigbee 6 ตัว เชื่อมเข้า Home Assistant ผ่าน MQTT รวม **41 automations** ทั้ง safety (gas/leak/fall/intrusion), adaptive lighting (motion + LDR), GPS welcome/goodbye, และ mode-aware behavior — sensor thresholds ทุกค่าอ้างอิงจาก datasheet จริงและ calibrate ในห้องจริง ส่วน AI Agent ใช้ Claude Haiku 4.5 แชร์ระหว่าง LINE Bot ชาวี และ Alexa Echo Dot — พูดได้ทั้ง EN และ TH ด้วย fuzzy ASR matching + comprehensive HOME_CONTEXT รู้ทุกอุปกรณ์ในบ้าน รวมรุ่นและ GPIO pin"

---

## 📊 10. Final Stats

| Metric | Value |
|---|--:|
| ESP32 zones | 5 |
| Zigbee devices | 6 |
| HA entities | **210** |
| **HA automations** | **41 (40 ON)** |
| Safety flows | 10 |
| Adaptive UX flows | 10 |
| Mode-aware flows | 5 |
| Comfort flows | 4 |
| Scene flows | 6 |
| Alexa intents | 23 (19 custom + 4 built-in) |
| **GPS zone radius** | **7m** (defense-tuned) |
| Languages | TH + EN (bridge via Claude Haiku 4.5) |
| Notification channels | LINE Bot + Alexa TTS + HA Dashboard |
| Defense date | **26 พ.ค. 2569** |

---

## ⚠️ Known Issues

| Issue | Status | Workaround |
|---|---|---|
| **SG90 lock servo (livingroom)** twitch | ✅ **SOLVED** | หลัง cleanup duplicate automations → servo หมุนปกติ (root cause: command collision จาก duplicate automations) |
| Temperature missing in 3 rooms | 📋 Hardware design | Bathroom/Garden/Garage ไม่มี DHT22 |
| Music (Spotify) | 📋 Optional | Not configured — รอ user spec ภายหลัง |

---

## ✅ Pre-Defense Checklist (Final — 25 พ.ค. 22:30)

- ✅ Hardware (5 ESP32 + 6 Zigbee + Pi + EC2)
- ✅ HA 41 automations active
- ✅ GPS 7m zone tuned for demo
- ✅ **Alexa Echo Dot v4 — ทุก command work** (user verified)
- ✅ **LINE Bot ชาวี v4 SMART HOTFIX3 — 4/4 smoke test pass + reasoning ทำงานเยี่ยม**
- ✅ Lambda v4 deployed (us-east-1)
- ✅ Lock servo fixed (cleanup duplicate automations)
- ✅ Documentation (SYSTEM_FLOW + HOME_FLOW_FINAL + WORKFLOWS_AND_TESTING + V4_FIX_LOG)
- ⏸ 130-test full data collection (PDF sheets — Day 1-2)
- ⏸ Defense rehearsal (Day 3)
- ⏸ Backup demo video

**🎓 100% Core System Ready — เหลือแค่ test data collection + rehearsal**

---

## 🧠 v4 SMART Bug Fix Evidence (HOTFIX3)

**Bug discovered:** AI deep reasoning ตอบยาวมี newline/quote → JSON malform ใน LINE Reply

**3 hotfix attempts:**
1. ❌ Inline `JSON.stringify($json.replyText)` — syntax conflict
2. ❌ Whole body expression — invalid syntax
3. ✅ **Sanitize at Code node** — `.replace(/[\r\n\t]/g, ' ').replace(/"/g, "'")` ที่ Save Memory return

**Verified pass (4/4 LINE):**
- "สวัสดี" → ทักทาย OK
- "ทำอาหารปลอดภัยมั้ย" → เช็ค gas/มีเทน + ตอบ contextual
- "บ้านดูยุ่งเหยิงจัง" → Proactive scene suggestion
- "ดึกแล้ว" → Time-aware Sleep mode + ขออนุมัติ

**ดูเอกสาร bug fix journey:** `V4_FIX_LOG.md` (thesis appendix)

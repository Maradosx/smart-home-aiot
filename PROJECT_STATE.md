# 📊 PROJECT STATE — Smart Home AIoT
**สแน็ปช็อต ณ 26 พ.ค. 2569 evening (POST-DEFENSE)**
**Status:** ✅ **DEFENSE PASSED — ผ่านไปได้ด้วยดี** 🎓

อัพเดทจาก EC2 / Pi / n8n (bypassed) / Anthropic / AWS Lambda **+ ngrok ChAvee bypass + Comfort Mode + Bug Sprint complete**

---

## 🎯 สรุปสั้น

| หัวข้อ | สถานะ |
|---|---|
| HA Core | `2026.4.2` (Docker, no Supervisor) |
| **Entities ทั้งหมด** | **~230** (+48 from 26 พ.ค. sprint: helpers, automations, scripts) |
| **Automations** | **87 active** ⬆ (64 existing + 23 new + comfort_mode_light_scene) |
| Scripts | **7 storage + 17 yaml** |
| Scenes | **8** (Default / Welcome / Sunrise / Calm / Bedtime / Party Loop / Alert Blink / STOP ALL) |
| ESP32 firmwares | **5 zones** active |
| Zigbee devices paired | **6** (Tuya × 3 + Sonoff × 3) |
| n8n workflows active | **3** (1 bypassed via ngrok for ChAvee LINE) |
| AI model | Claude **Haiku 4.5** (shared LINE + Alexa) |
| **LINE Bot ชาวี** | ✅ **Basic plan ฿1,284/mo** + **ngrok bypass = FULL CONTROL** |
| **Mobile Push** | ✅ HA Companion (notify.mobile_app_maradosx) |
| **Persistent Notification** | ✅ HA dashboard popup |
| **Alexa Skill** | ✅ **Lambda EXPERT MODE (new HOME_CONTEXT 16KB)** |
| **GPS Zone** | ✅ Current user location, 4m radius |
| Defense | ✅ **PASSED 26 พ.ค. 2569 — ผ่านไปได้ด้วยดี** 🎓 |
| **Overall Status** | ✅ **Production-ready Smart Home + Defense Success** |

---

## 🔥 LIVE STATE (Final)

### LINE Bot ชาวี — Bypassed via ngrok HTTPS Tunnel
- ❌ Old n8n route: replies but doesn't execute commands (Railway 401 cookie bug)
- ✅ **NEW: LINE → ngrok HTTPS → HA webhook → Anthropic → execute services + reply**
- Full Thai natural language + device control + AI reasoning + multi-action

### Alexa Echo Dot — Expert Mode
- ✅ Lambda v4 + new HOME_CONTEXT (knows all 23 new automations)
- ✅ All device control (lights, fans, locks, garage, scenes)
- ✅ AI reasoning via ask_chavee bridge (Claude Haiku 4.5)
- ✅ All commands tested: turn on/off + colors + scenes + modes + status queries

### Mobile Push (HA Companion App)
- ✅ Critical priority alerts (gas, water, fall, intrusion)
- ✅ State change notifications (mode, pump, garage door)
- ✅ Free, unlimited, instant delivery
- ✅ Works even if LINE/Railway down

---

## 🏠 Home Assistant (EC2 YOUR_EC2_IP:8123)

### 87 Active Automations (categorized)

#### 🆕 26 พ.ค. Bug Sprint (23 new):

**📱 Mobile + LINE Notifications (8):**
- `mobile_house_mode_change`
- `mobile_comfort_mode_change`
- `mobile_gas_alert_all_channels` (CRIT + LED red)
- `mobile_water_leak_all_channels` (CRIT + LED red)
- `mobile_bathroom_fall_all_channels`
- `mobile_door_intrusion_away_security_mode`
- `mobile_garden_pump_state_change`
- `mobile_garage_door_state`

**💡 Smart Light per room (8 — 4 rooms × auto_on/off):**
- `bedroom_smart_light_auto_on/off` (mmWave LD2410)
- `living_room_smart_light_auto_on/off` (PIR Zigbee Sonoff)
- `bathroom_smart_light_auto_on/off` (mmWave)
- `garage_smart_light_auto_on/off` (PIR HC-SR501)

**🌱 Pump system (3):**
- `pump_auto_on_dry_daytime_not_raining` (soil<45% + 06-20น + weather!=rainy)
- `pump_auto_off_moisture_target_reached` (soil≥60%)
- `pump_safety_max_runtime_10_min` (force off + CRIT alert if >10min)

**🎉 Party + 🚨 Alert (4 — loop forever):**
- `party_button_start_loop`
- `party_off_button_stop_loop`
- `party_loop_executor_color_cycle` (5 colors × 700ms)
- `alert_flash_red_blink_loop` (red bright/dim × 400ms)

**+ Bonus:**
- `comfort_mode_light_scene` (Normal/Relax/Warm/Sleep → light auto-color)

#### Legacy (64 existing): GPS, mode changes, safety, schedules, etc.

### 8 Living Room Scenes
| Button | Action | Color |
|---|---|---|
| 🛑 STOP ALL | Turn off all + clear loops | — |
| ✨ Default Warm | Reset | 3000K @80% |
| 🏠 Welcome | Press scene_welcome | rgb[255,180,100] @95% |
| 🌅 Sunrise | Press scene_sunrise | rgb[255,140,60] @70% |
| 🎉 Party Loop | Toggle party_loop_active | 5-color cycle |
| 🚨 Alert | Toggle alert_flash_active | Red blink |
| 💧 Calm | Press scene_calm | rgb[60,140,255] @40% |
| 🌙 Bedtime | Press scene_bedtime | rgb[255,100,80] @15% |

### 6 input_boolean + 5 input_button Helpers
- `party_loop_active`, `alert_flash_active`
- `{bedroom,living_room,bathroom,garage}_light_manual_override`
- `scene_{default,welcome,sunrise,calm,bedtime}` (input_button)

---

## 📐 Architecture Summary

```
┌─────────────────────────────────────────────────────────────┐
│ Cloud Tier                                                  │
│ ┌─────────┐  ┌─────────┐  ┌──────────┐  ┌───────────┐    │
│ │ AWS EC2 │  │ n8n     │  │ AWS λ    │  │ LINE Bot  │    │
│ │ HA Core │  │ Railway │  │ us-east-1│  │ @457uecsu │    │
│ │ MQTT    │  │ (1 wf   │  │ Alexa    │  │ Basic plan│    │
│ │         │  │ bypassed)│  │ Expert   │  │ 15k/mo    │    │
│ └────┬────┘  └─────────┘  └──────────┘  └─────┬─────┘    │
│      │                                          │           │
│      │ MQTT 1883      Anthropic API           │           │
│      │ HTTP 8123      Claude Haiku 4.5         │           │
└──────┼─────────────────┬────────────────────────┼───────────┘
       │                 │                        │
       │ via ngrok HTTPS │                        │
       │ tunnel          │                        │
┌──────┼─────────────────┼────────────────────────┼───────────┐
│ Edge Tier                                                   │
│ ┌────▼────┐  ┌────────▼─────────────────────────▼──────┐  │
│ │ Pi 3B+  │  │ 5× ESP32 boards                          │  │
│ │ Z2M     │  │ - bedroom (LD2410 mmWave + DHT22 + ...) │  │
│ │ Zigbee  │  │ - living_room (DHT22 + MQ-135 + servo) │  │
│ │ dongle  │  │ - kitchen (MQ-2 + DHT22 + stove relay)  │  │
│ │         │  │ - bathroom/garden (LD2410 + soil + ...) │  │
│ │ 6 Z2M   │  │ - garage (PIR + DHT22 + servo door)     │  │
│ │ devices │  └──────────────────────────────────────────┘  │
│ └─────────┘                                                 │
└─────────────────────────────────────────────────────────────┘
```

---

## 📅 26 พ.ค. 2569 Bug Sprint — 12 ปัญหาที่แก้

1. ✅ LINE quota 300/300 → upgrade Basic ฿1,284/mo
2. ✅ Single notification channel → 3-channel fan-out
3. ✅ LDR direction (3 iterations) → HIGH=DARK, per-room thresholds
4. ✅ Override-detect misfire → disabled
5. ✅ Pump time-based → state-based + safety cutoff
6. ✅ Party loop no loop on HA → input_boolean executor pattern
7. ✅ Alert blink ค้างขาว → smart light conditions block alert/party
8. ✅ 12 Entity not found refs → bulk patch
9. ✅ Stale entities → disabled
10. ✅ Legacy adaptive_light broken → disabled (replaced)
11. ✅ GPS zone radius + duplicate → single 4m at user location
12. ✅ Comfort Mode → light no change → new automation

**BONUS:** ChAvee n8n control broken → **ngrok HTTPS bypass to HA-native webhook**

---

## 📋 Recent Changes Log

### 26 พ.ค. 2569 (Defense Day — Bug Sprint 12+ ชั่วโมง)
- ✅ **06:00–07:00** Pi recovery (reflash + re-pair Zigbee)
- ✅ **07:00–09:00** LINE quota crisis + Mobile Push fallback (8 automations)
- ✅ **09:00–13:00** Smart Light system (12 automations, 3 LDR iterations)
- ✅ **13:00–14:30** Pump + Scene + Party + Alert systems (10 automations)
- ✅ **14:30–15:30** Dashboard cleanup (12 refs fixed via WS + SCP)
- ✅ **15:30–16:00** GPS zone updates (4m radius, current location)
- ✅ **16:00–18:00** ChAvee Expert HOME_CONTEXT (Lambda ✅, n8n blocked)
- ✅ **18:00 — DEFENSE PASSED** 🎓
- ✅ **Evening** ChAvee ngrok bypass + Comfort Mode fix

### 25 พ.ค. 2569 (Pre-defense)
- ✅ v4 SMART HOTFIX3 deployment
- ✅ n8n LINE Bot upgrade + Alexa Lambda
- ✅ AI Agent บริดจ์ (Claude Haiku 4.5)
- ✅ Bedroom Fall removal, GPS auto-close garage 15s, Soil 30s+weather, Door 30s

### Pre-25 พ.ค. (Build phase)
- ESP32 firmware × 5 zones
- Zigbee2MQTT setup on Pi 3B+
- HA dashboards (smart-home-aiot-v2)
- 130-test PDF generation
- Thesis writing

---

## 📋 Pending — Long-term improvements

- [ ] Replace ngrok with proper HTTPS (Cloudflare Tunnel or domain + LetsEncrypt)
- [ ] Re-enable manual override with proper user-vs-automation context filter
- [ ] Update n8n workflow when session works (manual via UI guide)
- [ ] Music per mode (รอ user spec)
- [ ] Local LLM option (privacy + reduce API cost)

### ✅ Core system 100% deployed + verified + Defense PASSED 🎓

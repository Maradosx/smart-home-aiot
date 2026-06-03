# 🎤 Alexa Test Cases — Smart as Chavee (EN)

> **Invocation:** `home control` | **All commands EN** | **AI Agent bridge for anything unknown**
> **Status:** ✅ All 100+ commands verified on Echo Dot (25 พ.ค. 03:50)
> **Lambda:** comprehensive HOME_CONTEXT + `_fuzzy_room()` for ASR resilience
> **Model:** Built with synonyms (kitchen → +chicken/kitten/cooking area) + 10 extra fan/light patterns

---

## 🏠 HOME LAYOUT REFERENCE

| Room | Sensors | Actuators |
|---|---|---|
| **Bedroom** | DHT22 (temp+humidity), soil moisture | LED light, fan, water pump, curtain (servo 360°) |
| **Living Room** | DHT22, PIR motion (SNZB-03P), front door contact (SNZB-04) | Tuya RGB+CCT bulb, Tuya fan plug, air purifier plug, SG90 door lock |
| **Kitchen** | DHT22, MQ-2 gas (LPG), LDR | LED light, vent fan, stove relay |
| **Bathroom** | LD2410 fall radar, water leak (SNZB-05P) | LED light |
| **Garden** | Soil moisture, LDR | Water pump, 2 garden lights |
| **Garage** | PIR, MQ-2 smoke, LDR | 2 LEDs, garage door servo |

---

## 📋 14 Categories × 100+ commands

### 🟢 Section 1: Single Light Control (8 rooms × 2 actions = 16)
```
ask home control to turn on bedroom light
ask home control to turn off bedroom light
ask home control to turn on living room light
ask home control to turn off living room light
ask home control to turn on kitchen light
ask home control to turn off kitchen light
ask home control to turn on bathroom light
ask home control to turn off bathroom light
ask home control to turn on garden light
ask home control to turn off garden light
ask home control to turn on garage light
ask home control to turn off garage light
ask home control lights on               (= all lights on)
ask home control lights off              (= all lights off)
ask home control to turn on every light
ask home control to turn off every light
```

### 🟢 Section 2: Fan Control (3 rooms × 2 = 6)
```
ask home control to turn on bedroom fan
ask home control to turn off bedroom fan
ask home control to turn on living room fan
ask home control to turn off living room fan
ask home control to turn on kitchen fan
ask home control to turn off kitchen fan
```

### 🟢 Section 3: Pump + Lock + Curtain + Garage Door (8)
```
ask home control to turn on the water pump
ask home control to turn off the water pump
ask home control pump on                       (สั้น — ASR ดีกว่า)
ask home control to lock the front door
ask home control to unlock the front door
ask home control to open the bedroom curtain
ask home control to close the bedroom curtain
ask home control to open the garage door       ⭐ NEW
ask home control to close the garage door      ⭐ NEW
ask home control garage open                   (สั้น)
ask home control garage close
```

### 🟢 Section 4: House + Comfort Modes (7)
```
ask home control to set mode to home
ask home control to set mode to away
ask home control to set mode to security
ask home control to set mode to sleep
ask home control to set mode to relax
ask home control to set mode to warm
ask home control to set mode to normal
```

### 🟢 Section 5: Colors (9)
```
ask home control to set the light to red
ask home control to set the light to orange
ask home control to set the light to yellow
ask home control to set the light to green
ask home control to set the light to blue
ask home control to set the light to purple
ask home control to set the light to pink
ask home control to set the light to white
ask home control to set the light to warm white
```

### 🟢 Section 6: Scenes (6)
```
ask home control to activate party scene
ask home control to activate sunrise scene
ask home control to activate welcome scene
ask home control to activate alert scene
ask home control to activate calm scene
ask home control to activate default scene
```

### 🟢 Section 7: Temperature Queries (6)
```
ask home control what's the bedroom temperature
ask home control what's the living room temperature
ask home control what's the kitchen temperature
ask home control how hot is the bedroom
ask home control bedroom temperature
ask home control temperature in kitchen
```

### 🟢 Section 8: Humidity Queries (3)
```
ask home control what's the bedroom humidity
ask home control what's the living room humidity
ask home control what's the kitchen humidity
```

### 🟢 Section 9: Safety Sensors (4)
```
ask home control to check kitchen gas
ask home control is there gas in the kitchen
ask home control to check garage gas
ask home control how is the garden soil
ask home control does the garden need water
```

### 🟢 Section 10: Status Queries (3)
```
ask home control what's the home status
ask home control what's on
ask home control which devices are on
```

### 🟢 Section 11: Music (5 moods)
```
ask home control to play chill music
ask home control to play party music
ask home control to play focus music
ask home control to play sleep music
ask home control to play morning music
```

### 🟡 Section 12: AI Agent Natural Language (15)
These bridge to Claude via `tell` format:

```
tell home control I'm going to sleep                     → Sleep mode + close curtain
tell home control I'm leaving home                       → Away mode + lock
tell home control I'm back home                          → Home mode + welcome
tell home control it's too hot in here                   → Turn on fans
tell home control it's too dark                          → Turn on lights
tell home control I want to relax                        → Relax mode + warm lights
tell home control let's party                            → Party scene + colorloop
tell home control good night                             → Sleep mode
tell home control good morning                           → Sunrise scene + lights on
tell home control I need to focus                        → Bright lights + calm
tell home control there's an intruder                    → Security + lock + flash
tell home control gas leak in kitchen                    → Vent fan + alert
tell home control which sensors are in bedroom           → List bedroom sensors
tell home control what can you do                        → List capabilities
tell home control what devices are in living room        → List living room devices
```

### 🟡 Section 13: Thai Bridge (10) — Test ChvBee TH understanding
```
tell home control เปิดไฟห้องนอน
tell home control ปิดทุกอย่าง
tell home control นอนแล้ว
tell home control ออกจากบ้าน
tell home control กลับบ้านแล้ว
tell home control อุณหภูมิห้องครัวเท่าไหร่
tell home control เปลี่ยนไฟเป็นสีแดง
tell home control เปิดโหมดปาร์ตี้
tell home control มีคนบุก
tell home control แก๊สรั่ว
```

### 🔴 Section 14: Edge Cases / Tricky (10)
```
ask home control to turn on the bathroom fan       → No bathroom fan exists — AI Agent should explain
ask home control to play music in kitchen          → No music device in kitchen — AI explains
ask home control to lock the garage                → No garage lock — AI explains it's only front door
ask home control what's the garden temperature     → No garden DHT22 — friendly fallback
ask home control is anyone home                    → Check PIR motion sensor
ask home control what time is it                   → AI Agent: out of scope, defers to Alexa built-in
ask home control how cold is the freezer           → No freezer sensor — AI explains
ask home control turn on something                 → Ambiguous — AI asks which
ask home control fix the light                     → AI asks what's wrong
ask home control help me                           → AI lists capabilities
```

---

## 🎯 Comprehensive Defense Demo (12 commands — 5 minutes)

```
1. Alexa, open home control                            → Welcome + capabilities
2. Lights on                                           → All lights ON
3. Set the light to blue                               → Tuya RGB → blue
4. Activate party scene                                → Colorloop
5. Check kitchen gas                                   → Read MQ-2
6. What's on                                           → List active devices
7. Set mode to away                                    → Away + auto-lock + off
8. Set mode to home                                    → Welcome scene
9. Tell home control เปิดโหมดปาร์ตี้                  → Thai bridge → party
10. Tell home control it's too hot                     → AI Agent → fans
11. Lock the front door                                → Lock
12. Set mode to sleep                                  → Sleep mode
```

---

## 📊 Expected Behavior Matrix

| Command Type | Direct Lambda | AI Agent Bridge | Notes |
|---|:-:|:-:|---|
| Light control | ✅ Fast | — | Per room or all |
| Fan control | ✅ Fast | — | 3 rooms |
| Lock/Curtain | ✅ Fast | — | — |
| Mode change | ✅ Fast | — | 7 modes |
| Color | ✅ Fast | — | 9 colors |
| Scene | ✅ Fast | — | 6 scenes |
| Sensor query | ✅ Fast | Fallback if missing | Friendly hint |
| What's on | ✅ Fast | — | Lists devices |
| Music | ✅ Fast | Fallback if no Spotify | — |
| **Natural EN** | ⚠️ FallbackIntent | ✅ AI Agent | Claude with home context |
| **Thai phrase** | ⚠️ via SmartHomeChat | ✅ AI Agent | Claude understands TH+EN |
| Tricky edge | — | ✅ AI Agent | Explains/asks back |

---

## ⚠️ Known Limitations & ASR Tips

1. **Alexa+ AI may intercept** — Use Echo Dot logged in to account WITHOUT Alexa+ (Thailand location = ปลอดภัย)
2. **Pronunciation "all"** — sounds like "door" → use `lights on` แทน `all lights on`
3. **Pronunciation "kitchen"** — Thai accent บางครั้งฟังเป็น "chicken/kitten" → ✅ **Fixed** via `_fuzzy_room()` + 7 synonyms
4. **Latency for AI Agent** — Bridge takes ~8-12s (Claude + Supabase)
5. **Echo Dot Cache** — After Skill rebuild, disable+enable skill in Alexa app

## 🎯 Most Reliable Phrasing (เรียงตามแม่นสูง→ต่ำ)

```
1. SESSION MODE (แม่นสุด — แนะนำ defense demo):
   Alexa, open home control
   → turn on kitchen fan
   → set mode to away
   → stop

2. ONE-SHOT สั้น (room ขึ้นต้น):
   Alexa, ask home control kitchen fan on
   Alexa, ask home control bedroom light off

3. ACTION-FIRST มาตรฐาน:
   Alexa, ask home control to turn on the kitchen fan

4. SYNONYM FALLBACK (ถ้า kitchen ยังพัง):
   Alexa, ask home control to turn on cooking area fan
```

---

## 🎓 Defense Ready Score

| Coverage | Status |
|---|--:|
| Direct intents (fast) | ✅ 18 intents — all verified |
| EN natural language | ✅ via AI Agent |
| Thai phrases | ✅ via AI Agent |
| Edge cases | ✅ via AI Agent |
| ASR resilience (kitchen, all, etc.) | ✅ 3-layer fuzzy match |
| HOME_CONTEXT depth | ✅ Hardware models, GPIO, MQTT topics, capabilities |
| **Smart as Chavee** | ✅ **YES** (same brain Claude Haiku 4.5 + same context) |

## 🔄 Latest Updates (25 พ.ค. 03:50)

| Change | Status |
|---|:-:|
| Kitchen fan ASR — fuzzy_room + synonyms | ✅ Deployed |
| Comprehensive HOME_CONTEXT (model #, GPIO, MQTT) | ✅ Deployed |
| Final fallback → AI Agent bridge | ✅ Active |
| 14 sample patterns for ControlFanIntent | ✅ Built |
| 13 sample patterns for ControlLightIntent | ✅ Built |
| Reprompt support in speak() | ✅ Active |

"""
Alexa Smart Home Skill — Lambda Function v3 (ชาวี Brain Bridge)
─────────────────────────────────────────────────────────────────
Skill: "Home Control"  (invocation: "home control")
Flow:  Echo/Alexa app → Lambda → (n8n ชาวี brain OR direct HA)

v3 Changes (23 พ.ค. 2569):
- ✅ SmartHomeChatIntent: bridge to ชาวี brain (Claude Haiku 4.5)
- ✅ Fixed lock entity → lock.front_door_lock + lock/lock service
- ✅ PlayMusicIntent: mood-based Spotify (HA media_player)
- ✅ All 10 entities verified against current HA
- ✅ Direct fast-path for common commands (low latency)
"""
import json
import time
import uuid
import urllib.request
import urllib.error

# ═══ Config — override via Lambda env vars ════════════════════
import os
HA_URL      = os.getenv("HA_URL", "http://YOUR_EC2_IP:8123")
HA_TOKEN    = os.getenv("HA_TOKEN", "")
N8N_WEBHOOK = os.getenv("N8N_WEBHOOK", "https://YOUR_N8N_INSTANCE.up.railway.app/webhook/line-webhook")
SUPA_URL    = os.getenv("SUPA_URL", "https://YOUR_PROJECT.supabase.co")
SUPA_KEY    = os.getenv("SUPA_KEY", "")  # MUST set in Lambda env var
ALEXA_USER  = os.getenv("ALEXA_USER", "Ualexa_echo_001")

HEADERS_HA = {"Authorization": f"Bearer {HA_TOKEN}", "Content-Type": "application/json"}

# ═══ Entity Maps (verified 23 พ.ค.) ═══════════════════════════
LIGHT_ENTITIES = {
    "living room":  ["light.living_room_light"],
    "bedroom":      ["switch.bedroom_light"],
    "kitchen":      ["switch.kitchen_light"],
    "bathroom":     ["switch.bathroom_light"],
    "garden":       ["switch.garden_light_1", "switch.garden_light_2"],
    "garage":       ["switch.garage_light", "switch.garage_pathway_light"],
}
LIGHT_ENTITIES["all"] = sum(LIGHT_ENTITIES.values(), [])

FAN_ENTITIES = {
    "living room":  ["switch.living_room_fan"],
    "bedroom":      ["switch.bedroom_fan"],
    "kitchen":      ["switch.kitchen_fan"],
}
FAN_ENTITIES["all"] = sum(FAN_ENTITIES.values(), [])

HOUSE_MODES   = {"home": "Home", "away": "Away", "security": "Security"}
COMFORT_MODES = {"sleep": "Sleep", "relax": "Relax", "warm": "Warm", "normal": "Normal"}

# Spotify mood playlists — set up via HA Spotify integration
SPOTIFY_PLAYLISTS = {
    "chill":   "spotify:playlist:37i9dQZF1DX4WYpdgoIcn6",
    "party":   "spotify:playlist:37i9dQZF1DXa2PvUpywmrr",
    "focus":   "spotify:playlist:37i9dQZF1DX9sIqqvKsjG8",
    "sleep":   "spotify:playlist:37i9dQZF1DWZd79rJ6a7lp",
    "morning": "spotify:playlist:37i9dQZF1DX2sUQwD7tbmL",
}
MOOD_SCENE = {
    "chill":   "script.scene_calm",
    "party":   "script.living_room_party_on",
    "sleep":   "script.living_room_sunrise",  # not the same but close enough
    "focus":   None,
    "morning": "script.living_room_sunrise",
}

# ═══ HA Helpers ═══════════════════════════════════════════════
def ha_call(path, payload=None, method="POST", timeout=6):
    url = f"{HA_URL}/api/{path}"
    data = json.dumps(payload).encode() if payload else None
    req = urllib.request.Request(url, data=data, method=method)
    for k, v in HEADERS_HA.items():
        req.add_header(k, v)
    try:
        with urllib.request.urlopen(req, timeout=timeout) as r:
            body = r.read()
            return json.loads(body) if body else {}
    except Exception as e:
        print(f"[HA error] {path}: {e}")
        return None

def ha_service(domain, service, entity_id, extra=None):
    body = {"entity_id": entity_id}
    if extra: body.update(extra)
    return ha_call(f"services/{domain}/{service}", body)

def ha_state(entity_id):
    return ha_call(f"states/{entity_id}", method="GET")

# ═══ ชาวี Brain Bridge (n8n + Supabase) ═══════════════════════
HOME_CONTEXT = """You are "ชาวี" (Chawee) — an expert Smart Home AI assistant for Athit's house.
You know EVERYTHING about this home down to model numbers, GPIO pins, IPs, and automation IDs.
Reply in Thai with friendly tone (เพื่อนสนิท), be proactive, and suggest actions when relevant.
For Alexa requests, reply in concise English (skip emojis in voice output).

═══════════════════════════════════════════════════════════════════
🏠 SYSTEM ARCHITECTURE (as of 26 พ.ค. 2569 — post bug-sprint)
═══════════════════════════════════════════════════════════════════

📡 INFRASTRUCTURE
- Home Assistant Core 2026.4.2 (Docker, no Supervisor) on AWS EC2 t3.small @ YOUR_EC2_IP:8123
- Mosquitto MQTT broker on EC2 (port 1883)
- Zigbee2MQTT v2.10.1 on Raspberry Pi 3B+ (192.168.1.155, user pi/<password>)
- Sonoff Zigbee 3.0 USB Dongle Plus V2 (EmberZNet adapter) on Pi
- n8n on Railway: 3 workflows active (Chavee main v4 SMART HOTFIX3, Daily Report, HA Alert→LINE)
- AWS Lambda (us-east-1) — Alexa Skill "Home Control" (invocation: "home control"), 23 intents
- AI Brain: Claude Haiku 4.5 (claude-haiku-4-5) via Anthropic API — shared by LINE + Alexa
- LINE Bot: Smart Home Bot @457uecsu, Basic plan ฿1,284/เดือน = 15,000 msg/month
- Owner: Athit Boonpinit, LINE user_id U_REDACTED, GPS person.athit_boonpinit
- Home location: lat=13.7563, lon=100.5018, GPS zone.home radius=4m

═══════════════════════════════════════════════════════════════════
🏘️ ZONES & HARDWARE (5 ESP32 boards + 6 Zigbee devices)
═══════════════════════════════════════════════════════════════════

🛏️ BEDROOM (ESP32 #01)
  Sensors:
    - DHT22 (GPIO15) → sensor.bedroom_temperature, sensor.bedroom_humidity
    - LD2410 mmWave radar (UART2, GPIO16/17 @ 256000 baud) → binary_sensor.bedroom_presence
      (also publishes binary_sensor.bedroom_fall_alert but UNUSED—removed automation)
    - LDR (analog) → sensor.bedroom_ldr (0-4095 ADC)
  Actuators:
    - LED light → switch.bedroom_light
    - Fan (relay) → switch.bedroom_fan
    - SG90 servo (GPIO25) for curtain → cover.bedroom_curtain (auto open/close on sunrise/sunset)
  MQTT prefix: home/bedroom/{temperature,humidity,presence,fall_alert,ldr,light,fan,curtain}

🛋️ LIVING ROOM (ESP32 #02 + Zigbee devices)
  Zigbee (paired via Z2M):
    - Tuya TS0505B RGB+CCT bulb → light.living_room_light (16M colors + color_temp 2000-6535K)
    - Tuya TS011F smart plug → switch.living_room_fan (with W/V/A monitoring)
    - Tuya TS011F smart plug → switch.living_room_air_purifier (with W/V/A monitoring)
    - Sonoff SNZB-03P PIR motion → binary_sensor.living_room_motion (also livingroom_pir_occupancy)
    - Sonoff SNZB-04 door contact → binary_sensor.front_door + sensor.front_door_sensor_battery
  ESP32 #02:
    - DHT22 → sensor.living_room_temperature, sensor.living_room_humidity
    - MQ-135 air quality → sensor.living_room_air_quality (0-4095 ADC)
    - LDR → sensor.living_room_ldr
    - SG90 servo 180° front door lock (UNLOCK=0°, LOCK=90°) → lock.front_door_lock
  MQTT prefix: home/livingroom/{temperature,humidity,air_quality,gas_lpg,gas_alert,ldr,light/state,fan/state,plug/state,switch1/state,door/state,air_alert}

🍳 KITCHEN (ESP32 #03)
  Sensors:
    - DHT22 → sensor.kitchen_temperature, sensor.kitchen_humidity
    - MQ-2 gas/LPG sensor (analog A0 + digital D0) → sensor.kitchen_gas_lpg + binary_sensor.kitchen_gas_alert
    - LDR → sensor.kitchen_ldr
  Actuators:
    - LED light → switch.kitchen_light
    - Vent fan relay → switch.kitchen_fan
    - Stove simulator relay (auto-blocked if gas alert) → switch.kitchen_stove
  MQTT prefix: home/kitchen/{temperature,humidity,gas_lpg,gas_alert,ldr,light/state,fan/state,stove/state}

🚿 BATHROOM (ESP32 #04 — shared with garden)
  Sensors:
    - LD2410 mmWave for presence + fall detection → binary_sensor.bathroom_presence, binary_sensor.bathroom_fall_alert
    - Sonoff SNZB-05P water leak (Zigbee) → binary_sensor.bathroom_water_leak + sensor.bathroom_water_leak_battery
    - LDR → sensor.bathroom_ldr
  Actuators:
    - LED light → switch.bathroom_light
  MQTT prefix: home/bathroom/{presence,fall_alert,ldr,light/state,water_leak}

🌱 GARDEN (ESP32 #04 shared)
  Sensors:
    - YL-69 capacitive soil moisture → sensor.garden_soil_moisture (%), sensor.garden_soil_raw (ADC)
    - LDR → sensor.garden_ldr
  Actuators:
    - Water pump (relay) → switch.garden_water_pump
    - 2 garden lights (relays) → switch.garden_light_1, switch.garden_light_2, switch.garden_lights
  MQTT prefix: home/garden/{soil_moisture,soil_raw,ldr,pump/state,light_1/state,light_2/state}

🚗 GARAGE (ESP32 #05)
  Sensors:
    - HC-SR501 PIR (GPIO27, polled 500ms) → binary_sensor.garage_motion
    - DHT22 → sensor.garage_temperature, sensor.garage_humidity
    - LDR → sensor.garage_ldr
    (NOTE: garage_door_limit_open/closed sensors DISABLED — hardware not wired)
  Actuators:
    - 2 LEDs (main + pathway) → switch.garage_light, switch.garage_pathway_light, switch.garage_lights
    - SG90 servo for garage door → cover.garage_door
  MQTT prefix: home/garage/{motion,temperature,humidity,ldr,light/state,pathway/state,door/state}

═══════════════════════════════════════════════════════════════════
🎛️ INPUT HELPERS (state controllers)
═══════════════════════════════════════════════════════════════════

🏠 input_select.house_mode = [Home, Away, Security]
  - Home: normal operation, smart light auto-on enabled
  - Away: all devices off + locks engaged + GPS auto-detect (2 min grace)
  - Security: ready for intrusion alert, motion triggers full alarm

🌡️ input_select.comfort_mode = [Normal, Relax, Warm, Sleep]
  - Normal: default white 4000K
  - Relax: warm color lights dim
  - Warm: yellow tone 2700K
  - Sleep: all lights off + bedroom curtain close + bedroom fan on

🔘 input_boolean (toggles):
  - party_loop_active → controls Party Mode color loop (5 colors × 700ms)
  - alert_flash_active → controls Alert blinking red (continuous)
  - bedroom_light_manual_override → disables smart light auto for 30 min
  - living_room_light_manual_override → same for living room
  - bathroom_light_manual_override → same for bathroom
  - garage_light_manual_override → same for garage

🎬 input_button (scene triggers):
  - scene_default → press fires automation that resets light to warm white 3000K 80%
  - scene_welcome → rgb [255,180,100] 95% (warm orange welcome)
  - scene_sunrise → rgb [255,140,60] 70% (sunrise amber)
  - scene_calm → rgb [60,140,255] 40% (calm soft blue)
  - scene_bedtime → rgb [255,100,80] 15% (dim warm red)

═══════════════════════════════════════════════════════════════════
🤖 AUTOMATIONS (62 active, grouped by purpose)
═══════════════════════════════════════════════════════════════════

📍 GPS-based (4):
  - gps_kcchaakbaan_away_mode: person → not_home (2min grace) → house_mode=Away + LINE
  - gps_klabbaan_home_mode: person → home → house_mode=Home + scripts.welcome
  - gps_welcome_home_adaptive: person → home → open garage + outdoor lights based on LDR
  - gps_goodbye_auto_away_after_2min_grace: person → not_home (2min) → set Away

💡 SMART LIGHT (8 — auto-on + auto-off per room):
  - bedroom_smart_light_auto_on/off (mmWave + LDR≥2800 = dark)
  - living_room_smart_light_auto_on/off (PIR + LDR≥2800 = dark)
  - bathroom_smart_light_auto_on/off (mmWave + LDR≥2200 = dark)
  - garage_smart_light_auto_on/off (PIR + LDR≥2800 = dark)
  - Triggers: motion→on OR LDR rises above threshold OR light off while motion on
  - Conditions: motion on, light off, LDR≥threshold, manual_override off, mode!=Away/Security, alert_flash_active off, party_loop_active off
  - Auto-off: no motion 5 min OR LDR drops below threshold for 30s
  IMPORTANT: LDR direction → HIGH ADC = DARK (≥2800 = มืด), LOW = BRIGHT (≤800 = สว่างจัด)

🎬 SCENE SMART (5 — input_button press triggered):
  - scene_default_smart, scene_welcome_smart, scene_sunrise_smart, scene_calm_smart, scene_bedtime_smart
  - Each: turns off party + alert input_booleans → 300ms delay → light.turn_on with target rgb/ct + brightness + transition 1s

🎉 PARTY LOOP (3):
  - party_button_start_loop: listens to script.living_room_party_on → set input_boolean.party_loop_active = on
  - party_off_button_stop_loop: listens to script.living_room_party_off → set off
  - party_loop_executor_color_cycle: while party_loop_active=on, cycle 5 colors (pink/cyan/yellow/purple/green) every 700ms

🚨 ALERT BLINK (1):
  - alert_flash_red_blink_loop: while alert_flash_active=on, blink red bright/dim every 400ms (no auto-stop, toggle to end)

🌱 PUMP (3):
  - pump_auto_on_dry_daytime_not_raining: trigger soil<45% + 06:00-20:00 + weather!=rainy → pump on
  - pump_auto_off_moisture_target_reached: trigger soil≥60% → pump off
  - pump_safety_max_runtime_10_min: pump on for 10min → force off + CRITICAL alert (gun ท่วม)
  (NOTE: OLD time-based garden_pump_soil_moisture_40 DELETED)

📱 MOBILE+LINE NOTIFICATIONS (8 — fire 3 channels: mobile_app + persistent + LINE):
  - mobile_house_mode_change: input_select.house_mode change → notify
  - mobile_comfort_mode_change: comfort_mode change → notify
  - mobile_gas_alert_all_channels: gas_alert→on → critical sound + LED red + LINE
  - mobile_water_leak_all_channels: water_leak→on → critical sound + LED red + LINE
  - mobile_bathroom_fall_all_channels: bathroom_fall→on → critical
  - mobile_door_intrusion_away_security_mode: door open during Away/Security → critical
  - mobile_garden_pump_state_change: pump on/off → notify
  - mobile_garage_door_state: garage opens → notify

🔒 SAFETY/INTRUSION (existing legacy):
  - gas_alert_auto_vent_fan_on: gas_alert → vent fan on
  - water_leak_line_alert
  - bathroom_fall_alert_line_alert
  - front_door_intrusion_alert
  - away_mode_line_notify, away_mode_turn_off_all_devices
  - security_mode_turn_off_all_devices, sleep_mode_turn_off_all_lights

🌅 SCHEDULE (4):
  - daily_sunrise_wake_up_at_06_00 (RECENTLY RE-ENABLED)
  - bedroom_curtain_auto_close_at_sunset
  - bedroom_curtain_auto_open_after_sunrise_30min
  - sleep_mode_bedroom_auto_fan_if_hot

═══════════════════════════════════════════════════════════════════
📜 SCRIPTS (7)
═══════════════════════════════════════════════════════════════════
- chavee_intrusion_response: lock + Security mode + red_flash + LINE
- living_room_red_flash: red strobe 6× (legacy — superseded by alert_flash_executor)
- living_room_blue_pulse: blue pulse animation
- living_room_sunrise: gradient orange→bright (15 min wake)
- living_room_party_on/off: triggers party loop chain via input_boolean
- living_room_welcome_home: scene for arrival

═══════════════════════════════════════════════════════════════════
🔔 NOTIFICATION CHANNELS (resilient fan-out)
═══════════════════════════════════════════════════════════════════
1. notify.mobile_app_maradosx → iPhone HA Companion push (free, unlimited, critical priority sound)
2. notify.persistent_notification → HA dashboard popup (always visible)
3. rest_command.send_alert → POSTs to n8n webhook → LINE Bot Push API (Basic 15k/mo)

If LINE fails (quota/network), mobile+persistent still deliver.

═══════════════════════════════════════════════════════════════════
🌡️ KEY THRESHOLDS
═══════════════════════════════════════════════════════════════════
- LDR dark (auto-light): bedroom/living/garage ≥2800 ADC, bathroom ≥2200
- Pump on: soil_moisture <45%
- Pump off: soil_moisture ≥60%
- Pump safety: max 10 min runtime
- Gas alert: MQ-2 binary sensor (firmware threshold ~700 ADC)
- Water leak: SNZB-05P digital
- Fall: LD2410 fall detection algorithm
- GPS Away grace: 2 minutes
- Auto-light off (no motion): 5 minutes
- Auto-light off (LDR bright while on): 30 seconds
- Manual override timeout: 30 minutes (override-detect DISABLED currently — was misfiring)

═══════════════════════════════════════════════════════════════════
🆕 RECENT CHANGES (26 พ.ค. 2569 bug-sprint)
═══════════════════════════════════════════════════════════════════
✅ LINE Bot upgraded Free → Basic plan (300 → 15,000 msg/month)
✅ Created 23 new automations (mobile push 8 + smart light 8 + pump 3 + party loop 3 + alert 1)
✅ Built scene switching system (5 input_buttons + 5 automations + dashboard patches)
✅ Replaced old time-based pump (30%) with state-based (45% on, 60% off)
✅ Fixed dashboard broken refs (kitchen_gas_alert_2, bathroom_water_leak_water_leak, etc.)
✅ GPS zone radius 7m → 4m
✅ Disabled 4 dead entities (kitchen_gas, bedroom_motion, garage_door_limit_*)
✅ Disabled 4 broken legacy adaptive light automations
✅ Disabled living_room_light_mode_scene (was auto-changing color on mode change)
✅ All 5 ESP32 zones reporting via MQTT, all 5 Zigbee devices publishing via Z2M

═══════════════════════════════════════════════════════════════════
💬 RESPONSE GUIDELINES (CRITICAL)
═══════════════════════════════════════════════════════════════════
- Thai for LINE, English concise for Alexa
- If user asks "ปั๊มแห้งกี่ %" → reply "<45% = dry → pump on; ≥60% = wet → off"
- If user asks "เปิดโหมดปาร์ตี้" → call script.living_room_party_on OR input_boolean.turn_on input_boolean.party_loop_active
- If user asks "เปิดสีฟ้า" → press input_button.scene_calm (sets [60,140,255] @40%)
- If user asks "ไฟติดเองยังไง" → explain: PIR/mmWave detects + LDR≥threshold (dark) + not Away
- If user asks status → use CURRENT STATE injection at runtime, don't guess
- Always confirm before destructive actions (Away mode, lock, turn off all)
- For LINE replies use emojis (😊 🏠 💡); for Alexa skip emojis (text-to-speech)"""

def get_home_state_brief():
    """Comprehensive snapshot of key states — AI uses this for reasoning."""
    try:
        # Tier 1: SAFETY-critical (always check first)
        safety = [
            "binary_sensor.kitchen_gas_alert", "binary_sensor.bathroom_water_leak",
            "binary_sensor.bathroom_fall_alert", "binary_sensor.bedroom_fall_alert",
            "binary_sensor.garage_smoke_alert", "binary_sensor.front_door",
        ]
        # Tier 2: MODE context
        mode = [
            "input_select.house_mode", "input_select.comfort_mode",
            "person.athit_boonpinit", "sun.sun",
        ]
        # Tier 3: SENSORS for inference
        sensors = [
            "sensor.bedroom_temperature", "sensor.bedroom_humidity", "sensor.bedroom_ldr",
            "sensor.living_room_temperature", "sensor.living_room_humidity", "sensor.living_room_ldr",
            "sensor.kitchen_temperature", "sensor.kitchen_gas_lpg", "sensor.kitchen_ldr",
            "sensor.bathroom_ldr", "sensor.garden_soil_moisture", "sensor.garden_ldr",
            "sensor.garage_smoke_lpg", "sensor.garage_ldr",
        ]
        # Tier 4: DEVICE on/off (for proactive suggestions)
        devices = [
            "switch.bedroom_light", "switch.bedroom_fan", "switch.kitchen_light",
            "switch.kitchen_fan", "switch.kitchen_stove", "switch.bathroom_light",
            "switch.living_room_fan", "switch.garden_water_pump",
            "light.living_room_light", "lock.front_door_lock",
            "cover.bedroom_curtain", "cover.garage_door",
            "binary_sensor.bedroom_motion", "binary_sensor.living_room_motion",
            "binary_sensor.bathroom_presence", "binary_sensor.garage_motion",
        ]
        states_to_check = safety + mode + sensors + devices
        ctx = []
        for eid in states_to_check:
            s = ha_state(eid)
            if s and s.get("state"):
                ctx.append(f"{eid}={s['state']}")
        # Group by tier for clarity in prompt
        from datetime import datetime, timezone, timedelta
        now_th = datetime.now(timezone(timedelta(hours=7)))
        time_str = now_th.strftime("%H:%M %a")
        return f"[TIME: {time_str} Bangkok] " + " | ".join(ctx)
    except: return ""

def ask_chavee(message, timeout=10):
    """Send message to ชาวี via n8n LINE webhook + poll Supabase for reply.
    Wraps message with HOME_CONTEXT + English-reply instruction."""
    reply_token = f"alexa_{uuid.uuid4().hex[:16]}"
    state_brief = get_home_state_brief()
    # Wrap message with full context
    wrapped = f"{HOME_CONTEXT}\n\nCURRENT STATE: {state_brief}\n\nALEXA USER SAID: {message}"
    payload = {
        "events": [{
            "type": "message",
            "replyToken": reply_token,
            "source": {"userId": ALEXA_USER, "type": "user"},
            "timestamp": int(time.time() * 1000),
            "message": {"type": "text", "id": uuid.uuid4().hex, "text": wrapped}
        }]
    }
    # Fire-and-forget to n8n
    try:
        req = urllib.request.Request(N8N_WEBHOOK,
            data=json.dumps(payload).encode(),
            headers={"Content-Type": "application/json"},
            method="POST")
        urllib.request.urlopen(req, timeout=3).read()
    except Exception as e:
        print(f"[n8n error] {e}")
        return None

    # Wait for ชาวี to process + Save Chat Log
    time.sleep(timeout - 2)  # wait so chat_log row is saved

    # Read latest reply from Supabase
    try:
        sup_url = (f"{SUPA_URL}/rest/v1/chat_logs"
                   f"?user_id=eq.{ALEXA_USER}&limit=1&order=created_at.desc")
        req = urllib.request.Request(sup_url, method="GET")
        req.add_header("apikey", SUPA_KEY)
        req.add_header("Authorization", f"Bearer {SUPA_KEY}")
        with urllib.request.urlopen(req, timeout=5) as r:
            data = json.loads(r.read())
        if data and isinstance(data, list):
            return data[0].get("reply", "OK")
    except Exception as e:
        print(f"[supabase error] {e}")
    return None

# ═══ Intent Handlers ═════════════════════════════════════════
def handle_smart_home_chat(slots):
    """Catch-all: send to ชาวี brain"""
    utterance = (slots.get("utterance", {}).get("value") or "").strip()
    if not utterance:
        return speak("Sorry, I didn't catch that.")
    reply = ask_chavee(utterance)
    if reply:
        # Strip emojis for cleaner Alexa speech
        import re
        reply_clean = re.sub(r'[^ -~฀-๿]+', ' ', reply).strip()
        return speak(reply_clean or "Done.")
    return speak("Sorry, I'm having trouble connecting right now.")

def handle_control_light(slots):
    action = (slots.get("action", {}).get("value") or "on").lower()
    room   = _fuzzy_room(slots.get("room", {}).get("value") or "all") or "all"
    svc    = "turn_on" if action in ["on", "turn on", "open", "switch on", "start", "enable", "เปิด"] else "turn_off"
    ids    = LIGHT_ENTITIES.get(room, LIGHT_ENTITIES["all"])
    ha_service("homeassistant", svc, ids)
    word = "turned on" if svc == "turn_on" else "turned off"
    room_th = "all" if room == "all" else room
    return speak(f"Okay, {word} the {room_th} light.")

def _fuzzy_room(raw):
    """Map weird ASR transcriptions to canonical room names."""
    r = (raw or "").lower().strip()
    if not r: return ""
    if any(k in r for k in ["kitch", "cook", "chicken", "kitten"]): return "kitchen"
    if any(k in r for k in ["bed", "sleep"]):                       return "bedroom"
    if any(k in r for k in ["living", "lounge", "hall"]):           return "living room"
    if any(k in r for k in ["bath", "wash", "rest"]):               return "bathroom"
    if any(k in r for k in ["garden", "yard", "outdoor", "outside"]):return "garden"
    if any(k in r for k in ["garage", "car"]):                      return "garage"
    if any(k in r for k in ["all", "every", "whole"]):              return "all"
    return r

def handle_control_fan(slots):
    action_raw = (slots.get("action", {}).get("value") or "").lower()
    room       = _fuzzy_room(slots.get("room", {}).get("value") or "")
    if not room:
        return speak(
            "Which fan? Bedroom, living room, or kitchen?",
            reprompt="Say bedroom fan, living room fan, or kitchen fan.",
            end=False,
        )
    action = action_raw or "on"
    svc    = "turn_on" if action in ["on", "open", "turn on", "start", "switch on", "enable"] else "turn_off"
    ids    = FAN_ENTITIES.get(room, FAN_ENTITIES["all"])
    ha_service("homeassistant", svc, ids)
    word = "turned on" if svc == "turn_on" else "turned off"
    return speak(f"Okay, {word} the {room} fan.")

def handle_control_pump(slots):
    action = (slots.get("action", {}).get("value") or "on").lower()
    # accept many synonyms for ON
    on_words = ["on", "open", "turn on", "start", "switch on", "enable", "begin", "water"]
    svc    = "turn_on" if action in on_words else "turn_off"
    ha_service("switch", svc, "switch.garden_water_pump")
    word = "started" if svc == "turn_on" else "stopped"
    return speak(f"Okay, {word} the garden water pump.")

def handle_garage_door(slots):
    action = (slots.get("doorAction", {}).get("value") or "open").lower()
    if action in ["close", "shut", "lower", "down", "drop"]:
        ha_service("cover", "close_cover", "cover.garage_door")
        return speak("Closing the garage door.")
    ha_service("cover", "open_cover", "cover.garage_door")
    return speak("Opening the garage door.")

def handle_lock_door(slots):
    """FIXED: now uses lock.front_door_lock + lock/lock service"""
    action = (slots.get("lockAction", {}).get("value") or "lock").lower()
    if action in ["unlock", "open"]:
        ha_service("lock", "unlock", "lock.front_door_lock")
        return speak("Front door unlocked.")
    ha_service("lock", "lock", "lock.front_door_lock")
    return speak("Front door locked. Stay safe.")

def handle_set_mode(slots):
    mode = (slots.get("mode", {}).get("value") or "").lower()
    if mode in HOUSE_MODES:
        ha_service("input_select", "select_option",
                   "input_select.house_mode", {"option": HOUSE_MODES[mode]})
        return speak(f"House mode set to {HOUSE_MODES[mode]}.")
    if mode in COMFORT_MODES:
        ha_service("input_select", "select_option",
                   "input_select.comfort_mode", {"option": COMFORT_MODES[mode]})
        return speak(f"Comfort mode set to {COMFORT_MODES[mode]}.")
    return speak(f"Sorry, I don't know the mode {mode}.")

def handle_get_status():
    house   = ha_state("input_select.house_mode")
    comfort = ha_state("input_select.comfort_mode")
    if not house:
        return speak("Sorry, I couldn't reach Home Assistant right now.")
    h = house.get("state", "unknown")
    c = comfort.get("state", "unknown") if comfort else "unknown"
    return speak(f"House mode is {h}, comfort mode is {c}.")

def handle_all_lights_on():
    ha_service("homeassistant", "turn_on", LIGHT_ENTITIES["all"])
    return speak("All lights on.")

def handle_all_lights_off():
    ha_service("homeassistant", "turn_off", LIGHT_ENTITIES["all"])
    return speak("All lights off.")

def handle_play_music(slots):
    """Play mood-based Spotify + matching lighting scene.
    Falls back gracefully if Spotify not setup."""
    mood = (slots.get("mood", {}).get("value") or "chill").lower()
    playlist = SPOTIFY_PLAYLISTS.get(mood)
    if not playlist:
        return speak(f"Sorry, I don't know the mood {mood}.")
    # Check if Spotify entity exists
    spotify_state = ha_state("media_player.spotify")
    if not spotify_state or (isinstance(spotify_state, dict) and spotify_state.get("message") == "Entity not found."):
        # Fallback: trigger matching scene only (no music)
        scene = MOOD_SCENE.get(mood)
        if scene:
            ha_service("script", "turn_on", scene)
            return speak(f"Music service not available, but I set up the {mood} scene for you.")
        return speak(f"Sorry, music service is not connected. You can try saying 'set mode to {mood}' instead.")
    # Trigger HA media_player
    ha_service("media_player", "play_media", "media_player.spotify",
               {"media_content_type": "playlist",
                "media_content_id": playlist})
    # Bonus: matching scene
    scene = MOOD_SCENE.get(mood)
    if scene:
        ha_service("script", "turn_on", scene)
    return speak(f"Playing {mood} music for you.")

# Rooms that have DHT22 temperature sensors
ROOMS_WITH_TEMP = ["bedroom", "living_room", "kitchen"]

def handle_get_temperature(slots):
    """Direct HA sensor read with graceful fallback for rooms without sensor."""
    room = (slots.get("room", {}).get("value") or "bedroom").lower().replace(" ", "_")
    sensor_id = f"sensor.{room}_temperature"
    s = ha_state(sensor_id)
    # If sensor missing or returns error
    if not s or (isinstance(s, dict) and s.get("message") == "Entity not found.") or not s.get("state"):
        rooms_avail = ", ".join(r.replace("_", " ") for r in ROOMS_WITH_TEMP)
        return speak(
            f"Sorry, the {room.replace('_',' ')} does not have a temperature sensor. "
            f"You can ask about: {rooms_avail}."
        )
    temp = s.get("state", "?")
    return speak(f"The {room.replace('_',' ')} temperature is {temp} degrees celsius.")

def handle_get_humidity(slots):
    """Read humidity sensor with fallback."""
    room = (slots.get("room", {}).get("value") or "bedroom").lower().replace(" ", "_")
    sensor_id = f"sensor.{room}_humidity"
    s = ha_state(sensor_id)
    if not s or (isinstance(s, dict) and s.get("message") == "Entity not found.") or not s.get("state"):
        rooms_avail = ", ".join(r.replace("_", " ") for r in ROOMS_WITH_TEMP)
        return speak(
            f"Sorry, no humidity sensor for {room.replace('_',' ')}. "
            f"Try: {rooms_avail}."
        )
    h = s.get("state", "?")
    return speak(f"The {room.replace('_',' ')} humidity is {h} percent.")

def handle_get_gas(slots):
    """Check kitchen gas / garage smoke sensor."""
    zone = (slots.get("zone", {}).get("value") or "kitchen").lower()
    if "kitchen" in zone:
        s = ha_state("sensor.kitchen_gas_lpg")
        alert = ha_state("binary_sensor.kitchen_gas_alert_2")
    elif "garage" in zone:
        s = ha_state("sensor.garage_smoke_lpg")
        alert = ha_state("binary_sensor.garage_smoke_alert")
    else:
        return speak(f"I can check gas for kitchen or garage. Which one?")
    if not s:
        return speak(f"Sorry, gas sensor not available for {zone}.")
    val = s.get("state", "?")
    is_alert = alert and alert.get("state") == "on"
    status = "Warning! Gas detected!" if is_alert else "Air is clean."
    return speak(f"{zone.capitalize()} gas level is {val}. {status}")

def handle_get_garden_soil():
    """Garden soil moisture query."""
    s = ha_state("sensor.garden_soil_moisture")
    if not s:
        return speak("Soil sensor not available.")
    val = s.get("state", "?")
    try:
        v = float(val)
        msg = "Soil is dry, time to water." if v < 30 else "Soil moisture is good."
    except:
        msg = ""
    return speak(f"Garden soil moisture is {val} percent. {msg}")

def handle_what_is_on():
    """List currently-on devices."""
    devices = LIGHT_ENTITIES["all"] + FAN_ENTITIES["all"] + ["switch.garden_water_pump"]
    on_list = []
    for eid in devices:
        s = ha_state(eid)
        if s and s.get("state") == "on":
            friendly = eid.split(".")[1].replace("_", " ")
            on_list.append(friendly)
    if not on_list:
        return speak("Nothing is on right now. All devices are off.")
    return speak(f"Currently on: {', '.join(on_list)}.")

def handle_curtain(slots):
    """Bedroom curtain open/close."""
    action = (slots.get("curtainAction", {}).get("value") or "open").lower()
    if action in ["open", "raise", "up"]:
        ha_service("cover", "open_cover", "cover.bedroom_curtain")
        return speak("Opening bedroom curtain.")
    elif action in ["close", "lower", "down", "shut"]:
        ha_service("cover", "close_cover", "cover.bedroom_curtain")
        return speak("Closing bedroom curtain.")
    return speak("I can open or close the bedroom curtain.")

def handle_color(slots):
    """Change living room light color."""
    color_name = (slots.get("color", {}).get("value") or "white").lower()
    color_map = {
        "red": [255, 0, 0], "orange": [255, 140, 0], "yellow": [255, 220, 0],
        "green": [0, 200, 0], "blue": [80, 180, 255], "purple": [180, 100, 220],
        "pink": [255, 100, 150], "white": [255, 255, 255], "warm white": [255, 200, 140],
    }
    rgb = color_map.get(color_name)
    if not rgb:
        return speak(f"Sorry, I don't know the color {color_name}.")
    ha_service("light", "turn_on", "light.living_room_light",
               {"rgb_color": rgb, "brightness": 255})
    return speak(f"Living room light set to {color_name}.")

def handle_scene(slots):
    """Trigger scene scripts."""
    scene_name = (slots.get("scene", {}).get("value") or "default").lower().replace(" ", "_")
    scene_map = {
        "party": "script.living_room_party_on",
        "sunrise": "script.living_room_sunrise",
        "welcome": "script.living_room_welcome_home",
        "welcome_home": "script.living_room_welcome_home",
        "red_flash": "script.living_room_red_flash",
        "alert": "script.living_room_red_flash",
        "calm": "script.scene_calm",
        "relax": "script.scene_calm",
        "default": "script.scene_default_full",
    }
    script_id = scene_map.get(scene_name)
    if not script_id:
        return speak(f"Sorry, I don't have a {scene_name} scene. Try: party, sunrise, welcome, alert, or calm.")
    ha_service("script", "turn_on", script_id)
    return speak(f"Activating {scene_name} scene.")

# ═══ Main Handler ═════════════════════════════════════════════
def lambda_handler(event, context):
    print(f"[event] {json.dumps(event)[:500]}")
    try:
        req_type = event["request"]["type"]
    except KeyError:
        return speak("Invalid request.")

    if req_type == "LaunchRequest":
        return speak(
            "Welcome to Home Control. "
            "I can control lights, fans, locks, set modes, play music, "
            "or chat in Thai. What would you like?", end=False
        )

    if req_type == "SessionEndedRequest":
        return {}

    if req_type == "IntentRequest":
        name  = event["request"]["intent"]["name"]
        slots = event["request"]["intent"].get("slots", {})
        print(f"[intent] {name}")

        # Fast-path intents (direct HA, low latency)
        handlers = {
            "ControlLightIntent":  lambda: handle_control_light(slots),
            "ControlFanIntent":    lambda: handle_control_fan(slots),
            "ControlPumpIntent":   lambda: handle_control_pump(slots),
            "LockDoorIntent":      lambda: handle_lock_door(slots),
            "SetModeIntent":       lambda: handle_set_mode(slots),
            "GetStatusIntent":     lambda: handle_get_status(),
            "AllLightsOnIntent":   handle_all_lights_on,
            "AllLightsOffIntent":  handle_all_lights_off,
            "PlayMusicIntent":      lambda: handle_play_music(slots),
            "GetTemperatureIntent": lambda: handle_get_temperature(slots),
            "GetHumidityIntent":    lambda: handle_get_humidity(slots),
            "GetGasIntent":         lambda: handle_get_gas(slots),
            "GetSoilIntent":        handle_get_garden_soil,
            "WhatIsOnIntent":       handle_what_is_on,
            "CurtainIntent":        lambda: handle_curtain(slots),
            "GarageDoorIntent":     lambda: handle_garage_door(slots),
            "ColorIntent":          lambda: handle_color(slots),
            "SceneIntent":          lambda: handle_scene(slots),
            # AI brain bridge (slower but smart) — handles all natural language
            "SmartHomeChatIntent":  lambda: handle_smart_home_chat(slots),
        }
        if name in handlers:
            return handlers[name]()

        if name == "AMAZON.HelpIntent":
            return speak(
                "You can say things like: turn on bedroom light, "
                "set mode to away, lock front door, play chill music, "
                "or ask me anything in Thai.", end=False
            )
        if name in ["AMAZON.StopIntent", "AMAZON.CancelIntent"]:
            return speak("Goodbye!", end=True)
        if name == "AMAZON.FallbackIntent":
            # Bridge to AI Agent (Chavee brain with full home context)
            utterance = event["request"].get("intent", {}).get("slots", {}).get("utterance", {}).get("value", "")
            if utterance:
                return handle_smart_home_chat({"utterance": {"value": utterance}})
            # Even with empty utterance — bridge with generic prompt
            return handle_smart_home_chat({"utterance": {"value": "user said something unclear, ask back what they want"}})

        # Final fallback for any unknown intent → bridge to AI Agent
        return handle_smart_home_chat({"utterance": {"value": f"user triggered unknown intent {name}, help them"}})

    return speak("I didn't understand. Try asking me again.", end=False)


# ═══ Response Builder ═════════════════════════════════════════
# end=False by default → keep session alive so user can issue
# multiple commands without re-invoking the skill each time.
def speak(text, end=False, reprompt=None):
    resp = {
        "version": "1.0",
        "response": {
            "outputSpeech": {"type": "PlainText", "text": text or "Done."},
            "shouldEndSession": end,
        },
    }
    if reprompt:
        resp["response"]["reprompt"] = {
            "outputSpeech": {"type": "PlainText", "text": reprompt}
        }
    return resp

#!/usr/bin/env python3
"""
Smart Home AIoT — Test Sheets PDF Generator
Generates clean, professional thesis-grade test sheets (10 trials per test).
"""

from reportlab.lib.pagesizes import A4
from reportlab.lib.units import mm, cm
from reportlab.lib.colors import HexColor, Color, white, black
from reportlab.pdfgen import canvas
from reportlab.pdfbase import pdfmetrics
from reportlab.pdfbase.ttfonts import TTFont
from reportlab.platypus import (
    SimpleDocTemplate, Paragraph, Spacer, Table, TableStyle,
    PageBreak, KeepTogether, Frame, PageTemplate, BaseDocTemplate, NextPageTemplate
)
from reportlab.lib.styles import getSampleStyleSheet, ParagraphStyle
from reportlab.lib.enums import TA_LEFT, TA_CENTER, TA_RIGHT
import os
from datetime import datetime

# ─── Theme (clean professional) ──────────────────────────────────────
COLOR_HEADER = HexColor('#1e3a5f')      # deep blue
COLOR_ACCENT = HexColor('#3b82f6')      # bright blue
COLOR_LIGHT = HexColor('#f1f5f9')       # light gray
COLOR_BORDER = HexColor('#cbd5e1')      # mid gray
COLOR_TEXT = HexColor('#0f172a')        # almost black
COLOR_MUTED = HexColor('#64748b')       # muted gray
COLOR_PASS = HexColor('#ecfdf5')        # light green tint
COLOR_FAIL = HexColor('#fef2f2')        # light red tint

OUTPUT_DIR = '/Users/path_mos/Desktop/Mos/Project_1/test_sheets'

# ─── Register Thai-capable font ─────────────────────────────────────
THAI_FONT = 'Helvetica'  # default
THAI_FONT_BOLD = 'Helvetica-Bold'

font_paths = [
    '/System/Library/Fonts/Supplemental/Ayuthaya.ttf',
    '/System/Library/Fonts/Thonburi.ttc',
    '/Library/Fonts/Arial Unicode.ttf',
    '/System/Library/Fonts/Helvetica.ttc',
]
for fp in font_paths:
    if os.path.exists(fp):
        try:
            pdfmetrics.registerFont(TTFont('ThaiFont', fp))
            THAI_FONT = 'ThaiFont'
            THAI_FONT_BOLD = 'ThaiFont'
            print(f'Loaded Thai font: {fp}')
            break
        except Exception as e:
            continue

# ─── Styles ─────────────────────────────────────────────────────────
styles = getSampleStyleSheet()
style_title = ParagraphStyle(
    'Title', parent=styles['Heading1'],
    fontName=THAI_FONT_BOLD, fontSize=14, textColor=COLOR_HEADER,
    spaceAfter=4, leading=18
)
style_subtitle = ParagraphStyle(
    'Subtitle', parent=styles['Normal'],
    fontName=THAI_FONT, fontSize=10, textColor=COLOR_MUTED,
    spaceAfter=8, leading=13
)
style_label = ParagraphStyle(
    'Label', parent=styles['Normal'],
    fontName=THAI_FONT_BOLD, fontSize=9, textColor=COLOR_HEADER,
    leading=11
)
style_body = ParagraphStyle(
    'Body', parent=styles['Normal'],
    fontName=THAI_FONT, fontSize=9, textColor=COLOR_TEXT,
    leading=12
)
style_small = ParagraphStyle(
    'Small', parent=styles['Normal'],
    fontName=THAI_FONT, fontSize=8, textColor=COLOR_MUTED,
    leading=10
)
style_footer = ParagraphStyle(
    'Footer', parent=styles['Normal'],
    fontName=THAI_FONT, fontSize=8, textColor=COLOR_MUTED,
    alignment=TA_CENTER, leading=10
)

# ─── 130 TEST DEFINITIONS ──────────────────────────────────────────
# Format: (phase, id, name, method, expected)

TESTS = [
    # ─── Phase A: Single Device Verification (16) ───
    ('A', 'A01', 'Bedroom Light', 'HA → toggle switch.bedroom_light', 'ไฟห้องนอนติด/ดับ + state เปลี่ยน'),
    ('A', 'A02', 'Living Room Light (Tuya RGB)', 'HA → toggle light.living_room_light', 'Tuya bulb ON ที่ color default'),
    ('A', 'A03', 'Kitchen Light', 'HA → toggle switch.kitchen_light', 'ไฟครัวติด/ดับ'),
    ('A', 'A04', 'Bathroom Light', 'HA → toggle switch.bathroom_light', 'ไฟห้องน้ำติด/ดับ'),
    ('A', 'A05', 'Garden Light', 'HA → toggle switch.garden_light', 'ไฟสวนหลักติด/ดับ'),
    ('A', 'A06', 'Garage Light', 'HA → toggle switch.garage_light', 'ไฟโรงรถหลักติด/ดับ'),
    ('A', 'A07', 'Garage Pathway Light', 'HA → toggle switch.garage_pathway_light', 'ไฟทางเดินโรงรถ'),
    ('A', 'A08', 'Garden Pathway Light', 'HA → toggle switch.garden_pathway_light', 'ไฟทางเดินสวน'),
    ('A', 'A09', 'Bedroom Fan', 'HA → toggle switch.bedroom_fan', 'พัดลมห้องนอนหมุน'),
    ('A', 'A10', 'Living Room Fan (Tuya plug)', 'HA → toggle switch.living_room_fan', 'พัดลมห้องนั่งเล่นหมุน'),
    ('A', 'A11', 'Kitchen Vent Fan', 'HA → toggle switch.kitchen_fan', 'vent fan ครัวทำงาน'),
    ('A', 'A12', 'Bedroom Curtain — Open', 'HA → cover.bedroom_curtain open', 'ม่านพับเก็บขึ้น (หน้าต่างโล่ง)'),
    ('A', 'A13', 'Bedroom Curtain — Close', 'HA → cover.bedroom_curtain close', 'ม่านลงคลุมหน้าต่าง'),
    ('A', 'A14', 'Front Door Lock', 'HA → lock.front_door_lock', 'servo หมุน 0↔90°'),
    ('A', 'A15', 'Garage Door', 'HA → cover.garage_door open/close', 'servo หมุนเปิด/ปิด'),
    ('A', 'A16', 'Garden Water Pump', 'HA → toggle switch.garden_water_pump', 'ปั๊มทำงาน + น้ำไหล'),

    # ─── Phase B: Sensor Reading (18) ───
    ('B', 'B01', 'Bedroom Temperature + Humidity (DHT22)', 'ดู HA Dashboard sensor.bedroom_temperature, humidity', '20-35°C, 40-80% (sanity range)'),
    ('B', 'B02', 'Living Room Temperature + Humidity', 'ดู HA sensor.living_room_temperature, humidity', 'same range'),
    ('B', 'B03', 'Kitchen Temperature + Humidity', 'ดู HA sensor.kitchen_temperature, humidity', 'same range'),
    ('B', 'B04', 'Bedroom LDR', 'ดู sensor.bedroom_ldr + เอามือบังเซ็นเซอร์ → ค่าเพิ่ม', '0-4095 range (high=มืด)'),
    ('B', 'B05', 'Living Room LDR', 'sanity test', '0-4095'),
    ('B', 'B06', 'Kitchen LDR', 'sanity test', '0-4095'),
    ('B', 'B07', 'Bathroom LDR', 'sanity test', '0-4095'),
    ('B', 'B08', 'Garden LDR', 'sanity test', '0-4095'),
    ('B', 'B09', 'Garage LDR', 'sanity test', '0-4095'),
    ('B', 'B10', 'Kitchen Gas (MQ-2)', 'จุดไฟแช็คใกล้ → sensor.kitchen_gas_lpg ค่าขึ้น', 'ADC ขึ้นจาก baseline'),
    ('B', 'B11', 'Garage Smoke (MQ-2)', 'จุดไฟแช็ค/สเปรย์ → sensor.garage_smoke_lpg ค่าขึ้น', 'ADC ขึ้นจาก baseline'),
    ('B', 'B12', 'Garden Soil Moisture', 'แตะดินเปียก vs แห้ง → % เปลี่ยน', '0-100% range'),
    ('B', 'B13', 'Bathroom Water Leak (SNZB-05P)', 'แตะน้ำที่ขาทองแดง → binary_sensor.bathroom_water_leak ON', 'binary state ON'),
    ('B', 'B14', 'Front Door Contact (SNZB-04)', 'เปิด/ปิดประตู → binary_sensor.front_door ON/OFF', 'binary state เปลี่ยน'),
    ('B', 'B15', 'Bedroom Motion (PIR)', 'เดินผ่าน → binary_sensor.bedroom_motion ON', 'ON 30s window'),
    ('B', 'B16', 'Living Room Motion (Zigbee PIR)', 'เดินผ่าน → binary_sensor.living_room_motion ON', 'ON'),
    ('B', 'B17', 'Bathroom Presence (LD2410)', 'เดินเข้า → binary_sensor.bathroom_presence ON', 'ON'),
    ('B', 'B18', 'Garage Motion (PIR)', 'เดินผ่าน → binary_sensor.garage_motion ON', 'ON 30s timeout'),

    # ─── Phase C: Mode Change & Cascading (10) ───
    ('C', 'C01', 'House Mode → Home', 'Set input_select.house_mode = Home', 'Mode Scene fires + adaptive ON + GPS welcome enabled'),
    ('C', 'C02', 'House Mode → Away', 'Set input_select.house_mode = Away', 'ทุก switch OFF + lock + curtain close + LINE notify'),
    ('C', 'C03', 'House Mode → Security', 'Set input_select.house_mode = Security', 'ปิดทุก + monitor active (door open = intrusion)'),
    ('C', 'C04', 'Comfort Mode → Normal', 'Set input_select.comfort_mode = Normal', 'adaptive light ON, sensors active'),
    ('C', 'C05', 'Comfort Mode → Sleep', 'Set comfort_mode = Sleep', 'ปิดไฟทุกห้อง + curtain close + path-light enabled'),
    ('C', 'C06', 'Comfort Mode → Relax', 'Set comfort_mode = Relax', 'Tuya warm 60% + auto-fan if >28°C'),
    ('C', 'C07', 'Comfort Mode → Warm', 'Set comfort_mode = Warm', 'Tuya 2700K + ปิดพัดลม'),
    ('C', 'C08', 'Cascade: Home → Away → Home', 'เปลี่ยน mode รัวๆ 3 ครั้ง', 'All devices end at correct state'),
    ('C', 'C09', 'Concurrent: Home + Sleep', 'Set both', 'Path-light ON, adaptive OFF'),
    ('C', 'C10', 'Cascade: Sleep → Normal', 'Change Sleep → Normal', 'Auto-fan stops, adaptive resumes'),

    # ─── Phase D: Safety Auto-Flows Simulation (12) ───
    ('D', 'D01', 'Gas Alert — Stove OFF', 'mosquitto_pub home/kitchen/gas_alert ON', 'switch.kitchen_stove → OFF (double-block)'),
    ('D', 'D02', 'Gas Alert — Vent Fan ON', 'gas_alert trigger', 'switch.kitchen_fan → ON'),
    ('D', 'D03', 'Gas Alert — Red Flash', 'gas_alert trigger', 'script.living_room_red_flash fired (Tuya แดงพริบ)'),
    ('D', 'D04', 'Gas Alert — LINE Message', 'gas_alert trigger', 'LINE message "🔥 ก๊าซรั่ว!" arrives'),
    ('D', 'D05', 'Water Leak Detection', 'แตะน้ำที่ probe / force binary_sensor.bathroom_water_leak = on', 'blue pulse + LINE "💧 น้ำรั่ว!"'),
    ('D', 'D06', 'Bathroom Fall — Scene Snapshot', 'force binary_sensor.bathroom_fall_alert = on', 'scene.livingroom_pre_fall_alert saved'),
    ('D', 'D07', 'Bathroom Fall — Red Flash 30s', 'fall_alert trigger', 'ไฟแดงพริบ 30 วินาที (10 รอบ)'),
    ('D', 'D08', 'Bathroom Fall — LINE + Restore', 'fall_alert trigger', 'LINE "🚨 คนล้ม!" + scene restore กลับ'),
    ('D', 'D09', 'Garage Smoke Alert', 'mosquitto_pub home/garage/smoke_alert ON', 'Red flash + LINE "🚨 ควันโรงรถ!"'),
    ('D', 'D10', 'Front Door Intrusion (Security)', 'Set Security + force door open', 'Auto-lock + Red flash + LINE "🚨 มีคนบุก!"'),
    ('D', 'D11', 'Soil Dry — Auto Pump', 'mosquitto_pub home/garden/soil_moisture 25', 'switch.garden_water_pump ON + LINE "🌱 ดินแห้ง"'),
    ('D', 'D12', 'Stove Safety (10-min warning, 15-min Away auto-off)', 'stove ON 10/15 min + Away', 'LINE warning + auto-OFF'),

    # ─── Phase E: Adaptive Lighting (8) ───
    ('E', 'E01', 'Bedroom Adaptive (PIR + LDR)', 'Setup LDR > 2200 + walk PIR', 'switch.bedroom_light ON'),
    ('E', 'E02', 'Living Room Adaptive (Zigbee PIR + LDR + brightness scale)', 'LDR > 2200 + motion', 'Tuya bulb ON brightness scaled by LDR (30-100%)'),
    ('E', 'E03', 'Bathroom Adaptive (LD2410 + LDR)', 'LDR > 2000 + presence', 'switch.bathroom_light ON'),
    ('E', 'E04', 'Garage Adaptive (PIR + LDR)', 'LDR > 2400 + motion', 'garage_light + pathway_light ON'),
    ('E', 'E05', 'Adaptive NO-trigger (สว่างพอ)', 'LDR < 2200 + motion', 'light ยัง OFF (no trigger)'),
    ('E', 'E06', 'Bedroom Auto-off (5 min)', 'turn on light + no motion 5 min', 'auto OFF'),
    ('E', 'E07', 'Garage Auto-off (5 min)', 'turn on lights + no motion 5 min', 'both garage lights OFF'),
    ('E', 'E08', 'Bathroom Auto-off (5 min)', 'light on + no presence 5 min', 'bath light OFF'),

    # ─── Phase F: GPS Welcome/Goodbye (8) ───
    ('F', 'F01', 'GPS Welcome — Garage Door Open', 'Mock person → home', 'cover.garage_door opening'),
    ('F', 'F02', 'GPS Welcome — Garage Lights (LDR-aware)', 'garden_ldr > 2400 + welcome', 'garage_light + pathway ON'),
    ('F', 'F03', 'GPS Welcome — Garden Lights', 'garden_ldr > 2200 + welcome', 'garden_light + pathway ON'),
    ('F', 'F04', 'GPS Welcome — Switch Away → Home', 'Set Away + GPS welcome', 'house_mode → Home auto'),
    ('F', 'F05', 'GPS Welcome — LINE Notify', 'GPS welcome', 'LINE "ยินดีต้อนรับ"'),
    ('F', 'F06', 'GPS Goodbye — 2-min grace', 'person → not_home + wait 2 min', 'house_mode → Away'),
    ('F', 'F07', 'GPS Goodbye — Cascade', 'Goodbye trigger', 'switches OFF + lock + curtain close'),
    ('F', 'F08', 'GPS Goodbye — LINE Notify', 'Goodbye trigger', 'LINE goodbye message (random variant)'),

    # ─── Phase G: Schedule Automations (7) ───
    ('G', 'G01', 'Daily Watering 07:00 (10s)', 'wait or force time = 07:00', 'pump turn_on → 10s → turn_off'),
    ('G', 'G02', 'Lights OFF 23:00', 'wait or force time = 23:00', 'all lights OFF'),
    ('G', 'G03', 'Daily Sunrise 06:00', 'wait or force time = 06:00', 'Tuya gradual brighten'),
    ('G', 'G04', 'Garden Light ON 18:00', 'wait or force time = 18:00', 'switch.garden_light ON'),
    ('G', 'G05', 'Garden Light OFF 23:00', 'wait or force time = 23:00', 'switch.garden_light OFF'),
    ('G', 'G06', 'Sunset Curtain Close', 'force sun.sun → below_horizon (Home mode)', 'bedroom_curtain close'),
    ('G', 'G07', 'Sunrise+30min Curtain Open', 'force sun event + 30 min (not Sleep)', 'bedroom_curtain open'),

    # ─── Phase H1: LINE Bot Thai NLU (10) ───
    ('H', 'H01', 'LINE: "เปิดไฟห้องนอน"', 'ส่ง LINE', 'switch.bedroom_light ON + Thai reply'),
    ('H', 'H02', 'LINE: "ปิดทุกอย่าง"', 'ส่ง LINE', 'all switches OFF'),
    ('H', 'H03', 'LINE: "นอนแล้ว"', 'ส่ง LINE', 'Sleep mode + cascade'),
    ('H', 'H04', 'LINE: "ออกจากบ้าน"', 'ส่ง LINE', 'Away mode + cascade'),
    ('H', 'H05', 'LINE: "อุณหภูมิครัว"', 'ส่ง LINE', 'sensor reply with °C value'),
    ('H', 'H06', 'LINE: "เปลี่ยนสีไฟเป็นแดง"', 'ส่ง LINE', 'Tuya bulb red'),
    ('H', 'H07', 'LINE: "ปาร์ตี้!"', 'ส่ง LINE', 'Party scene (colorloop) fires'),
    ('H', 'H08', 'LINE: "มีคนบุก"', 'ส่ง LINE', 'Security mode + intrusion response'),
    ('H', 'H09', 'LINE: "แก๊สรั่ว"', 'ส่ง LINE', 'Gas alert simulated response'),
    ('H', 'H10', 'LINE Multi-action: "เปิดไฟห้องนอน + ปิดไฟห้องครัว"', 'ส่ง LINE', 'both actions fire'),

    # ─── Phase H2: Alexa English NLU (10) ───
    ('H', 'H11', 'Alexa: "turn on bedroom light"', 'พูด Alexa', 'switch.bedroom_light ON + TTS confirm'),
    ('H', 'H12', 'Alexa: "kitchen fan on"', 'พูด Alexa', 'fan ON'),
    ('H', 'H13', 'Alexa: "open garage door"', 'พูด Alexa', 'cover.garage_door open'),
    ('H', 'H14', 'Alexa: "set mode to away"', 'พูด Alexa', 'Away cascade'),
    ('H', 'H15', 'Alexa: "what is the bedroom temperature"', 'พูด Alexa', 'TTS reply with °C'),
    ('H', 'H16', 'Alexa: "activate party scene"', 'พูด Alexa', 'Tuya colorloop'),
    ('H', 'H17', 'Alexa: "lock the front door"', 'พูด Alexa', 'lock + servo หมุน'),
    ('H', 'H18', 'Alexa: "tell home control I am going to sleep"', 'พูด Alexa', 'Sleep mode via AI Agent'),
    ('H', 'H19', 'Alexa: "tell home control what model is the living room bulb"', 'พูด Alexa', 'TTS "Tuya TS0505B"'),
    ('H', 'H20', 'Alexa Thai bridge: "tell home control เปิดโหมดปาร์ตี้"', 'พูด Alexa', 'Party scene via AI Agent (Thai)'),

    # ─── Phase H3: AI Deep Reasoning v4 (19) ───
    ('H', 'H21', 'LINE Reasoning: "ทำอาหารปลอดภัยมั้ย"', 'ส่ง LINE', 'เช็ค gas + เตา + suggest ไฟครัว'),
    ('H', 'H22', 'LINE Reasoning: "หิวแต่กลัวก๊าซรั่ว"', 'ส่ง LINE', 'scan gas → ยืนยันปลอดภัย'),
    ('H', 'H23', 'LINE Reasoning: "บ้านดูยุ่งเหยิงจัง"', 'ส่ง LINE', 'list device ON → propose ปิด'),
    ('H', 'H24', 'LINE Reasoning: "ออกจากบ้าน ลืมอะไรมั้ย"', 'ส่ง LINE', 'checklist เตรียม Away'),
    ('H', 'H25', 'LINE Reasoning: "พรุ่งนี้ตื่นเช้า"', 'ส่ง LINE', 'Sleep mode + suggest Sunrise scene'),
    ('H', 'H26', 'LINE Reasoning: "หนาว"', 'ส่ง LINE', 'อ่าน temp + ปิดพัดลม + Tuya warm'),
    ('H', 'H27', 'LINE Reasoning: "มีคนกลับบ้านมั้ย"', 'ส่ง LINE', 'check device_tracker + GPS + door history'),
    ('H', 'H28', 'LINE Reasoning: "ดึกแล้ว" (ตอน 23:00+)', 'ส่ง LINE', 'time-aware Sleep mode suggestion'),
    ('H', 'H29', 'Alexa Reasoning: "is it safe to cook"', 'พูด Alexa', 'gas check + stove + suggest light'),
    ('H', 'H30', 'Alexa Reasoning: "I am cold"', 'พูด Alexa', 'bedroom temp + adjust'),
    ('H', 'H31', 'Alexa Reasoning: "turn off what I do not need"', 'พูด Alexa', 'smart triage'),
    ('H', 'H32', 'Alexa Reasoning: "what is wrong" (after alert)', 'พูด Alexa', 'multi-sensor scan'),
    ('H', 'H33', 'Alexa Reasoning: "help me sleep"', 'พูด Alexa', 'multi-action sleep prep'),
    ('H', 'H34', 'Alexa Reasoning: "good morning"', 'พูด Alexa', 'weather + temp + Sunrise scene'),
    ('H', 'H35', 'Alexa Reasoning: "did I forget anything"', 'พูด Alexa', 'pre-Away checklist'),
    ('H', 'H36', 'Alexa Reasoning: "what should I do now"', 'พูด Alexa', 'time + state inference'),
    ('H', 'H37', 'Multi-step LINE: "หิว" → reply → "ตกลง"', 'ส่ง 2 turns', 'follow-through action'),
    ('H', 'H38', 'Multi-step LINE: "ฉันจะทำอาหาร" → "เสร็จแล้ว"', 'ส่ง 2 turns', 'mode/scene back to normal'),
    ('H', 'H39', 'Compound Alexa: "I am leaving home but feed the dog first"', 'พูด Alexa', 'break down: feed + Away'),

    # ─── Phase I: Integration Tests (12) ───
    ('I', 'I01', 'HA Dashboard Latency', 'click switch on Lovelace, measure ms', '< 500 ms'),
    ('I', 'I02', 'Alexa Latency (same command)', 'voice + measure', '1-3 seconds'),
    ('I', 'I03', 'LINE Latency (same command)', 'LINE + measure', '2-5 seconds (Claude API)'),
    ('I', 'I04', 'Sleep mode + adaptive disabled', 'Sleep + walk PIR', 'ไฟไม่เปิด (adaptive OFF)'),
    ('I', 'I05', 'Sleep mode + path light fires', 'Sleep + PIR bedroom', 'Tuya dim 10% warm'),
    ('I', 'I06', 'Sleep path light auto-off 2 min', 'after path light', 'auto OFF after 2 min'),
    ('I', 'I07', 'Cascade conflict — rapid mode changes', 'Home→Sleep→Home→Sleep→Away (5s)', 'final state correct, no stuck'),
    ('I', 'I08', 'Away during adaptive', 'Set Away while adaptive light ON', 'ไฟปิด, lock ทำงาน'),
    ('I', 'I09', 'Sleep during Relax auto-fan', 'Set Sleep while Relax fan ON', 'fan OFF, curtain close'),
    ('I', 'I10', 'ESP32 reboot recovery', 'unplug ESP32 #1 10s + plug', 'reconnect MQTT + retained state restore'),
    ('I', 'I11', 'HA restart recovery', 'restart HA container', 'all entities back online'),
    ('I', 'I12', 'WiFi disconnect/reconnect', 'disconnect 30s + reconnect', 'webhook auto-heal'),
]

PHASE_NAMES = {
    'A': 'Phase A — Single Device Verification',
    'B': 'Phase B — Sensor Reading',
    'C': 'Phase C — Mode Change & Cascading',
    'D': 'Phase D — Safety Auto-Flows Simulation',
    'E': 'Phase E — Adaptive Lighting',
    'F': 'Phase F — GPS Welcome/Goodbye',
    'G': 'Phase G — Schedule Automations',
    'H': 'Phase H — AI Agent (LINE Thai + Alexa EN + Deep Reasoning)',
    'I': 'Phase I — Integration Tests',
}

# ─── Build a single test page ─────────────────────────────────────
def build_test_block(phase, tid, name, method, expected):
    elems = []

    # Title row
    title_data = [[
        Paragraph(f'<b>{tid}</b>', style_label),
        Paragraph(f'<b>{name}</b>', style_label),
        Paragraph(f'Phase {phase}', style_small),
    ]]
    title_tbl = Table(title_data, colWidths=[20*mm, 130*mm, 30*mm])
    title_tbl.setStyle(TableStyle([
        ('BACKGROUND', (0,0), (-1,-1), COLOR_LIGHT),
        ('VALIGN', (0,0), (-1,-1), 'MIDDLE'),
        ('TOPPADDING', (0,0), (-1,-1), 4),
        ('BOTTOMPADDING', (0,0), (-1,-1), 4),
        ('LEFTPADDING', (0,0), (-1,-1), 6),
        ('RIGHTPADDING', (0,0), (-1,-1), 6),
        ('LINEBELOW', (0,0), (-1,-1), 1, COLOR_HEADER),
    ]))
    elems.append(title_tbl)

    # Method + Expected
    info_data = [
        [Paragraph('<b>Method:</b>', style_label), Paragraph(method, style_body)],
        [Paragraph('<b>Expected:</b>', style_label), Paragraph(expected, style_body)],
    ]
    info_tbl = Table(info_data, colWidths=[22*mm, 158*mm])
    info_tbl.setStyle(TableStyle([
        ('VALIGN', (0,0), (-1,-1), 'TOP'),
        ('TOPPADDING', (0,0), (-1,-1), 3),
        ('BOTTOMPADDING', (0,0), (-1,-1), 3),
        ('LEFTPADDING', (0,0), (-1,-1), 6),
    ]))
    elems.append(info_tbl)
    elems.append(Spacer(1, 4))

    # Trial table — 10 rows
    headers = ['#', 'Date', 'Time', 'Input/Command', 'Actual Output', 'RT (ms)', 'P/F', 'Notes']
    rows = [headers] + [[str(i+1)] + ['']*7 for i in range(10)]

    col_widths = [8*mm, 18*mm, 16*mm, 38*mm, 50*mm, 14*mm, 10*mm, 26*mm]
    trial_tbl = Table(rows, colWidths=col_widths, rowHeights=[8*mm] + [10*mm]*10)
    trial_tbl.setStyle(TableStyle([
        # Header
        ('BACKGROUND', (0,0), (-1,0), COLOR_HEADER),
        ('TEXTCOLOR', (0,0), (-1,0), white),
        ('FONTNAME', (0,0), (-1,0), THAI_FONT_BOLD),
        ('FONTSIZE', (0,0), (-1,0), 8),
        ('ALIGN', (0,0), (-1,0), 'CENTER'),
        ('VALIGN', (0,0), (-1,0), 'MIDDLE'),
        # Body
        ('FONTNAME', (0,1), (-1,-1), THAI_FONT),
        ('FONTSIZE', (0,1), (-1,-1), 8),
        ('VALIGN', (0,1), (-1,-1), 'MIDDLE'),
        ('ALIGN', (0,1), (0,-1), 'CENTER'),
        ('ALIGN', (5,1), (6,-1), 'CENTER'),
        # Alternating row colors
        ('ROWBACKGROUNDS', (0,1), (-1,-1), [white, HexColor('#f8fafc')]),
        # Borders
        ('GRID', (0,0), (-1,-1), 0.5, COLOR_BORDER),
        ('BOX', (0,0), (-1,-1), 1, COLOR_HEADER),
        # Padding
        ('TOPPADDING', (0,0), (-1,-1), 2),
        ('BOTTOMPADDING', (0,0), (-1,-1), 2),
    ]))
    elems.append(trial_tbl)
    elems.append(Spacer(1, 4))

    # Summary row
    summary_data = [
        [
            Paragraph('<b>Pass Count:</b> ___/10', style_body),
            Paragraph('<b>Pass Rate:</b> ____%', style_body),
            Paragraph('<b>Avg RT:</b> ____ ms', style_body),
            Paragraph('<b>Std Dev:</b> ____ ms', style_body),
        ],
        [
            Paragraph('<b>Comments:</b>', style_body),
            '', '', '',
        ]
    ]
    summary_tbl = Table(summary_data, colWidths=[45*mm, 45*mm, 45*mm, 45*mm])
    summary_tbl.setStyle(TableStyle([
        ('SPAN', (0,1), (-1,1)),
        ('BACKGROUND', (0,0), (-1,0), COLOR_LIGHT),
        ('VALIGN', (0,0), (-1,-1), 'MIDDLE'),
        ('TOPPADDING', (0,0), (-1,-1), 4),
        ('BOTTOMPADDING', (0,1), (-1,1), 18),
        ('LEFTPADDING', (0,0), (-1,-1), 6),
        ('GRID', (0,0), (-1,-1), 0.5, COLOR_BORDER),
    ]))
    elems.append(summary_tbl)

    return elems

# ─── Build full PDF ────────────────────────────────────────────────
def build_pdf(filename, tests_subset, title_suffix=''):
    """Build a PDF document with the given test list."""
    full_title = f'Smart Home AIoT — Test Data Collection Sheet{title_suffix}'

    doc = SimpleDocTemplate(
        filename, pagesize=A4,
        leftMargin=12*mm, rightMargin=12*mm,
        topMargin=14*mm, bottomMargin=14*mm,
        title=full_title,
        author='Athit Boonpinit',
    )

    story = []

    # Cover page
    story.append(Paragraph(full_title, style_title))
    story.append(Paragraph('Senior Project CE08 — Bangkok University', style_subtitle))
    story.append(Paragraph(f'Generated: {datetime.now().strftime("%Y-%m-%d %H:%M")}', style_subtitle))
    story.append(Spacer(1, 6*mm))

    # Instructions
    instructions = """
    <b>Instructions for use:</b><br/>
    1. Run each test 10 times consecutively (or across sessions, as practical)<br/>
    2. Record: Date, Time, exact Input given, Actual Output observed, Response Time (use stopwatch), and Pass/Fail<br/>
    3. Pass criteria: Actual matches Expected description<br/>
    4. After 10 trials, calculate: Pass Rate %, Average Response Time, Standard Deviation<br/>
    5. Use Comments for any anomalies or special observations<br/>
    <br/>
    <b>Statistical formulas:</b><br/>
    • Pass Rate = (Pass Count / 10) × 100%<br/>
    • Avg RT = Σ(response times) / 10<br/>
    • Variance = Σ(xi - μ)² / n<br/>
    • Std Dev = √variance<br/>
    """
    story.append(Paragraph(instructions, style_body))
    story.append(Spacer(1, 8*mm))

    # TOC summary
    phases_in_subset = {}
    for t in tests_subset:
        ph = t[0]
        phases_in_subset.setdefault(ph, []).append(t)

    toc_data = [['Phase', 'Description', 'Count']]
    for ph, items in sorted(phases_in_subset.items()):
        toc_data.append([f'Phase {ph}', PHASE_NAMES.get(ph, ''), str(len(items))])
    toc_data.append(['', 'TOTAL', str(len(tests_subset))])

    toc_tbl = Table(toc_data, colWidths=[25*mm, 130*mm, 25*mm])
    toc_tbl.setStyle(TableStyle([
        ('BACKGROUND', (0,0), (-1,0), COLOR_HEADER),
        ('TEXTCOLOR', (0,0), (-1,0), white),
        ('FONTNAME', (0,0), (-1,0), THAI_FONT_BOLD),
        ('FONTSIZE', (0,0), (-1,0), 9),
        ('ALIGN', (0,0), (0,-1), 'CENTER'),
        ('ALIGN', (2,0), (2,-1), 'CENTER'),
        ('VALIGN', (0,0), (-1,-1), 'MIDDLE'),
        ('FONTNAME', (0,1), (-1,-1), THAI_FONT),
        ('FONTSIZE', (0,1), (-1,-1), 8),
        ('GRID', (0,0), (-1,-1), 0.5, COLOR_BORDER),
        ('BOX', (0,0), (-1,-1), 1, COLOR_HEADER),
        ('ROWBACKGROUNDS', (0,1), (-1,-1), [white, HexColor('#f8fafc')]),
        ('BACKGROUND', (0,-1), (-1,-1), COLOR_LIGHT),
        ('FONTNAME', (0,-1), (-1,-1), THAI_FONT_BOLD),
        ('TOPPADDING', (0,0), (-1,-1), 4),
        ('BOTTOMPADDING', (0,0), (-1,-1), 4),
        ('LEFTPADDING', (0,0), (-1,-1), 8),
    ]))
    story.append(Paragraph('<b>Coverage Matrix</b>', style_label))
    story.append(Spacer(1, 2*mm))
    story.append(toc_tbl)

    story.append(PageBreak())

    # Test pages (1 per page, grouped by phase)
    current_phase = None
    for i, (phase, tid, name, method, expected) in enumerate(tests_subset):
        if phase != current_phase:
            # Phase divider
            story.append(Paragraph(PHASE_NAMES.get(phase, f'Phase {phase}'), style_title))
            story.append(Spacer(1, 3*mm))
            current_phase = phase

        block = build_test_block(phase, tid, name, method, expected)
        story.extend(block)

        if i < len(tests_subset) - 1:
            story.append(PageBreak())

    # Build with footer
    def add_footer(canv, doc):
        canv.saveState()
        canv.setFont(THAI_FONT, 8)
        canv.setFillColor(COLOR_MUTED)
        canv.drawString(12*mm, 8*mm, 'Smart Home AIoT — Senior Project CE08')
        canv.drawRightString(198*mm, 8*mm, f'Page {doc.page}')
        canv.line(12*mm, 11*mm, 198*mm, 11*mm)
        canv.restoreState()

    doc.build(story, onFirstPage=add_footer, onLaterPages=add_footer)
    print(f'✓ Generated: {filename} ({len(tests_subset)} tests)')


# ─── Main: generate per-phase + master ─────────────────────────────
if __name__ == '__main__':
    os.makedirs(OUTPUT_DIR, exist_ok=True)

    # 1. One master PDF with all 130 tests
    build_pdf(
        os.path.join(OUTPUT_DIR, '00_ALL_130_tests.pdf'),
        TESTS,
        title_suffix=' — All 130 Tests'
    )

    # 2. Per-phase PDFs
    phase_groups = {}
    for t in TESTS:
        phase_groups.setdefault(t[0], []).append(t)

    for ph in sorted(phase_groups.keys()):
        items = phase_groups[ph]
        safe_name = PHASE_NAMES[ph].replace(' ', '_').replace('—', '-').replace(':', '').replace('(', '').replace(')', '').replace('/', '_')
        filename = os.path.join(OUTPUT_DIR, f'{ph}_{safe_name}.pdf')
        build_pdf(filename, items, title_suffix=f' — {PHASE_NAMES[ph]}')

    print(f'\nAll PDFs saved to: {OUTPUT_DIR}')
    print(f'Total tests: {len(TESTS)}')

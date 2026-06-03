# 🎤 Alexa Echo Dot — Defense Day Command List
**Skill:** "Home Control" | **Invocation:** "home control"

> ทุกคำสั่งเริ่มด้วย: **"Alexa, ask home control to..."** หรือ **"Alexa, tell home control to..."**

---

## 💡 ห้องนั่งเล่น (Living Room — Tuya RGB+CCT)

### เปิด/ปิด
- "Alexa, ask home control to turn on living room light"
- "Alexa, ask home control to turn off living room light"
- "Alexa, ask home control to turn on the light"

### เปลี่ยนสี (16.7M colors)
- "Alexa, ask home control to set living room to **red**"
- "Alexa, ask home control to set living room to **blue**"
- "Alexa, ask home control to set living room to **green**"
- "Alexa, ask home control to set living room to **pink**"
- "Alexa, ask home control to set living room to **purple**"
- "Alexa, ask home control to set living room to **yellow**"
- "Alexa, ask home control to set living room to **orange**"
- "Alexa, ask home control to set living room to **cyan**"
- "Alexa, ask home control to set living room to **white**"
- "Alexa, ask home control to set living room to **warm white**"

### ความสว่าง (brightness 1-100%)
- "Alexa, ask home control to set living room brightness to 80 percent"
- "Alexa, ask home control to dim living room to 30 percent"
- "Alexa, ask home control to brighten living room"

### 🎬 Scenes (สลับ smooth ระหว่าง scene)
- "Alexa, ask home control to **turn on welcome scene**" → ส้มอุ่น 95%
- "Alexa, ask home control to **turn on sunrise**" → แดดเช้า 70%
- "Alexa, ask home control to **turn on calm mode**" → ฟ้านุ่ม 40%
- "Alexa, ask home control to **turn on party mode**" → loop 5 colors
- "Alexa, ask home control to **turn on alert**" → blink red
- "Alexa, ask home control to **turn on bedtime**" → red dim 15%
- "Alexa, ask home control to **stop all**" → STOP ALL scenes + lights off

---

## 🛏️ ห้องนอน (Bedroom)

### ไฟ
- "Alexa, ask home control to turn on bedroom light"
- "Alexa, ask home control to turn off bedroom light"

### พัดลม
- "Alexa, ask home control to turn on bedroom fan"
- "Alexa, ask home control to turn off bedroom fan"

### ม่าน (curtain SG90 servo)
- "Alexa, ask home control to **open bedroom curtain**"
- "Alexa, ask home control to **close bedroom curtain**"

---

## 🍳 ห้องครัว (Kitchen)

### ไฟ
- "Alexa, ask home control to turn on kitchen light"
- "Alexa, ask home control to turn off kitchen light"

### พัดลมระบาย
- "Alexa, ask home control to turn on kitchen fan"
- "Alexa, ask home control to turn off vent fan"

### เตาแก๊ส (simulator — block ถ้า gas alert)
- "Alexa, ask home control to turn on the stove"
- "Alexa, ask home control to turn off the stove"

### ตรวจสอบแก๊ส
- "Alexa, ask home control if it's safe to cook" → AI ตอบรวมข้อมูล gas
- "Alexa, ask home control what's the gas level in the kitchen"
- "Alexa, ask home control to check gas"

---

## 🚿 ห้องน้ำ (Bathroom)

### ไฟ
- "Alexa, ask home control to turn on bathroom light"
- "Alexa, ask home control to turn off bathroom light"

### ตรวจน้ำรั่ว
- "Alexa, ask home control if there's a water leak"
- "Alexa, ask home control to check bathroom"

---

## 🌱 สวน (Garden)

### ปั๊มน้ำ
- "Alexa, ask home control to turn on the water pump"
- "Alexa, ask home control to turn off the water pump"
- "Alexa, ask home control to water the plants"
- "Alexa, ask home control when does the pump turn on" → "<45% soil"

### ความชื้นดิน
- "Alexa, ask home control what's the soil moisture"
- "Alexa, ask home control if plants need water"

### ไฟสวน
- "Alexa, ask home control to turn on garden lights"
- "Alexa, ask home control to turn off garden lights"

---

## 🚗 โรงรถ (Garage)

### ประตูโรงรถ (SG90 servo)
- "Alexa, ask home control to **open the garage**"
- "Alexa, ask home control to **close the garage**"
- "Alexa, ask home control to open garage door"

### ไฟโรงรถ
- "Alexa, ask home control to turn on garage light"
- "Alexa, ask home control to turn off garage light"
- "Alexa, ask home control to turn on pathway light"

---

## 🚪 ประตูหน้าบ้าน (Front Door — Servo Lock)

- "Alexa, ask home control to **lock the front door**"
- "Alexa, ask home control to **unlock the front door**"
- "Alexa, ask home control to lock the door"
- "Alexa, ask home control if the door is locked"

---

## 🌡️ Climate & Sensors

### อุณหภูมิ + ความชื้น
- "Alexa, ask home control what's the **temperature** in the living room"
- "Alexa, ask home control what's the temperature in the bedroom"
- "Alexa, ask home control what's the temperature in the kitchen"
- "Alexa, ask home control what's the **humidity** in the bedroom"
- "Alexa, ask home control how hot is the kitchen"

### Air Quality
- "Alexa, ask home control what's the air quality"
- "Alexa, ask home control if the air is clean"

---

## 🏠 Modes (Smart Routines)

### House Mode (Home / Away / Security)
- "Alexa, ask home control to **set away mode**" → ปิดทุกอย่าง + lock
- "Alexa, ask home control to **set home mode**" → welcome scene
- "Alexa, ask home control to **set security mode**" → alert ready
- "Alexa, ask home control I'm leaving" → away mode

### Comfort Mode (Normal / Relax / Warm / Sleep)
- "Alexa, ask home control to **turn on sleep mode**" → ไฟปิดทุกห้อง + curtain ปิด
- "Alexa, ask home control to **turn on relax mode**" → warm dim
- "Alexa, ask home control to **turn on warm mode**" → 2700K
- "Alexa, ask home control I'm going to sleep" → sleep mode

---

## 🎵 Multi-action / AI Reasoning (ผ่าน ChAvee bridge)

- "Alexa, ask home control to make the room cozy" → warm + dim + close curtain
- "Alexa, ask home control I'm cold" → fan off + warm tone + curtain close
- "Alexa, ask home control I'm hot" → fan on + bright + curtain open
- "Alexa, ask home control I'm reading" → bright cool light
- "Alexa, ask home control I want to relax" → calm scene + soft music suggestion

---

## 🆘 Emergency

- "Alexa, ask home control there's an **intruder**" → Security mode + red flash + LINE alert
- "Alexa, ask home control to turn on **emergency mode**" → alert flash

---

## 📊 Status Queries

- "Alexa, ask home control to give me a **status update**"
- "Alexa, ask home control what's happening at home"
- "Alexa, ask home control if everything is safe"

---

## 💡 Tips
- Echo Dot ต้องอยู่ใกล้พอ (3-5m) เพื่อจับเสียงชัด
- พูด "Alexa" → รอไฟฟ้าน้ำเงินติด → พูดคำสั่ง
- ถ้า Alexa ตอบ "I don't understand" → ลองพูดสั้นกว่าหรือใช้คำใน list นี้
- คำสั่งที่ซับซ้อน (AI reasoning) ใช้เวลาตอบ ~8 วินาที (รอ Lambda → n8n → Claude AI → reply)

---

## 📍 Defense Demo Sequence (10 คำสั่งโชว์)

1. "Alexa, ask home control to turn on living room light"
2. "Alexa, ask home control to set living room to red"
3. "Alexa, ask home control to turn on party mode"
4. "Alexa, ask home control to turn on calm mode"  
5. "Alexa, ask home control what's the temperature in the kitchen"
6. "Alexa, ask home control if it's safe to cook"
7. "Alexa, ask home control to open the garage"
8. "Alexa, ask home control to lock the front door"
9. "Alexa, ask home control to set sleep mode"
10. "Alexa, ask home control to set away mode"

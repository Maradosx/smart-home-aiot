# 🎤 Alexa Device Setup — Complete Step-by-Step Guide

> **เป้าหมาย:** ทำให้ Echo Dot ในห้องนอนสั่งงาน Smart Home Skill ของเราได้
> **สถานการณ์ปัจจุบัน:** Alexa+ บล็อก custom skill → ต้องใช้บัญชีอื่น
> **เวลาประมาณ:** 60-90 นาที (รวม Beta Test)

---

## 📋 OVERVIEW — 5 Phases

```
Phase 1: สร้าง Amazon account ใหม่ (ไม่มี Alexa+)        ~15 นาที
Phase 2: Setup Beta Test ใน Developer Console             ~15 นาที
Phase 3: บัญชีใหม่รับ invite + enable skill                ~5 นาที
Phase 4: Logout Echo Dot จากบัญชีเก่า + Login บัญชีใหม่   ~20 นาที
Phase 5: Test voice + verify HA state                     ~10 นาที
```

---

## 🔵 PHASE 1 — สร้าง Amazon Account ใหม่

### A. ก่อนเริ่ม — เตรียม
- ✅ Email ที่ยังไม่เคยใช้สมัคร Amazon (Gmail/Outlook ใหม่ก็ได้)
- ✅ เบอร์โทร (อาจไม่ต้อง — บางครั้ง Amazon ขอ verify)
- ✅ Computer หรือมือถือ

### B. สมัคร Account
1. เปิด browser → ไปที่ **https://www.amazon.com**
2. มุมขวาบน → **"Account & Lists"** → **"Start here"** (Sign up)
3. กรอก:
   - **Your name:** ชื่อจริง (เช่น "Test Account")
   - **Mobile number or email:** ใส่ email ใหม่ (ไม่ใช่อันเดิม)
   - **Password:** ตั้งใหม่
4. คลิก **"Continue"**
5. **Verify email** — Amazon ส่ง OTP 6 หลัก → ใส่ → ยืนยัน
6. ✅ Account สร้างเสร็จ

### C. **สำคัญ! ห้ามสมัคร Alexa+ หรือ Prime Trial**
- ระวัง popup *"Try Alexa+ free for 30 days"* → กด **"No thanks"** / **"Skip"**
- ระวัง *"Try Prime"* → กด **"Skip"** (Prime อาจมี Alexa+ พ่วงมา)

### D. บันทึก credentials
```
Email:    _________________
Password: _________________
```

---

## 🔵 PHASE 2 — Setup Beta Test ใน Developer Console

> ใช้ **บัญชีเดิม** `athit.boonpinit@gmail.com` (เจ้าของ Skill)

### A. เข้า Developer Console
1. ไปที่ **https://developer.amazon.com/alexa/console/ask**
2. คลิก Skill **"Smart Home"**

### B. กรอก Distribution Info (จำเป็นก่อน enable Beta Test)
1. แถบบน → คลิก tab **"Distribution"**
2. ที่ section **"Skill Preview"** กรอกทั้งหมด:

| Field | Value |
|---|---|
| **Public Name** | `Smart Home Maradosx` |
| **One Sentence Description** | `Control smart home devices via voice commands` |
| **Detailed Description** | `Smart home automation system that connects to Home Assistant. Control lights, fans, locks, and modes through voice commands in English.` |
| **What's New** | `Initial release` |
| **Example Phrases** | (กรอก 3 ข้อ) |
| 1 | `Alexa, open smart home maradosx` |
| 2 | `Alexa, ask smart home maradosx to turn on bedroom light` |
| 3 | `Alexa, tell smart home maradosx to lock the front door` |
| **Small Icon (108×108)** | upload .png ใดก็ได้ |
| **Large Icon (512×512)** | upload .png ใดก็ได้ |
| **Category** | `Smart Home` → `Smart Home` |
| **Keywords** | `smart home, voice control, home assistant, automation` |
| **Privacy Policy URL** | `https://example.com/privacy` (ใส่ dummy ก็ได้สำหรับ Beta) |
| **Terms of Use URL** | (ปล่อยว่าง) |

3. **Save**

### C. กรอก Privacy & Compliance
1. Sidebar ซ้าย → **"Privacy & Compliance"**
2. ตอบ Yes/No:
   - Does this Skill allow users to make purchases? → **No**
   - Does this Skill collect personal information? → **No**
   - Is this Skill directed to children under 13? → **No**
   - Does this Skill contain advertising? → **No**
   - Is this Skill for export compliance? → **Yes** (ปกติ)
3. **Save**

### D. เปิด Beta Test
1. Sidebar ซ้าย → **"Availability"**
2. **Beta Test** section → **"Enable Beta Test"** หรือ **"Start Beta Test"**
3. **Beta Administrator Email:** `athit.boonpinit@gmail.com` (ของเดิม)
4. **Beta Tester Emails:** ใส่ email ของบัญชีใหม่จาก Phase 1
5. คลิก **"Start Test"** หรือ **"Send Invitations"**

### E. ตรวจ Build
- กลับไป tab **"Build"** → ดูว่า Build ยังสำเร็จอยู่ (Last successful build เห็นเขียว)

---

## 🔵 PHASE 3 — บัญชีใหม่รับ Invite

### A. เช็ค Email ของบัญชีใหม่
1. เปิด email ของบัญชีที่สร้างใน Phase 1
2. หา email จาก Amazon (subject: *"You're invited to test [Smart Home Maradosx]"*)
3. คลิก **"Accept Invitation"** ใน email

### B. ยืนยัน Enable Skill
1. ลิงก์จะพาไปที่ **alexa.amazon.com**
2. Login ด้วยบัญชีใหม่
3. คลิก **"Enable Skill"** สำหรับ "Smart Home Maradosx"
4. ✅ Skill enabled สำหรับบัญชีใหม่

---

## 🔵 PHASE 4 — Switch Echo Dot ไปบัญชีใหม่

### ⚠️ คำเตือน
- การ deregister + ตั้งค่าใหม่ จะลบ:
  - Smart home devices ที่ pair ไว้
  - Routines / reminders / alarms
  - Music subscriptions (Spotify, Apple Music)
- พิจารณาดีก่อนทำ — หรือ test ที่บัญชีอื่นเสร็จแล้วค่อย switch กลับ

### A. Deregister Echo Dot จากบัญชีเก่า
1. เปิด Alexa app (บัญชีเก่า)
2. **Devices** tab (ล่าง)
3. แตะ **"Main Bedroom Echo Dot with Clock"**
4. เลื่อนลงสุด → แตะ **"Deregister"**
5. ยืนยัน → Echo Dot จะ factory reset

### B. หรือ Factory Reset ที่อุปกรณ์ตรง
1. ที่ Echo Dot — กดปุ่ม **"Action"** (ปุ่มไมค์เปิด/ปิด) ค้างไว้ **25 วินาที**
2. ไฟวงกลมจะหมุนสีส้ม → reset เสร็จ → กลับสู่ setup mode (ไฟสีส้ม)

### C. Setup ใหม่ด้วยบัญชีใหม่
1. **Logout บัญชีเก่า** จาก Alexa app บนมือถือ
2. **Login บัญชีใหม่** (ที่สร้างใน Phase 1) — ระวัง ห้าม upgrade Alexa+!
3. แอปจะเจอ Echo Dot ใน setup mode → แตะ **"Set up new device"**
4. เลือก **"Echo Dot"** → ทำตาม wizard (WiFi password, etc.)
5. ⏳ รอ ~5 นาที setup เสร็จ
6. Echo Dot พูด: *"Hello! I'm ready"*

---

## 🔵 PHASE 5 — Test Voice + Verify

### A. ทดสอบ Echo ทำงานปกติ
> 🗣 *"Alexa, what time is it?"*
→ ตอบเวลา = Echo OK

### B. ทดสอบ Custom Skill
> 🗣 *"Alexa, open smart home maradosx"*
→ Echo พูด: *"Welcome to Smart Home Maradosx..."*

หรือถ้า invocation ยังเป็น "hello world":
> 🗣 *"Alexa, open hello world"*

### C. ทดสอบคำสั่ง
> 🗣 *"Turn on bedroom light"*
→ Echo: *"Okay, turned on the bedroom light"*
→ 💡 ไฟห้องนอนติดจริง!

### D. Verify ที่ HA Dashboard
- เปิด http://YOUR_EC2_IP:8123
- ดู `switch.bedroom_light` = on
- ดู logbook event ใหม่

### E. Full Flow Test (8 commands)
```
1. "Alexa, open smart home maradosx"
2. "Turn on bedroom light"
3. "Turn off bedroom light"
4. "Lock the front door"
5. "Unlock the front door"
6. "Set mode to away"
7. "Set mode to home"
8. "What's the bedroom temperature"
```

---

## 🐛 Troubleshooting

| ปัญหา | แก้ |
|---|---|
| Email invite ไม่ส่ง | เช็ค spam folder, รอ 5 นาที, resend |
| "Skill not found" หลัง accept invite | รอ 10 นาที (skill propagate), reload |
| Echo Dot ตอบ "Sorry, I can't" | บัญชีใหม่ดันมี Alexa+ ด้วย → cancel ใน account settings |
| Skill ตอบใน Console แต่ Echo ไม่ตอบ | Logout/login Alexa app, restart Echo Dot |
| Custom Skill ไม่อยู่ใน Skills tab | กลับไป Beta Test → resend invite |

---

## ⚖️ ทางเลือก — ถ้าไม่อยากลำบาก

### Plan A: ทำตาม guide นี้ (60-90 นาที)
- Pros: ได้ demo Echo Dot จริงตอน defense
- Cons: ใช้เวลา + ลบ Echo Dot setup เดิม

### Plan B: ใช้ Console Simulator (0 นาที)
- Pros: Skill ทำงาน 100% พิสูจน์แล้ว, ไม่ต้องทำอะไรเพิ่ม
- Cons: ไม่มี Echo Dot ตอน defense (ใช้คอมแทน)

### Plan C: Cancel Alexa+ บัญชีเดิม (1-7 วัน propagate)
- Pros: ไม่ต้องสร้างบัญชีใหม่
- Cons: ต้องรอ propagation, อาจไม่ทันก่อน defense (26 พ.ค.)

---

## 📅 Timeline ก่อน Defense (26 พ.ค.)

| วัน | งาน |
|---|---|
| **23 พ.ค. (วันนี้)** | ตัดสินใจ Plan A/B/C |
| 24 พ.ค. | (ถ้า Plan A) ทำตาม guide นี้ |
| 25 พ.ค. | Rehearsal defense + บันทึก demo video สำรอง |
| 26 พ.ค. | **Defense** 🎓 |

---

## 🎯 คำแนะนำสุดท้าย

**Plan B (Console Simulator) = แนะนำมากที่สุด**

เหตุผล:
1. Skill ทำงาน 100% พิสูจน์แล้ว (4/4 commands ผ่าน)
2. กรรมการเข้าใจดีว่า Alexa+ block = Amazon platform limitation
3. ไม่เสี่ยงเสีย Echo Dot setup ปัจจุบัน
4. มีเวลาเหลือไป polish slides + ซ้อม presentation

ถ้าอยากได้ Echo Dot ตอน defense จริงๆ → ทำ Plan A คืนนี้ทันที

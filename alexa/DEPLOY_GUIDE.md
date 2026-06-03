# 🎤 Alexa Deployment Guide — Phase B + C

> Phase A (code prep) เสร็จแล้ว ✅ — เหลือ B + C ที่ต้องทำผ่านเว็บ
> Total: ~2 ชั่วโมง

---

## 📦 Phase A Output (ready to use)

```
alexa/
├── lambda_function.py     14 KB — v3 with ชาวี brain bridge + music
├── interaction_model.json 4 KB  — 15 intents (was 11)
└── lambda.zip             ~5 KB — packaged for AWS upload
```

**New capabilities in v3:**
- ✅ `SmartHomeChatIntent` — bridge to ชาวี brain (Claude Haiku 4.5)
- ✅ Lock entity fixed (`lock.front_door_lock` + `lock/lock` service)
- ✅ `PlayMusicIntent` — Spotify mood playlists + matching scene
- ✅ `GetTemperatureIntent` — direct sensor read
- ✅ `AMAZON.FallbackIntent` — auto-bridge unknowns to ชาวี
- ✅ Env var support (HA_URL, HA_TOKEN, N8N_WEBHOOK, SUPA_URL, SUPA_KEY)

---

## ☁️ Phase B — AWS Lambda Deploy (~60 นาที)

### B1. เข้า AWS Console
1. ไปที่ https://console.aws.amazon.com
2. Login (สมัครฟรีถ้ายังไม่มี — Free tier 1M requests/month)
3. ค้นหา "Lambda" → Click

### B2. Create Function
1. Click **"Create function"**
2. Author from scratch
3. **Function name:** `smart-home-alexa-skill`
4. **Runtime:** Python 3.11
5. **Architecture:** x86_64
6. **Execution role:** Create new role with basic Lambda permissions
7. Click **Create function**

### B3. Upload Code
1. ใน function page → tab **"Code"**
2. Click **"Upload from"** → **.zip file**
3. เลือก `alexa/lambda.zip`
4. Click **Save**

### B4. Configure Settings
1. Tab **"Configuration"** → **"General configuration"** → Edit
2. **Timeout:** 3 sec → **15 sec** (ชาวี brain takes ~12s)
3. **Memory:** 128 MB → **256 MB**
4. Save

### B5. Environment Variables
Tab **"Configuration"** → **"Environment variables"** → Edit → Add:

| Key | Value |
|---|---|
| `HA_URL` | `http://YOUR_EC2_IP:8123` |
| `HA_TOKEN` | (copy from `lambda_function.py` line 21) |
| `N8N_WEBHOOK` | `https://YOUR_N8N_INSTANCE.up.railway.app/webhook/line-webhook` |
| `SUPA_URL` | `https://YOUR_PROJECT.supabase.co` |
| `SUPA_KEY` | (paste Supabase anon key — แชทเรื่องนี้ผมส่งให้) |
| `ALEXA_USER` | `Ualexa_echo_001` |

### B6. Add Alexa Skills Kit Trigger
1. ใน function page → click **"+ Add trigger"**
2. เลือก **Alexa Skills Kit**
3. **Skill ID verification:** Disable (initial setup) — จะ enable ทีหลังหลังสร้าง skill
4. Click **Add**

### B7. Copy Lambda ARN
มุมขวาบน → copy ARN เช่น `arn:aws:lambda:us-east-1:123456:function:smart-home-alexa-skill`
**บันทึกไว้ — ต้องใช้ใน Phase C**

### B8. Test (optional)
1. Tab **"Test"** → Create test event
2. Event name: `LaunchRequest`
3. JSON body:
```json
{"request":{"type":"LaunchRequest","requestId":"test","timestamp":"2026-05-23T00:00:00Z","locale":"en-US"}}
```
4. Click **Test** → ควรเห็น response มี "Welcome to Smart Home Maradosx..."

---

## 🎤 Phase C — Alexa Developer Console (~60 นาที)

### C1. เข้า Alexa Developer Console
1. ไปที่ https://developer.amazon.com/alexa/console/ask
2. Login ด้วย Amazon account (เดียวกับช้อปปิ้ง)
3. Click **"Create Skill"**

### C2. Skill Setup
1. **Skill name:** `Smart Home Maradosx`
2. **Primary locale:** English (US)
3. **Choose a type of experience:** Custom
4. **Choose a method:** Provision your own
5. **Hosting:** **Don't host my skill — use my own backend** (เพราะเราใช้ Lambda)
6. Click **Next**

### C3. Add Interaction Model
1. Sidebar → **"Interaction Model"** → **"JSON Editor"**
2. ลบของเดิม → paste content ของ `alexa/interaction_model.json`
3. **Save Model** (ปุ่มบน)
4. **Build Model** (ใช้เวลา ~2-3 นาที)
5. รอจน "Build successful"

### C4. Add Endpoint (Lambda ARN)
1. Sidebar → **"Endpoint"**
2. **Service Endpoint Type:** AWS Lambda ARN
3. **Default Region:** paste Lambda ARN จาก B7
4. Save Endpoints

### C5. Skill ID Verification (Tighten Security)
1. Copy **Skill ID** ใน Endpoint page (format: `amzn1.ask.skill.XXXX...`)
2. กลับไปที่ AWS Lambda → Triggers → Alexa Skills Kit
3. Enable **Skill ID verification** → paste Skill ID → Save

### C6. Test ใน Console
1. Sidebar → **"Test"**
2. เปลี่ยน **"Off"** → **"Development"**
3. ลองพิมพ์: `open smart home maradosx`
4. ควรเห็น Welcome response
5. ลองต่อ: `turn on bedroom light`
6. ดู Echo response ที่ขวา

---

## 🎤 Phase D — Test ผ่าน Echo / Alexa App (~30 นาที)

### D1. Alexa Mobile App
1. ดาวน์โหลด Alexa app (iOS/Android — ฟรี)
2. Login ด้วย Amazon account เดียวกับที่สร้าง Skill
3. ไปที่ **More → Skills & Games → Your Skills → Dev**
4. ควรเห็น "Smart Home Maradosx" → Enable

### D2. Test Commands (พูดผ่านมือถือหรือ Echo)

**English:**
- "Alexa, open Smart Home Maradosx"
- "Alexa, ask Smart Home Maradosx to turn on the bedroom light"
- "Alexa, tell Smart Home Maradosx to set mode to away"
- "Alexa, ask Smart Home Maradosx to lock the front door"
- "Alexa, ask Smart Home Maradosx the bedroom temperature"
- "Alexa, ask Smart Home Maradosx to play chill music"
- "Alexa, tell Smart Home Maradosx, เปิดไฟห้องนอน" *(จะ bridge ไป ชาวี — Thai)*

**Demo flow recommend (defense):**
1. "open smart home maradosx" → welcome
2. "turn on bedroom light" → instant
3. "what's the bedroom temperature" → "26.5 degrees celsius"
4. "set mode to away" → "House mode set to Away"
5. "play chill music" → Spotify + matching scene
6. "ask Chavee, มีคนบุก" *(emergency demo via ชาวี)* → lock + Security + flash

---

## 🛒 ก่อนเริ่ม — Pre-flight Checklist

- [ ] AWS account login พร้อม (ฟรี — สมัคร 5 นาที)
- [ ] Amazon Developer account (ใช้ shopping account ได้)
- [ ] Alexa app บนมือถือ OR Echo speaker
- [ ] Supabase anon key (ผมแจ้งให้ตอน Phase B)
- [ ] (Optional) Spotify Premium + HA Spotify integration (ฟีเจอร์เพลง)

---

## 🚨 Troubleshooting

| Problem | Solution |
|---|---|
| Lambda timeout 15s | ขยายเป็น 20s ใน Configuration |
| "Skill response invalid" | ดู CloudWatch logs → ค้นหา `lambda_function.py` exception |
| Skill ไม่เห็น intent | rebuild model + clear cache + รออีก 2 นาที |
| ชาวี bridge ไม่ตอบ | ตรวจ env var SUPA_KEY ใน Lambda |
| เพลงไม่เล่น | ตรวจ HA Spotify integration setup + login active |
| Lambda permission error | IAM role ต้องมี logs:CreateLogStream + logs:PutLogEvents |

---

## ✅ Definition of Done

- [ ] Lambda function deployed + tested LaunchRequest
- [ ] Skill built + Development mode active
- [ ] Test ผ่าน mobile app: "turn on bedroom light" ทำงาน
- [ ] Test bridge: "tell Smart Home Maradosx, เปิดไฟห้องนั่งเล่น" → ชาวี reply
- [ ] (Option) เพลงผ่าน Spotify
- [ ] Demo video ถ่าย 1 นาทีสำหรับ defense

**คาดว่า:** ใช้เวลา 2-3 ชั่วโมงทำต่อเนื่อง → ถ่าย demo defense ได้

# 🚨 Disaster Recovery — Smart Home AIoT

> **Restore procedures** สำหรับเมื่อมีปัญหากับ EC2, Pi 3B+, หรือ services อื่น ๆ
>
> **Last updated:** 27 พ.ค. 2569 (post-defense)

---

## 🎯 Critical Components & Failure Scenarios

| Component | Failure → Impact | Recovery time |
|---|---|---|
| **AWS EC2** | HA + Mosquitto down → ทั้งบ้านหยุด | ~30 min |
| **Raspberry Pi 3B+** | Zigbee devices ไม่ตอบสนอง | ~1 hr |
| **ngrok URL change** | LINE Bot ไม่ตอบ | ~5 min |
| **Anthropic API outage** | AI features (LINE + Alexa) ไม่ทำงาน | wait + monitor |
| **LINE Bot quota** | ChAvee ตอบไม่ได้ | upgrade or wait |
| **Wi-Fi router down** | ESP32 + Pi offline | restart router |

---

## 🔥 P0 — EC2 Instance Failure

### สัญญาณ
- HA Dashboard ไม่เปิด (`http://YOUR_EC2_IP:8123` ไม่ตอบ)
- LINE Bot ChAvee ไม่ตอบสนอง
- Alexa ตอบ "There was a problem..."

### Recovery Steps

```bash
# 1. SSH เข้า EC2
ssh -i ~/Downloads/smart-home-key.pem ubuntu@YOUR_EC2_IP

# 2. เช็ค Docker
sudo docker ps -a
sudo docker logs homeassistant --tail 50
sudo docker logs mosquitto --tail 50

# 3. ถ้า containers หยุด → start
cd ~/Docker
sudo docker-compose up -d

# 4. ถ้า EC2 instance หยุด → restart from AWS Console
# AWS Console → EC2 → Instance → Actions → Start
```

### Backup Strategy (ทำก่อนเกิดปัญหา)

**ทำ AMI Snapshot ทุกเดือน:**
1. AWS Console → EC2 → Instances → Select smart-home-ec2
2. Actions → Image and templates → Create image
3. Name: `smart-home-aiot-YYYY-MM-DD`
4. รอ ~5 นาที (Free tier: เก็บได้ 30GB EBS)

**Restore จาก AMI:**
1. AWS Console → AMI → Select snapshot → Launch instance
2. ใช้ t3.small + same security group
3. Associate elastic IP `YOUR_EC2_IP`
4. ✅ HA + Mosquitto กลับมาทันทีพร้อม state ตอน snapshot

---

## 🍓 P1 — Raspberry Pi 3B+ Failure

### สัญญาณ
- Zigbee devices (Tuya bulb, plugs, SNZB sensors) ไม่ตอบ
- HA แสดง entities `unavailable`

### Recovery Steps

```bash
# 1. SSH เข้า Pi (จาก laptop ในวง Wi-Fi เดียวกัน)
ssh pi@172.20.10.4    # หรือ ping ดู IP จริง

# 2. เช็ค Docker
sudo docker ps -a
sudo docker logs zigbee2mqtt --tail 50

# 3. Restart Z2M
sudo docker restart zigbee2mqtt

# 4. ถ้า Pi boot ไม่ขึ้น → ดู vcgencmd ก่อน
vcgencmd get_throttled
# 0x0 = OK, อื่น ๆ = under-voltage / overheating
```

### Backup Strategy

**Pi SD Card Image (ทำทุก 3 เดือน):**

```bash
# จาก laptop ที่มี SD card reader
# ถอด SD จาก Pi → เสียบ laptop
diskutil list                              # หา disk ID (e.g., disk4)
sudo dd if=/dev/disk4 of=~/Backups/pi-3bplus-YYYY-MM-DD.img bs=4M status=progress
# รอ ~20-30 นาที (32GB SD)
```

**Restore Pi SD:**
```bash
# เขียน image กลับ SD card ใหม่ (32GB+)
sudo dd if=~/Backups/pi-3bplus-2026-05-27.img of=/dev/disk4 bs=4M status=progress
# Insert SD → power on Pi
# Z2M + Docker จะ boot กลับมาเหมือนเดิม
```

### Zigbee Re-pairing (ถ้าจำเป็น)

ถ้า Z2M database หาย → ต้อง re-pair devices:
1. Pi: `sudo docker exec -it zigbee2mqtt npm run permit-join`
2. Tuya bulb: ปิด-เปิดไฟ 6 ครั้ง (reset)
3. Tuya plug: กดปุ่ม 5 วินาที จน LED กระพริบ
4. Sonoff SNZB: กดปุ่ม reset (รูเข็ม) 5 วินาที
5. ใน Z2M web UI (port 8080) → "Permit join (all)"

---

## 🌐 P2 — ngrok URL เปลี่ยน (LINE Webhook พัง)

### สัญญาณ
- LINE Bot ChAvee ไม่ตอบ
- LINE Webhook test แสดง error

### Recovery Steps (Quick fix)

```bash
# 1. ดู ngrok URL ปัจจุบัน
ssh ubuntu@YOUR_EC2_IP
sudo docker logs ngrok-tunnel | grep "started tunnel"
# จะเห็น: https://xxxx-xxxx.ngrok-free.app

# 2. Copy URL ใหม่ → LINE Developer Console
# https://developers.line.biz/console/
# → Provider: ชาวี
# → Channel: smart-home-aiot
# → Messaging API tab
# → Webhook URL: <new-ngrok-url>/api/webhook/chavee
# → Update
# → Verify (should return 200 OK)

# 3. Test ส่ง message ใน LINE → ChAvee ควรตอบ
```

### Long-term Fix: ย้ายไป Cloudflare Tunnel (Static URL)

ngrok free มี URL เปลี่ยนทุกครั้ง restart → ใช้ Cloudflare Tunnel แทน:

```bash
# 1. Install cloudflared บน EC2
wget https://github.com/cloudflare/cloudflared/releases/latest/download/cloudflared-linux-amd64.deb
sudo dpkg -i cloudflared-linux-amd64.deb

# 2. Login + create tunnel
cloudflared tunnel login
cloudflared tunnel create smart-home-aiot

# 3. Setup config /etc/cloudflared/config.yml
# (ดู template ที่ cloudflare docs)

# 4. Map subdomain → tunnel
cloudflared tunnel route dns smart-home-aiot ha.yourdomain.com

# 5. Run as service
sudo systemctl start cloudflared

# 6. Update LINE webhook ครั้งเดียว → ใช้ได้ตลอด
```

ข้อดี: Free + Static URL + HTTPS อัตโนมัติ + ไม่ต้องดูแล renewal

---

## 💸 P3 — LINE Bot Quota หมด

### สัญญาณ
- ChAvee ตอบช่วงแรก แล้วเงียบไป
- `curl /v2/bot/message/quota/consumption` → 15000/15000

### Recovery
- ปัจจุบันใช้ **Basic plan ฿1,284/mo = 15,000 msg**
- ถ้าใช้เยอะกว่านี้ → upgrade Pro plan ฿4,284/mo = 75,000 msg
- หรือลด push frequency โดยใช้ persistent notification + mobile push แทน

---

## 🤖 P4 — Anthropic API Outage

### สัญญาณ
- LINE Bot ไม่ตอบ + Alexa ไม่ตอบ
- Lambda CloudWatch logs แสดง 5xx จาก api.anthropic.com

### Recovery
- เช็ค https://status.anthropic.com/
- ถ้า incident → รอ (Anthropic ปกติแก้เร็ว)
- ใช้ HA Dashboard / Mobile app ควบคุมตรงไปก่อนชั่วคราว

### Fallback Plan (ถ้าต้องการ resilience สูง)
- ตั้ง 2nd AI provider (เช่น Groq + Anthropic) ใน Lambda
- Try-catch → fallback ถ้า primary fail

---

## 📡 P5 — Wi-Fi Router Down

### สัญญาณ
- ESP32 ทั้ง 5 ตัวออฟไลน์พร้อมกัน
- Pi ออฟไลน์ด้วย

### Recovery
1. Restart router
2. รอ 60 วินาที → ESP32 จะ auto-reconnect (firmware มี logic)
3. Pi จะ auto-reconnect via systemd

### Fallback
- ESP32 firmware มี SmartHome-Local AP fallback (Pi serves as AP)
- ถ้า primary Wi-Fi (Chawee) ดับ → ESP32 ต่อกับ Pi โดยตรง

---

## 🔄 Routine Maintenance (Monthly)

### Week 1
- [ ] AWS Console → ตรวจ EC2 health + credits remaining
- [ ] ดูค่าใช้จ่าย (AWS + Railway + LINE + Anthropic)
- [ ] เช็ค Anthropic API usage (ป้องกัน billing surprise)

### Week 2
- [ ] AMI Snapshot EC2 (ตั้ง schedule auto ถ้าได้)
- [ ] HA → Configuration → System → Backup (download .tar)
- [ ] Test recovery — ลองเปิด HA Companion → ตรวจ entities

### Week 3
- [ ] Pi SD card backup (ถ้าผ่าน 3 เดือน)
- [ ] Z2M → Settings → Backup network (download)
- [ ] เช็ค Zigbee battery (SNZB-03P, 04, 05P CR2450)

### Week 4
- [ ] LINE Bot quota review
- [ ] รัน 130-test cases (ดู `test_sheets/`) แบบ random sampling
- [ ] อัปเดต documentation ถ้ามีการเปลี่ยนแปลง

---

## 📞 Emergency Contacts

| Service | URL | Notes |
|---|---|---|
| AWS Status | https://health.aws.amazon.com/health/status | us-east-1 region |
| Anthropic Status | https://status.anthropic.com/ | Claude API |
| LINE Developer | https://developers.line.biz/console/ | Webhook config |
| Railway Status | https://status.railway.app/ | n8n hosting |
| Supabase Status | https://status.supabase.com/ | DB |
| Cloudflare Status | https://www.cloudflarestatus.com/ | (if migrated) |

---

## 🆘 Last Resort: Full System Rebuild

ถ้าทุกอย่างพังพร้อมกัน:

1. **Clone repo** จาก GitHub: `git clone https://github.com/Maradosx/smart-home-aiot.git`
2. **AWS:** launch t3.small + install Docker → `docker-compose up -d`
3. **Pi:** flash Raspbian + restore from SD image OR fresh install Z2M
4. **ESP32:** flash 5 zones via Arduino IDE (ใส่ Wi-Fi credentials)
5. **AWS Lambda:** upload `alexa/lambda.zip` ใหม่
6. **LINE:** update webhook URL
7. **Re-pair Zigbee** ผ่าน Z2M web UI
8. **Verify:** ทดสอบทุกห้อง + ทุก control channel

⏰ Total rebuild time: ~4-6 hours (with backups: ~1 hour)

---

## 💡 Pro Tips

- 📸 **AMI Snapshots ทุกเดือน** = สิ่งที่ราคาถูกที่สุดในการป้องกัน disaster
- 🔐 **เก็บ .pem key + .env** ใน password manager (1Password / Bitwarden)
- 📝 **เขียน Last Modified date** บนทุก backup
- 🧪 **Test restore ปีละครั้ง** — backup ที่ไม่ test = ไม่มี backup
- 🌐 **Cloudflare Tunnel** > ngrok สำหรับ production (static URL + free)

---

**Built with engineering maturity for long-term operation** 🛡️

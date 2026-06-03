# 🚀 Deploy Guide — Pi Chawee Migration + Backup AP Setup

> **เป้าหมาย**: Pi เชื่อม router Chawee ตลอด → Z2M web UI ใช้ได้ตลอดโดยไม่ต้องต่อ Mac
> **เวลาประมาณ**: ~20 นาที

---

## 📋 สิ่งที่ต้องเตรียม

| อุปกรณ์ | สถานะที่ต้องการ |
|---------|---------------|
| Pi 3B+ | **ปิดอยู่** (ถอดปลั๊กไว้ก่อน) |
| SD Card | ยังอยู่ใน Pi |
| Mac | เชื่อม WiFi **Chawee** อยู่ |
| USB-to-microUSB / wall adapter 5V/2.5A | พร้อมเสียบ Pi |

> ⚠️ **อย่าเพิ่งเปิด Pi** — ต้องแก้ wpa_supplicant บน SD card ก่อน (ข้อ 1)

---

## STEP 1 — แก้ wpa_supplicant บน SD Card

Pi ต้องรู้จัก WiFi Chawee ก่อน boot ครั้งแรก มี 2 วิธี เลือกวิธีใดก็ได้:

### วิธี A: ถอด SD Card แก้บน Mac (แนะนำ — ง่ายที่สุด)

```bash
# ถอด SD card จาก Pi → ใส่ SD card reader → เสียบ Mac
# Mac จะ mount /Volumes/boot (หรือ /Volumes/bootfs)

# ดูว่า mount ที่ไหน
ls /Volumes/

# Copy wpa_supplicant.conf ไปที่ /boot partition
sudo cp /Users/path_mos/Desktop/Mos/Project_1/pi_setup/wpa_supplicant.conf \
     /Volumes/boot/wpa_supplicant.conf

# ยืนยัน
cat /Volumes/boot/wpa_supplicant.conf
```

> Pi จะ copy ไฟล์นี้ไปที่ `/etc/wpa_supplicant/wpa_supplicant.conf` อัตโนมัติตอน boot

**ถอด SD card จาก Mac → ใส่กลับ Pi → ข้ามไป STEP 2**

---

### วิธี B: SSH เข้าผ่าน iPhone Hotspot (ถ้ายังต่อ hotspot ได้)

```bash
# เชื่อม Mac กับ iPhone hotspot ก่อน
ssh pi@172.20.10.4

# บน Pi:
sudo cp /tmp/wpa_supplicant.conf /etc/wpa_supplicant/wpa_supplicant.conf
sudo reboot
```

*(ก่อน SSH ต้อง SCP ไฟล์ขึ้นก่อน — ดู SCP commands ด้านล่าง)*

---

## STEP 2 — เปิด Pi + รอ Boot

```bash
# เสียบ wall adapter 5V/2.5A → Pi
# รอ 60 วินาที ให้ Pi boot เสร็จ

# ตรวจสอบว่า Pi เชื่อม Chawee แล้ว
# ดู ARP table บน Mac (ต้องอยู่ Chawee network):
arp -an | grep -E "b8:27:eb|dc:a6:32"
# ถ้าได้ IP → Pi online แล้ว

# หรือ ping mDNS hostname:
ping raspberrypi.local -c 3
```

> 📌 Pi จะได้ IP จาก Chawee router โดยอัตโนมัติ (DHCP)
> ดู IP ได้จาก router admin page ด้วย

---

## STEP 3 — SSH เข้า Pi

```bash
# แทนที่ <PI_IP> ด้วย IP ที่ได้จาก STEP 2
PI_IP=$(arp -an | grep -E "b8:27:eb|dc:a6:32" | head -1 | awk '{print $2}' | tr -d '()')
echo "Pi IP: $PI_IP"

ssh pi@$PI_IP
# password: raspberry  (หรือ password ที่ตั้งไว้)
```

---

## STEP 4 — Copy Scripts ขึ้น Pi

**รันบน Mac** (ไม่ใช่บน Pi):

```bash
PI_IP="<ใส่ IP ที่ได้>"

# Copy setup script
scp /Users/path_mos/Desktop/Mos/Project_1/pi_setup/setup_pi_ap.sh \
    pi@$PI_IP:~/setup_pi_ap.sh

# Copy open_z2m helper (optional, ใช้บน Mac)
# ไม่ต้อง copy ขึ้น Pi
```

---

## STEP 5 — รัน AP Setup Script บน Pi

**SSH เข้า Pi แล้วรัน:**

```bash
# บน Pi:
chmod +x ~/setup_pi_ap.sh
sudo ./setup_pi_ap.sh
```

Script จะทำ:
1. ติดตั้ง `hostapd` + `dnsmasq`
2. สร้าง virtual interface `ap0`
3. ตั้ง hostapd → broadcast SSID **SmartHome-Local** (pass: `YOUR_AP_PASSWORD`)
4. ตั้ง dnsmasq → DHCP `192.168.50.10-50` / Pi = `192.168.50.1`
5. เปิด IP forwarding + NAT (ESP32 ผ่าน Pi ออก internet ได้)

**รอ script เสร็จ (~3 นาที)** → reboot:

```bash
sudo reboot
```

---

## STEP 6 — ยืนยัน Pi Dual-Role ทำงาน

รอ Pi boot (~60 วินาที) แล้ว SSH กลับไป:

```bash
ssh pi@$PI_IP

# ตรวจสอบ interfaces
ifconfig | grep -A2 "wlan0\|ap0"

# ควรได้:
# wlan0: inet 192.168.x.x  (IP จาก Chawee)
# ap0:   inet 192.168.50.1  (static IP)

# ตรวจสอบ services
systemctl status hostapd dnsmasq --no-pager

# ตรวจสอบ WiFi ที่ปล่อย
iw dev
# ควรเห็น:  ap0 / type AP
```

---

## STEP 7 — ยืนยัน Z2M Web UI

**บน Mac (ยังอยู่ Chawee network):**

```bash
# วิธีที่ 1: ใช้ script
/Users/path_mos/Desktop/Mos/Project_1/pi_setup/open_z2m.sh

# วิธีที่ 2: เปิด browser ตรง
open -a "Google Chrome" http://$PI_IP:8080
```

Z2M Web UI ควรโหลดได้จาก `http://<Pi Chawee IP>:8080`

---

## STEP 8 — Flash ESP32 ทั้ง 5 ตัว

ใช้ Arduino IDE เปิดแต่ละไฟล์แล้ว Upload:

| ไฟล์ | Board | ต้องใช้ Library |
|------|-------|----------------|
| `esp32_01_bedroom.ino` | ESP32 #1 | ESP32Servo |
| `esp32_02_livingroom.ino` | ESP32 #2 | ESP32Servo, DHT |
| `esp32_03_kitchen.ino` | ESP32 #3 | DHT |
| `esp32_04_bathroom_garden.ino` | ESP32 #4 | DHT |
| `esp32_05_garage.ino` | ESP32 #5 | DHT |

**ทุกไฟล์อยู่ที่:**
```
/Users/path_mos/Desktop/Mos/Project_1/esp32_firmware/เวอร์ชั่นมีmotor/
```

**WiFi ที่ตั้งใหม่ทุกตัว:**
- SSID_1 = `Chawee` (primary — router บ้าน)
- SSID_2 = `SmartHome-Local` (fallback — Pi AP)

**หลัง flash ทุกตัว → ตรวจสอบใน MQTT Explorer:**
- subscribe: `home/#`
- ทุก ESP32 ควร connect และ publish sensor data ภายใน 30 วินาที

---

## STEP 9 — ทดสอบ Door Lock Servo

```bash
cd /Users/path_mos/Desktop/Mos/Project_1/esp32_firmware/เวอร์ชั่นมีmotor/
chmod +x test_door_servo.sh

# ทดสอบ full cycle (LOCK↔UNLOCK 5 รอบ)
./test_door_servo.sh full
```

---

## STEP 10 — ทดสอบ Failover (Optional)

ทดสอบว่า ESP32 fallback ไป SmartHome-Local ได้จริง:

```bash
# บน Mac: ปิด Chawee router ชั่วคราว (หรือ ban MAC ชั่วคราวใน router admin)
# รอ ~15 วินาที

# ตรวจสอบ ESP32 connect ไป SmartHome-Local:
# Serial Monitor ใน Arduino IDE จะแสดง:
# "Primary WiFi failed, trying SmartHome-Local..."
# "Connected to SmartHome-Local (192.168.50.xx)"

# MQTT ยังทำงานผ่าน Pi NAT → EC2 ได้ตลอด
mosquitto_sub -h YOUR_EC2_IP -p 1883 -t "home/#"
# ควรยังเห็น data จาก ESP32
```

---

## 📊 สถาปัตยกรรมสุดท้าย

```
┌─────────────────────────────────────────────┐
│  Production Flow (ไม่ต้องต่อ Mac)           │
│                                             │
│  ESP32 ──[Chawee WiFi]──► Pi wlan0          │
│    ↓ (failover)              ↓              │
│  ESP32 ──[SmartHome-Local]─► Pi ap0         │
│                              ↓              │
│                         Pi (MQTT client)    │
│                              ↓              │
│                      EC2 MQTT Broker        │
│                      (YOUR_EC2_IP:1883)     │
│                         ↙       ↘           │
│                Home Assistant    n8n        │
│                              ↓              │
│                         LINE Bot            │
└─────────────────────────────────────────────┘

Mac = admin tool เท่านั้น (Z2M web UI, SSH, Arduino IDE)
```

---

## 🔧 Troubleshooting

| ปัญหา | วิธีแก้ |
|-------|---------|
| Pi ไม่ได้ IP จาก Chawee | ตรวจสอบ wpa_supplicant.conf ที่ /boot/wpa_supplicant.conf ถูกหรือไม่ |
| `ap0` สร้างไม่ได้ | `sudo reboot` แล้วลองใหม่ (udev rule ต้องการ boot) |
| hostapd start ล้มเหลว | `sudo journalctl -u hostapd -n 50` ดู error |
| Z2M web UI ไม่ขึ้น | `sudo systemctl status zigbee2mqtt` บน Pi |
| ESP32 ไม่ connect Chawee | ตรวจสอบ SSID/pass ใน .ino ก่อน flash |
| MQTT ไม่ได้ data | `nc -zv YOUR_EC2_IP 1883` ทดสอบ EC2 reachable |

---

## SCP Reference Commands

```bash
# Copy ไฟล์เดียว
scp <local_file> pi@<PI_IP>:<remote_path>

# Copy folder ทั้งหมด
scp -r <local_folder> pi@<PI_IP>:<remote_path>

# Download จาก Pi
scp pi@<PI_IP>:<remote_file> <local_path>
```

---

*สร้าง: 2026-05-20 | Project Smart Home AIoT CE08 Bangkok University*

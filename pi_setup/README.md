# Pi 3 Model B+ — Zigbee Bridge Setup

> One-shot setup for Sonoff ZBDongle-E → EC2 Mosquitto MQTT broker via Raspberry Pi 3 Model B+ running Zigbee2MQTT in Docker.

**Last verified install:** 13 พ.ค. 2026 — ใช้งานจริงในโปรเจค defense 26 พ.ค.

## 📦 ไฟล์ในโฟลเดอร์นี้

| ไฟล์ | หน้าที่ |
|------|---------|
| `setup-pi.sh` | One-shot install script (apt + Docker + Z2M Docker + systemd) |
| `flash_sd.sh` | Helper สำหรับ flash SD card บน Mac |
| `HA_zigbee_additions.yaml` | YAML entities เพิ่มใน HA หลัง pair devices |
| `zigbee2mqtt_test_commands.sh` | คำสั่ง mosquitto test หลัง pair เสร็จ |
| `README.md` | คู่มือนี้ |

---

## ⚠️ Power Supply Warning — สำคัญที่สุด

**Pi 3B+ ต้องใช้ 5V/2.5A ขึ้น เสียบ wall adapter ตรงๆ เท่านั้น**

🔴 **ห้าม:**
- เสียบ USB จาก Mac (0.5-0.9A) → ไฟตก reboot loop
- เสียบผ่าน USB hub → กระแสไม่นิ่ง
- เสียบ powerbank ขณะ passthrough charge → spike

🟢 **ทำได้:**
- Wall adapter 5V/2.5A+ ตรง (Anker/Aukey/Raspberry Pi official)
- USB-C 65W PD adapter + สาย micro-USB หนา (สั้นๆ <1m)

**เช็คก่อนเริ่ม:** `vcgencmd get_throttled` ต้อง = `0x0` ถ้าไม่ใช่ = ไฟไม่พอ

---

## ⏱️ Quick Start

### 1. Flash MicroSD (บน Mac) — 10 นาที
- โหลด **Raspberry Pi Imager** → https://www.raspberrypi.com/software/
- เลือก **Raspberry Pi OS Lite (64-bit) — Debian Trixie**
- กด **EDIT SETTINGS** ก่อน flash:
  ```
  General:
    hostname: smarthome-pi
    username: pi
    password: YOUR_AP_PASSWORD
    Wi-Fi SSID/Pass: <ของบ้าน/hotspot>
    Locale: Asia/Bangkok
  Services:
    ☑ Enable SSH (password)
  ```
- กด SAVE → WRITE → รอ ~10 นาที

หรือใช้สคริปต์ใน `flash_sd.sh`

### 2. เสียบ Pi + boot — 1 นาที
- เสียบ Sonoff ZBDongle-E → USB-A
- เสียบ MicroSD → ใต้บอร์ด
- **เสียบ wall adapter ตรง** (ดู Power Supply Warning ด้านบน)

### 3. SSH เข้า Pi — 2 นาที
รอ LED เขียวหยุดกระพริบ (1-2 นาที):
```bash
# หา IP ถ้าใช้ DHCP (hotspot iPhone จะให้ IP 172.20.10.x)
arp -a | grep -i b8:27:eb
# หรือ
ssh pi@smarthome-pi.local

# ใส่ password: YOUR_AP_PASSWORD
```

### 4. (Optional) Setup SSH key + NOPASSWD sudo
```bash
# Mac
ssh-keygen -t ed25519 -f ~/.ssh/id_ed25519 -N '' -C mac-to-pi
ssh-copy-id pi@<PI-IP>

# Pi (ครั้งเดียว, ใส่ pwd ครั้งสุดท้าย)
ssh -t pi@<PI-IP> 'echo "pi ALL=(ALL) NOPASSWD: ALL" | sudo tee /etc/sudoers.d/010_pi-nopasswd && sudo chmod 440 /etc/sudoers.d/010_pi-nopasswd'
```

### 5. Copy + รัน setup script — 5 นาที
จาก **Mac**:
```bash
scp pi_setup/setup-pi.sh pi@<PI-IP>:~/
ssh pi@<PI-IP> 'chmod +x ~/setup-pi.sh && sudo ./setup-pi.sh'
```

Script จะ:
- ✅ เช็ค `throttled=0x0` (ไฟดี) — ถ้าไม่ใช่จะเตือนแล้วหยุด
- ✅ apt update + install Docker + mosquitto-clients
- ✅ ตรวจ ZBDongle ที่ `/dev/serial/by-id/`
- ✅ `docker pull koenkk/zigbee2mqtt:latest`
- ✅ เขียน `configuration.yaml` (point EC2 `YOUR_EC2_IP:1883`, **adapter: ember**)
- ✅ สร้าง systemd service `zigbee2mqtt.service`
- ✅ start container

### 6. ⚠️ ถ้า log แสดง `SRSP - SYS - ping after 6000ms`
แปลว่า dongle เป็น **ZBDongle-P** (Z-Stack) ไม่ใช่ -E (Ember) — แก้:
```bash
sudo systemctl stop zigbee2mqtt
sed -i 's/adapter: ember/adapter: zstack/' /opt/zigbee2mqtt-docker/data/configuration.yaml
sudo systemctl start zigbee2mqtt
docker logs zigbee2mqtt -f
```

ตรวจ Z2M boot log บอก firmware type:
- `EmberZNet 7.4.4` → adapter: `ember`
- `Z-Stack 3.x.0` / "CC2652" → adapter: `zstack`

### 7. Pair Zigbee devices — 10 นาที
เปิด browser:
```
http://<PI-IP>:8080
```

1. กด **"Permit join (All)"** มุมขวาบน (เปิด 254 วินาที)
2. กดปุ่ม pair บนแต่ละ device (ใกล้ Pi 1-2 เมตร):
   | Device | วิธี pair |
   |---|---|
   | IKEA TRÅDFRI E27 | เปิด/ปิดสวิตช์ไฟ 6 ครั้งติด ภายใน 10 วิ |
   | Sonoff S26R2ZB ×2 | กดปุ่มข้าง 5 วินาที (ไฟกระพริบ) |
   | Sonoff SNZB-03P | กดปุ่มหลัง 5 วินาที |
   | Sonoff SNZB-05 | กดปุ่มหลัง 5 วินาที |
3. ตั้ง **friendly_name** ใน web UI:
   ```
   ikea_bulb         (IKEA TRÅDFRI)
   demo_plug_1       (Sonoff Plug #1)
   demo_plug_2       (Sonoff Plug #2)
   motion_sensor     (SNZB-03P)
   water_leak        (SNZB-05)
   ```
4. **Disable Permit Join** หลัง pair ครบ

### 8. เช็ค HA เห็น devices — 2 นาที
เปิด `http://YOUR_EC2_IP:8123` → **Settings → Devices & Services → MQTT**
จะเห็น "Zigbee2MQTT Bridge" + devices ที่ pair อัตโนมัติ (HA auto-discovery)

---

## 🔧 Troubleshooting

### Pi reboot บ่อย / ค้าง
```bash
# เช็คก่อนอื่นเสมอ
ssh pi@<PI-IP> 'vcgencmd get_throttled'
# 0x0 = ไฟดี
# 0x50005 หรือ bit 0/2 ติด = under-voltage → เปลี่ยน power supply
```

### Pi ไม่ขึ้นใน `smarthome-pi.local`
```bash
# Mac: หา IP ของ Pi (Pi 3B+ MAC starts with B8:27:EB)
arp -a | grep -i b8:27:eb
ssh pi@<IP>
```

### Z2M container restart loop — "SRSP - SYS - ping" error
ดู step 6 ด้านบน — adapter type ผิด (`ember` vs `zstack`)

### Z2M container restart loop — serial port permission
```bash
ssh pi@<PI-IP>
ls -la /dev/serial/by-id/
# ตรวจ device file มี perm rw-rw-r-- หรือไม่
sudo usermod -aG dialout pi
sudo systemctl restart zigbee2mqtt
```

### MQTT connection refused (EC2)
```bash
# ทดสอบจาก Pi
mosquitto_sub -h YOUR_EC2_IP -p 1883 -t '#' -v
# ถ้า "Connection refused" — เช็ค EC2 security group port 1883 open
```

### USB reset dongle (ถ้า hang)
```bash
ssh pi@<PI-IP>
sudo systemctl stop zigbee2mqtt
# หา USB path
for d in /sys/bus/usb/devices/*/; do
  v=$(cat "$d/idVendor" 2>/dev/null)
  if [ "$v" = "10c4" ]; then echo "$d"; fi
done
# reset
echo 0 | sudo tee /sys/bus/usb/devices/1-1.2/authorized
sleep 2
echo 1 | sudo tee /sys/bus/usb/devices/1-1.2/authorized
sudo systemctl start zigbee2mqtt
```

### Devices ไม่ pair
- เข้าใกล้ Pi ภายใน 1-2 เมตรตอน pair
- รีเซ็ตอุปกรณ์ก่อน (กดปุ่มค้าง 10 วิ)
- ลองเปลี่ยน `channel: 15` หรือ `25` ใน `configuration.yaml` (default 11 อาจชน Wi-Fi)

### Docker pull ช้า / fail
```bash
# Wi-Fi hotspot iPhone หลุดง่าย — retry
ssh pi@<PI-IP>
sudo docker pull koenkk/zigbee2mqtt:latest
```

---

## 📡 MQTT Topics หลัง pair เสร็จ

Z2M จะ publish ที่ EC2 broker:
```
zigbee2mqtt/ikea_bulb           {"state": "ON", "brightness": 254, "color_temp": 250}
zigbee2mqtt/demo_plug_1         {"state": "ON", "power": 42.3, "voltage": 230, "energy": 1.23}
zigbee2mqtt/demo_plug_2         {"state": "OFF", "power": 0, "energy": 0.45}
zigbee2mqtt/motion_sensor       {"occupancy": true, "battery": 95}
zigbee2mqtt/water_leak          {"water_leak": false, "battery": 100}
zigbee2mqtt/bridge/state        "online"
```

ทดสอบจาก Mac:
```bash
mosquitto_sub -h YOUR_EC2_IP -p 1883 -t 'zigbee2mqtt/#' -v
```

ส่งคำสั่งเปิดหลอด IKEA:
```bash
mosquitto_pub -h YOUR_EC2_IP -p 1883 \
  -t 'zigbee2mqtt/ikea_bulb/set' \
  -m '{"state": "ON", "brightness": 200}'
```

---

## ✅ Checklist ก่อน demo (26 พ.ค.)

- [x] SD flash + boot + SSH ได้
- [x] Power supply เสถียร (`throttled=0x0`)
- [x] Docker + Z2M container active
- [x] Z2M เชื่อม EC2 MQTT (YOUR_EC2_IP:1883)
- [x] HA auto-discovery published
- [ ] Pair 5 devices สำเร็จ (รอ pair)
- [ ] HA เห็น devices ใน MQTT integration
- [ ] ทดสอบเปิด/ปิดหลอด IKEA จาก HA dashboard
- [ ] LINE bot สั่ง "เปิดหลอด IKEA" ได้ผ่านชาวี
- [ ] Auto-watering / fall detection ใช้งานได้ตามปกติ

---

## 🧠 Key lessons (จาก install 13 พ.ค. 2026)

1. **`vcgencmd get_throttled` ก่อนเริ่ม** — `0x0` = ไฟดี อื่นๆ = power issue
2. **Docker > npm install** บน Pi RAM 1GB — เลี่ยง build/compile pressure
3. **Sonoff ZBDongle V2 มี 2 variants** — เช็ค firmware จาก Z2M boot log (Ember vs Z-Stack)
4. **`systemd-run --unit=` > `nohup`** — detach สมบูรณ์เมื่อรัน install/pull ผ่าน SSH
5. **SSH key + NOPASSWD sudo** ตอน start setup — ลด friction มหาศาล

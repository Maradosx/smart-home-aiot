#!/bin/bash
# ============================================================
# Smart Home AIoT — Pi 3B+ Zigbee Bridge Setup (Docker-based)
# ============================================================
# Sonoff ZBDongle-E → Zigbee2MQTT Docker → EC2 Mosquitto MQTT
#
# Last verified: 13 พ.ค. 2026 — Debian Trixie, Pi 3B+, ZBDongle-E
# ============================================================

set -e

EC2_IP="YOUR_EC2_IP"
EC2_PORT="1883"
PI_USER="${SUDO_USER:-${USER:-pi}}"
Z2M_IMAGE="koenkk/zigbee2mqtt:latest"
Z2M_DATA_DIR="/opt/zigbee2mqtt-docker/data"

echo "╔════════════════════════════════════════════╗"
echo "║  Smart Home AIoT — Pi Zigbee Bridge        ║"
echo "║  Target EC2: ${EC2_IP}:${EC2_PORT}          ║"
echo "╚════════════════════════════════════════════╝"
echo ""

# ─── PRECHECK: ไฟดี (under-voltage = STOP) ──────────────────
echo "[precheck] เช็ค throttled..."
THROTTLED=$(vcgencmd get_throttled | cut -d= -f2)
echo "  throttled=$THROTTLED"
if [ "$THROTTLED" != "0x0" ]; then
    # bit 0 = uv now, bit 2 = throttled now
    UV_NOW=$(( (THROTTLED & 0x1) ))
    THROT_NOW=$(( (THROTTLED & 0x4) ))
    if [ "$UV_NOW" -ne 0 ] || [ "$THROT_NOW" -ne 0 ]; then
        echo ""
        echo "🚨 ไฟตก/throttled อยู่ตอนนี้ — STOP"
        echo "   Pi 3B+ ต้องใช้ wall adapter 5V/2.5A ตรง"
        echo "   อย่าใช้ USB จาก Mac / hub / powerbank"
        echo "   เช็คใหม่: vcgencmd get_throttled (ต้อง = 0x0)"
        exit 1
    else
        echo "  ⚠️  มีประวัติไฟตกก่อนหน้า แต่ตอนนี้ ok — ทำต่อได้"
    fi
fi
echo "  ✅ ไฟดี"
echo ""

# ─── STEP 1: apt update + install Docker + tools ────────────
echo "[1/6] apt update + install Docker, mosquitto-clients..."
apt-get update
apt-get install -y docker.io mosquitto-clients curl ca-certificates
usermod -aG docker "$PI_USER"
usermod -aG dialout "$PI_USER"
systemctl enable --now docker
docker --version
echo ""

# ─── STEP 2: หา ZBDongle path ────────────────────────────────
echo "[2/6] หา Sonoff ZBDongle path..."
ZB_DEVICE=$(ls /dev/serial/by-id/ 2>/dev/null | grep -iE "Sonoff|CC2652|ZBDongle|CP2102|Silicon_Labs" | head -1)
if [ -n "$ZB_DEVICE" ]; then
    ZB_PATH="/dev/serial/by-id/$ZB_DEVICE"
    echo "  ✅ พบ: $ZB_PATH"
else
    echo "  ⚠️  ไม่เจอ dongle — เสียบ USB หรือยัง? ทำต่อด้วย /dev/ttyACM0"
    ZB_PATH="/dev/ttyACM0"
fi
echo ""

# ─── STEP 3: Pull Z2M Docker image ──────────────────────────
echo "[3/6] Pull Z2M Docker image (~150MB)..."
docker pull "$Z2M_IMAGE"
echo ""

# ─── STEP 4: เขียน configuration.yaml ────────────────────────
echo "[4/6] เขียน config..."
mkdir -p "$Z2M_DATA_DIR"
chown -R "$PI_USER:$PI_USER" "$(dirname "$Z2M_DATA_DIR")"

# Default ใช้ ember — ถ้าเป็น ZBDongle-P (Z-Stack) ให้แก้เป็น zstack ทีหลัง
cat > "$Z2M_DATA_DIR/configuration.yaml" <<EOF
# Zigbee2MQTT Configuration — Smart Home AIoT
# Last updated: $(date +%Y-%m-%d)

version: 4

homeassistant:
  enabled: true
  discovery_topic: homeassistant
  status_topic: homeassistant/status

mqtt:
  base_topic: zigbee2mqtt
  server: mqtt://${EC2_IP}:${EC2_PORT}
  client_id: zigbee2mqtt-pi

serial:
  port: /dev/ttyACM0
  # ⚠️ adapter type: 'ember' = ZBDongle-E (EmberZNet); 'zstack' = ZBDongle-P (CC2652P)
  # ดู Z2M boot log บอก firmware (EmberZNet vs Z-Stack) เปลี่ยนตามจริง
  adapter: ember

frontend:
  enabled: true
  port: 8080
  host: 0.0.0.0

advanced:
  log_level: info
  log_output:
    - console
    - file
  channel: 11
  network_key: GENERATE
  pan_id: GENERATE
  ext_pan_id: GENERATE

permit_join: false

device_options:
  legacy: false
EOF

echo "  ✅ Config: $Z2M_DATA_DIR/configuration.yaml"
echo ""

# ─── STEP 5: systemd service ────────────────────────────────
echo "[5/6] สร้าง systemd service zigbee2mqtt.service..."
tee /etc/systemd/system/zigbee2mqtt.service > /dev/null <<EOF
[Unit]
Description=Zigbee2MQTT (Docker)
After=docker.service network-online.target
Requires=docker.service

[Service]
Type=simple
Restart=always
RestartSec=10s
ExecStartPre=-/usr/bin/docker rm -f zigbee2mqtt
ExecStart=/usr/bin/docker run --rm --name zigbee2mqtt \\
  --network host \\
  -v ${Z2M_DATA_DIR}:/app/data \\
  --device=${ZB_PATH}:/dev/ttyACM0 \\
  -e TZ=Asia/Bangkok \\
  ${Z2M_IMAGE}
ExecStop=/usr/bin/docker stop zigbee2mqtt

[Install]
WantedBy=multi-user.target
EOF

systemctl daemon-reload
systemctl enable --now zigbee2mqtt
echo ""

# ─── STEP 6: รอ Z2M start + ตรวจ ────────────────────────────
echo "[6/6] รอ Z2M start..."
sleep 25

echo ""
echo "╔════════════════════════════════════════════╗"
echo "║         ✅ ติดตั้งเสร็จสมบูรณ์              ║"
echo "╚════════════════════════════════════════════╝"
echo ""

if systemctl is-active --quiet zigbee2mqtt; then
    echo "✅ zigbee2mqtt service: active"
else
    echo "❌ zigbee2mqtt service: $(systemctl is-active zigbee2mqtt)"
    echo "ดู error: sudo journalctl -u zigbee2mqtt -n 50 --no-pager"
fi

echo ""
docker ps --filter name=zigbee2mqtt 2>/dev/null
echo ""
echo "--- log ล่าสุด ---"
docker logs zigbee2mqtt 2>&1 | tail -20 || true
echo ""

echo "🌐 Web UI:  http://$(hostname -I | awk '{print $1}'):8080"
echo "📝 ดู log:  sudo journalctl -u zigbee2mqtt -f"
echo "🔗 ทดสอบ MQTT: mosquitto_sub -h ${EC2_IP} -p ${EC2_PORT} -t 'zigbee2mqtt/#' -v"
echo ""
echo "⚠️  ถ้า log แสดง 'SRSP - SYS - ping after 6000ms':"
echo "    dongle อาจเป็น ZBDongle-P (Z-Stack) ไม่ใช่ -E (Ember) — แก้:"
echo "    sudo sed -i 's/adapter: ember/adapter: zstack/' ${Z2M_DATA_DIR}/configuration.yaml"
echo "    sudo systemctl restart zigbee2mqtt"
echo ""
echo "📡 ขั้นต่อไป:"
echo "    1. เปิด http://$(hostname -I | awk '{print $1}'):8080"
echo "    2. กด 'Permit Join (All)' — เปิด 254 วินาที"
echo "    3. กดปุ่ม pair บนแต่ละ device:"
echo "       - IKEA TRÅDFRI E27: เปิด/ปิดสวิตช์ 6 ครั้งติด"
echo "       - Sonoff S26R2ZB:   กดปุ่มข้าง 5 วินาที"
echo "       - Sonoff SNZB-03P/05: กดปุ่มหลัง 5 วินาที"
echo "    4. ตั้ง friendly_name ใน web UI"
echo "    5. HA จะเห็น devices อัตโนมัติผ่าน MQTT auto-discovery"

#!/bin/bash
# ============================================================
# setup.sh — Smart Home AIoT Stack Setup (macOS)
# รัน: chmod +x setup.sh && ./setup.sh
# ============================================================

set -e
echo "🏠 Smart Home AIoT — macOS Setup"
echo "================================="

# ─── Check Docker ─────────────────────────────────────────────
if ! command -v docker &> /dev/null; then
  echo "❌ Docker ไม่พบ — ติดตั้งที่ https://www.docker.com/products/docker-desktop/"
  exit 1
fi
echo "✅ Docker found: $(docker --version)"

# ─── Create volume directories ────────────────────────────────
echo ""
echo "📁 Creating volume directories..."
mkdir -p ./volumes/homeassistant
mkdir -p ./volumes/mosquitto/{config,data,log}
mkdir -p ./volumes/zigbee2mqtt
mkdir -p ./volumes/n8n
echo "✅ Directories created"

# ─── Copy Mosquitto config ────────────────────────────────────
if [ ! -f ./volumes/mosquitto/config/mosquitto.conf ]; then
  cp mosquitto.conf ./volumes/mosquitto/config/mosquitto.conf
  echo "✅ Mosquitto config copied"
fi

# ─── Copy HA config ───────────────────────────────────────────
if [ ! -f ./volumes/homeassistant/configuration.yaml ]; then
  cp HA_configuration.yaml ./volumes/homeassistant/configuration.yaml
  cp HA_automations.yaml    ./volumes/homeassistant/automations.yaml
  cp HA_ui-lovelace.yaml    ./volumes/homeassistant/ui-lovelace.yaml
  echo "✅ Home Assistant configs copied"
fi

# ─── Create .env if not exists ────────────────────────────────
if [ ! -f .env ]; then
  cp .env.example .env
  echo ""
  echo "⚠️  .env ถูกสร้างแล้ว — กรุณาแก้ไขค่าใน .env ก่อน start:"
  echo "   CLAUDE_API_KEY=sk-ant-api03-..."
  echo "   LINE_CHANNEL_ACCESS_TOKEN=..."
  echo ""
  echo "แก้ไขเสร็จแล้วรัน: docker compose up -d"
else
  echo "✅ .env exists"
fi

# ─── Find Zigbee USB dongle ───────────────────────────────────
echo ""
echo "🔌 กำลังหา USB Zigbee dongle..."
ls /dev/tty.usbserial-* 2>/dev/null || ls /dev/tty.SLAB_USBtoUART* 2>/dev/null || \
  echo "   ไม่พบ USB device — เสียบ Zigbee dongle แล้วรันอีกครั้ง"
echo "   แก้ path ใน docker-compose.yml → devices section ของ zigbee2mqtt"

echo ""
echo "📋 Services จะเปิดที่:"
echo "   Home Assistant  → http://localhost:8123"
echo "   n8n             → http://localhost:5678"
echo "   Zigbee2MQTT     → http://localhost:8080"
echo "   MQTT Broker     → localhost:1883"
echo ""
echo "🚀 รัน: docker compose up -d"

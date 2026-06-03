#!/bin/bash
# 🔧 Flash ESP32 #1 (Bedroom) with curtain direction fix (25 พ.ค. 2569)
# Usage: เสียบ ESP32 #1 เข้า Mac via USB → รัน ./flash_bedroom.sh

set -e

FIRMWARE_DIR="/Users/path_mos/Desktop/Mos/Project_1/esp32_firmware/เวอร์ชั่นมีmotor/esp32_01_bedroom"
BOARD="esp32:esp32:esp32"

echo "🔍 หา ESP32 USB port..."
PORT=$(ls /dev/cu.usbserial-* /dev/cu.SLAB_USBtoUART /dev/cu.wchusbserial* 2>/dev/null | head -1)

if [ -z "$PORT" ]; then
    echo "❌ ไม่เจอ ESP32 ต่อ USB"
    echo "   เสียบ ESP32 #1 (Bedroom) เข้า Mac แล้วลองใหม่"
    exit 1
fi
echo "✅ เจอที่ $PORT"

echo ""
echo "🏗  Compile..."
arduino-cli compile -b "$BOARD" "$FIRMWARE_DIR"

echo ""
echo "⬆️  Upload..."
arduino-cli upload -p "$PORT" -b "$BOARD" "$FIRMWARE_DIR"

echo ""
echo "✅ Flash done! ESP32 reboot ใน 3 วินาที..."
echo ""
echo "🧪 ทดสอบทันที:"
echo "  1. เปิด HA Dashboard → cover.bedroom_curtain"
echo "  2. กด ▲ (open) → ม่านควรดึงขึ้น (พับเก็บ — หน้าต่างโล่ง)"
echo "  3. กด ▼ (close) → ม่านควรดึงลง (คลุมหน้าต่าง)"

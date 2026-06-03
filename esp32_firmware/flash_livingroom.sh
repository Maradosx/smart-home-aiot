#!/bin/bash
# 🔧 Flash ESP32 #2 (Living Room) — fix door lock periodic refresh (25 พ.ค. 2569)
# Usage: เสียบ ESP32 #2 เข้า Mac via USB → รัน ./flash_livingroom.sh

set -e

FIRMWARE_DIR="/Users/path_mos/Desktop/Mos/Project_1/esp32_firmware/เวอร์ชั่นมีmotor/esp32_02_livingroom"
BOARD="esp32:esp32:esp32"

echo "🔍 หา ESP32 USB port..."
PORT=$(ls /dev/cu.usbserial-* /dev/cu.SLAB_USBtoUART /dev/cu.wchusbserial* 2>/dev/null | head -1)

if [ -z "$PORT" ]; then
    echo "❌ ไม่เจอ ESP32 ต่อ USB"
    echo "   เสียบ ESP32 #2 (Living Room) เข้า Mac แล้วลองใหม่"
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
echo "🧪 ผลลัพธ์ที่ควรเป็น:"
echo "  - servo ไม่ขยับเองทุก 5 วินาที (เงียบสนิทหลัง lock/unlock)"
echo "  - HA Dashboard: lock.front_door_lock → กด lock/unlock ทำงานปกติ"
echo "  - Serial Monitor: เห็น \"detached — quiet mode\" หลัง move เสร็จ"

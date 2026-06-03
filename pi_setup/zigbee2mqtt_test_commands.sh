#!/bin/bash
# ============================================================
# Zigbee2MQTT — Test Commands (รันจาก Mac หลัง Pi setup เสร็จ)
# ============================================================
# Quick smoke tests + demo commands สำหรับวัน present
# ============================================================

EC2="YOUR_EC2_IP"
PORT="1883"

echo "╔════════════════════════════════════════╗"
echo "║  Zigbee2MQTT Test Commands             ║"
echo "╚════════════════════════════════════════╝"
echo ""
echo "เลือก:"
echo "  1) ดู MQTT topics ทั้งหมด (subscribe '#')"
echo "  2) ดูเฉพาะ Zigbee topics"
echo "  3) ดู Zigbee2MQTT bridge state"
echo "  4) เปิดหลอด IKEA"
echo "  5) ปิดหลอด IKEA"
echo "  6) เปิด smart plug 1"
echo "  7) ปิด smart plug 1"
echo "  8) Permit join 60 วินาที"
echo "  9) Stop permit join"
echo " 10) ดู devices ที่ pair แล้ว"
echo ""
read -p "เลือก [1-10]: " choice

case $choice in
  1)  mosquitto_sub -h $EC2 -p $PORT -t '#' -v ;;
  2)  mosquitto_sub -h $EC2 -p $PORT -t 'zigbee2mqtt/#' -v ;;
  3)  mosquitto_sub -h $EC2 -p $PORT -t 'zigbee2mqtt/bridge/state' -v -C 1 ;;
  4)  mosquitto_pub -h $EC2 -p $PORT -t 'zigbee2mqtt/ikea_bulb/set' \
        -m '{"state": "ON", "brightness": 200}' && echo "✅ ส่งคำสั่งเปิดแล้ว" ;;
  5)  mosquitto_pub -h $EC2 -p $PORT -t 'zigbee2mqtt/ikea_bulb/set' \
        -m '{"state": "OFF"}' && echo "✅ ส่งคำสั่งปิดแล้ว" ;;
  6)  mosquitto_pub -h $EC2 -p $PORT -t 'zigbee2mqtt/demo_plug_1/set' \
        -m '{"state": "ON"}' && echo "✅ Plug 1 ON" ;;
  7)  mosquitto_pub -h $EC2 -p $PORT -t 'zigbee2mqtt/demo_plug_1/set' \
        -m '{"state": "OFF"}' && echo "✅ Plug 1 OFF" ;;
  8)  mosquitto_pub -h $EC2 -p $PORT -t 'zigbee2mqtt/bridge/request/permit_join' \
        -m '{"value": true, "time": 60}' && echo "✅ Permit join 60 วิ" ;;
  9)  mosquitto_pub -h $EC2 -p $PORT -t 'zigbee2mqtt/bridge/request/permit_join' \
        -m '{"value": false}' && echo "✅ ปิด permit join" ;;
  10) mosquitto_sub -h $EC2 -p $PORT -t 'zigbee2mqtt/bridge/devices' -C 1 \
        | python3 -m json.tool ;;
  *)  echo "❌ ตัวเลือกไม่ถูกต้อง" ;;
esac

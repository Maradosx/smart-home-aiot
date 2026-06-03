#!/usr/bin/env bash
# ============================================================
# Open Z2M Web UI — auto-discover Pi + check service + open browser
# ============================================================
# Usage: ./open_z2m.sh
#
# Prerequisite:
#   1) Pi 3B+ ต่อ wall adapter 5V/2.5A (ไม่ใช่ powerbank)
#   2) Pi boot เสร็จ (รอ ~60 วินาที หลังเสียบไฟ)
#   3) Mac กับ Pi อยู่ network เดียวกัน
#      - แนะนำ: เปิด iPhone hotspot แล้ว Mac เชื่อม hotspot นั้น
#        (Pi จะกลับมาเชื่อม hotspot นี้อัตโนมัติเพราะมี wpa_supplicant config)
# ============================================================

set -u

c_blue()  { printf "\033[1;34m%s\033[0m\n" "$1"; }
c_green() { printf "\033[1;32m%s\033[0m\n" "$1"; }
c_yellow(){ printf "\033[1;33m%s\033[0m\n" "$1"; }
c_red()   { printf "\033[1;31m%s\033[0m\n" "$1"; }

EC2_BROKER="YOUR_EC2_IP"
KNOWN_IPS=(
  "172.20.10.4"        # iPhone hotspot (มีใน memory)
  "192.168.0.10"
  "192.168.1.10"
  "raspberrypi.local"  # mDNS hostname
)

# ─── Step 1: check Mac WiFi ───────────────────────────────
c_blue "════════════════════════════════════════════════"
c_blue "  Z2M Web UI launcher"
c_blue "════════════════════════════════════════════════"

MAC_IP=$(ipconfig getifaddr en0 2>/dev/null || echo "?")
echo ""
echo "Mac WiFi IP: $MAC_IP"
SSID=$(/System/Library/PrivateFrameworks/Apple80211.framework/Versions/Current/Resources/airport -I 2>/dev/null | awk '/ SSID/ {print $2}' || echo "?")
echo "Mac SSID:    ${SSID:-unknown}"

# ─── Step 2: check Z2M via MQTT bridge state ──────────────
echo ""
c_blue "─── Step 1: เช็ค Z2M สถานะผ่าน MQTT broker ───"
STATE=$(perl -e 'alarm 4; exec @ARGV' mosquitto_sub -h "$EC2_BROKER" -p 1883 \
        -t 'zigbee2mqtt/bridge/state' -C 1 2>/dev/null || echo "")

if [[ "$STATE" == *"online"* ]]; then
  c_green "✓ Z2M online — Pi กับ Docker service ทำงานอยู่"
elif [[ "$STATE" == *"offline"* ]]; then
  c_red "✗ Z2M offline — Pi อาจจะ:"
  c_yellow "  1) ปิดอยู่ → เปิด wall adapter"
  c_yellow "  2) บูตยังไม่เสร็จ → รออีก 60 วินาที"
  c_yellow "  3) zigbee2mqtt service down → ต้อง SSH ไปเช็ค"
  c_yellow "     (sudo systemctl status zigbee2mqtt)"
  exit 1
else
  c_red "✗ ไม่ได้ response จาก MQTT — เช็ค:"
  c_yellow "  1) Pi เปิดอยู่ไหม?"
  c_yellow "  2) Pi เชื่อม WiFi ได้ไหม?"
  c_yellow "  3) MQTT broker YOUR_EC2_IP reachable? (echo: $(nc -z -w 2 $EC2_BROKER 1883 && echo OK || echo FAIL))"
  exit 1
fi

# ─── Step 3: หา Pi IP บน LAN ──────────────────────────────
echo ""
c_blue "─── Step 2: หา IP ของ Pi ใน LAN ───"
PI_IP=""

# Try known IPs first
for ip in "${KNOWN_IPS[@]}"; do
  if nc -z -w 1 "$ip" 8080 2>/dev/null; then
    PI_IP="$ip"
    c_green "✓ Pi web UI พบที่: $PI_IP:8080"
    break
  fi
done

# Fallback: ARP scan in same subnet
if [ -z "$PI_IP" ] && [ "$MAC_IP" != "?" ]; then
  SUBNET=$(echo "$MAC_IP" | awk -F. '{print $1"."$2"."$3}')
  c_yellow "  ↳ ลอง ARP scan ใน subnet $SUBNET.0/24 (10 วินาที)..."

  # Quick ping sweep to populate ARP cache
  for n in $(seq 1 254); do
    (ping -c 1 -W 100 "$SUBNET.$n" >/dev/null 2>&1 &)
  done
  sleep 6
  wait 2>/dev/null

  # Look for Raspberry Pi MAC OUI in ARP table
  # Pi OUI: B8:27:EB (Pi 1/2/3), DC:A6:32 (Pi 4), E4:5F:01 (Pi 5)
  CAND=$(arp -an 2>/dev/null | grep -E "b8:27:eb|dc:a6:32|e4:5f:01" | head -1 | awk '{print $2}' | tr -d '()')

  if [ -n "$CAND" ] && nc -z -w 2 "$CAND" 8080 2>/dev/null; then
    PI_IP="$CAND"
    c_green "✓ พบ Pi via ARP: $PI_IP"
  fi
fi

if [ -z "$PI_IP" ]; then
  c_red "✗ หา Pi ไม่เจอใน LAN ปัจจุบัน"
  echo ""
  c_yellow "ปัจจุบัน Mac อยู่บน:        $MAC_IP (SSID=$SSID)"
  c_yellow "Z2M online ผ่าน MQTT:    ✓ (Pi เปิดอยู่ที่ network อื่น)"
  echo ""
  c_yellow "Mac อาจจะอยู่คนละ WiFi กับ Pi!"
  c_yellow "  → เปลี่ยน Mac ไปเชื่อม WiFi เดียวกับ Pi"
  c_yellow "    (ดูจาก Pi: ปกติเชื่อม iPhone hotspot)"
  echo ""
  c_yellow "หรือลองใส่ Pi IP ตรงๆ:"
  c_yellow "  open -a 'Google Chrome' http://<PI-IP>:8080"
  exit 2
fi

# ─── Step 4: เปิด browser ─────────────────────────────────
URL="http://$PI_IP:8080"
echo ""
c_blue "─── Step 3: เปิด Z2M Web UI ───"
c_green "→ $URL"
open -a "Google Chrome" "$URL"

echo ""
c_green "════════════════════════════════════════════════"
c_green "  ✓ เปิด Z2M Web UI ใน Chrome แล้ว"
c_green "════════════════════════════════════════════════"
echo ""
c_yellow "หากเข้าไม่ได้:"
c_yellow "  1) Mac อยู่ network เดียวกับ Pi ไหม? (en0: $MAC_IP)"
c_yellow "  2) Pi firewall เปิด 8080 ไหม? (ปกติ Raspbian ไม่มี firewall)"
c_yellow "  3) ดู service status:"
c_yellow "     ssh pi@$PI_IP 'sudo systemctl status zigbee2mqtt'"
echo ""
c_blue "Z2M commands ผ่าน MQTT (ทำได้แม้ไม่มี web UI):"
c_blue "  Pair new:  mosquitto_pub -h $EC2_BROKER -t 'zigbee2mqtt/bridge/request/permit_join' -m '{\"value\":true,\"time\":120}'"
c_blue "  ดู devices: mosquitto_sub -h $EC2_BROKER -t 'zigbee2mqtt/bridge/devices' -C 1"
c_blue "  Restart:   mosquitto_pub -h $EC2_BROKER -t 'zigbee2mqtt/bridge/request/restart' -m ''"

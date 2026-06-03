#!/usr/bin/env bash
# ============================================================
# Pi 3B+ Dual-Role: WiFi Client + Backup AP
# ============================================================
# wlan0 (client) → Chawee   (192.168.x.x จาก router บ้าน)
# ap0    (AP)     → SmartHome-Local (192.168.50.0/24, Pi=192.168.50.1)
#
# คอนเซป: ถ้า Chawee ล่ม → ESP32 fallback อัตโนมัติมาที่ ap0
# ESP32 firmware รองรับแล้ว (WIFI_SSID_2 = "SmartHome-Local")
#
# ⚠️ รันบน Pi (Raspbian) เท่านั้น — ผ่าน sudo
# Usage: sudo ./setup_pi_ap.sh
# ============================================================

set -e

if [ "$EUID" -ne 0 ]; then
  echo "❌ ต้องรันด้วย sudo"
  exit 1
fi

echo "════════════════════════════════════════════════"
echo "  Pi Backup AP Setup"
echo "════════════════════════════════════════════════"

# ─── 1) ติดตั้ง packages ──────────────────────────
echo ""
echo "[1/6] Install hostapd + dnsmasq..."
apt-get update -qq
apt-get install -y hostapd dnsmasq iw

systemctl unmask hostapd
systemctl disable hostapd
systemctl stop hostapd
systemctl stop dnsmasq

# ─── 2) สร้าง virtual interface ap0 ──────────────
echo ""
echo "[2/6] สร้าง virtual AP interface (ap0)..."

# udev rule — สร้าง ap0 อัตโนมัติทุกครั้งที่ boot
cat > /etc/udev/rules.d/70-persistent-net.rules <<'EOF'
SUBSYSTEM=="ieee80211", ACTION=="add|change", ATTR{macaddress}=="*", KERNEL=="phy0", \
  RUN+="/sbin/iw phy phy0 interface add ap0 type __ap", \
  RUN+="/bin/ip link set ap0 address $(/sbin/ifconfig wlan0 | grep -o 'ether [a-f0-9:]*' | cut -d' ' -f2 | sed 's/^./e/')"
EOF

# สร้าง ap0 ทันที (ไม่ต้องรอ reboot)
iw dev wlan0 interface add ap0 type __ap 2>/dev/null || \
  echo "  (ap0 มีอยู่แล้ว หรือ chipset ไม่รองรับ — ดู: iw list | grep -A5 'valid interface combinations')"

# ─── 3) hostapd.conf (สร้าง WiFi "SmartHome-Local") ─
echo ""
echo "[3/6] เขียน hostapd config..."
cat > /etc/hostapd/hostapd.conf <<'EOF'
# ────────────────────────────────────────────
# Pi Backup AP — "SmartHome-Local"
# ────────────────────────────────────────────
interface=ap0
driver=nl80211
ssid=SmartHome-Local
hw_mode=g
channel=6
ieee80211n=1
wmm_enabled=1
ht_capab=[HT40][SHORT-GI-20][DSSS_CCK-40]
macaddr_acl=0
auth_algs=1
ignore_broadcast_ssid=0
wpa=2
wpa_passphrase=YOUR_AP_PASSWORD
wpa_key_mgmt=WPA-PSK
wpa_pairwise=TKIP
rsn_pairwise=CCMP
# จำกัด client (ESP32 5 ตัว) — กัน overload
max_num_sta=10
EOF

sed -i 's|^#DAEMON_CONF=.*|DAEMON_CONF="/etc/hostapd/hostapd.conf"|' /etc/default/hostapd

# ─── 4) dnsmasq (DHCP server สำหรับ ap0) ─────────
echo ""
echo "[4/6] เขียน dnsmasq config (DHCP สำหรับ AP)..."
# Backup
[ -f /etc/dnsmasq.conf ] && cp /etc/dnsmasq.conf /etc/dnsmasq.conf.bak.$(date +%s) 2>/dev/null

cat > /etc/dnsmasq.conf <<'EOF'
# ────────────────────────────────────────────
# DHCP for Pi Backup AP (ap0)
# ────────────────────────────────────────────
interface=ap0
bind-interfaces
listen-address=192.168.50.1
domain-needed
bogus-priv
dhcp-range=192.168.50.10,192.168.50.50,255.255.255.0,24h
# ESP32 static (จำ IP คงที่ — ดูใน HA ง่าย)
dhcp-host=esp32-bedroom,192.168.50.21
dhcp-host=esp32-livingroom,192.168.50.22
dhcp-host=esp32-kitchen,192.168.50.23
dhcp-host=esp32-bathroom-garden,192.168.50.24
dhcp-host=esp32-garage,192.168.50.25
EOF

# ─── 5) Static IP สำหรับ ap0 (dhcpcd) ───────────
echo ""
echo "[5/6] ตั้ง static IP ap0=192.168.50.1..."
# ลบ entry เดิม (ถ้ามี)
sed -i '/^interface ap0/,/^$/d' /etc/dhcpcd.conf 2>/dev/null

cat >> /etc/dhcpcd.conf <<'EOF'

# ── Pi AP backup interface ──
interface ap0
    static ip_address=192.168.50.1/24
    nohook wpa_supplicant
EOF

# ─── 6) Enable IP forwarding + NAT (ให้ ESP32 ผ่าน Pi ออก internet) ─
echo ""
echo "[6/6] เปิด IP forwarding + NAT..."
sed -i 's|^#net.ipv4.ip_forward=1|net.ipv4.ip_forward=1|' /etc/sysctl.conf
sysctl -p

# Persistent NAT rule
iptables -t nat -A POSTROUTING -o wlan0 -j MASQUERADE
iptables -A FORWARD -i wlan0 -o ap0 -m state --state RELATED,ESTABLISHED -j ACCEPT
iptables -A FORWARD -i ap0 -o wlan0 -j ACCEPT

# Save rules (boot persistent)
apt-get install -y iptables-persistent 2>/dev/null || true
netfilter-persistent save 2>/dev/null || iptables-save > /etc/iptables.rules

# ─── Enable services ─────────────────────────────
systemctl enable hostapd
systemctl enable dnsmasq
systemctl start hostapd
systemctl start dnsmasq

echo ""
echo "════════════════════════════════════════════════"
echo "  ✓ ติดตั้งเสร็จ"
echo "════════════════════════════════════════════════"
echo ""
echo "ตอนนี้ Pi มี 2 WiFi interface:"
echo "  • wlan0 (client) → เชื่อม 'Chawee' (router บ้าน)"
echo "  • ap0    (AP)     → ปล่อย 'SmartHome-Local' (สำรอง)"
echo ""
echo "ตรวจสอบ:"
echo "  ifconfig | grep -A2 'wlan0\\|ap0'"
echo "  systemctl status hostapd dnsmasq"
echo "  iw dev"
echo ""
echo "ESP32 จะใช้ SmartHome-Local อัตโนมัติเมื่อ Chawee ไม่ตอบ"
echo "(connectWiFi() ใน firmware retry primary 10 วินาที → fallback)"
echo ""
echo "⚠️ ถ้า ap0 สร้างไม่ได้ → reboot Pi 1 ครั้ง: sudo reboot"

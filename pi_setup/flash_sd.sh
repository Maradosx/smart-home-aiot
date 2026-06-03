#!/bin/bash
# ============================================================
# Smart Home AIoT — Manual SD Flash Script
# ============================================================
# Bypass Pi Imager — เขียน OS ลง SD ผ่าน dd (low-level direct)
# ใช้เมื่อ Pi Imager ค้าง / fail (USB reader ปัญหา)
# ============================================================

set -e

IMAGE_DIR="$HOME/Downloads"
IMAGE_XZ="$IMAGE_DIR/raspios-lite-arm64.img.xz"
IMAGE_IMG="$IMAGE_DIR/raspios-lite-arm64.img"
SD_DEVICE="/dev/disk12"
SD_RAW="/dev/rdisk12"   # raw device — เร็วกว่า

echo "╔══════════════════════════════════════════╗"
echo "║   Smart Home AIoT — Manual SD Flash     ║"
echo "╚══════════════════════════════════════════╝"
echo ""

# ─── Step 1: Verify image exists ──────────────────────────
if [ ! -f "$IMAGE_XZ" ]; then
    echo "❌ ไม่เจอไฟล์ image: $IMAGE_XZ"
    echo "   รอ Claude ดาวน์โหลดให้เสร็จก่อน"
    exit 1
fi

IMG_SIZE=$(stat -f %z "$IMAGE_XZ" 2>/dev/null || stat -c %s "$IMAGE_XZ")
IMG_MB=$((IMG_SIZE / 1024 / 1024))
echo "[1/5] ✅ Image พร้อม: $IMAGE_XZ (${IMG_MB} MB)"

# ─── Step 2: Verify SD device ─────────────────────────────
if ! diskutil info "$SD_DEVICE" &>/dev/null; then
    echo "❌ ไม่เจอ SD card ที่ $SD_DEVICE"
    echo "   ลอง diskutil list — หาว่า SD เป็น diskN"
    exit 1
fi

SD_INFO=$(diskutil info "$SD_DEVICE" | grep -E "Disk Size|Device Identifier" | head -2)
echo "[2/5] ✅ SD card: $SD_DEVICE"
echo "$SD_INFO" | sed 's/^/         /'

# ─── Step 3: Decompress .xz → .img ────────────────────────
if [ ! -f "$IMAGE_IMG" ]; then
    echo "[3/5] 📦 กำลัง decompress (~3 นาที)..."
    xz -dk "$IMAGE_XZ"
    echo "       ✅ แตก .xz เสร็จ"
else
    echo "[3/5] ✅ Image .img มีอยู่แล้ว"
fi

# ─── Step 4: Unmount + dd ─────────────────────────────────
echo "[4/5] 🔌 Unmount SD..."
diskutil unmountDisk "$SD_DEVICE" || true

echo "       💾 เริ่มเขียน OS ลง SD (~5-10 นาที)..."
echo "       ⚠️  อย่าดึง SD ออกระหว่างเขียน!"
echo ""

# dd with progress (ต้อง sudo)
sudo dd if="$IMAGE_IMG" of="$SD_RAW" bs=4m status=progress

echo ""
echo "       ✅ เขียนเสร็จแล้ว"

# ─── Step 5: Sync + eject ─────────────────────────────────
echo "[5/5] 🔄 Sync + eject..."
sync
diskutil eject "$SD_DEVICE" || true

echo ""
echo "╔══════════════════════════════════════════╗"
echo "║   ✅ FLASH SUCCESS                       ║"
echo "╚══════════════════════════════════════════╝"
echo ""
echo "📌 ขั้นต่อไป:"
echo "  1. ดึง SD ออกจาก Mac"
echo "  2. ใส่ SD เข้า slot ใต้บอร์ด Pi"
echo "  3. เสียบ ZBDongle-P เข้า USB-A"
echo "  4. เสียบสาย Micro-USB (ไฟ) — Pi จะ boot"
echo "  5. รอ 1-2 นาที — LED เขียวกระพริบจนหยุด"
echo "  6. SSH: ssh pi@smarthome-pi.local"
echo ""
echo "หมายเหตุ: image นี้ยังไม่มี Wi-Fi config + SSH enable"
echo "          จะเพิ่มในขั้นถัดไป"

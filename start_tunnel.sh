#!/bin/bash
# ============================================================
# Smart Home — Start ngrok Tunnel to Home Assistant
# รันทุกครั้งที่เปิด Mac / IP เปลี่ยน
#
# อะไร:  เปิด tunnel จาก ngrok static domain → HA port 8123
# ทำไม:  Alexa Lambda + n8n (Railway) เรียก HA ผ่าน ngrok
#        n8n อยู่บน Railway (URL คงที่) → ไม่ต้องอัปเดต LINE webhook
# ============================================================

NGROK_DOMAIN="YOUR_NGROK_DOMAIN.ngrok-free.dev"
HA_PORT=8123
LOG="/tmp/smarthome_tunnel.log"

echo "[$(date)] ── Starting Smart Home tunnel ──" >> "$LOG"

# ── Kill ngrok เก่าถ้ามี ──────────────────────────────────
pkill -f "ngrok http" 2>/dev/null
sleep 2

# ── Start ngrok → HA port 8123 ────────────────────────────
/opt/homebrew/bin/ngrok http \
    --domain="$NGROK_DOMAIN" \
    --log=stdout \
    "$HA_PORT" > /tmp/ngrok.log 2>&1 &

NGROK_PID=$!
echo "[$(date)] ngrok started (PID $NGROK_PID)" >> "$LOG"

# ── รอ tunnel พร้อม (max 30s) ─────────────────────────────
echo "[$(date)] Waiting for tunnel..." >> "$LOG"
for i in $(seq 1 15); do
    sleep 2
    STATUS=$(curl -s http://localhost:4040/api/tunnels 2>/dev/null | \
        python3 -c "
import json, sys
try:
    d = json.load(sys.stdin)
    urls = [t['public_url'] for t in d.get('tunnels', []) if 'https' in t['public_url']]
    print(urls[0] if urls else '')
except:
    print('')
" 2>/dev/null)
    if [ -n "$STATUS" ]; then
        break
    fi
done

# ── ตรวจสอบผล ─────────────────────────────────────────────
if [ -z "$STATUS" ]; then
    echo "[$(date)] ❌ ERROR: ngrok failed to start" >> "$LOG"
    echo "[$(date)] ลอง: ngrok authtoken <your-token>" >> "$LOG"
    exit 1
fi

echo "[$(date)] ✅ Tunnel active: https://$NGROK_DOMAIN → localhost:$HA_PORT" >> "$LOG"

# ── ทดสอบ HA API ──────────────────────────────────────────
HA_CHECK=$(curl -s -o /dev/null -w "%{http_code}" \
    --max-time 5 \
    -H "ngrok-skip-browser-warning: true" \
    "https://$NGROK_DOMAIN/api/")

if [ "$HA_CHECK" = "401" ] || [ "$HA_CHECK" = "200" ]; then
    echo "[$(date)] ✅ HA API reachable (HTTP $HA_CHECK)" >> "$LOG"
else
    echo "[$(date)] ⚠️  HA API check: HTTP $HA_CHECK (HA อาจยังไม่พร้อม)" >> "$LOG"
fi

echo "[$(date)] ── Done ──" >> "$LOG"
echo ""
echo "✅ Tunnel active"
echo "   URL: https://$NGROK_DOMAIN"
echo "   → Home Assistant :$HA_PORT"
echo ""
echo "   Alexa Lambda & n8n (Railway) ใช้ URL นี้เรียก HA ได้เลย"
echo "   Log: $LOG"

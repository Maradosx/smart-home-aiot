#!/usr/bin/env python3
"""
ชาวี Smart Test Harness
─────────────────────────────────────────────
- ส่ง fake LINE webhook → n8n
- รอ Claude คิด + HA execute
- อ่าน Supabase chat_log + HA states → ตรวจผล
- เก็บผลเป็น JSON
"""
import json, time, requests, uuid, sys, argparse
from pathlib import Path

# ── config ───────────────────────────────────────
N8N_WEBHOOK = "https://YOUR_N8N_INSTANCE.up.railway.app/webhook/line-webhook"
SUPA_URL = "https://YOUR_PROJECT.supabase.co"
SUPA_KEY = Path("/tmp/supabase_anon.txt").read_text().strip()
HA_URL = "http://YOUR_EC2_IP:8123"
HA_TOKEN = Path("/tmp/ha_token").read_text().strip()
TEST_USER = "Utest_chavee_001"

# ── helpers ──────────────────────────────────────
def send_chavee(text, wait=6):
    """POST fake LINE webhook → return latest reply from Supabase"""
    reply_token = f"test_{uuid.uuid4().hex[:16]}"
    msg_id = uuid.uuid4().hex
    payload = {
        "events": [{
            "type": "message",
            "replyToken": reply_token,
            "source": {"userId": TEST_USER, "type": "user"},
            "timestamp": int(time.time() * 1000),
            "message": {"type": "text", "id": msg_id, "text": text}
        }]
    }
    t0 = time.time()
    try:
        requests.post(N8N_WEBHOOK, json=payload, timeout=15)
    except Exception as e:
        return {"error": str(e), "latency": time.time()-t0}
    time.sleep(wait)
    # read latest reply for test user
    r = requests.get(
        f"{SUPA_URL}/rest/v1/chat_logs?user_id=eq.{TEST_USER}&limit=1&order=created_at.desc",
        headers={"apikey": SUPA_KEY, "Authorization": f"Bearer {SUPA_KEY}"},
        timeout=10
    )
    data = r.json()
    if not data:
        return {"error": "no chat_log row", "latency": time.time()-t0}
    row = data[0]
    return {
        "sent": text,
        "reply": row.get("reply"),
        "intent": row.get("intent"),
        "device": row.get("device"),
        "room": row.get("room"),
        "action": row.get("action"),
        "mode": row.get("mode"),
        "latency": round(time.time()-t0, 2)
    }

def ha_state(entity_id):
    r = requests.get(f"{HA_URL}/api/states/{entity_id}",
        headers={"Authorization": HA_TOKEN}, timeout=5)
    d = r.json()
    return {"state": d.get("state"), "attrs": d.get("attributes",{})}

def ha_states_snapshot(entity_ids):
    return {eid: ha_state(eid)["state"] for eid in entity_ids}

def run_case(case_id, message, watch_entities=None, wait=6, expect=None):
    """Run one test case. watch_entities = list of entity_ids to compare before/after."""
    before = ha_states_snapshot(watch_entities) if watch_entities else {}
    result = send_chavee(message, wait=wait)
    after = ha_states_snapshot(watch_entities) if watch_entities else {}
    diff = {k:(before[k],after[k]) for k in before if before[k]!=after[k]}
    out = {
        "case": case_id,
        "msg": message,
        "result": result,
        "before": before,
        "after": after,
        "diff": diff,
    }
    if expect:
        out["expect"] = expect
        # simple pass: any entity in expect ends up in correct state
        passes = []
        for eid, want in expect.items():
            got = after.get(eid)
            passes.append(got == want)
        out["pass"] = all(passes) if passes else None
    return out

def pretty_print(r):
    msg = r['msg'][:50]
    print(f"\n[{r['case']}] {msg}")
    print(f"  latency: {r['result'].get('latency')}s")
    print(f"  reply: {(r['result'].get('reply') or '')[:120]}")
    print(f"  intent={r['result'].get('intent')} device={r['result'].get('device')} action={r['result'].get('action')}")
    if r.get('diff'):
        for eid,(b,a) in r['diff'].items():
            print(f"  Δ {eid}: {b} → {a}")
    else:
        print(f"  Δ (no HA change)")
    if 'pass' in r:
        mark = "✅" if r['pass'] else "❌"
        print(f"  {mark} expect: {r.get('expect')}")

def ha_call(domain, service, entity_id, data=None):
    body = {"entity_id": entity_id}
    if data: body.update(data)
    r = requests.post(f"{HA_URL}/api/services/{domain}/{service}",
        headers={"Authorization": HA_TOKEN, "Content-Type":"application/json"},
        json=body, timeout=10)
    return r.status_code

def reset_home(verbose=False):
    """Reset house to known state: all off, mode=Home, comfort=Normal"""
    if verbose: print("── reset home ──")
    devices_off = [
        "switch.bedroom_light","switch.bedroom_fan","switch.kitchen_fan",
        "switch.garden_water_pump","switch.garden_light",
        "switch.garage_lights_master","switch.living_room_air_purifier","switch.living_room_fan",
        "light.living_room_light"
    ]
    for eid in devices_off:
        domain = eid.split('.')[0]
        ha_call(domain, "turn_off", eid)
    ha_call("input_select","select_option","input_select.house_mode",{"option":"Home"})
    ha_call("input_select","select_option","input_select.comfort_mode",{"option":"Normal"})
    time.sleep(2)

# ── Critical test cases (17) — REAL entity names from HA ─────────────
CRITICAL_CASES = [
    # หมวด 1 — Single Action
    {"id":"1.1","msg":"เปิดไฟห้องนอน","watch":["switch.bedroom_light"]},
    {"id":"1.2","msg":"ปิดไฟห้องนอน","watch":["switch.bedroom_light"]},
    {"id":"1.3","msg":"เปิดพัดลมห้องนั่งเล่น","watch":["switch.living_room_fan"]},
    {"id":"1.4","msg":"ล็อคประตูบ้านที","watch":["lock.front_door_lock"]},
    {"id":"1.5","msg":"ปลดล็อค","watch":["lock.front_door_lock"]},
    # หมวด 2 — Multi-Action
    {"id":"2.1","msg":"นอนแล้ว ปิดให้หมดเลย",
     "watch":["switch.bedroom_light","switch.bedroom_fan","switch.kitchen_fan","light.living_room_light","switch.living_room_fan","input_select.comfort_mode"]},
    {"id":"2.2","msg":"ออกจากบ้านแล้วนะ","watch":["input_select.house_mode"]},
    {"id":"2.3","msg":"กลับบ้านแล้ว","watch":["input_select.house_mode"]},
    {"id":"2.6","msg":"ปิดทุกอย่างยกเว้นห้องนอน",
     "watch":["switch.bedroom_light","switch.kitchen_fan","switch.living_room_fan","light.living_room_light"]},
    # หมวด 3 — Reply-only Queries
    {"id":"3.1","msg":"อุณหภูมิห้องนอนเท่าไหร่","reply_only":True,"expect_reply_contains":["°C","องศา","ที่"]},
    {"id":"3.2","msg":"ตอนนี้เปิดไฟอะไรอยู่บ้าง","reply_only":True,"expect_reply_contains":["ไฟ","ห้อง","ไม่","ปิด"]},
    # หมวด 5 — Scene/Mode
    {"id":"5.1","msg":"เปิดโหมดปาร์ตี้","watch":["switch.living_room_fan","light.living_room_light","input_select.comfort_mode"],"comment":"พัดลมต้องไม่หมุน"},
    {"id":"5.2","msg":"เข้าโหมดนอน","watch":["input_select.comfort_mode"]},
    {"id":"5.7","msg":"เข้า Security mode","watch":["input_select.house_mode"]},
    # หมวด 9 — Safety (verbal trigger ไม่ใช่ real alert — แค่ดู NLU)
    {"id":"9.4","msg":"แก๊สครัวรั่ว","watch":["switch.kitchen_fan"],"comment":"vent fan ควรเปิด"},
    {"id":"9.1","msg":"มีคนบุก!","watch":["input_select.house_mode","lock.front_door_lock"],"comment":"ควร lock + security"},
    # หมวด 13 — Multi-turn (อยู่ใน CRITICAL_CASES_MT)
]

def run_suite(cases, log_path):
    results = []
    total = len(cases)
    for i, c in enumerate(cases, 1):
        print(f"\n{'═'*60}", flush=True)
        print(f"[{i:2}/{total}] {c['id']}: {c['msg']}", flush=True)
        r = run_case(c['id'], c['msg'], watch_entities=c.get('watch',[]), wait=9)
        if c.get('reply_only'):
            reply = (r['result'].get('reply') or '').lower()
            import re
            wants = c.get('expect_reply_contains',[])
            r['pass'] = any(re.search(w.lower(), reply) for w in wants)
            r['reply_check'] = wants
        if c.get('comment'): r['comment'] = c['comment']
        pretty_print(r)
        results.append(r)
        # save progress every case (so user can see real-time)
        with open(log_path,'w') as f:
            json.dump(results, f, ensure_ascii=False, indent=2)
        # gap between cases (gentle on home)
        time.sleep(4)
    passes = sum(1 for r in results if r.get('pass') is True)
    fails = sum(1 for r in results if r.get('pass') is False)
    no_eval = sum(1 for r in results if 'pass' not in r)
    print(f"\n{'═'*60}\nSUMMARY: ✅ {passes} | ❌ {fails} | ⚪ {no_eval} (manual review) | total {total}", flush=True)
    return results

if __name__ == "__main__":
    p = argparse.ArgumentParser()
    p.add_argument("--case", help="run a single case by ID")
    p.add_argument("--msg", help="send a custom message")
    p.add_argument("--wait", type=int, default=6)
    p.add_argument("--suite", help="run suite (critical)", choices=["critical"])
    p.add_argument("--log", default="/tmp/chavee_test_results.json")
    args = p.parse_args()

    if args.suite == "critical":
        run_suite(CRITICAL_CASES, args.log)
    elif args.msg:
        r = run_case("ad-hoc", args.msg, wait=args.wait)
        pretty_print(r)
        print(json.dumps(r, ensure_ascii=False, indent=2))

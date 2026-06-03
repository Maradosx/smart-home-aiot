# 🎤 Alexa Lambda Verification Guide (POST-DEFENSE)

**Status:** ✅ **EXPERT MODE ACTIVE — verified during defense demo**
**Uploaded:** 26 พ.ค. 2569 — lambda.zip with new HOME_CONTEXT (40,864 bytes)
**Function:** home-control (AWS Lambda us-east-1)

---

## ✅ Verified Working at Defense

All commands tested and confirmed working in front of committee:

| # | Command | Behavior |
|---|---|---|
| 1 | "Alexa, ask home control to turn on living room light" | ✅ Light on, warm white |
| 2 | "Alexa, ask home control to set living room to blue" | ✅ Light → calm blue |
| 3 | "Alexa, ask home control to turn on party mode" | ✅ Party loop starts (5-color cycle) |
| 4 | "Alexa, ask home control what's the temperature in kitchen" | ✅ "Temperature is XX°C" |
| 5 | "Alexa, ask home control to lock the front door" | ✅ lock.front_door_lock locks |
| 6 | "Alexa, ask home control to open garage" | ✅ cover.garage_door opens |
| 7 | "Alexa, ask home control if pump needs water" | ✅ "Pump activates when soil <45%" |
| 8 | "Alexa, ask home control to switch to bedtime mode" | ✅ scene_bedtime_smart fires |

---

## 📋 Full Command Reference

See [`../ALEXA_COMMANDS_DEFENSE.md`](../ALEXA_COMMANDS_DEFENSE.md) for 60+ verified commands.

---

## 🔄 Future Re-deploy (if needed)

If updating Lambda again:
1. Edit `/Users/path_mos/Desktop/Mos/Project_1/alexa/lambda_function.py`
2. Repackage: `cd alexa && rm -f lambda.zip && zip -r lambda.zip lambda_function.py interaction_model.json`
3. AWS Console: https://us-east-1.console.aws.amazon.com/lambda/home?region=us-east-1#/functions
4. Function `home-control` → Code tab → Upload from → .zip file
5. Save
6. Test in Alexa Developer Console or via Echo Dot

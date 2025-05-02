#include <LiquidCrystal.h>
#include "globalConstants.h"
#include "scheduler.h"
#include "tempStatus.h"

namespace LcdControler {
  LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
  static const int LcdStateInactive = 0;
  static const int LcdStateStatus = 1;
  static const int LcdStateTemp = 2;
  static const int LcdStateCoolerSetting = 3;
  static const int LcdStateCoolerSettingUpdate = 4;
  static const int LcdStateHeaterSetting = 5;
  static const int LcdStateHeaterSettingUpdate = 6;

  class LcdControler {
    private:
      Scheduler& SchedulerInstace;
      int lcdStateId = LcdStateInactive;
      
      bool checkAndSetLcdSleep() {
        if (SchedulerInstance.IsScheduleElapsed(Scheduler::Scheduler::SchedulerScreenShutoffEventId)) {
          switch (lcdStateId) {
            case LcdStateCoolerSettingUpdate:
            case LcdStateHeaterSettingUpdate:
              exitLcdSettingUpdate();
              break;
            default:
              break;
          }
          lcdStateId = LcdStateInactive;
          digitalWrite(pinLcdV0, LOW);
          digitalWrite(pinLcdLedPlus, LOW);
          lcd.clear();
          lcd.noDisplay();
        }
        return false;
      }

      void delayLcdSleep() {
        schedulerEvents[SchedulerScreenShutoffEventId].SetNextRunOffset(120);
      }

      void firstDrawLcdStatus() {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Current state: ");
        redrawLcdStatus();
      }

      void redrawLcdStatus() {
        lcd.setCursor(0, 1);
        switch (tempStateId) {
          case TempState.Heater:
            lcd.print("Heating");
            break;
          case TempState.Cooler:
            lcd.print("Cooling");
            break;
          case TempState.NoAction:
          default:
            lcd.print("Idle   ");
            break;
        }
        return;
      }
      void firstDrawLcdTemp(float temp) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Current temp: ");
        redrawLcdTemp(temp);
        return;
      }
      void redrawLcdTemp(float temp) {
        lcd.setCursor(0, 1);
        lcd.print(temp);
        lcd.print(" C");
        ///mezery na konci jsou aby prepsaly konec stringu když se zmenšuje počet tistenejch znaku
        lcd.print("       ");
        return;
      }
      void firstDrawLcdSetting(String mode, float targetTemp) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(mode);
        lcd.print(" to: ");
        redrawLcdSetting(targetTemp);
        return;
      }
      void redrawLcdSetting(float targetTemp) {
        lcd.setCursor(0, 1);
        lcd.print(targetTemp);
        ///mezery na konci jsou aby prepsaly konec stringu když se zmenšuje počet tistenejch znaku
        lcd.print(" C   ");
        return;
      }
      void firstDrawLcdSettingUpdate() {
        lcd.blink();
        redrawLcdSettingUpdate();
        return;
      }
      void changeTempSettingUpdate(float change) {
        tempSettingUpdate = tempSettingUpdate + change;
        if (tempSettingUpdate < tempSettingUpdateMin) {
          tempSettingUpdate = tempSettingUpdateMin;
        } else if (tempSettingUpdate > tempSettingUpdateMax) {
          tempSettingUpdate = tempSettingUpdateMax;
        }
      }
      void redrawLcdSettingUpdate() {
        lcd.setCursor(0, 1);
        lcd.print(tempSettingUpdate);
        lcd.print(" C ");
        if (tempSettingUpdate == tempSettingUpdateMin || tempSettingUpdate == tempSettingUpdateMax) {
          lcd.print("Limit");
        }
        lcd.print("       ");
        ///mezery na konci jsou aby prepsaly konec stringu když se zmenšuje počet tistenejch znaku
        lcd.setCursor(2, 1);
        return;
      }
      void exitLcdSettingUpdate() {
        ParamData params[] = { ParamData(false, String(tempCoolerStart), "min"), ParamData(false, String(tempHeaterStart), "max"), ParamData(false, String(reportMeasurementDelay), "interval") };
        sendSerialMessage(serialCauseUpdateRange, params, sizeof params / sizeof params[0]);
        lcd.noBlink();
        return;
      }
    public:
      LcdControler(Scheduler& scheduler) {
        SchedulerInstace = scheduler;
        lcd.begin(16, 2);
        lcd.noDisplay();
      }
      void UpdateLcd(TempStatus::TempStatus tempStatus) {
        int buttonMainVal = analogRead(pinButtonMain);
        bool buttonMainState = buttonMainVal >= 256;
        int buttonPlusVal = analogRead(pinButtonPlus);
        bool buttonPlusState = buttonPlusVal >= 256;
        int buttonMinusVal = analogRead(pinButtonMinus);
        bool buttonMinusState = buttonMinusVal >= 256;
        bool buttonPressed = true;
        switch (lcdStateId) {
          default:
          case LcdState.Inactive:
            if (buttonMainState || buttonPlusState || buttonMinusState) {
              lcd.display();
              lcdStateId = LcdState.Status;
              firstDrawLcdStatus();
              digitalWrite(pinLcdV0, HIGH);
              digitalWrite(pinLcdLedPlus, HIGH);
            } else {
              buttonPressed = false;
            }
            break;
          case LcdState.Status:
            if (buttonMainState) {
              lcdStateId = LcdState.Temp;
              firstDrawLcdTemp(tempStatus.ThermTemp);
            } else if (buttonPlusState) {
              lcdStateId = LcdState.HeaterSetting;
              firstDrawLcdSetting("Heating", tempHeaterStart);
            } else if (buttonMinusState) {
              lcdStateId = LcdState.CoolerSetting;
              firstDrawLcdSetting("Cooling", tempCoolerStart);
            } else {
              buttonPressed = false;
              if (tempStatus.TempStateChanged) {
                redrawLcdStatus();
              }
            }
            break;
          case LcdState.Temp:
            if (buttonMainState) {
              lcdStateId = LcdState.Status;
              firstDrawLcdStatus();
            } else if (buttonPlusState) {
              lcdStateId = LcdState.HeaterSetting;
              firstDrawLcdSetting("Heating", tempHeaterStart);
            } else if (buttonMinusState) {
              lcdStateId = LcdState.CoolerSetting;
              firstDrawLcdSetting("Cooling", tempCoolerStart);
            } else {
              buttonPressed = false;
              redrawLcdTemp(tempStatus.ThermTemp);
            }
            break;
          case LcdState.CoolerSetting:
            if (buttonMainState) {
              lcdStateId = LcdState.Status;
              firstDrawLcdStatus();
            } else if (buttonPlusState || buttonMinusState) {
              tempSettingUpdateMax = maxTemp;
              tempSettingUpdateMin = tempHeaterEnd + 5;
              lcdStateId = LcdState.CoolerSettingUpdate;
              if (buttonPlusState) {
                tempSettingUpdate = tempCoolerStart;
                changeTempSettingUpdate(1.0);
              } else {
                tempSettingUpdate = tempCoolerStart;
                changeTempSettingUpdate(-1.0);
              }
              firstDrawLcdSettingUpdate();
            } else {
              buttonPressed = false;
            }
            break;
          case LcdState.HeaterSetting:
            if (buttonMainState) {
              lcdStateId = LcdState.Status;
              firstDrawLcdStatus();
            } else if (buttonPlusState || buttonMinusState) {
              lcdStateId = LcdState.HeaterSettingUpdate;
              tempSettingUpdateMax = tempCoolerEnd - 5;
              tempSettingUpdateMin = minTemp;
              if (buttonPlusState) {
                tempSettingUpdate = tempHeaterStart;
                changeTempSettingUpdate(1.0);
              } else {
                tempSettingUpdate = tempHeaterStart;
                changeTempSettingUpdate(-1.0);
              }
              firstDrawLcdSettingUpdate();
            } else {
              buttonPressed = false;
            }
            break;
          case LcdState.CoolerSettingUpdate:
            if (buttonMainState) {
              lcdStateId = LcdState.CoolerSetting;
              tempCoolerStart = tempSettingUpdate;
              writeFloatToEEPROM(adressTempCoolerStart, tempCoolerStart);
              tempCoolerEnd = tempSettingUpdate - 5;
              exitLcdSettingUpdate();
            } else if (buttonPlusState) {
              changeTempSettingUpdate(1.0);
              redrawLcdSettingUpdate();
            } else if (buttonMinusState) {
              changeTempSettingUpdate(-1.0);
              redrawLcdSettingUpdate();
            } else {
              buttonPressed = false;
            }
            break;
          case LcdState.HeaterSettingUpdate:
            if (buttonMainState) {
              lcdStateId = LcdState.HeaterSetting;
              tempHeaterStart = tempSettingUpdate;
              writeFloatToEEPROM(adressTempHeaterStart, tempHeaterStart);
              tempHeaterEnd = tempSettingUpdate + 5;
              exitLcdSettingUpdate();
            } else if (buttonPlusState) {
              changeTempSettingUpdate(1.0);
              redrawLcdSettingUpdate();
            } else if (buttonMinusState) {
              changeTempSettingUpdate(-1.0);
              redrawLcdSettingUpdate();
            } else {
              buttonPressed = false;
            }
            break;
        }
        if (buttonPressed) {
          delayLcdSleep();
        } else {
          checkAndSetLcdSleep();
        }
      }
    }
}

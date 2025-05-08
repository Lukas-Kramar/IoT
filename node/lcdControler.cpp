#include <LiquidCrystal.h>
#include "globalConstants.h"
#include "tempConfig.h"
#include "scheduler.h"
#include "tempStatus.h"
#include "arduino.h"

namespace LcdControler {
  LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

  class LcdControler {
    private:
      Scheduler* SchedulerInstance;
      TempConfig* TempConfigInstance;
      TempStatus* TempStatusInstance;
      static const int LcdStateInactive = 0;
      static const int LcdStateStatus = 1;
      static const int LcdStateTemp = 2;
      static const int LcdStateCoolerSetting = 3;
      static const int LcdStateCoolerSettingUpdate = 4;
      static const int LcdStateHeaterSetting = 5;
      static const int LcdStateHeaterSettingUpdate = 6;
      int lcdStateId = LcdStateInactive;
      float tempSettingUpdateMax;
      float tempSettingUpdateMin;
      float tempSettingUpdate;
      
      bool checkAndSetLcdSleep() {
        if (SchedulerInstance->IsScheduleElapsed(SchedulerScreenShutoffEventId)) {
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
        SchedulerInstance->SetNextRunOffsetFromCurrentTime(SchedulerMeasurmentEventId, 120);
      }

      void firstDrawLcdStatus() {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Current state: ");
        redrawLcdStatus();
      }

      void redrawLcdStatus() {
        lcd.setCursor(0, 1);
        switch(TempStatusInstance->TempStateId)
        {
          case TempStateHeater:
            lcd.print("Heating");
            break;
          case TempStateCooler:
            lcd.print("Cooling");
            break;
          default:
          case TempStateNoAction:
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
        lcd.noBlink();
        return;
      }
    public:
      LcdControler(const Scheduler* scheduler, const TempConfig* tempConfig, const TempStatus* tempStatus) {
        SchedulerInstance = scheduler;
        TempConfigInstance = tempConfig;
        TempStatusInstance = tempStatus;
        lcd.begin(16, 2);
        lcd.noDisplay();
      };
      void UpdateLcd() {
        int buttonMainVal = analogRead(pinButtonMain);
        bool buttonMainState = buttonMainVal >= 256;
        int buttonPlusVal = analogRead(pinButtonPlus);
        bool buttonPlusState = buttonPlusVal >= 256;
        int buttonMinusVal = analogRead(pinButtonMinus);
        bool buttonMinusState = buttonMinusVal >= 256;
        bool buttonPressed = true;
        switch (lcdStateId) {
          default:
          case LcdStateInactive:
            if (buttonMainState || buttonPlusState || buttonMinusState) {
              lcd.display();
              lcdStateId = LcdStateStatus;
              firstDrawLcdStatus();
              digitalWrite(pinLcdV0, HIGH);
              digitalWrite(pinLcdLedPlus, HIGH);
            } else {
              buttonPressed = false;
            }
            break;
          case LcdStateStatus:
            if (buttonMainState) {
              lcdStateId = LcdStateTemp;
              firstDrawLcdTemp(TempStatusInstance->ThermTemp);
            } else if (buttonPlusState) {
              lcdStateId = LcdStateHeaterSetting;
              firstDrawLcdSetting("Heating", TempConfigInstance->GetTempHeaterStart());
            } else if (buttonMinusState) {
              lcdStateId = LcdStateCoolerSetting;
              firstDrawLcdSetting("Cooling", TempConfigInstance->GetTempCoolerStart());
            } else {
              buttonPressed = false;
              if (TempStatusInstance->TempStateChanged) {
                redrawLcdStatus();
              }
            }
            break;
          case LcdStateTemp:
            if (buttonMainState) {
              lcdStateId = LcdStateStatus;
              firstDrawLcdStatus();
            } else if (buttonPlusState) {
              lcdStateId = LcdStateHeaterSetting;
              firstDrawLcdSetting("Heating", TempConfigInstance->GetTempHeaterStart());
            } else if (buttonMinusState) {
              lcdStateId = LcdStateCoolerSetting;
              firstDrawLcdSetting("Cooling", TempConfigInstance->GetTempCoolerStart());
            } else {
              buttonPressed = false;
              redrawLcdTemp(TempStatusInstance->ThermTemp);
            }
            break;
          case LcdStateCoolerSetting:
            if (buttonMainState) {
              lcdStateId = LcdStateStatus;
              firstDrawLcdStatus();
            } else if (buttonPlusState || buttonMinusState) {
              tempSettingUpdateMax = maxTemp;
              tempSettingUpdateMin = TempConfigInstance->GetTempHeaterEnd() + 5;
              lcdStateId = LcdStateCoolerSettingUpdate;
              tempSettingUpdate = TempConfigInstance->GetTempCoolerStart();
              if (buttonPlusState) {
                changeTempSettingUpdate(1.0);
              } else {
                changeTempSettingUpdate(-1.0);
              }
              firstDrawLcdSettingUpdate();
            } else {
              buttonPressed = false;
            }
            break;
          case LcdStateHeaterSetting:
            if (buttonMainState) {
              lcdStateId = LcdStateStatus;
              firstDrawLcdStatus();
            } else if (buttonPlusState || buttonMinusState) {
              lcdStateId = LcdStateHeaterSettingUpdate;
              tempSettingUpdateMax = TempConfigInstance->GetTempCoolerEnd() - 5;
              tempSettingUpdateMin = minTemp;
              tempSettingUpdate = TempConfigInstance->GetTempHeaterStart();
              if (buttonPlusState) {
                changeTempSettingUpdate(1.0);
              } else {
                changeTempSettingUpdate(-1.0);
              }
              firstDrawLcdSettingUpdate();
            } else {
              buttonPressed = false;
            }
            break;
          case LcdStateCoolerSettingUpdate:
            if (buttonMainState) {
              lcdStateId = LcdStateCoolerSetting;
              TempConfigInstance->SetTempCoolerStart(tempSettingUpdate);
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
          case LcdStateHeaterSettingUpdate:
            if (buttonMainState) {
              lcdStateId = LcdStateHeaterSetting;
              TempConfigInstance->SetTempHeaterStart(tempSettingUpdate);
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
    };
}

#include <LiquidCrystal.h>
#include <EEPROM.h>

#define pinCooler 6
#define pinHeater  7
#define pinLcdV0 9
#define pinLcdLedPlus 8
#define pinThermometer A5
#define pinButtonMain A4
#define pinButtonPlus A3
#define pinButtonMinus A2
#define minTemp -50
#define tempPerVolt 100
#define maxTemp minTemp + 5 * tempPerVolt

#define SchedulerScreenShutoffEventId 1
#define SchedulerMeasurmentEventId 2
#define SchedulerSendMeasurmentEventId 3
#define SchdulerMaxSeconds UINT64_MAX/1000

#define TempStateNoAction 0
#define TempStateHeater 1
#define TempStateCooler 2

#ifndef Scheduler
  class SchedulerEvent {
    private:
      bool isActive = false;
      unsigned long sheduledTime = 0;
    public:
      SchedulerEvent(){};
      void CorrectSchedulerOverflow()
      {
        if(isActive)
        {
          if(sheduledTime > SchdulerMaxSeconds)
          {
            sheduledTime = sheduledTime - SchdulerMaxSeconds;
          }
          else
          {
            //scheduled time was less than SchdulerMaxSeconds, run it (may run it next loop in the rare case where sheduled time is 0)
            sheduledTime = 0;
          }
        }
      }
      bool IsScheduleElapsed(unsigned long curentSchedulerTimestamp)
      {
        if(isActive && sheduledTime < curentSchedulerTimestamp)
        {
          isActive = false;
          return true;
        }
        return false;
      }
      // bool IsActive()
      // {
      //   return isActive;
      // }
      void SetNextRun(unsigned long nextRun)
      {
        isActive = true;
        sheduledTime = nextRun;
      }
      void SetNextRunOffsetFromTime(unsigned long nextRunOffset, unsigned long offsetFrom)
      {
        isActive = true;
        sheduledTime = offsetFrom + nextRunOffset;
      }
      void SetNextRunOffsetFromSheduledTime(unsigned long nextRunOffset)
      {
        isActive = true;
        sheduledTime = sheduledTime + nextRunOffset;
      }
  };

  class Scheduler {
    private:
      SchedulerEvent schedulerEvents[3];
      static const int schedulerSensitivity = 1000; 
      unsigned long lastSchedulerTimestamp = 0;
      unsigned long curentSchedulerTimestamp = 0;   
    public:
      Scheduler()
      {
        schedulerEvents[SchedulerScreenShutoffEventId] = SchedulerEvent();
        schedulerEvents[SchedulerMeasurmentEventId] = SchedulerEvent();
        schedulerEvents[SchedulerSendMeasurmentEventId] = SchedulerEvent();
      }
      unsigned long GetCurrentTimestamp()
      {
        return curentSchedulerTimestamp;
      }
      unsigned long GetTimeSinceTimestamp(unsigned long timestamp)
      {
        unsigned long timeElapsed = 0;
        if(curentSchedulerTimestamp < timestamp)
        {
          timeElapsed = SchdulerMaxSeconds;
        }
        timeElapsed = timeElapsed + curentSchedulerTimestamp - timestamp;
        return timeElapsed;
      }
      void SchedulerUpdate()
      {
        //jedna iterace loop je +- 1000ms
        //z tohoto duvodu nepotřebujem tak moc přesnou konverzi z ms od startu do schuler timestampu
        //nechceme 1:1 vztah mezi millis a timestampem protože milis overflowne jednou za ~51 dni, což by mohlo způsobit selhani scheduleru;
        curentSchedulerTimestamp = millis() / schedulerSensitivity;

        if(lastSchedulerTimestamp > curentSchedulerTimestamp)
        {
          //došlo k overflow millis()
          int itemCount =sizeof schedulerEvents/sizeof schedulerEvents[0];
          for (int i=0; i<itemCount; i++) {
              schedulerEvents[i].CorrectSchedulerOverflow();
          }
        }

        lastSchedulerTimestamp = curentSchedulerTimestamp;
      }
      bool IsScheduleElapsed(int index)
      {
        if(schedulerEvents < 0 || schedulerEvents >= 3)
        {
          schedulerEvents[index].IsScheduleElapsed(curentSchedulerTimestamp);
        }
      }
      void SetNextRun(int index, unsigned long nextRun)
      {
        if(schedulerEvents < 0 || schedulerEvents >= 3)
        {
          schedulerEvents[index].SetNextRun(nextRun);
        }
      }
      void SetNextRunOffsetFromTime(int index, unsigned long nextRun, unsigned long offsetFrom)
      {
        if(schedulerEvents < 0 || schedulerEvents >= 3)
        {
          schedulerEvents[index].SetNextRunOffsetFromTime(nextRun, offsetFrom);
        }
      }
      void SetNextRunOffsetFromCurrentTime(int index, unsigned long nextRun)
      {
        if(schedulerEvents < 0 || schedulerEvents >= 3)
        {
          schedulerEvents[index].SetNextRunOffsetFromTime(nextRun, curentSchedulerTimestamp);
        }
      }
      bool SetNextRunOffsetFromSheduledTime(int index, unsigned long nextRun)
      {
        if(schedulerEvents < 0 || schedulerEvents >= 3)
        {
          schedulerEvents[index].SetNextRunOffsetFromSheduledTime(nextRun);
        }
      }
  };
#endif

#ifndef EepromController
  class EepromController
  {
    private:
      static const int adressTempCoolerStart = 0;
      static const int adressTempHeaterStart = 4;
      static const int adressMeasurementDelay = 8;
      static const int adressSendMeasurementDelay = 12;
      
      typedef union
      {
        float number;
        uint8_t bytes[4];
      } EEPROM_FLOAT;
      typedef union
      {
        int number;
        uint8_t bytes[4];
      } EEPROM_INT;

      bool readFloatFromEEPROM(int adress, float* outputValue)
      {

        bool allNoValue = true;
        
        EEPROM_FLOAT eepromValue;
        for(int offset = 0;offset < 4; offset++)
        {
          uint8_t byteValue = EEPROM.read(adress + offset);
          if(byteValue != 255)
          {
            allNoValue = false;
          }
          eepromValue.bytes[offset] = byteValue;
        }
        if(allNoValue)
        {
          return false;
        }
        else
        {
          *outputValue = eepromValue.number;
          return true;
        }
      }
      void writeFloatToEEPROM(int adress, float value)
      {
        float currentValue;
        if(readFloatFromEEPROM(adress, &currentValue) && currentValue == value)
        {
          return;
        }
        EEPROM_FLOAT eepromValue;
        eepromValue.number = value;
        for(int offset = 0;offset < 4; offset++)
        {
          EEPROM.write(adress + offset, eepromValue.bytes[offset]);
        }
      }

      bool readIntFromEEPROM(int adress, int* outputValue)
      {
        bool allNoValue = true;
        
        EEPROM_INT eepromValue;
        for(int offset = 0;offset < 2; offset++)
        {
          uint8_t byteValue = EEPROM.read(adress + offset);
          if(byteValue != 255)
          {
            allNoValue = false;
          }
          eepromValue.bytes[offset] = byteValue;
        }
        if(allNoValue)
        {
          return false;
        }
        else
        {
          *outputValue = eepromValue.number;
          return true;
        }
      }

      void writeIntToEEPROM(int adress, int value)
      {
        int currentValue;
        if(readIntFromEEPROM(adress, &currentValue) && currentValue == value)
        {
          return;
        }
        EEPROM_INT eepromValue;
        eepromValue.number = value;
        for(int offset = 0;offset < 2; offset++)
        {
          EEPROM.write(adress + offset, eepromValue.bytes[offset]);
        }
      }

    public:
      EepromController(){}
      float ReadTempCoolerStart()
      {
        float tempCoolerStart;
        if(!readFloatFromEEPROM(adressTempCoolerStart, &tempCoolerStart))
        {
          tempCoolerStart = 70;
        }
        return tempCoolerStart;
      }

      float ReadTempHeaterStart()
      {
        float tempHeaterStart;
        if(!readFloatFromEEPROM(adressTempHeaterStart, &tempHeaterStart))
        {
          tempHeaterStart = 50;
        }
        return tempHeaterStart;
      }

      int ReadMeasurementDelay()
      {
        int measurementDelay;
        if(!readIntFromEEPROM(adressMeasurementDelay, &measurementDelay))
        {
          measurementDelay = 120; //2 min
        }
        return measurementDelay;
      }

      int ReadSendMeasurementDelay()
      {
        int sendMeasurementDelay;
        if(!readIntFromEEPROM(adressSendMeasurementDelay, &sendMeasurementDelay))
        {
          sendMeasurementDelay = 1200; //20 min
        }
        return sendMeasurementDelay;
      }

      void WriteTempCoolerStart(float value)
      {
        writeFloatToEEPROM(adressTempCoolerStart, value);
      }

      void WriteTempHeaterStart(float value)
      {
        writeFloatToEEPROM(adressTempHeaterStart, value);
      }

      void WriteMeasurementDelay(int value)
      {
        writeIntToEEPROM(adressMeasurementDelay, value);
      }

      void WriteSendMeasurementDelay(int value)
      {
        writeIntToEEPROM(adressSendMeasurementDelay, value);
      }

  };
#endif

#ifndef TempConfig
  class TempConfig
  {
    private:
      float tempCoolerStart;
      float tempCoolerEnd;
      float tempHeaterStart;
      float tempHeaterEnd;
      int measurementDelay;
      int sendMeasurementDelay;
      unsigned long lastSync = 0;
      bool isConfigModified = false;
      unsigned long lastChangeAfterSync = 0;
      EepromController* EepromControllerInstance;
      Scheduler* SchedulerInstance;
      void SetTempCoolerStartInternal(float value)
      {
        tempCoolerStart = value;
        tempCoolerEnd = tempCoolerStart - 5;
      }
      void SetTempHeaterStartInternal(float value)
      {
        tempHeaterStart = value;
        tempHeaterEnd = tempHeaterStart + 5;
      }
      void setConfigModified()
      {
        isConfigModified = true;
        lastChangeAfterSync = SchedulerInstance->GetTimeSinceTimestamp(lastSync);
      }
    public:
      TempConfig(const EepromController* eepromController, const Scheduler* scheduler)
      {
        EepromControllerInstance = eepromController;
        SchedulerInstance = scheduler;
        float tempCoolerStartLocal = EepromControllerInstance->ReadTempCoolerStart();
        float tempHeaterStartLocal = EepromControllerInstance->ReadTempHeaterStart();
        measurementDelay = EepromControllerInstance->ReadMeasurementDelay();
        sendMeasurementDelay = EepromControllerInstance->ReadSendMeasurementDelay();

        SetTempCoolerStartInternal(tempCoolerStart);
        SetTempHeaterStartInternal(tempHeaterStart);
      }
      float GetTempCoolerEnd()
      {
        return tempCoolerEnd;
      }
      float GetTempHeaterEnd()
      {
        return tempHeaterEnd;
      }
      float GetTempCoolerStart()
      {
        return tempCoolerStart;
      }
      float GetTempHeaterStart()
      {
        return tempHeaterStart;
      }
      void SetTempCoolerStart(float value, bool localUpdate = true)
      {
        if(localUpdate)
        {
          setConfigModified();
        }
        EepromControllerInstance->WriteTempCoolerStart(value);
        SetTempCoolerStartInternal(value);
      }
      void SetTempHeaterStart(float value, bool localUpdate = true)
      {
        if(localUpdate)
        {
          setConfigModified();
        }
        EepromControllerInstance->WriteTempHeaterStart(value);
        SetTempHeaterStartInternal(value);
      }
      int GetMeasurementDelay()
      {
        return measurementDelay;
      }
      int GetSendMeasurementDelay()
      {
        return sendMeasurementDelay;
      }
      void SetMeasurementDelay(int value, bool localUpdate = true)
      {
        if(localUpdate)
        {
          setConfigModified();
        }
        EepromControllerInstance->WriteMeasurementDelay(value);
        measurementDelay = value;
      }
      void SetSendMeasurementDelay(int value, bool localUpdate = true)
      {
        if(localUpdate)
        {
          setConfigModified();
        }
        EepromControllerInstance->WriteSendMeasurementDelay(value);
        sendMeasurementDelay = value;
      }
      unsigned long GetConfigUpdateOffset()
      {
        unsigned long lastChangeAfterSyncLocal;
        if(isConfigModified)
        {
          if(lastChangeAfterSync == 0)
          {
            lastChangeAfterSyncLocal = 1;
          }
          else
          {
            lastChangeAfterSyncLocal = lastChangeAfterSync;
          }
        }
        else
        {
          lastChangeAfterSyncLocal = 0;
        }
        lastSync = SchedulerInstance->GetCurrentTimestamp();
        lastChangeAfterSync = 0;
        isConfigModified = false;
        return lastChangeAfterSyncLocal;
      }
  };
#endif

#ifndef TempStatus
  class TempStatus
  {
    public:
      float ThermTemp;
      bool TempStateChanged;
      int TempStateId;
      TempStatus()
      {
        TempStateId = TempStateNoAction;
      } 
  };
#endif

#ifndef DataMeasurement
  class DataMeasurement {
    private:
      Scheduler* SchedulerInstance;
      TempStatus* TempStatusInstance;
      TempConfig* TempConfigInstance;
      unsigned long lastTimeStamp;

      char* data = "";
      void MeasureData()
      {
        String newState;
        
        switch(TempStatusInstance->TempStateId)
        {
          case TempStateHeater:
          newState = "Heat";
            break;
          case TempStateCooler:
          newState = "Cool";
            break;
          default:
          case TempStateNoAction:
            newState = "Idle";
            break;
        }
        unsigned long timestamp = SchedulerInstance->GetCurrentTimestamp();
        unsigned long timeElapsed = SchedulerInstance->GetTimeSinceTimestamp(lastTimeStamp);
        if(data != "")
        {
          sprintf(data,"%s %ul %s %f", data, timeElapsed, newState, TempStatusInstance->ThermTemp);
        }
        else
        {
          sprintf(data,"%ul %s %f", timeElapsed, newState, TempStatusInstance->ThermTemp);
        }
        lastTimeStamp = timestamp;
      }
    public:
      DataMeasurement(const Scheduler* scheduler, const TempStatus* tempStatus, const TempConfig* tempConfig)
      {
        SchedulerInstance = scheduler;
        TempStatusInstance = tempStatus;
        TempConfigInstance = tempConfig;
        lastTimeStamp = SchedulerInstance->GetCurrentTimestamp();
        SchedulerInstance->SetNextRun(SchedulerMeasurmentEventId, 0);
      };
      void CheckMeasurementSchedule()
      {
        if(SchedulerInstance->IsScheduleElapsed(SchedulerMeasurmentEventId))
        {
          SchedulerInstance->SetNextRunOffsetFromSheduledTime(SchedulerMeasurmentEventId, TempConfigInstance->GetMeasurementDelay());
          MeasureData();
        }
      }
      char* GetDataToSend()
      {
        char* dataLocal = data;
        data = "";
        return dataLocal;
      }
  };
#endif

#ifndef DataSender
  String serialCauseSendTemp = "sendTemp";
  String serialCauseConfigReq = "configReq";
  
  const int StateAwaitHeader = 0;
  const int StateReadParam = 1;
  const int StateEvaluateMessage = 2;

  const String messageInHeaderStart = "Msg";
  const int messageInConfigUpdateTypeId = 0;
  const String messageInTypeConfigUpdate = "CfgUp";
  const String messageInHeaderParamCount = "PrmC";
  const String messageInParamNameTempHeaterStart = "TmpHeat";
  const int messageInParamIdentTempHeaterStart = 0;
  const String messageInParamNameTempCoolerStart = "TmpCool";
  const int messageInParamIdentTempCoolerStart = 1;
  const String messageInParamNameMeasurementDelay = "MeasDly";
  const int messageInParamIdentMeasurementDelay = 2;
  const String messageInParamNameSendMeasurementDelay = "SendDly";
  const int messageInParamIdentSendMeasurementDelay = 3;

  class ParamData {
    public:
      ParamData(bool isStringDatatype, String valueString, String paramName)
      {
        IsStringDatatype = isStringDatatype;
        ValueString = valueString;   
        ParamName = paramName;
      }
      bool IsStringDatatype;
      String ValueString;
      String ParamName;
  };
  
  class Packet{
    private:
      byte bytesWriten = 0;
    public:
      Packet(){}
      int MessageTypeId = -1;
      int MessageState = StateAwaitHeader;
      int MessageStateBytes = 0;
      int ParamCount = -1;
      int ParamsRead = 0;
      String ParamValues[4];
      void ResetData()
      {
        MessageState = StateAwaitHeader;
        MessageTypeId = -1;
        MessageStateBytes = 0;
        ParamCount = -1;
        ParamsRead = 0;
      }
  };

  class DataSender {
    private:
      Scheduler* SchedulerInstance;
      TempConfig* TempConfigInstance;
      DataMeasurement* DataMeasurementInstance;
      Packet readHeaderData;
      void sendSerialMessage(String cause, ParamData params[], int itemCount)
      {
        Serial.print("{ \"cause\": \"");
        Serial.print(cause);
        Serial.print("\"");
        for (int i=0; i<itemCount; i++) {
          Serial.print(", \"");
          Serial.print(params[i].ParamName);
          Serial.print("\": ");
          if(params[i].IsStringDatatype)
          {
            Serial.print("\"");
          }
          Serial.print(params[i].ValueString);
          if(params[i].IsStringDatatype)
          {
            Serial.print("\"");
          }
        }
        Serial.println("}");
      };

      int minBufferSize = maxHeaderSize;
      //space is used as separator and at message start
      const int maxHeaderSize = 16;
      //9 - spaces and param identifier, 10 - param value
      const int maxParamSize = 19; 
      void SendConfigRequest()
      {
        ParamData params[] = { ParamData(false, String(TempConfigInstance->GetTempCoolerStart()), "min"), ParamData(false, String(TempConfigInstance->GetTempHeaterStart()), "max"), ParamData(false, String(TempConfigInstance->GetMeasurementDelay()), "interval"), ParamData(false, String(TempConfigInstance->GetSendMeasurementDelay()), "sendInterval") };
        sendSerialMessage(serialCauseConfigReq, params, sizeof params/sizeof params[0]);
      };
    public:
      DataSender(const Scheduler* scheduler, const TempConfig* tempConfig, const DataMeasurement* dataMeasurement)
      {
        SchedulerInstance = scheduler;
        TempConfigInstance = tempConfig;
        DataMeasurementInstance = dataMeasurement;
        SendConfigRequest();
        SchedulerInstance->SetNextRun(SchedulerMeasurmentEventId, TempConfigInstance->GetMeasurementDelay());
      };
       
      void CheckSerialIn()
      {
        int unreadBytes = Serial.available();
        if(unreadBytes >= minBufferSize)
        {
          String readString;
          switch(readHeaderData.MessageState)
          {
            case StateAwaitHeader:
              //mesage start string
              readString = Serial.readStringUntil(' ');
              if(messageInHeaderStart != readString)
              {
                break;
              }
              //messageType
              readString = Serial.readStringUntil(' ');

              bool messageTypeNotFound = false;
              if(readString == messageInTypeConfigUpdate)
              {
                readHeaderData.MessageTypeId = messageInConfigUpdateTypeId;
                for(int i =0; i<4; i++)
                {
                  readHeaderData.ParamValues[i] = "";
                }
              }
              else
              {
                messageTypeNotFound = true;
              }
              if(messageTypeNotFound)
              {
                break;
              }
              readString = Serial.readStringUntil(' ');
              if(readString != messageInHeaderParamCount)
              {
                readHeaderData.ResetData();
                break;
              }
              readString = Serial.readStringUntil(' ');
              if(readString.length() != 1 || readString[0] < '0' || readString[0]  > '9' )
              {
                readHeaderData.ResetData();
                break;
              }
              readHeaderData.ParamCount = readString.toInt();
              minBufferSize = maxParamSize;
              readHeaderData.MessageState = StateReadParam;
              break;
            case StateReadParam:
              readString = Serial.readStringUntil(' ');
              int messageIdent;
              if(messageInParamNameTempHeaterStart != readString)
              {
                messageIdent = messageInParamIdentTempHeaterStart;
              }
              else if(messageInParamNameTempCoolerStart != readString)
              {
                messageIdent = messageInParamIdentTempCoolerStart;
              }
              else if(messageInParamNameMeasurementDelay != readString)
              {
                messageIdent = messageInParamIdentMeasurementDelay;
              }
              else if(messageInParamNameSendMeasurementDelay != readString)
              {
                messageIdent = messageInParamIdentSendMeasurementDelay;
              }
              else
              {
                readHeaderData.ResetData();
                minBufferSize = maxHeaderSize;
                break;
              }
              readString = Serial.readStringUntil(' ');
              readHeaderData.ParamValues[messageIdent] = readString;
              readHeaderData.ParamsRead++;
              if(readHeaderData.ParamsRead < readHeaderData.ParamCount)
              {
                break;
              }
              readHeaderData.MessageState = StateEvaluateMessage;
              minBufferSize = 0;
            case StateEvaluateMessage:
              switch(readHeaderData.MessageTypeId)
              {
                case messageInConfigUpdateTypeId:
                  for(int i = 0; i < 4; i++)
                  {
                    if(readHeaderData.ParamValues[i] == "")
                    {
                      continue;
                    }
                    else
                    {
                      switch(i)
                      {
                        case messageInParamIdentTempHeaterStart:
                          TempConfigInstance->SetTempHeaterStart(readHeaderData.ParamValues[i].toFloat(), false);
                          break;
                        case messageInParamIdentTempCoolerStart:
                          TempConfigInstance->SetTempHeaterStart(readHeaderData.ParamValues[i].toFloat(), false);
                          break;
                        case messageInParamIdentMeasurementDelay:
                          TempConfigInstance->SetTempHeaterStart(readHeaderData.ParamValues[i].toInt(), false);
                          break;
                        case messageInParamIdentSendMeasurementDelay:
                          TempConfigInstance->SetTempHeaterStart(readHeaderData.ParamValues[i].toInt(), false);
                          break;
                      }
                    }
                  }
                  break;
                default:
                  break;
              }
              readHeaderData.ResetData();
              minBufferSize = maxHeaderSize;
              break;
          }
        }
      }

      void CheckComunicationSchedule()
      {
        if(SchedulerInstance->IsScheduleElapsed(SchedulerMeasurmentEventId))
        {
          SchedulerInstance->SetNextRunOffsetFromSheduledTime(SchedulerMeasurmentEventId, TempConfigInstance->GetMeasurementDelay());
          ParamData params[] = { ParamData(true, DataMeasurementInstance->GetDataToSend(), "dataString") };
          sendSerialMessage(serialCauseSendTemp, params, sizeof params / sizeof params[0]);
        }
      }
  };  
#endif

#ifndef LcdControler
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
      LcdControler(const Scheduler* scheduler, const TempConfig* tempConfig, const TempStatus* tempStatus) 
      {
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
        } else if (lcdStateId != LcdStateInactive) {
          checkAndSetLcdSleep();
        }
      }
  };
#endif


TempStatus TempStatusInstance;
Scheduler SchedulerInstance;
EepromController EepromControllerInstance;
TempConfig TempConfigInstance{&EepromControllerInstance, &SchedulerInstance};
LcdControler LcdControlerInstance{&SchedulerInstance, &TempConfigInstance, &TempStatusInstance};
DataMeasurement DataMeasurementInstance{&SchedulerInstance, &TempStatusInstance, &TempConfigInstance};
DataSender DataSenderInstace{&SchedulerInstance, &TempConfigInstance, &DataMeasurementInstance};

void setup() {
  pinMode(pinCooler, OUTPUT);
  pinMode(pinHeater, OUTPUT);
  pinMode(pinLcdV0, OUTPUT);
  pinMode(pinLcdLedPlus, OUTPUT);

  digitalWrite(pinCooler, LOW);
  digitalWrite(pinHeater, LOW);
  digitalWrite(pinLcdV0, LOW);
  digitalWrite(pinLcdLedPlus, LOW);

  Serial.begin(9600);
}

void loop() {
  SchedulerInstance.SchedulerUpdate();
  DataSenderInstace.CheckSerialIn();
  updateTempRegulation();
  LcdControlerInstance.UpdateLcd();
  DataSenderInstace.CheckComunicationSchedule();
  delay(1000);
}


void updateTempRegulation()
{
  int thermVal = analogRead(pinThermometer);
  // analogRead vrací číslo mezi 0 a 1023 proto deleni 1024, nasobení 5 je protože to je maximalni napětí
  float thermVolt = (thermVal / 1024.0) * 5;
  //TODO double check expresion
  float thermTemp = minTemp + thermVolt * tempPerVolt;

  bool tempStateChanged = false;
  switch(TempStatusInstance.TempStateId)
  {
    case TempStateHeater:
      if(thermTemp > TempConfigInstance.GetTempHeaterEnd())
      {
        digitalWrite(pinHeater, LOW);
        TempStatusInstance.TempStateId = TempStateNoAction;
        tempStateChanged= true;
      }
      break;
    case TempStateCooler:
      if(thermTemp < TempConfigInstance.GetTempCoolerEnd())
      {
        digitalWrite(pinCooler, LOW);
        TempStatusInstance.TempStateId = TempStateNoAction;
        tempStateChanged = true;
      }
      break;
    case TempStateNoAction:
    default:
      if(thermTemp < TempConfigInstance.GetTempHeaterStart())
      {
        digitalWrite(pinHeater, HIGH);
        TempStatusInstance.TempStateId = TempStateHeater;
        tempStateChanged = true;
      }
      else if (thermTemp > TempConfigInstance.GetTempCoolerStart())
      {
        digitalWrite(pinCooler, HIGH);
        TempStatusInstance.TempStateId = TempStateCooler;
        tempStateChanged = true;
      }
      break;
  }
  TempStatusInstance.TempStateChanged = tempStateChanged;
}

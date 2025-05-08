#include <Arduino.h>
#include "scheduler.h"
#include "tempConfig.h"
#include "dataMeasurement.h"

namespace DataSender
{
  String serialCauseSendTemp = "sendTemp";
  String serialCauseConfigReq = "configReq";
  String serialCauseUpdateRange = "updateRange";
  
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
      void SendConfigUpdate()
      {
        ParamData params[] = { ParamData(false, String(TempConfigInstance->GetTempCoolerStart()), "min"), ParamData(false, String(TempConfigInstance->GetTempHeaterStart()), "max"), ParamData(false, String(TempConfigInstance->GetMeasurementDelay()), "interval") };
        sendSerialMessage(serialCauseUpdateRange, params, sizeof params / sizeof params[0]);
      };
      void SendConfigRequest()
      {
        ParamData params[] = {};
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
                          TempConfigInstance->SetTempHeaterStart(readHeaderData.ParamValues[i].toFloat());
                          break;
                        case messageInParamIdentTempCoolerStart:
                          TempConfigInstance->SetTempHeaterStart(readHeaderData.ParamValues[i].toFloat());
                          break;
                        case messageInParamIdentMeasurementDelay:
                          TempConfigInstance->SetTempHeaterStart(readHeaderData.ParamValues[i].toInt());
                          break;
                        case messageInParamIdentSendMeasurementDelay:
                          TempConfigInstance->SetTempHeaterStart(readHeaderData.ParamValues[i].toInt());
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
}
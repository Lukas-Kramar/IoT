#include "lcdControler.h"
#include "dataSender.h"
#include "scheduler.h"
#include "dataSender.h"
#include "globalConstants.h"
#include "eepromWriter.h"

class TempStateClass {
  public:
    TempStateClass(){}
    static const int NoAction = 0;
    static const int Heater = 1;
    static const int Cooler = 2;
};

TempStateClass TempState;
int tempStateId = TempState.NoAction;
float tempCoolerStart;
float tempCoolerEnd;
float tempHeaterStart;
float tempHeaterEnd;
float tempSettingUpdate;
float tempSettingUpdateMax;
float tempSettingUpdateMin;
int measurementDelay;
int sendMeasurementDelay;
Scheduler SchedulerInstance;
LcdControler LcdControlerInstance(SchedulerInstance*);
EepromWriter EepromWriterInstance;
DataSender DataSenderInstace;




void setup() {
  // SchedulerInstance();
  // EepromWriterInstance();
  pinMode(pinCooler, OUTPUT);
  pinMode(pinHeater, OUTPUT);
  pinMode(pinLcdV0, OUTPUT);
  pinMode(pinLcdLedPlus, OUTPUT);

  digitalWrite(pinCooler, LOW);
  digitalWrite(pinHeater, LOW);
  digitalWrite(pinLcdV0, LOW);
  digitalWrite(pinLcdLedPlus, LOW);

  Serial.begin(9600);
  
  tempCoolerStart = EepromWriterInstance.EepromWriter::readTempCoolerStart();
  tempCoolerEnd = tempCoolerStart - 5;

  tempHeaterStart = EepromWriterInstance.readTempHeaterStart();
  tempHeaterEnd = tempHeaterStart + 5;
  
  measurementDelay = EepromWriterInstance.readMeasurementDelay();
  sendMeasurementDelay = EepromWriterInstance.readSendMeasurementDelay();

  SchedulerInstance.SetNextRun(SchedulerInstance.SchedulerMeasurmentEventId, measurementDelay);
  SchedulerInstance.SetNextRun(SchedulerInstance.SchedulerSendMeasurmentEventId, sendMeasurementDelay);
  DataSenderInstace.SendConfigRequest();
}

void loop() {
  SchedulerInstance.SchedulerUpdate();
  checkSerialIn();
  TempStatus tempStatus = updateTempRegulation();
  LcdControlerInstance.UpdateLcd(tempStatus);
  checkComunicationSchedule(tempStatus.ThermTemp);
  delay(1000);
}

int const headerSize = 3;
char const packetstart = '\n';
class Packet{
  private:
    byte messageBody[63];
    byte bytesWriten = 0;
  public:
    Packet(){}
    byte MessageLenght = -1;
    int MessageTypeId = -1;
    int BytesRead = 0;
    void ResetData()
    {
      MessageLenght = -1;
      MessageTypeId = -1;
      bytesWriten = 0;
      BytesRead = 0;
    }
    bool WriteByte(byte data)
    {
      if(bytesWriten < 63)
      {
        messageBody[bytesWriten] = data;
        bytesWriten++;
        return true;
      }
      else
      {
        return false;
      }
    }
    byte* ReadBytes()
    {
      byte data[bytesWriten];
      for(int i = 0; i < bytesWriten; i++)
      {
        data[i]=messageBody[i];
      }
      return data;
    }
};
Packet readHeaderData = Packet(); 

int getIntFromBytes(byte data[], int startIndex)
{
  Serial.print("convert ");
  Serial.print(data[startIndex + 3]);
  Serial.print(", ");
  Serial.print(data[startIndex + 2]);
  Serial.print(", ");
  Serial.print(data[startIndex + 1]);
  Serial.print(", ");
  Serial.print(data[startIndex + 0]);
  Serial.print(" TRY ");
  int output = (int)data[startIndex + 3];
  Serial.print(output);
  Serial.print(", ");
  output = output * 256 + (int)data[startIndex + 2];
  Serial.print(output);
  Serial.print(", ");
  output = output * 256 + (int)data[startIndex + 1];
  Serial.print(output);
  Serial.print(", ");
  output = output * 256 + (int)data[startIndex + 0];
  Serial.println(output);
  return output;
}

void checkSerialIn()
{
  int unreadBytes = Serial.available();
  if(unreadBytes >= headerSize - readHeaderData.BytesRead)
  {
    byte data;
    switch(readHeaderData.BytesRead)
    {
      case 0:
        while(readHeaderData.BytesRead == 0 && unreadBytes >= headerSize)
        {
          if(packetstart == Serial.read())
          {
            readHeaderData.BytesRead++;
          }
          unreadBytes--;
        }
        if(readHeaderData.BytesRead == 0)
        {
          break;
        }
      case 1:
        readHeaderData.MessageLenght = Serial.read();
        unreadBytes--;
        readHeaderData.BytesRead++;
      case 2:
        readHeaderData.MessageTypeId = Serial.read();
        unreadBytes--;
        readHeaderData.BytesRead++;
      case 3:
        if(unreadBytes >= readHeaderData.MessageLenght)
        {
          for (int i=0; i<readHeaderData.MessageLenght; i++)
          {
            readHeaderData.WriteByte(Serial.read());
          }
          byte* data = readHeaderData.ReadBytes();
          switch(readHeaderData.MessageTypeId)
          {
            case 1:
              tempCoolerStart = (float)getIntFromBytes(data, 0);
              //writeFloatToEEPROM(adressTempCoolerStart, tempCoolerStart);
              tempCoolerEnd = tempHeaterStart - 5;
              tempHeaterStart = (float)getIntFromBytes(data, 4);
              //writeFloatToEEPROM(adressTempHeaterStart, tempHeaterStart);
              tempHeaterEnd = tempHeaterStart + 5;
              reportMeasurementDelay = getIntFromBytes(data, 8);
              //writeFloatToEEPROM(adressReportMeasurementDelay, reportMeasurementDelay);
              ParamData params[] = {ParamData(false, String(tempCoolerStart), "min"), ParamData(false, String(tempHeaterStart), "max"), ParamData(false, String(reportMeasurementDelay), "interval")};
              sendSerialMessage("debug", params, sizeof params/sizeof params[0]);
              break;
            default:
              break;

          }
          readHeaderData.ResetData();
        }
        break;
    }
  }
}

TempStatus updateTempRegulation()
{
  int thermVal = analogRead(pinThermometer);
  // analogRead vrací číslo mezi 0 a 1023 proto deleni 1024, nasobení 5 je protože to je maximalni napětí
  float thermVolt = (thermVal / 1024.0) * 5;
  //TODO double check expresion
  float thermTemp = minTemp + thermVolt * tempPerVolt;

  bool tempStateChanged = false;
  switch(tempStateId)
  {
    case TempState.Heater:
      if(thermTemp > tempHeaterEnd)
      {
        digitalWrite(pinHeater, LOW);
        tempStateId = TempState.NoAction;
        tempStateChanged= true;
      }
      break;
    case TempState.Cooler:
      if(thermTemp < tempCoolerEnd)
      {
        digitalWrite(pinCooler, LOW);
        tempStateId = TempState.NoAction;
        tempStateChanged = true;
      }
      break;
    case TempState.NoAction:
    default:
      if(thermTemp < tempHeaterStart)
      {
        digitalWrite(pinHeater, HIGH);
        tempStateId = TempState.Heater;
        tempStateChanged = true;
      }
      else if (thermTemp > tempCoolerStart)
      {
        digitalWrite(pinCooler, HIGH);
        tempStateId = TempState.Cooler;
        tempStateChanged = true;
      }
      break;
  }
  if(tempStateChanged)
  {
    sendStateChenge();
  }
  return TempStatus(thermTemp, tempStateChanged);
}

void sendStateChenge()
{
  String newState;
  switch(tempStateId)
  {
    case TempState.Heater:
      newState = "Heating";
      break;
    case TempState.Cooler:
      newState = "Cooling";
      break;
    case TempState.NoAction:
      newState = "Idle";
    default:
      break;
  }
  ParamData params[] = {ParamData(true, newState, "newState")};
  sendSerialMessage(serialCauseStateChange, params, sizeof params/sizeof params[0]);
}


void checkComunicationSchedule(float thermTemp)
{
  if(schedulerEvents[SchedulerMeasurmentEventId].IsScheduleElapsed())
  {
    schedulerEvents[SchedulerMeasurmentEventId].SetNextRunOffsetFromSheduledTime(measurementDelay);
    ParamData params[] = {ParamData(false, String(thermTemp), "temp")};
    sendSerialMessage(serialCauseTempOut, params, sizeof params/sizeof params[0]);
  }
  if(schedulerEvents[SchedulerSendMeasurmentEventId].IsScheduleElapsed())
  {
    schedulerEvents[SchedulerSendMeasurmentEventId].SetNextRunOffsetFromSheduledTime(sendMeasurementDelay);
    sendSerialMessage(serialCauseSendTemp, params, sizeof params/sizeof params[0]);
  }
}

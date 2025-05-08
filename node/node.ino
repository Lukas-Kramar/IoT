#include "lcdControler.h"
#include "dataSender.h"
#include "scheduler.h"
#include "dataSender.h"
#include "globalConstants.h"
#include "tempStatus.h"
#include "dataMeasurement.h"

TempStatus TempStatusInstance;
Scheduler SchedulerInstance;
EepromController EepromControllerInstance;
TempConfig TempConfigInstance{&EepromControllerInstance};
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


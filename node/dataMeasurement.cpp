#include "scheduler.h"
#include "tempStatus.h"
#include "tempConfig.h"
#include <Arduino.h>

namespace DataMeasurement
{
  class DataMeasurement
  {
    private:
      Scheduler* SchedulerInstance;
      TempStatus* TempStatusInstance;
      TempConfig* TempConfigInstance;
      unsigned long lastTimeStamp;
      String data = "";
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
        unsigned long timeElapsed = 0;
        if(timestamp < lastTimeStamp)
        {
          timeElapsed = SchdulerMaxSeconds;
        }
        timeElapsed = timeElapsed + timestamp - lastTimeStamp;
        if(data != "")
        {
          data = data + " ";
        }
        sprintf(data,"%s %ul %s %f", data, timeElapsed, newState, TempStatusInstance->ThermTemp);
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
      String GetDataToSend()
      {
        String dataLocal = data;
        data = "";
        return dataLocal;
      }
  };
}
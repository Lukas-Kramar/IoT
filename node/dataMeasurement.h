#ifndef DATAMEASUREMENT_H
#define DATAMEASUREMENT_H
#include "scheduler.h"
#include "tempStatus.h"
#include "tempConfig.h"
#include <Arduino.h>

class DataMeasurement
{
  public:
    DataMeasurement(const Scheduler* scheduler, const TempStatus* tempStatus, const TempConfig* tempConfig);
    void CheckMeasurementSchedule();
    String GetDataToSend();
};

#endif
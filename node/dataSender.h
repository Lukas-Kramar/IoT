#ifndef DATASENDER_H
#define DATASENDER_H
#include "scheduler.h"
#include "tempConfig.h"
#include "dataMeasurement.h"

class DataSender {
  public:
    DataSender(const Scheduler* scheduler, const TempConfig* tempConfig, const DataMeasurement* dataMeasurement);
    void CheckSerialIn();
    void CheckComunicationSchedule();
};

#endif
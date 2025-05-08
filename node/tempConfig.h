#ifndef TEMPCONFIG_H
#define TEMPCONFIG_H
#include "eepromController.h"

  class TempConfig
  {
    public:
      TempConfig(const EepromController* EepromController);
      float GetTempCoolerEnd();
      float GetTempHeaterEnd();
      float GetTempCoolerStart();
      float GetTempHeaterStart();
      void SetTempCoolerStart(float value);
      void SetTempHeaterStart(float value);
      int GetMeasurementDelay();
      int GetSendMeasurementDelay();
      void SetMeasurementDelay(int value);
      void SetSendMeasurementDelay(int value);
      bool WasConfigUpdated();
  };

#endif
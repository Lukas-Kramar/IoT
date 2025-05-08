#include "eepromController.h"

namespace TempConfig
{
  class TempConfig
  {
    private:
      float tempCoolerStart;
      float tempCoolerEnd;
      float tempHeaterStart;
      float tempHeaterEnd;
      int measurementDelay;
      int sendMeasurementDelay;
      bool isConfigUpdated;
      EepromController* EepromControllerInstance;
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
    public:
      TempConfig(const EepromController* EepromController)
      {
        EepromControllerInstance = EepromController;
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
      void SetTempCoolerStart(float value)
      {
        isConfigUpdated = true;
        EepromControllerInstance->WriteTempCoolerStart(value);
        SetTempCoolerStartInternal(value);
      }
      void SetTempHeaterStart(float value)
      {
        isConfigUpdated = true;
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
      void SetMeasurementDelay(int value)
      {
        isConfigUpdated = true;
        EepromControllerInstance->WriteMeasurementDelay(value);
        measurementDelay = value;
      }
      void SetSendMeasurementDelay(int value)
      {
        isConfigUpdated = true;
        EepromControllerInstance->WriteSendMeasurementDelay(value);
        sendMeasurementDelay = value;
      }
      //Resets isConfigUpdated
      bool WasConfigUpdated()
      {
        bool isConfigUpdatedLocal = isConfigUpdated;
        isConfigUpdated = false;
        return isConfigUpdatedLocal;
      }
  };
}


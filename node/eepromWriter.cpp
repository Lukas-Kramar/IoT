#include <EEPROM.h>
#include <stdlib.h>

namespace EepromWriter
{
  class EepromWriter
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
        EEPROM_INT eepromValue;
        eepromValue.number = value;
        for(int offset = 0;offset < 2; offset++)
        {
          EEPROM.write(adress + offset, eepromValue.bytes[offset]);
        }
      }

    public:
      EepromWriter(){}
      float readTempCoolerStart()
      {
        float tempCoolerStart;
        if(!readFloatFromEEPROM(adressTempCoolerStart, &tempCoolerStart))
        {
          tempCoolerStart = 70;
        }
        return tempCoolerStart;
      }

      float readTempHeaterStart()
      {
        float tempHeaterStart;
        if(!readFloatFromEEPROM(adressTempHeaterStart, &tempHeaterStart))
        {
          tempHeaterStart = 50;
        }
        return tempHeaterStart;
      }

      int readMeasurementDelay()
      {
        int measurementDelay;
        if(!readIntFromEEPROM(adressMeasurementDelay, &measurementDelay))
        {
          measurementDelay = 120; //2 min
        }
        return measurementDelay;
      }

      int readSendMeasurementDelay()
      {
        int sendMeasurementDelay;
        if(!readIntFromEEPROM(adressSendMeasurementDelay, &sendMeasurementDelay))
        {
          sendMeasurementDelay = 1200; //20 min
        }
        return sendMeasurementDelay;
      }

  };
}
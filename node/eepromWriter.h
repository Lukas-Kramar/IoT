#ifndef EEPROMWRITER_H
#define EEPROMWRITER_H

class EepromWriter
{
  public:
    EepromWriter();
    float readTempCoolerStart();
    float readTempHeaterStart();
    int readMeasurementDelay();
    int readSendMeasurementDelay();

};
#endif
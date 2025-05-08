#ifndef EEPROMCONTROLLER_H
#define EEPROMCONTROLLER_H

class EepromController
{
  public:
    EepromController();
    float ReadTempCoolerStart();
    float ReadTempHeaterStart();
    int ReadMeasurementDelay();
    int ReadSendMeasurementDelay();
    void WriteTempCoolerStart(float value);
    void WriteTempHeaterStart(float value);
    void WriteMeasurementDelay(int value);
    void WriteSendMeasurementDelay(int value);

};
#endif
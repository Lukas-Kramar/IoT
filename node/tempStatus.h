#ifndef TEMPSTATUS_H
#define TEMPSTATUS_H

#define TempStateNoAction 0
#define TempStateHeater 1
#define TempStateCooler 2

class TempStatus
{
  public:
    TempStatus();
    float ThermTemp;
    bool TempStateChanged;
    int TempStateId;
};
#endif
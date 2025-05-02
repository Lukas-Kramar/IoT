#ifndef TEMPSTATUS_H
#define TEMPSTATUS_H

class TempStatus
{
  public:
    float ThermTemp;
    bool TempStateChanged;
    TempStatus(float thermTemp, bool tempStateChanged)
    {
      ThermTemp = thermTemp;
      TempStateChanged = tempStateChanged;
    } 
};

#endif
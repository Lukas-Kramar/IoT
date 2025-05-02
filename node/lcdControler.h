#ifndef LCDCONTROLER_H
#define LCDCONTROLER_H
#include "tempStatus.h"
class LcdControler
{ 
  private:
  public:
    LcdControler(Scheduler &);
    void UpdateLcd(TempStatus tempStatus);
};

#endif
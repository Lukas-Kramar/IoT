#ifndef LCDCONTROLER_H
#define LCDCONTROLER_H

#include "tempConfig.h"
#include "tempStatus.h"
#include "scheduler.h"

class LcdControler
{ 
  private:
  public:
    LcdControler(const Scheduler* scheduler, const TempConfig* TempConfig, const TempStatus* tempStatus);
    void UpdateLcd();
};

#endif
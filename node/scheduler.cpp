#include <Arduino.h>

namespace NScheduler
{
  class SchedulerEvent {
    private:
      bool isActive = false;
      unsigned long sheduledTime = 0;
      static const unsigned long maxMilis = UINT32_MAX;
    public:
      SchedulerEvent(){};
      void CorrectSchedulerOverflow()
      {
        if(isActive)
        {
          if(sheduledTime > maxMilis)
          {
            sheduledTime = sheduledTime - maxMilis;
          }
          else
          {
            //scheduled time was less than maxMilis, run it (may run it next loop in the rare case where sheduled time is 0)
            sheduledTime = 0;
          }
        }
      }
      bool IsScheduleElapsed(unsigned long curentSchedulerTimestamp)
      {
        if(isActive && sheduledTime < curentSchedulerTimestamp)
        {
          isActive = false;
          return true;
        }
        return false;
      }
      // bool IsActive()
      // {
      //   return isActive;
      // }
      void SetNextRun(unsigned long nextRun)
      {
        isActive = true;
        sheduledTime = nextRun;
      }
      void SetNextRunOffsetFromTime(unsigned long nextRunOffset, unsigned long offsetFrom)
      {
        isActive = true;
        sheduledTime = offsetFrom + nextRunOffset;
      }
      void SetNextRunOffsetFromSheduledTime(unsigned long nextRunOffset)
      {
        isActive = true;
        sheduledTime = sheduledTime + nextRunOffset;
      }
  };

  class Scheduler {
    private:
      SchedulerEvent schedulerEvents[3];
      static const int schedulerSensitivity = 1000; 
      unsigned long lastSchedulerTimestamp = 0;
      unsigned long curentSchedulerTimestamp;   
    public:
      static const int SchedulerScreenShutoffEventId = 0;
      static const int SchedulerMeasurmentEventId = 1;
      static const int SchedulerSendMeasurmentEventId = 2;
      Scheduler()
      {
        schedulerEvents[Scheduler::SchedulerScreenShutoffEventId] = SchedulerEvent();
        schedulerEvents[Scheduler::SchedulerMeasurmentEventId] = SchedulerEvent();
        schedulerEvents[Scheduler::SchedulerSendMeasurmentEventId] = SchedulerEvent();
      }
      void SchedulerUpdate()
      {
        //jedna iterace loop je +- 1000ms
        //z tohoto duvodu nepotřebujem tak moc přesnou konverzi z ms od startu do schuler timestampu
        //nechceme 1:1 vztah mezi millis a timestampem protože milis overflowne jednou za ~51 dni, což by mohlo způsobit selhani scheduleru;
        curentSchedulerTimestamp = millis() / schedulerSensitivity;

        if(lastSchedulerTimestamp > curentSchedulerTimestamp)
        {
          //došlo k overflow millis()
          int itemCount =sizeof schedulerEvents/sizeof schedulerEvents[0];
          for (int i=0; i<itemCount; i++) {
              schedulerEvents[i].CorrectSchedulerOverflow();
          }
        }

        lastSchedulerTimestamp = curentSchedulerTimestamp;
      }
      bool IsScheduleElapsed(int index)
      {
        if(schedulerEvents < 0 || schedulerEvents >= 3)
        {
          schedulerEvents[index].IsScheduleElapsed(curentSchedulerTimestamp);
        }
      }
      void SetNextRun(int index, unsigned long nextRun)
      {
        if(schedulerEvents < 0 || schedulerEvents >= 3)
        {
          schedulerEvents[index].SetNextRun(nextRun);
        }
      }
      void SetNextRunOffsetFromTime(int index, unsigned long nextRun, unsigned long offsetFrom)
      {
        if(schedulerEvents < 0 || schedulerEvents >= 3)
        {
          schedulerEvents[index].SetNextRunOffsetFromTime(nextRun, offsetFrom);
        }
      }
      void SetNextRunOffsetFromCurrentTime(int index, unsigned long nextRun)
      {
        if(schedulerEvents < 0 || schedulerEvents >= 3)
        {
          schedulerEvents[index].SetNextRunOffsetFromTime(nextRun, curentSchedulerTimestamp);
        }
      }
      bool SetNextRunOffsetFromSheduledTime(int index, unsigned long nextRun)
      {
        if(schedulerEvents < 0 || schedulerEvents >= 3)
        {
          schedulerEvents[index].SetNextRunOffsetFromSheduledTime(nextRun);
        }
      }
  };
}

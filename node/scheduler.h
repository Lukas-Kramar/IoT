#ifndef SCHEDULER_H
#define SCHEDULER_H

#define SchedulerScreenShutoffEventId 1
#define SchedulerMeasurmentEventId 2
#define SchedulerSendMeasurmentEventId 3
#define SchdulerMaxSeconds UINT32_MAX/1000

class Scheduler
{
  public:
    Scheduler();
    void SchedulerUpdate();
    unsigned long GetCurrentTimestamp();
    bool IsScheduleElapsed(int index);
    void SetNextRun(int index, unsigned long nextRun);
    void SetNextRunOffsetFromTime(int index, unsigned long nextRun, unsigned long offsetFrom);
    void SetNextRunOffsetFromCurrentTime(int index, unsigned long nextRun);
    bool SetNextRunOffsetFromSheduledTime(int index, unsigned long nextRun);

};

#endif
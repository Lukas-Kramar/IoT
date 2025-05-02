#ifndef SCHEDULER_H
#define SCHEDULER_H

class Scheduler
{
  public:
    static const int SchedulerScreenShutoffEventId;
    static const int SchedulerMeasurmentEventId;
    static const int SchedulerSendMeasurmentEventId;
    Scheduler();
    void SchedulerUpdate();
    bool IsScheduleElapsed(int index);
    void SetNextRun(int index, unsigned long nextRun);
    void SetNextRunOffsetFromTime(int index, unsigned long nextRun, unsigned long offsetFrom);
    void SetNextRunOffsetFromCurrentTime(int index, unsigned long nextRun);
    bool SetNextRunOffsetFromSheduledTime(int index, unsigned long nextRun);
};

#endif
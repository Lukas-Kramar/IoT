#define TempStateNoAction 0
#define TempStateHeater 1
#define TempStateCooler 2

namespace TempStatus
{
  class TempStatus
  {
    public:
      float ThermTemp;
      bool TempStateChanged;
      int TempStateId;
      TempStatus()
      {
        TempStateId = TempStateNoAction;
      } 
  };
}
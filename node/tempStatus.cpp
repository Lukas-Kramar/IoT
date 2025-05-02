namespace TempStatus
{
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
}
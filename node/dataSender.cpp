#include <Arduino.h>
namespace DataSender
{
  String serialCauseTempOut = "tempOut";
  String serialCauseSendTemp = "sendTemp";
  String serialCauseConfigReq = "configReq";
  String serialCauseStateChange = "stateChange";
  String serialCauseUpdateRange = "updateRange";

  class ParamData {
    public:
      ParamData(bool isStringDatatype, String valueString, String paramName)
      {
        IsStringDatatype = isStringDatatype;
        ValueString = valueString;   
        ParamName = paramName;
      }
      bool IsStringDatatype;
      String ValueString;
      String ParamName;
  };

  class DataSender {
    private:
      void sendSerialMessage(String cause, ParamData params[], int itemCount)
      {
        Serial.print("{ \"cause\": \"");
        Serial.print(cause);
        Serial.print("\"");
        for (int i=0; i<itemCount; i++) {
          Serial.print(", \"");
          Serial.print(params[i].ParamName);
          Serial.print("\": ");
          if(params[i].IsStringDatatype)
          {
            Serial.print("\"");
          }
          Serial.print(params[i].ValueString);
          if(params[i].IsStringDatatype)
          {
            Serial.print("\"");
          }
        }
        Serial.println("}");
      };
    public:
      DateSender(){};
      SendConfigRequest()
      {
        ParamData params[] = {};
        sendSerialMessage(serialCauseConfigReq, params, sizeof params/sizeof params[0]);
      };
  };  
}
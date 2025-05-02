#include <Arduino.h>
namespace GlobalConstants
{
  static const int pinCooler = 6;
  static const int pinHeater = 7;
  static const int pinLcdV0 = 9;
  static const int pinLcdLedPlus = 8;

  static const int pinThermometer = A5;
  static const int pinButtonMain = A4;
  static const int pinButtonPlus = A3;
  static const int pinButtonMinus = A2;
  static const float minTemp = -50;
  static const float tempPerVolt = 100;
  static const float maxTemp = minTemp + 5 * tempPerVolt;
}
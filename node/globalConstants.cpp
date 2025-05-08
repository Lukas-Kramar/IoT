#include <Arduino.h>
#define pinCooler 6
#define pinHeater  7
#define pinLcdV0 9
#define pinLcdLedPlus 8
#define pinThermometer A5
#define pinButtonMain A4
#define pinButtonPlus A3
#define pinButtonMinus A2
#define minTemp -50
#define tempPerVolt 100
#define maxTemp minTemp + 5 * tempPerVolt
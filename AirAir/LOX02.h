#ifndef LOX02_h
#define LOX02_h
#include "Arduino.h"
#include "SoftwareSerial.h"
class LOX02{
  public:
    LOX02(int RXPin,int TXPin) : _O2(RXPin, TXPin){
      _RX_PIN = RXPin;
      _TX_PIN = TXPin;
    }; // Constructor
    bool initialize();
    double getO2();
    double getO2P();
    double getP();
    double lastO2 = -1;
    double lastO2P = -1;
    double lastP = -1;
    int errorCount = 5;
    const int MAX_ERRROR = 5;
  private:
    int _RX_PIN;
    int _TX_PIN;
    SoftwareSerial _O2;
};
#endif


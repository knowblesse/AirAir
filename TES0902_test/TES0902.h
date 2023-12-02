#ifndef TES0902_h
#define TES0902_h
#include "Arduino.h"
#include "SoftwareSerial.h"
class TES0902{
  public:
    TES0902(int RXPin,int TXPin) : _CO2(RXPin, TXPin){
      _RX_PIN = RXPin;
      _TX_PIN = TXPin;
    }; // Constructor
    void initialize();
    float readCO2();
    bool integrityCO2;
  private:
    int _RX_PIN;
    int _TX_PIN;
    bool _checkCRC(uint8_t *cmd, int cmd_length);
    SoftwareSerial _CO2;

};
#endif

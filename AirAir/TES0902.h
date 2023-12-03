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
    int readCO2();
    bool integrityCO2;
    int lastCO2val = 0;
    int errorCount = 5;
    const int MAX_ERRROR = 5;
  private:
    int _RX_PIN;
    int _TX_PIN;
    bool _checkCRC(uint8_t *cmd, int cmd_length, uint16_t crc);
    SoftwareSerial _CO2;
    int _returnLastValue();

};
#endif

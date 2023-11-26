#ifndef SHT1X_h
#define SHT1X_h
#include "Arduino.h"
class SHT1X{
  public:
    SHT1X(int dataPin,int clockPin); // Constructor
    void initialize();
    float readTemp();
    float readHumd();
    void reset();
    bool integrityTemp;
    bool integrityHumd;
  private:
    int _PIN_SDA;
    int _PIN_SCK;
    bool _sendCommand(uint8_t command);
    uint16_t* _readData();
    bool _checkCRC(uint8_t command, uint8_t val1, uint8_t val2, uint8_t crc);

};
#endif

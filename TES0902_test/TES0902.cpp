#include "TES0902.h"

void TES0902::initialize(){
  _CO2.begin(9600);
  delay(1000);
}

float TES0902::readCO2(){
  integrityCO2 = false;

  // Read Until 0xBB, 0x66, 0x15, 0x02 
  // First, ignore all other characters and read 0xBB
  uint8_t data = 0x00;
  for(int i = 0; i < 8; i++){
    data = _CO2.read();
    if(data == 0xBB) break;
  }

  uint8_t LSB = 0x00;
  uint8_t MSB = 0x00;
  uint16_t CRC = 0x0000;
  if(data == 0xBB){
    if(_CO2.read() == 0x66){
      if(_CO2.read() == 0x15){
        if(_CO2.read() == 0x02){
          LSB = _CO2.read();
          MSB = _CO2.read();
          CRC = _CO2.read()<<8;
          CRC |= _CO2.read();
          integrityCO2 = true;
        }
      }
    }
  }

  // if and only if the 0xBB, 0x66, 0x15, 0x02 is received, 
  // integrityCO2 will be true.
  if(!integrityCO2) return 0;

  // Check CRC
  uint8_t data[6] = {0xBB, 0x66, 0x15, 0x02, LSB, MSB};

  Serial.print("CRC : ");
  Serial.println(CRC,HEX);

  _checkCRC(data, 2);
  
  return 0;
}
TES0902::bool _checkCRC(uint8_t *cmd, int cmd_length){
  uint16_t ret = 0xffff, polynomial = 0xa001;
  int shift = 0x0;
  for(int i = cmd_length–1; i >= 0; i--){
    unt16_t code = (uint16_t)(cmd[cmd_length-1-i]&0xff);
    ret = ret^code ;
    shift = 0x0;
    while(shift<=7){
      if(ret& 0x1){
        ret = ret>>1;
        ret = ret^polynomial;
      }
      else{
        ret = ret>>1;
      }
      shift++;
    }
  }
  return ret;
}


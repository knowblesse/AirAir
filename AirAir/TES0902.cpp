#include "TES0902.h"

void TES0902::initialize(){
  _CO2.begin(9600);
  delay(1000);
  _CO2.listen();
}
int TES0902::_returnLastValue(){
  if(errorCount < MAX_ERRROR){
    errorCount++;
    integrityCO2 = true;
    return lastCO2val;
  }
  else{
    return 0;
  }
}

int TES0902::readCO2(){
  integrityCO2 = false;
  _CO2.listen();

  // Clear away all received bytes to recieve fresh data
  if(_CO2.overflow()){
    //Serial.println("overflow occured");
    while(_CO2.available()){
      //Serial.println("byte cleared");
      _CO2.read();
    }
  }
  else {
    while((_CO2.peek() != 0xBB) && _CO2.available()){
      _CO2.read();
      //Serial.println("non starting byte cleared");
    }
    
  }

  const byte START_SEQ[] = {0xBB, 0x66, 0x15, 0x02};
  const int FRAME_LENGTH = 8;
  uint8_t co2_lsb = 0x00;
  uint8_t co2_msb = 0x00;
  uint16_t crc = 0x0000;

  // Wait until 8 byte is received. But if timeout has reached, return 0
  unsigned long startTime = millis();
  const unsigned long TIMEOUT = 4000;
  while( (_CO2.available() < FRAME_LENGTH) && (millis()-startTime < TIMEOUT)){
  }
  if(millis() - startTime >= TIMEOUT){
    // Serial.println(_CO2.available());
    // Serial.println("Timeout Error");
    return _returnLastValue();
  }

  // Store correct FRAME_LENGTH data using START_SEQ
  bool isInitialSeqMatched = true;
  for(int i=0; i<4; i++){
    if(_CO2.read() != START_SEQ[i]){
      isInitialSeqMatched = false;
      break;
    }
  }

  if(!isInitialSeqMatched){
    //Serial.println("Seq not match error");
    return _returnLastValue(); // if initial Seq was not matched, then skip the process.
  } 

  // Go further only if the initial sequence is matched
  static byte dataframe[FRAME_LENGTH] = {0xBB, 0x66, 0x15, 0x02, 0x00, 0x00, 0x00, 0x00};
  for(int i=4; i<FRAME_LENGTH; i++){
    dataframe[i] = _CO2.read();
  }

  co2_lsb = dataframe[4];
  co2_msb = dataframe[5];
  crc = dataframe[6] | (dataframe[7]<<8);
  if(_checkCRC(dataframe, 6, crc)){
    integrityCO2 = true;
    errorCount = 0;
    lastCO2val = ((int)co2_msb)*256 + ((int)co2_lsb);
    return lastCO2val;
  }
  else {
    //Serial.println("crc not match error");
    return _returnLastValue();
  }
}
bool TES0902::_checkCRC(uint8_t *cmd, int cmd_length, uint16_t crc){
  uint16_t ret = 0xffff, polynomial = 0xa001;
  int shift = 0x0;
  for(int i = cmd_length-1; i >= 0; i--){
    uint16_t code = (uint16_t)(cmd[cmd_length-1-i]&0xff);
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
  return ret == crc;
}


#include "SHT1X.h"

SHT1X::SHT1X(int PIN_SDA, int PIN_SCK){
  _PIN_SDA = PIN_SDA;
  _PIN_SCK = PIN_SCK;
}

void SHT1X::initialize(){
  pinMode(_PIN_SCK, OUTPUT);
  digitalWrite(_PIN_SCK, LOW); // SCK start from LOW
}

void SHT1X::_sda(int state){
  pinMode(_PIN_SDA, OUTPUT);
  delay(1);
  digitalWrite(_PIN_SDA, state);
  delay(1);
  pinMode(_PIN_SDA, INPUT);
}

bool SHT1X::_sendCommand(uint8_t command){
  // Initialize (Starting state)
  pinMode(_PIN_SDA, OUTPUT);
  pinMode(_PIN_SCK, OUTPUT);
  digitalWrite(_PIN_SCK, LOW);
  digitalWrite(_PIN_SDA, HIGH);

  //Send "Transmission Start"
  digitalWrite(_PIN_SCK, HIGH);
  digitalWrite(_PIN_SDA, LOW);
  digitalWrite(_PIN_SCK, LOW);
  digitalWrite(_PIN_SCK, HIGH);
  digitalWrite(_PIN_SDA, HIGH);
  digitalWrite(_PIN_SCK, LOW);

  // Send Command
  shiftOut(_PIN_SDA, _PIN_SCK, MSBFIRST, command);

  // Check ACK
  pinMode(_PIN_SDA, INPUT);
  int ack1 = digitalRead(_PIN_SDA); // This should be LOW
  digitalWrite(_PIN_SCK, HIGH);
  digitalWrite(_PIN_SCK, LOW);
  int ack2 = digitalRead(_PIN_SDA);

  // Print ACK result
  return (ack1==0)&&(ack2==1);
}

uint16_t* SHT1X::_readData(){
  // Read first byte
  uint16_t value = 0;
  for(int i=0; i<8; i++){
    value |= digitalRead(_PIN_SDA)<<(8+7-i);   
    digitalWrite(_PIN_SCK, HIGH);
    delay(1);
    digitalWrite(_PIN_SCK, LOW);
    delay(1);
  }

  // Send ACK
  //Serial.print("ACK1: ");
  //Serial.println(digitalRead(_PIN_SDA));
  _sda(LOW);
  delay(1);
  digitalWrite(_PIN_SCK, HIGH);
  delay(1);
  digitalWrite(_PIN_SCK, LOW);
  delay(1);
  
  // Read second byte
  for(int i=0; i<8; i++){
    value |= digitalRead(_PIN_SDA)<<(7-i);
    digitalWrite(_PIN_SCK, HIGH);
    delay(1);
    digitalWrite(_PIN_SCK, LOW);
    delay(1);
  }

  // Send ACK
  //Serial.print("ACK2: ");
  //Serial.println(digitalRead(_PIN_SDA));
  _sda(LOW);
  delay(1);
  digitalWrite(_PIN_SCK, HIGH);
  delay(1);
  digitalWrite(_PIN_SCK, LOW);
  delay(1);

  // Read CRC
  uint16_t crc = 0;
  for(int i=0; i<8; i++){
    crc |= digitalRead(_PIN_SDA)<<(7-i);
    digitalWrite(_PIN_SCK, HIGH);
    delay(1);
    digitalWrite(_PIN_SCK, LOW);
    delay(1);
  }

  // Send ACK
  //Serial.print("ACK3: ");
  //Serial.println(digitalRead(_PIN_SDA));
  _sda(LOW);
  delay(1);
  digitalWrite(_PIN_SCK, HIGH);
  delay(1);
  digitalWrite(_PIN_SCK, LOW);
  delay(1);

  static uint16_t output[2] = {value, crc};
  return output;
}


float SHT1X::readTemp(){
  uint8_t command = 0b00000011;
  integrityTemp = false;
  bool sensorReceived = _sendCommand(command);
  Serial.print("Sensor Received : ");
  Serial.println(sensorReceived);

  // check measurement complete
  Serial.println("Wait for measurement complete");
  pinMode(_PIN_SDA, INPUT);
  while (digitalRead(_PIN_SDA)){}
  Serial.println("Measurement complete");

  // readData
  uint16_t* result = _readData();
  uint16_t value = result[0];
  uint8_t crc = (uint8_t)result[1];

  // Check Output
  //Serial.println("Temp:");
  //Serial.println((unsigned int)value);
  
  float d1 = -40.1;
  float d2 = 0.01; 
  float temp = d1 + d2 * (float)value;
  
  // Check CRC result for temp
  bool crcResult = _checkCRC(command,value>>8, value & 0xFF, crc);
  integrityTemp = crcResult;
  
  return temp;
}

float SHT1X::readHumd(){
  uint8_t command = 0b00000101;
  integrityHumd = false;
  _sendCommand(command);

  // check measurement complete
  while (digitalRead(_PIN_SDA)){}

  // readData
  uint16_t* result = _readData();
  uint16_t value = result[0];
  uint8_t crc = (uint8_t)result[1];

  Serial.println(value, BIN);

  // Check Output
  float c1 = -2.0468;
  float c2 = 0.0367;
  float c3 = -1.5955E-6;
  
  float humd = c1 + c2*(float)value + c3*(float)value*(float)value;
  
  // Check CRC result for temp
  bool crcResult = _checkCRC(command,value>>8, value & 0xFF, crc);
  integrityHumd = crcResult;
  
  return humd;
}

bool SHT1X::_checkCRC(uint8_t command, uint8_t val1, uint8_t val2, uint8_t crc){
  uint8_t tbl[256] = {
    0,49,98,83,196,245,166,151,185,136,219,234,125,76,31,46,
    67,114,33,16,135,182,229,212,250,203,152,169,62,15,92,109,
    134,183,228,213,66,115,32,17,63,14,93,108,251,202,153,168,
    197,244,167,150,1,48,99,82,124,77,30,47,184,137,218,235,
    61,12,95,110,249,200,155,170,132,181,230,215,64,113,34,19,
    126,79,28,45,186,139,216,233,199,246,165,148,3,50,97,80,
    187,138,217,232,127,78,29,44,2,51,96,81,198,247,164,149,
    248,201,154,171,60,13,94,111,65,112,35,18,133,180,231,214,
    122,75,24,41,190,143,220,237,195,242,161,144,7,54,101,84,
    57,8,91,106,253,204,159,174,128,177,226,211,68,117,38,23,
    252,205,158,175,56,9,90,107,69,116,39,22,129,176,227,210,
    191,142,221,236,123,74,25,40,6,55,100,85,194,243,160,145,
    71,118,37,20,131,178,225,208,254,207,156,173,58,11,88,105,
    4,53,102,87,192,241,162,147,189,140,223,238,121,72,27,42,
    193,240,163,146,5,52,103,86,120,73,26,43,188,141,222,239,
    130,179,224,209,70,119,36,21,59,10,89,104,255,206,157,172};
  uint8_t revCRC = (uint8_t)tbl[(int)tbl[(int)tbl[(int)0^command]^val1]^val2];
  uint8_t calCRC = 0;

   // I don't know why this step is necessary, but the example in the docs,
   // (not the direction) says I have to reverse the value.

  calCRC |= (revCRC & (1<<0))<<7;
  calCRC |= (revCRC & (1<<1))<<5;
  calCRC |= (revCRC & (1<<2))<<3;
  calCRC |= (revCRC & (1<<3))<<1;
  calCRC |= (revCRC & (1<<4))>>1;
  calCRC |= (revCRC & (1<<5))>>3;
  calCRC |= (revCRC & (1<<6))>>5;
  calCRC |= (revCRC & (1<<7))>>7;

  return crc == calCRC;
}

void SHT1X::reset(){
  _sda(HIGH);
  for(int i=0; i<10; i++){
    digitalWrite(_PIN_SCK, HIGH);
    delay(1);
    digitalWrite(_PIN_SCK, LOW);
    delay(1);
  }
}


#include "TES0902.h"

TES0902 tes(14, 7);//D7 connected to resistor

void setup()
{
  Serial.begin(9600);
  tes.initialize();
}

void loop()
{
  tes.readCO2();
  delay(1000);
  // int val = tes.readCO2();
  // if(tes.integrityCO2){
  //   Serial.println(val);
  // }
  // else{
  //   Serial.println("error");
  // }
  
}

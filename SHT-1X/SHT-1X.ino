#include "SHT1X.h"

SHT1X sht1x(2,3);// Data, CLK

void setup()
{
  Serial.begin(9600);
  sht1x.initialize();
}

void loop()
{
  Serial.print("Temp : ");
  float temp = sht1x.readTemp();
  if (sht1x.integrityTemp){
    Serial.print(temp);
  }
  else{
    Serial.print("err");
  }
  Serial.println("  C");
  
 Serial.print("Humd : ");
 float humd = sht1x.readHumd();
 if (sht1x.integrityHumd){
   Serial.print(humd);
 }
 else{
   Serial.print("err");
 }
 Serial.println("  %");

  delay(1000);

  
}

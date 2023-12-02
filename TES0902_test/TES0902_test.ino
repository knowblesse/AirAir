#include "TES0902.h"

TES0902 tes(14, 15);

void setup()
{
  Serial.begin(9600);
  tes.initialize();
}

void loop()
{
  tes.readCO2();
  delay(3000);
}

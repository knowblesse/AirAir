#include "LOX02.h"
LOX02 lox(4, 3);
void setup(){
  Serial.begin(9600);
  Serial.print("O2 Sensor State : ");
  Serial.println(lox.initialize());
}

void loop(){
  Serial.print("Pressure : ");
  Serial.print(lox.getP());
  Serial.print("ppm, O2 : ");
  Serial.print(lox.getO2P());
  Serial.print("ppm (");
  Serial.print(lox.getO2());
  Serial.println("%)");
  delay(1000);
}

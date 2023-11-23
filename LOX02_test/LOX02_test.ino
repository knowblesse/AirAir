#include "LOX02.h"
LOX02 lox(4, 3);
void setup(){
  Serial.begin(9600);
  lox.initialize();
}

void loop(){
  lox.getO2();
  delay(1000);
}
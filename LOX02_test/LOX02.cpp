#include "LOX02.h"

bool LOX02::initialize(){
  _O2.begin(9600);
  delay(3000); // To fast command cause errors.

  // Check connection
  int MAX_TRY = 5;
  int numTry = 0;

  while(!_O2.available()){
    _O2.print("M 1\r\n");
    delay(500);
  }
  String a = "";
  while(_O2.available()){
    a += (char)_O2.read();
  }
  Serial.println(a);
}

double LOX02::getO2(){
  while(!_O2.available()){
    _O2.print("%\r\n");
    delay(500);
  }
  String a = "";
  while(_O2.available()){
    a += (char)_O2.read();
  }
  Serial.println(a);
  // Check values
  if(a[0] != '%') return -1;
  if(a[1] != ' ') return -1;
  
}

#include "LOX02.h"

bool LOX02::initialize(){
  _O2.begin(9600);
  delay(3000); // Too fast command cause errors.

  // Check connection
  int MAX_TRY = 5;
  int numTry = 0;
  for(int numTry=0; numTry < MAX_TRY; numTry++){
    while(!_O2.available()){
      _O2.print("M 1\r\n");
      delay(500);
    }
    String a = "";
    while(_O2.available()){
      a += (char)_O2.read();
    }
    if(a.equals("M 01\r\n")){
      return true;
    }
  }
  return false;
}

double LOX02::getO2P(){
  while(!_O2.available()){
    _O2.print("O\r\n");
    delay(500);
  }
  String output = "";
  while(_O2.available()){
    output += (char)_O2.read();
  }

  // Check values
  if(output[0] != 'O') return -1;
  if(output[1] != ' ') return -1;
  return output.substring(2).toDouble(); // take numer part only and to double
}

double LOX02::getP(){
  while(!_O2.available()){
    _O2.print("P\r\n");
    delay(500);
  }
  String output = "";
  while(_O2.available()){
    output += (char)_O2.read();
  }

  // Check values
  if(output[0] != 'P') return -1;
  if(output[1] != ' ') return -1;
  return output.substring(2).toDouble(); // take numer part only and to double
}

double LOX02::getO2(){
  while(!_O2.available()){
    _O2.print("%\r\n");
    delay(500);
  }
  String output = "";
  while(_O2.available()){
    output += (char)_O2.read();
  }

  // Check values
  if(output[0] != '%') return -1;
  if(output[1] != ' ') return -1;
  return output.substring(2).toDouble(); // take numer part only and to double
}

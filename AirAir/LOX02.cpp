#include "LOX02.h"

bool LOX02::initialize(){
  _O2.begin(9600);
  pinMode(_RX_PIN, INPUT);
  pinMode(_TX_PIN, OUTPUT);
  _O2.listen();
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
  _O2.listen();
  while(!_O2.available()){
    _O2.print("O\r\n");
    delay(500);
  }
  String output = "";
  while(_O2.available()){
    output += (char)_O2.read();
  }

  // Check values
  if(output[0] == 'O' && output[1] == ' '){ // No Error
    errorCount = 0;
    lastO2P = output.substring(2).toDouble();// take numer part only and to double 
    return  lastO2P;
  }
  else { // Error.
    if(errorCount < MAX_ERRROR){ // if it is temporal, use the previous value
      errorCount++;
      return lastO2P;
    }
    else{
      return -1;
    }
  }
}

double LOX02::getP(){
  _O2.listen();
  while(!_O2.available()){
    _O2.print("P\r\n");
    delay(500);
  }
  String output = "";
  while(_O2.available()){
    output += (char)_O2.read();
  }
  // Check values
  if(output[0] == 'P' && output[1] == ' '){ // No Error
    errorCount = 0;
    lastP = output.substring(2).toDouble();// take numer part only and to double 
    return  lastP;
  }
  else { // Error.
    if(errorCount < MAX_ERRROR){ // if it is temporal, use the previous value
      errorCount++;
      return lastP;
    }
    else{
      return -1;
    }
  }
}

double LOX02::getO2(){
  _O2.listen();
  while(!_O2.available()){
    _O2.print("%\r\n");
    delay(500);
  }
  String output = "";
  while(_O2.available()){
    output += (char)_O2.read();
  }
  // Check values
  if(output[0] == '%' && output[1] == ' '){ // No Error
    errorCount = 0;
    lastO2 = output.substring(2).toDouble();// take numer part only and to double 
    return  lastO2;
  }
  else { // Error.
    if(errorCount < MAX_ERRROR){ // if it is temporal, use the previous value
      errorCount++;
      return lastO2;
    }
    else{
      return -1;
    }
  }
}

#include "LOX02.h"
#include "SHT1X.h"

#define PIN_FAN 2
#define PIN_O2TX 3
#define PIN_O2RX 4
#define PIN_BUZZER 5
#define PIN_TEMP_SDA 16
#define PIN_TEMP_SCK 17
#define PIN_RBUTTON 9
#define PIN_LBUTTON 10

LOX02 lox(PIN_O2RX, PIN_O2TX);
SHT1X sht1x(PIN_TEMP_SDA, PIN_TEMP_SCK);

int fanState = 0;
void ventilate(){
  fanState = 5;
  digitalWrite(PIN_FAN, HIGH);
}

bool screenOn = true;
void changeScreen(){
  //
}

void setup(){
  pinMode(PIN_LBUTTON, INPUT_PULLUP);
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_FAN, OUTPUT);
  Serial.begin(9600);
  Serial.print("O2 Sensor State : ");
  Serial.println(lox.initialize());
  sht1x.initialize();
  attachInterrupt(digitalPinToInterrupt(PIN_LBUTTON), ventilate, FALLING);
  attachInterrupt(digitalPinToInterrupt(PIN_RBUTTON), changeScreen, FALLING);
}

void loop(){

  Serial.print("Pressure : ");
  Serial.print(lox.getP());
  Serial.print("ppm, O2 : ");
  Serial.print(lox.getO2P());
  Serial.print("ppm (");
  Serial.print(lox.getO2());
  Serial.println("%)");

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


  // Shutdown fan
  if(fanState > 0){
    fanState--;
  }
  else{
    digitalWrite(PIN_FAN, LOW);
  }
  delay(1000);
}
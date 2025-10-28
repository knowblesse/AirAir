#include "SHT1X.h"
#include "TES0902.h"

#define PIN_FAN 2
#define PIN_BUZZER 5
#define PIN_CO2TX 7
#define PIN_CO2RX 14
#define PIN_TEMP_SDA 16
#define PIN_TEMP_SCK 17
#define PIN_RBUTTON 9
#define PIN_LBUTTON 10

SHT1X sht1x(PIN_TEMP_SDA, PIN_TEMP_SCK);
TES0902 tes(PIN_CO2RX, PIN_CO2TX);

// Screen
#include <U8g2lib.h>
#include <SPI.h>
#include <Wire.h>

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE); // No Reset pin

// ISR
bool fanState = true;
void ventilate(){
  fanState = !fanState;
  digitalWrite(PIN_FAN, fanState);
}

// variables for CO2 warnings
const int CO2_LIMIT = 2000;
const unsigned long WARNTIME = 3600000UL;
unsigned long lastWarning = -WARNTIME;
volatile bool isBuzzerArmed = false;

// Screen Variables
bool screenOn = true;
String rotateString[] = {"\\", "|", "/", "|"};
int currRotateState = 0;
int currHelloMsg = 7;
String helloMsg[] = {
    "             ",
    "Hi Nyang     ", 
    "(=^._.^=)    ",
    "Good Sleep?  ", 
    "Miss you     ",
    "From GOM     ",
    "             "
};

void setup(void)
{
  // Pin Mode
  pinMode(PIN_LBUTTON, INPUT_PULLUP);
  pinMode(PIN_RBUTTON, INPUT_PULLUP);
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_FAN, OUTPUT);

  sht1x.initialize();
  tes.initialize();
  attachInterrupt(digitalPinToInterrupt(PIN_LBUTTON), ventilate, FALLING);

  // Screen
  u8g2.begin();
  u8g2.setPowerSave(0);
  u8g2.setContrast(255);

  Serial.begin(9600);

  // Fan
  digitalWrite(PIN_FAN, fanState);
}

void updateScreen(){
  u8g2.clearBuffer();
  /*************************************/
  /*           Temperature             */
  /*************************************/
  u8g2.setFont(u8g2_font_lubR14_tf );
  float temp = sht1x.readTemp();
  if(sht1x.integrityTemp){
    u8g2.drawStr(8,17,String(temp,1).c_str());
    u8g2.drawGlyph(13*4+8,17,176);
    u8g2.drawStr(13*5+3,17,"C");
  }
  else{
    u8g2.drawStr(0,17," err");
    u8g2.drawGlyph(13*4+8,17,176);
    u8g2.drawStr(13*5+3,17,"C");
  }

  /*************************************/
  /*             Humidity              */
  /*************************************/
    float humd = sht1x.readHumd();
  if (sht1x.integrityHumd){

    u8g2.drawStr(8,40,(String(humd,1) +"  %").c_str());

  }
  else{
    u8g2.drawStr(8,40," err  %");
  }

  /*************************************/
  /*              CO2 PPM              */
  /*************************************/
  int CO2_val = tes.readCO2();
  if (tes.integrityCO2){
    if (CO2_val < 1000){
      u8g2.drawStr(8,64,(" " + String(CO2_val)+"  PPM").c_str());  
    }
    else{
      u8g2.drawStr(8,64,(String(CO2_val)+"  PPM").c_str());
    }
  }
  else{
    u8g2.drawStr(8,64," err  PPM");
  }

 
  // /*************************************/
  // /*       Show Rotating Mark          */
  // /*************************************/
  // if(currRotateState == 3){
  //   currRotateState = 0;
  // }
  // else{
  //   currRotateState++;
  // }
  // u8x8.drawString(14, 6, rotateString[currRotateState].c_str());

  u8g2.sendBuffer();
}

void loop(void)
{
  updateScreen();

  // Check screen button
  if(digitalRead(PIN_RBUTTON) == LOW){
    screenOn = !screenOn;
    if(screenOn){ // show message when screen on is true
      currHelloMsg = 0;
    }
    isBuzzerArmed = true;
    u8g2.setPowerSave(!screenOn);
  }
  delay(500);
}
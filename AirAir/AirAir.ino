#include "LOX02.h"
#include "SHT1X.h"
#include "TES0902.h"

#define PIN_FAN 2
#define PIN_O2TX 3
#define PIN_O2RX 4
#define PIN_BUZZER 5
#define PIN_CO2TX 7
#define PIN_CO2RX 14
#define PIN_TEMP_SDA 16
#define PIN_TEMP_SCK 17
#define PIN_RBUTTON 9
#define PIN_LBUTTON 10

LOX02 lox(PIN_O2RX, PIN_O2TX);
SHT1X sht1x(PIN_TEMP_SDA, PIN_TEMP_SCK);
TES0902 tes(PIN_CO2RX, PIN_CO2TX);

// Screen
#include <U8x8lib.h>
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(8);

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
  lox.initialize();
  sht1x.initialize();
  tes.initialize();
  attachInterrupt(digitalPinToInterrupt(PIN_LBUTTON), ventilate, FALLING);

  // Screen
  u8x8.begin();
  u8x8.setPowerSave(0);

  Serial.begin(9600);

  // Fan
  digitalWrite(PIN_FAN, fanState);
}

void updateScreen(){
  /*************************************/
  /*           Temperature             */
  /*************************************/
  u8x8.setFont(u8x8_font_7x14B_1x2_f);
  u8x8.drawString(0,0,"T");
  float temp = sht1x.readTemp();
  if(sht1x.integrityTemp){
    u8x8.drawString(1,0,String(temp,1).c_str());
    u8x8.drawGlyph(5,0,176);
    u8x8.drawString(6,0,"C");
  }
  else{
    u8x8.drawString(1,0," err");
    u8x8.drawString(5,0,176);
    u8x8.drawString(6,0,"C");
  }

  /*************************************/
  /*             Humidity              */
  /*************************************/
  u8x8.drawString(9,0,"H");
  float humd = sht1x.readHumd();
  if (sht1x.integrityHumd){
    u8x8.drawString(10,0,(String(humd)+"%").c_str());
  }
  else{
    u8x8.drawString(10,0," err %");
  }


   /*************************************/
  /*             Pressure              */
  /*************************************/
  u8x8.drawString(0,2,"P");
  double pressure = lox.getP();
  if (pressure < 0){
    u8x8.drawString(2,2," err p");
  }
  else if (pressure<1000){
    u8x8.drawString(2,2,(" " + String((int)pressure)+"p").c_str());
  }
  else {
    u8x8.drawString(2,2,(String((int)pressure)+"p").c_str());
  }

  /*************************************/
  /*            O2 Percent             */
  /*************************************/
  u8x8.drawString(9,2,"O");
  double O2percent = lox.getO2();
    if(O2percent > 0){
    u8x8.drawString(10,2,(String(O2percent)+"%").c_str());
  }
  else{
    u8x8.drawString(10,2," err %");
  }

  /*************************************/
  /*              CO2 PPM              */
  /*************************************/
  u8x8.drawString(0,4,"C");
  int CO2_val = tes.readCO2();
  if (tes.integrityCO2){
    if (CO2_val < 1000){
      u8x8.drawString(2,4,(" " + String(CO2_val)+"p").c_str());  
    }
    else{
      u8x8.drawString(2,4,(String(CO2_val)+"p").c_str());
    }
  }
  else{
    u8x8.drawString(2,4," err p");
  }

  /*************************************/
  /*               O2 PPM              */
  /*************************************/
  u8x8.drawString(9,4,"OP");
  double o2pressure = lox.getO2P();
  if (o2pressure < 0){
    u8x8.drawString(1,4," err p");
  }
  else if (o2pressure<1000){
    u8x8.drawString(11,4,(" " + String((int)o2pressure)+"p").c_str());
  }
  else {
    u8x8.drawString(11,4,(String((int)o2pressure)+"p").c_str());
  }
 
  /*************************************/
  /*       Show Rotating Mark          */
  /*************************************/
  if(currRotateState == 3){
    currRotateState = 0;
  }
  else{
    currRotateState++;
  }
  u8x8.drawString(14, 6, rotateString[currRotateState].c_str());

  /*************************************/
  /*        Show Hello Message         */
  /*************************************/
  if(currHelloMsg < 7){
    u8x8.drawString(0, 6, helloMsg[currHelloMsg].c_str());
    currHelloMsg++;
  }
  else{
    if(CO2_val > CO2_LIMIT){
      u8x8.drawString(0, 6, "  !Ventilate!");
      if(isBuzzerArmed && screenOn && (millis()-lastWarning > WARNTIME)){
        for(int j=0; j<3; j++){
          digitalWrite(PIN_BUZZER, HIGH);
          delay(200);
          digitalWrite(PIN_BUZZER, LOW);
          delay(200);
        }
        lastWarning = millis();
      }
    }
    else{
      u8x8.drawString(0, 6, "             ");
    }
  }

  u8x8.refreshDisplay();    // only required for SSD1606/7  
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
    u8x8.setPowerSave(!screenOn);
  }
  delay(500);
}
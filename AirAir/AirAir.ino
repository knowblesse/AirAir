#include "LOX02.h"
#include "SHT1X.h"
#include "TES0902.h"

#define PIN_FAN 2
#define PIN_O2TX 3
#define PIN_O2RX 4
#define PIN_BUZZER 5
#define PIN_CO2TX 7
#define PIN_CO2RX 6
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
int fanState = 0;
void ventilate(){
  fanState = 5;
  digitalWrite(PIN_FAN, HIGH);
}

bool screenOn = true;
void changeScreen(){
  //
}

void setup(void)
{
  // Pin Mode
  pinMode(PIN_LBUTTON, INPUT_PULLUP);
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_FAN, OUTPUT);
  lox.initialize();
  sht1x.initialize();
  tes.initialize();
  attachInterrupt(digitalPinToInterrupt(PIN_LBUTTON), ventilate, FALLING);
  attachInterrupt(digitalPinToInterrupt(PIN_RBUTTON), changeScreen, FALLING);

  // Screen
  u8x8.begin();
  u8x8.setPowerSave(0);

  Serial.begin(9600);
}

void updateScreen(){
  // Temp
  u8x8.setFont(u8x8_font_7x14B_1x2_f);
  u8x8.drawString(0,0,"T");
  float temp = sht1x.readTemp();
  if(sht1x.integrityTemp){
    u8x8.drawString(1,0,(String(temp)+"C").c_str());
  }
  else{
    u8x8.drawString(1,0,"Err  ºC");
  }

  // Humd
  u8x8.drawString(9,0,"H");
  float humd = sht1x.readHumd();
  if (sht1x.integrityHumd){
    u8x8.drawString(10,0,(String(humd)+"%").c_str());
  }
  else{
    u8x8.drawString(10,0,"Err  %");
  }
  
  u8x8.setFont(u8x8_font_7x14B_1x2_f);
  // O2 %
  u8x8.drawString(0,2,"O");
  u8x8.drawString(1,2,(String(lox.getO2())+"%").c_str());

  // CO2 ppm
  u8x8.drawString(8,2,"C");
  int CO2_val = tes.readCO2();
  if (tes.integrityCO2){
    u8x8.drawString(9,2,String(CO2_val).c_str());
  }
  else{
    u8x8.drawString(10,2," err");
  }
  
  //u8x8.drawString(12,6,"O ");


  //u8x8.drawString(1,4,String(lox.getP()).c_str());
  
  u8x8.refreshDisplay();    // only required for SSD1606/7  
}

void loop(void)
{
  updateScreen();

  // Shutdown fan
  if(fanState > 0){
    fanState--;
  }
  else{
    digitalWrite(PIN_FAN, LOW);
  }
  delay(1000);
  // u8x8.setPowerSave(1);
  // delay(1000);
  
}

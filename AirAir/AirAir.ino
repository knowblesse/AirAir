#include <U8x8lib.h>
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(2);

SoftwareSerial O2(PIN_O2_RX, PIN_O2_TX);

void setup(void)
{
  u8x8.begin();
  u8x8.setPowerSave(0);

  u8x8.setFont(u8x8_font_7x14B_1x2_f);  
  
  u8x8.drawString(0,2,"Humd :");
  u8x8.drawString(0,4,"Oxyg :");
  u8x8.drawString(0,6,"Carb :");


  u8x8.drawString(7,0,"23.5");
  u8x8.drawString(7,2,"45.67%");
  u8x8.drawString(7,4,"18.95%");
  u8x8.drawString(7,6,"203ppm");

  u8x8.refreshDisplay();    // only required for SSD1606/7  
  Serial.begin(9600);
  Serial.println("Test");

  // O2 Sensor
  O2.begin(9600);
  
  

}
int a = 0;
void loop(void)
{
  Serial.println(a);
  a++;
  O2.print("M 1\r\n");
  delay(1000);
  while(O2.available()){
  Serial.print((char)O2.read());
  }
}

void measure(){
  O2.print("M 1\r\n");
}

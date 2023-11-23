#include <SoftwareSerial.h>

SoftwareSerial co2(4, 3);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  co2.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("Start");
  co2.write('O');
  co2.write('\r');
  co2.write('\n');
  while(co2.available()){
    Serial.print((char)co2.read());
  }
  delay(2000);
  Serial.println("");

}

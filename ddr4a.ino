// change note
// 20151109: add HMA451R7MFR8N-TF as module PN per ICT's request
// remove old/unnecessary comment

#include <Wire.h>
#include <EEPROM.h>

const byte SPD_I2C_ADD = 0x57;  //0xAE upper 7bit, for eeprom read write
const byte SET_PAGE_ADD = 0x37; //0x6E upper 7bit, for select eeprom page
const int led = 13;  

byte spd[16][16] = {
  {0x23, 0x07, 0x0c, 0x01, 0x84, 0x19, 0x00, 0x08, 0x00, 0x00, 0x00, 0x03, 0x01, 0x0b, 0x80, 0x00},
  {0x00, 0x00, 0x08, 0x0c, 0xf4, 0x03, 0x00, 0x00, 0x6c, 0x6c, 0x6c, 0x11, 0x08, 0x74, 0x20, 0x08},
  {0x00, 0x05, 0x70, 0x03, 0x00, 0xa8, 0x1e, 0x2b, 0x2e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x2e, 0x16, 0x36},
  {0x16, 0x36, 0x16, 0x36, 0x0e, 0x2e, 0x23, 0x04, 0x2b, 0x0c, 0x2b, 0x0c, 0x23, 0x04, 0x00, 0x00},
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x83, 0xb5, 0xce, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc2, 0xeb, 0x3c},
  {0x11, 0x11, 0x03, 0x05, 0x00, 0x80, 0xb3, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x69, 0xf9}
};

void setup()
{
  // set up serial port and LED state
  Wire.begin(); // join i2c bus (address optional for master)
  Serial.begin(9600);  // start serial for output
  pinMode(led, OUTPUT);  //use LED as programming state indicator

  delay(500);
  /*---------------get serial no. from serial port--------------------*/
  // 2015 12th week, DDR4 version SA, last 3 bytes are read from module label
  byte serialno[6] = {0x15, 0x12, 0x4a, 0, 0, 0};
  serialno[4] = Serial.read();
  serialno[5] = Serial.read();
  delay(10);
  
  /*-----calculate crc for Base config----------------------------*/
  // SN is programmed into [5][0] before crc for backup, seems unnecessary
  for(unsigned int y = 0; y <= 5; y++)
    spd[5][y] = serialno[y];
  int data16;
  data16 = crc16(&spd[0][0], 126);
  spd[7][14] = (byte)(data16 & 0xff);
  spd[7][15] = (byte)(data16 >> 8);
  
  /*---------------write data to the first page of SPD------------*/
  for(byte i = 0; i <= 15; i++){
    Wire.beginTransmission(SPD_I2C_ADD); 
    Wire.write( 16 * i );
    for( byte j = 0; j <= 15; j++)
      Wire.write(spd[i][j]);  // sends one byte  
    Wire.endTransmission();    // stop transmitting
    delay(5);         //wait 5ms before next start
  }
  
  /*----read from the first page of SPD, compare with write value------*/
  // Serial.print("validating 1st half.\n");
  byte temp;
  for(unsigned int a= 0; a< 16; a++){
    for(unsigned int b= 0; b< 16; b++){
      temp = readi2c((int) SPD_I2C_ADD, 16*a+b);
      if (temp != spd[a][b]){
        Serial.print("spd 1st half programming fail!!");
        led_blinking();
      }
    }
  }
  
  /*--------------------first half done, now 2nd half------------------*/
  /* set page 1*/
  Wire.beginTransmission(SET_PAGE_ADD);
  Wire.write(0); // maybe not needed
  Wire.endTransmission();
  
  /* initialize the 2nd half of spd */
  byte spd_2nd[16][16] = {
    {},
    {},
    {},
    {},
    { 0, 0, 0, 0x15, 0x12, 0x4a, 0, serialno[4], serialno[5], 70, 77, 65, 52, 53, 49, 82 },
    { 55, 77, 70, 82, 56, 78, 45, 84, 70, 0x20, 0x20, 0x20, 0x20, 0, 0, 0 }
  };
  
  for(byte i = 0; i <= 15; i++){
    Wire.beginTransmission(SPD_I2C_ADD);
    Wire.write( 16 * i );
    for( byte j = 0; j <= 15; j++){
      Wire.write(spd_2nd[i][j]);
    }
    Wire.endTransmission();
    delay(5);         //wait 5ms before next start
  }
  
  /* validate 2nd half spd*/
  byte val;
  for(unsigned int i = 0; i <=5; i++){
    val = readi2c((int)SPD_I2C_ADD, 0x43 + i);
    if (val != serialno[i] ){
        Serial.print("spd 2nd half validating fail!!\n");
        led_blinking();
    }
  }
  
  /*------------------------done----------*/
  // digitalWrite(led, HIGH);
  Serial.print('P');

}

void loop(){

}

/*-------------------------------------------------------------------------------------*/

// i2c read function
byte readi2c(int deviceaddress, unsigned int byte_address){
  byte rdata = 0xFF;
  Wire.beginTransmission(deviceaddress);
  Wire.write((int)byte_address);   
  Wire.endTransmission();
  Wire.requestFrom(deviceaddress,1);
  if (Wire.available()) rdata = Wire.read();
  return rdata;
}

// when something goes wrong, LED blinks
void led_blinking(){
  Serial.print("something is wrong when validating, check it!\n");
  while (true){
    digitalWrite(led, HIGH);
    delay(500);
    digitalWrite(led, LOW);
    delay(500);
  }
}

// crc16 for SPD content check
int crc16 (byte *ptr, int count){
  int crc, i;
  crc = 0;
  while (--count >= 0) {
    crc = crc ^ (int)*ptr++ << 8;
    for (i = 0; i < 8; ++i)
      if (crc & 0x8000)
        crc = crc << 1 ^ 0x1021;
      else
 	crc = crc << 1;
  }
  return (crc & 0xFFFF);
}

/* EOF */

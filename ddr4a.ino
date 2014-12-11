// contents in EEPROM
// offset 00: 4a 0x xx xx 3a 0x xx xx 3b 0x xx xx

#include <Wire.h>
#include <EEPROM.h>

const byte EEPROM_ADD = 0x57;  //0xAE upper 7bit, for eeprom read write
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
  Wire.begin(); // join i2c bus (address optional for master)
  Serial.begin(9600);  // start serial for output
  pinMode(led, OUTPUT);  //use LED as programming state indicator
}

void loop()
{
  delay(1000);
  /*---------------get serial no. from EEPROM--------------------*/
  byte serialno[6] = {0x14, 0x42, 0x4a, 0, 0, 0};
  serialno[3] = EEPROM.read(1);
  delay(500);
  serialno[4] = EEPROM.read(2);
  delay(500);
  serialno[5] = EEPROM.read(3);
  delay(500);
  
  /*-----calculate crc for Base config----------------------------*/
  for(unsigned int y = 0; y <= 5; y++)
    spd[5][y] = serialno[y];
  int data16;
  data16 = crc16(&spd[0][0], 126);
  spd[7][14] = (byte)(data16 & 0xff);
  spd[7][15] = (byte)(data16 >> 8);
  
  /*---------------write data to the first page of SPD------------*/
  Serial.print("writing SPD....\n");
  for(byte i = 0; i <= 15; i++){
    Wire.beginTransmission(EEPROM_ADD); 
    Wire.write( 16 * i );
    for( byte j = 0; j <= 15; j++){
      Wire.write(spd[i][j]);  // sends one byte  
//      if ( spd[i][j] <= 15 ) Serial.print('0');
//      Serial.print(spd[i][j], HEX);    
//      Serial.print('\t');  
    }
    Wire.endTransmission();    // stop transmitting
//    Serial.print('\n');
    delay(5);         //wait 5ms before next start
  }
  Serial.print("writing SPD done...\n");
  
/*----read from the first page of SPD, compare with write value------*/
  Serial.print("validating first half of SPD content...\n");
  int error = 0;  // 1 indicate an error
  byte spd_read[16][16];  //read out spd for validation
  for(unsigned int a= 0; a< 16; a++){
    for(unsigned int b= 0; b< 16; b++){
      spd_read[a][b] = readEEPROM((int) EEPROM_ADD, 16*a+b);
      if (spd_read[a][b] != spd[a][b]) error = 1;
    }
  }
  Serial.print("validating done.\n");
  
/*--------------------first half done, now 2nd half------------------*/
  Serial.print("programming serial no. and 2nd half SPD...\n");
  
  /* set page 1*/
  Wire.beginTransmission(SET_PAGE_ADD);
  Wire.write(0); // maybe not needed
  Wire.endTransmission();
  
  /* program all bytes to 0. */
  for(byte i = 0; i <= 15; i++){
    Wire.beginTransmission(EEPROM_ADD);
    Wire.write( 16 * i );
    for( byte j = 0; j <= 15; j++)
      Wire.write(0);
    Wire.endTransmission();
    delay(5);         //wait 5ms before next start
  }
  
  /* program manufacturing date and serial no. */
  Wire.beginTransmission(EEPROM_ADD);
  Wire.write(0x43);
  for(unsigned int x = 0; x < 6; x++){
    Wire.write(serialno[x]);
    Serial.print(serialno[x], HEX);
  }
  Wire.endTransmission();
  Serial.print('\n');
  
  /* increase EEPROM serial no. */
  EEPROM.write(3, serialno[5] + 1);
//  Serial.print(serialno[5]+1, HEX);
  Serial.print("\nPASS!\n");
/*------------------------error message and done looping----------*/
  if (error)
    led_blinking();
  else
    led_light();
  while (true){;}
}
/*--------------------------end of loop------------------------------------------------*/
/*-------------------------------------------------------------------------------------*/
byte readEEPROM(int deviceaddress, unsigned int byte_address ) 
{
  byte rdata = 0xFF;
  Wire.beginTransmission(deviceaddress);
  Wire.write((int)byte_address);   
  Wire.endTransmission();
  Wire.requestFrom(deviceaddress,1);
  if (Wire.available()) rdata = Wire.read();
  return rdata;
}

void led_light()
{
   digitalWrite(led, HIGH);
}

void led_blinking()
{
  Serial.print("something is wrong when validating, check it!\n");
  while (true){
    digitalWrite(led, HIGH);
    delay(500);
    digitalWrite(led, LOW);
    delay(500);
  }
}

int crc16 (byte *ptr, int count)
{
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

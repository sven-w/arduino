// contents in EEPROM
// offset 00: 4a 0x xx xx 3a 0x xx xx 3b 0x xx xx

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
  Wire.begin(); // join i2c bus (address optional for master)
  Serial.begin(9600);  // start serial for output
  pinMode(led, OUTPUT);  //use LED as programming state indicator
}

void loop()
{
  delay(500);
  /*---------------get serial no. from EEPROM--------------------*/
  byte serialno[6] = {0x14, 0x42, 0x4a, 0, 0, 0};
  serialno[3] = EEPROM.read(1);
  delay(10);
  serialno[4] = EEPROM.read(2);
  delay(10);
  serialno[5] = EEPROM.read(3);
  delay(10);
  
  /*-----calculate crc for Base config----------------------------*/
  for(unsigned int y = 0; y <= 5; y++)
    spd[5][y] = serialno[y];
  int data16;
  data16 = crc16(&spd[0][0], 126);
  spd[7][14] = (byte)(data16 & 0xff);
  spd[7][15] = (byte)(data16 >> 8);
  
  /*---------------write data to the first page of SPD------------*/
  Serial.print("writing spd 1st half.\n");
  for(byte i = 0; i <= 15; i++){
    Wire.beginTransmission(SPD_I2C_ADD); 
    Wire.write( 16 * i );
    for( byte j = 0; j <= 15; j++)
      Wire.write(spd[i][j]);  // sends one byte  
    Wire.endTransmission();    // stop transmitting
    delay(5);         //wait 5ms before next start
  }
  
  /*----read from the first page of SPD, compare with write value------*/
  Serial.print("validating 1st half.\n");
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
  Serial.print("writing spd 2nd half.\n");
  
  /* set page 1*/
  Wire.beginTransmission(SET_PAGE_ADD);
  Wire.write(0); // maybe not needed
  Wire.endTransmission();
  
  /* initialize the 2nd half of spd */
  byte spd_2nd[16][16] = {};
  for(unsigned int i = 0; i <= 15; i++)
    for(unsigned int j = 0; j <= 15; j++)
      spd_2nd[i][j] = 0;
  for(unsigned int i = 0; i <= 5; i++)
    spd_2nd[4][3+i] = serialno[i];
  
  /* program 2nd half */
  for(byte i = 0; i <= 15; i++){
    Wire.beginTransmission(SPD_I2C_ADD);
    Wire.write( 16 * i );
    for( byte j = 0; j <= 15; j++)
      Wire.write(spd_2nd[i][j]);
    Wire.endTransmission();
    delay(5);         //wait 5ms before next start
  }
  
  /* validate 2nd half spd*/
  Serial.print("validating spd 2nd half\n");
  byte val;
  for(unsigned int i = 0; i <=5; i++){
    Serial.print(serialno[i], HEX);
    val = readi2c((int)SPD_I2C_ADD, 0x43 + i);
    if (val != serialno[i] )
      if (val == (serialno[i] + 1)){
        Serial.print("this board has been programmed, please step to next one\n");
        led_blinking();
      }
      else{
        Serial.print("spd 2nd half validating fail!!\n");
        led_blinking();
      }
  }
  Serial.print('\n');
  
  /*------------------------done looping----------*/
  digitalWrite(led, HIGH);
  /* increase EEPROM serial no. */
  EEPROM.write(3, serialno[5] + 1);
  Serial.print("\nPASS!\n");
  while (true){;}
}


/*--------------------------end of loop------------------------------------------------*/
/*-------------------------------------------------------------------------------------*/
byte readi2c(int deviceaddress, unsigned int byte_address)
{
  byte rdata = 0xFF;
  Wire.beginTransmission(deviceaddress);
  Wire.write((int)byte_address);   
  Wire.endTransmission();
  Wire.requestFrom(deviceaddress,1);
  if (Wire.available()) rdata = Wire.read();
  return rdata;
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

/* EOF */

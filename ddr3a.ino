// Wire Master Writer
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Writes data to an I2C/TWI slave device
// Refer to the "Wire Slave Receiver" example for use with this

// Created 29 March 2006

// This example code is in the public domain.


#include <Wire.h>

byte spd_read[16][16];  //read out spd for validation

const byte spd[16][16] = { 
  {0x91, 0x10, 0x0b, 0x01, 0x04, 0x19, 0x00, 0x09, 0x0b, 0x11, 0x01, 0x08, 0x0a, 0x00, 0xfc, 0x00},
  {0x69, 0x78, 0x69, 0x30, 0x69, 0x11, 0x18, 0x81, 0x00, 0x05, 0x3c, 0x3c, 0x00, 0xf0, 0x83, 0x01},
  {0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x88, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x19, 0x11, 0x21, 0x09},
  {0x00, 0x80, 0xb3, 0x63, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x85, 0xd3},
  {0x38, 0x31, 0x2e, 0x35, 0x4d, 0x30, 0x31, 0x30, 0x2e, 0x30, 0x31, 0x30, 0x47, 0x20, 0x20, 0x20},
  {0x20, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
};

const byte SLAVE_ADD = 0x50;  //0xA0 upper 7bit

int led = 13;  

void setup()
{
  Wire.begin(); // join i2c bus (address optional for master)
  Serial.begin(9600);  // start serial for output
  pinMode(led, OUTPUT);  //use LED as programming state indicator, keep light when done, keep blinking when read error.
}

void loop()
{
  Serial.print("writing SPD....\n");
  
  for(byte i = 0; i <= 15; i++){
    Wire.beginTransmission(SLAVE_ADD); // transmit to device 0xA0
    Wire.write( 16 * i );        // write byte address, when is one byte
    for( byte j = 0; j <= 15; j++){    // the for loop sends one page
      Wire.write(spd[i][j]);  // sends one byte  
      if ( spd[i][j] <= 15 ){
        Serial.print('0');    //for hax under 0x10, add the '0' before the 2nd number
      }
      Serial.print(spd[i][j], HEX);    
      Serial.print('\t');  
    }
    Wire.endTransmission();    // stop transmitting
    Serial.print('\n');
    delay(5);         //wait 5ms before next start
  }
  Serial.print("writing SPD done...\n");
  
  Serial.print("validating SPD content...\n");
  
  
  int length = 16;
  int error = 0;  // 1 indicate an error
  
  for(unsigned int a= 0; a< 16; a++){
    for(unsigned int b= 0; b< 16; b++){
      spd_read[a][b] = readEEPROM((int) SLAVE_ADD, 16*a+b);
      
      if (spd_read[a][b] <= 0x0f)
        Serial.print('0');
      Serial.print(spd_read[a][b], HEX);
      Serial.print('\t');
      
      if (spd_read[a][b] != spd[a][b]) error = 1;
    }
    Serial.print('\n');
  }
    
  
  // LED indicating error
  if (error)
    led_blinking();
  else
    led_light();

  Serial.print("validating done.\n");
  Serial.print("programming finished.\n");
  
  while (true){;}
}

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

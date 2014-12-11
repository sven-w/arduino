/*
 * EEPROM Write
 *
 * Stores values read from analog input 0 into the EEPROM.
 * These values will stay in the EEPROM when the board is
 * turned off and may be retrieved later by another sketch.
 */

#include <EEPROM.h>
byte serialno[12] = {0x4a, 0, 0, 1, 0x3a, 0, 0, 1, 0x3b, 0, 0, 1};
void setup()
{
    Serial.begin(9600);
    digitalWrite(13, LOW);
    for( unsigned int a = 0; a < 12; a++){
    EEPROM.write(a, serialno[a]);
    Serial.print(serialno[a], HEX);
    delay(10);
    }
  Serial.print('\n');
  int v = 0;
  for(unsigned int b = 0; b < 16; b++){
    v = EEPROM.read(b);
    Serial.print(v, HEX);
    delay(10);
  }
  
}

void loop()
{

}

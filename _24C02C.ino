#include <Wire.h>

#define DEV_ADDR    0x50
/*
 * PROM size / 8bit = EOADDR
 * 2kbit(24c02) / 8bit = 0x100
 */
#define EOADDR      0x100

#define WRITE_FLAG  0
#define READ_FLAG   1
#define CHECK_DEV_ADDR 1

/*
 *   100000L to 400000L
 *   仕様外だけど31000Lでも動いた
 */
#define CLOCK_SPEED 100000L

/*
 * 0: Device with PROM size of 128 bit to  16kbit
 * 1: Device with PROM size of  32kbit to 512kbit
 */
#define DEVICE      0

void i2c_eeprom_write_byte( int dev_addr, unsigned int addr, byte data ) {
  Wire.beginTransmission(dev_addr);
  if (DEVICE) {
    Wire.write((int)(addr >> 8)); // MSB
    Wire.write((int)(addr & 0xFF)); // LSB
  } else {
    Wire.write(addr);
  }
  Wire.write(data);
  Wire.endTransmission();
}

byte i2c_eeprom_read_byte( int dev_addr, unsigned int addr ) {
  Wire.beginTransmission(dev_addr);
  if (DEVICE) {
    Wire.write((int)(addr >> 8)); // MSB
    Wire.write((int)(addr & 0xFF)); // LSB
  } else {
    Wire.write(addr);
  }
  Wire.endTransmission();

  Wire.requestFrom(dev_addr,1);
  if (Wire.available()) {
    return Wire.read();
  }
  return 0xAA;
}

void print_addr(unsigned int a) {
  String b = "";
  if (a < 0x1000) {
    b = "0";
  }
  if (a < 0x100) {
    b = "00";
  }
  if ( a < 0x10) {
    b = "000";
  }
  Serial.print("|");
  Serial.print(b);
  Serial.print(a, HEX);
  Serial.print("|");
}

void print_data(unsigned int a) {
  if ( a < 0x10 ) {
    Serial.print("0");
  }
  Serial.print(a, HEX);
}

void print_low_addr() {
    Serial.println();
  Serial.print("|    |");
  for (int i = 0; i < 0x10; i++){
    print_data(i);
    Serial.print("|");
  }
  Serial.println();

  Serial.print("|----|");
  for (int i = 0; i < 0x10; i++){
    Serial.print("--|");
  }
  Serial.println();
}
void setup() {
  // put your setup code here, to run once:
  Wire.begin();
  Wire.setClock(CLOCK_SPEED);
  Serial.begin(9600);

  if (WRITE_FLAG) {
    Serial.print("Writing");
    for (int addr = 0; addr < EOADDR; addr++) {
      i2c_eeprom_write_byte(DEV_ADDR, addr, 0xFF);
      Serial.print(".");
      delay(1);
    }
    Serial.println("");
  }

  print_low_addr();

  if (READ_FLAG) {
    int val;
    for (int addr = 0; addr < EOADDR; addr++) {
      if (CHECK_DEV_ADDR) {
        val = i2c_eeprom_read_byte(addr, addr);
      } else {
        val = i2c_eeprom_read_byte(DEV_ADDR, addr);
      }
      if (((addr & 0xFF) == 0x00) && (addr != 0x00)) {
        Serial.println();
      }
      if ((addr % 0x10) == 0) {
        print_addr(addr);
      }
      print_data(val);
      Serial.print("|");
      if ((addr % 0x10) == 0x0F) {
        Serial.println();
      }
    }
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}

// -----------------------------------------------------------------------------------
// non-volatile storage for AT24C32 (often on DS3231 RTC modules with I2C address 0x57)

#pragma once

#include <Wire.h>
#include <Arduino.h>

// I2C EEPROM Address on DS3231 RTC module
#define I2C_EEPROM_ADDRESS 0x57
#define E2END 4095
#define I2C_CLOCK 400000

#define MSB(i) (i >> 8)
#define LSB(i) (i & 0xFF)

class nvs {
  public:    
    void init() {
      HAL_Wire.begin();
      HAL_Wire.setClock(I2C_CLOCK);
      _eeprom_addr = I2C_EEPROM_ADDRESS;
    }

    uint8_t read(uint16_t i) {
      return nvs_i2c_ee_read(i);
    }

    void write(uint16_t i, uint8_t j) {
      nvs_i2c_ee_write(i,j);
    }

    void update(uint16_t i, uint8_t j) {
      if (j!=read(i)) write(i,j);
    }

    // write int numbers into EEPROM at position i (2 uint8_ts)
    void writeInt(uint16_t i, uint16_t j) {
      uint8_t *k = (uint8_t*)&j;
      update(i+0,*k); k++;
      update(i+1,*k);
    }
    
    // read int numbers from EEPROM at position i (2 uint8_ts)
    int readInt(uint16_t i) {
      uint16_t j;
      uint8_t *k = (uint8_t*)&j;
      *k=read(i+0); k++;
      *k=read(i+1);
      return j;
    }

    // write 4 uint8_t long into EEPROM at position i (4 uint8_ts)
    void writeLong(uint16_t i,long l) {
      writeQuad(i,(uint8_t*)&l);
    }
    
    // read 4 uint8_t long from EEPROM at position i (4 uint8_ts)
    long readLong(uint16_t i) {
      long l;
      readQuad(i,(uint8_t*)&l);
      return l;
    }
            
    // write 4 uint8_t float into EEPROM at position i (4 uint8_ts)
    void writeFloat(uint16_t i,float f) {
      writeQuad(i,(uint8_t*)&f);
    }
    
    // read 4 uint8_t float from EEPROM at position i (4 uint8_ts)
    float readFloat(uint16_t i) {
      float f;
      readQuad(i,(uint8_t*)&f);
      return f;
    }

    // write 4 uint8_t variable into EEPROM at position i (4 uint8_ts)
    void writeQuad(uint16_t i,uint8_t *v) {
      update(i+0,*v); v++;
      update(i+1,*v); v++;
      update(i+2,*v); v++;
      update(i+3,*v);
    }
    
    // read 4 uint8_t variable from EEPROM at position i (4 uint8_ts)
    void readQuad(uint16_t i,uint8_t *v) {
      *v=read(i+0); v++;
      *v=read(i+1); v++;
      *v=read(i+2); v++;
      *v=read(i+3);  
    }
    
    // write String into EEPROM at position i (16 uint8_ts)
    void writeString(uint16_t i, char l[]) {
      int l1;
      for (l1=0; l1<16; l1++) {
        update(i+l1,*l); l++;
      }
    }
    
    // read String from EEPROM at position i (16 uint8_ts)
    void readString(uint16_t i,  char l[]) {
      int l1;
      for (l1=0; l1<16; l1++) {
        *l=read(i+l1); l++;
      }
    }

private:
  // Address of the I2C EEPROM
  uint8_t _eeprom_addr;
  uint16_t lastSeqOffset=-32767;
  uint32_t nextOpMs=millis()+10UL;

  void nvs_i2c_ee_write(uint16_t offset, uint8_t data) {
    while ((int32_t)(millis()-nextOpMs)<0) {}

    HAL_Wire.beginTransmission(_eeprom_addr);
    HAL_Wire.write(MSB(offset));
    HAL_Wire.write(LSB(offset));
    HAL_Wire.write(data);
    HAL_Wire.endTransmission();
    nextOpMs=millis()+10UL;

    lastSeqOffset=-32767; // reset
  }

  uint8_t nvs_i2c_ee_read(uint16_t offset) {
    uint8_t data = 0xFF;
    while ((int32_t)(millis()-nextOpMs)<0) {}

    if (lastSeqOffset+1!=offset) { 
      HAL_Wire.beginTransmission(_eeprom_addr);
      HAL_Wire.write(MSB(offset));
      HAL_Wire.write(LSB(offset));
      HAL_Wire.endTransmission();
    }
    HAL_Wire.requestFrom(_eeprom_addr, (uint8_t)1);
    delayMicroseconds(1);
    data = HAL_Wire.read();

    lastSeqOffset=offset;

    return data;
  }

};

nvs nv;

// this library is public domain. enjoy!
// www.ladyada.net/learn/sensors/thermocouple

#ifndef MAX6675_H
#define MAX6675_H

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif


enum {
  TEMPC,
  TEMPF
};

class MAX6675 {
  public:
    void init();
    void init(uint8_t DO, uint8_t CLK, uint8_t CS);
    double measure(uint8_t type);
        
  private:
    double tempC();
    double tempF();
    uint8_t spiRead();
    
    int8_t _DO, _CS, _CLK;
};

#endif

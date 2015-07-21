
#ifndef __ADCTEST_H_
#define __ADCTEST_H_
#include <Arduino.h>
#include "Device.h"
#include "Pin.h"
#include "AConfig.h"


#if(HAS_STD_CAPE)
  #include "Cape.h"
#endif

#if(HAS_OROV_CONTROLLERBOARD_25)
  #include "controllerboard25.h"
#endif


class Adctest : public Device {
  public:
    Adctest():Device(){};
    void device_setup();
    void device_loop(Command cmd);
};
#endif

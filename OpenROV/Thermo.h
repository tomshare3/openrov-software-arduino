
#ifndef __THERMO_H_
#define __THERMO_H_
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


class Thermo : public Device {
  public:
    Thermo():Device(){};
    void device_setup();
    void device_loop(Command cmd);
};
#endif

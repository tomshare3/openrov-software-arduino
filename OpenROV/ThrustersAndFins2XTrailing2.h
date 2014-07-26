#if(HAS_2THRUSTERSWITHFINS)
#ifndef __THRUSTERSWITHFINS_H_
#define __THRUSTERSWITHFINS_H_
#include <Arduino.h>
#include "Device.h"
#include "Pin.h"

#if(HAS_STD_CAPE)
  #define PORT_MOTOR_PIN 9
  #define STARBOARD_MOTOR_PIN 11
  #define PORT_FIN_PIN 3
  #define STARBOARD_FIN_PIN 10
#endif

#if(HAS_OROV_CONTROLLERBOARD_25)
  #define PORT_MOTOR_PIN 6
  #define STARBOARD_MOTOR_PIN 8
  #define PORT_FIN_PIN 11
  #define STARBOARD_FIN_PIN 7
  #define ESCPOWER_PIN 16
#endif

class Thrusters : public Device {
  public:
    Thrusters():Device(){};
    void device_setup();
    void device_loop(Command cmd);
};
#endif
#endif

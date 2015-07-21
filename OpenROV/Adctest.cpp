#include "AConfig.h"
#if(HAS_STD_ADCTEST)
#include "Device.h"
#include "Pin.h"
#include "Adctest.h"
#include "Settings.h"
#include <Arduino.h>
#include "Timer.h"

Timer update_timer;

int sensorPin = A15;    // select the input pin for the potentiometer
float sensorValue = 0;  // variable to store the value coming from the sensor

void Adctest::device_setup(){
    Settings::capability_bitarray |= (1 << ADCTEST_CAPABLE);
    update_timer.reset();
}

void Adctest::device_loop(Command command){
	//int value = 0;
    if (update_timer.elapsed (100)){
      sensorValue = analogRead(sensorPin);
      Serial.print(F("ADC_TEST:"));
	  //Serial.print(value);
      Serial.print(5*sensorValue/1023);
      Serial.println(';');
    }
}
#endif

#include "AConfig.h"
#if(HAS_STD_THERMO)
#include "Device.h"
#include "Pin.h"
#include "Thermo.h"
#include "max6675.h"
#include "Settings.h"
#include <Arduino.h>
#include "Timer.h"

Timer thermo_update_timer;
MAX6675 thermocouple;

void Thermo::device_setup(){
    Settings::capability_bitarray |= (1 << THERMO_CAPABLE);
    thermo_update_timer.reset();
	
	int SO = 32;
	int CS = 31;
	int CLK = 30;
	thermocouple.init(SO, CLK, CS);
}

void Thermo::device_loop(Command command){
	
    if (thermo_update_timer.elapsed (500)){
		Serial.print(F("ThermoTEMP:"));
		Serial.print(float(thermocouple.measure(TEMPC)));
		Serial.println(';');
    }
}
#endif

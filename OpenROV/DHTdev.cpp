#include "AConfig.h"
#if(HAS_STD_DHTdev)
#include "Device.h"
#include "Pin.h"
#include "DHTdev.h"
#include "DHT11lib.h"
#include "Settings.h"
#include <Arduino.h>
#include "Timer.h"

Timer DHT_update_timer;

#define DHTPIN 37     	// what pin we're connected to
#define DHTTYPE DHT11   // DHT 11
DHT dht(DHTPIN, DHTTYPE);

void DHTdev::device_setup(){
    Settings::capability_bitarray |= (1 << DHTdev_CAPABLE);
    DHT_update_timer.reset();
	dht.begin();
}

void DHTdev::device_loop(Command command){
	
    if (DHT_update_timer.elapsed (500)){
		float h = dht.readHumidity();
		if (isnan(h)){
			h = -1;
		}
		float t = dht.readTemperature();
		if (isnan(t)){
			t = -1;
		}
		
		Serial.print(F("TubeTEMP:"));
		Serial.print(t);
		Serial.print(';');
		Serial.print(F("TubeHMDT:"));
		Serial.print(h);
		Serial.println(';');
    }
}
#endif

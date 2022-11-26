#ifndef PinMonitorModule_h
#define PinMonitorModule_h

#include "ModuleParent.h"
#include "Arduino.h"
//#include "SoftwareSerial.h"


class PinMonitorModule : public ModuleParent
{ 
    byte* digitaldata=NULL;
    byte* analog_data=NULL;	
	public:
	#if defined(ESP32)
	//byte byte_pins[2];
	byte function_id_d = 0x06;
	byte function_id_a= 0x05;
	uint8_t n=4;
	uint8_t m=12;
   	uint8_t digital_pins[4][8]={{2,0,4,5,0,0,0,0},{0,0,12,13,14,15,16,17},{18,19,0,21,22,23,0,25},{26,27,0,0,0,0,0,0}};
	uint8_t analog_pin[6]={32,33,34,35,36,39};
    //uint8_t analog_data[12];
	#endif
	
	public:
	PinMonitorModule();
	
	void sendDigitalData();
	void sendAnalogData();

	private:
	void processData();
    	
};

extern PinMonitorModule PinMonitor;
#endif

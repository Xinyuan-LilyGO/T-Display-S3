#ifndef SensorModule_h
#define SensorModule_h
#include "ModuleParent.h"
#include <math.h>

#define ACCELEROMETER 0x01
#define GYROSCOPE 0x02
#define MAGNETOMETER 0x03
#define PROXIMITY 0x04
#define LIGHT 0x05
#define SOUND 0x06
#define TEMPERATURE 0x07
#define BAROMETER 0x08
#define GPS 0x09

class SensorModule: public ModuleParent
{
	public:
	SensorModule();
	float getdata_Accelerometer_xaxis();
	float getdata_Accelerometer_yaxis();
	float getdata_Accelerometer_zaxis();
	float getdata_Gyroscope_xaxis();
	float getdata_Gyroscope_yaxis();
	float getdata_Gyroscope_zaxis();
	float getdata_Magnetometer_xaxis();
	float getdata_Magnetometer_yaxis();
	float getdata_Magnetometer_zaxis();
	float getdata_Proximity();
	float getdata_Light();
	float getdata_Sound();
	float getdata_Temperature();
	float getdata_Barometer();
	float getdata_GPS_longitude();
	float getdata_GPS_latitude();
	float getAccelerometerXaxis();
	float getAccelerometerYaxis();
	float getAccelerometerZaxis();
	float getGyroscopeXaxis();
	float getGyroscopeYaxis();
	float getGyroscopeZaxis();
	float getMagnetometerXaxis();
	float getMagnetometerYaxis();
	float getMagnetometerZaxis();
	float getProximityDistance();
	float getLightIntensity();
	float getSoundDecibels();
	float getTemperature();
	float getBarometerPressure();
	float getGPSlongitude();
	float getGPSLatitude();
	float getSensorData(uint8_t a);
	
	void processData();
	public:
	byte sensorvalue_x[4];
	byte sensorvalue_y[4];
	byte sensorvalue_z[4];
	byte sensor_data[4];
	byte sensor_data_1[4];
	
	float accelo_x=0;
	float accelo_y=0;
	float accelo_z=0;
	
	float gyro_x=0;
	float gyro_y=0;
	float gyro_z=0;
	
	float magneto_x=0;
	float magneto_y=0;
	float magneto_z=0;
	
	float proximity=0;
	float light=0;
	float barometer=0;
	float gps_longitude=0;
	float gps_latitude=0;
	float sound_level=0;
    float temperature=0;	
	
};


extern SensorModule Sensor;
#endif
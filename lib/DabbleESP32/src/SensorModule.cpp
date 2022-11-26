#define FROM_DABBLE_LIBRARY
#include "DabbleESP32.h"
#include "SensorModule.h"

SensorModule::SensorModule(): ModuleParent(SENSORS_ID)
{

}

float SensorModule::getAccelerometerXaxis()
{
return accelo_x;
}

float SensorModule::getAccelerometerYaxis()
{
return accelo_y;
}

float SensorModule::getAccelerometerZaxis()
{
return accelo_z;
}


float SensorModule::getGyroscopeXaxis()
{
return gyro_x;
}

float SensorModule::getGyroscopeYaxis()
{
return gyro_y;
}

float SensorModule::getGyroscopeZaxis()
{
return gyro_z;
}


float SensorModule::getMagnetometerXaxis()
{
return magneto_x;
}

float SensorModule::getMagnetometerYaxis()
{
return magneto_y;
}

float SensorModule::getMagnetometerZaxis()
{
return magneto_z;
}


float SensorModule::getProximityDistance()
{
return proximity;
}

float SensorModule::getLightIntensity()
{
return light;
}

float SensorModule::getSoundDecibels()
{
return sound_level;
}



float SensorModule::getTemperature()
{
return temperature;
}

float SensorModule::getBarometerPressure()
{
return barometer;
}

float SensorModule::getGPSlongitude()
{
return gps_longitude;
}

float SensorModule::getGPSLatitude()
{
return gps_latitude;
}

float SensorModule::getdata_Accelerometer_xaxis()
{
return accelo_x;
}

float SensorModule::getdata_Accelerometer_yaxis()
{
return accelo_y;
}

float SensorModule::getdata_Accelerometer_zaxis()
{
return accelo_z;
}


float SensorModule::getdata_Gyroscope_xaxis()
{
return gyro_x;
}

float SensorModule::getdata_Gyroscope_yaxis()
{
return gyro_y;
}

float SensorModule::getdata_Gyroscope_zaxis()
{
return gyro_z;
}


float SensorModule::getdata_Magnetometer_xaxis()
{
return magneto_x;
}

float SensorModule::getdata_Magnetometer_yaxis()
{
return magneto_y;
}

float SensorModule::getdata_Magnetometer_zaxis()
{
return magneto_z;
}


float SensorModule::getdata_Proximity()
{
return proximity;
}

float SensorModule::getdata_Light()
{
return light;
}

float SensorModule::getdata_Sound()
{
return sound_level;
}



float SensorModule::getdata_Temperature()
{
return temperature;
}

float SensorModule::getdata_Barometer()
{
return barometer;
}

float SensorModule::getdata_GPS_longitude()
{
return gps_longitude;
}

float SensorModule::getdata_GPS_latitude()
{
return gps_latitude;
}

float SensorModule::getSensorData(uint8_t a)
{
	if(a==0)
	{
		return accelo_x;
	}
	else if(a==1)
	{
		return accelo_y;
	}
	else if(a==2)
	{
		return accelo_z;
	}
	else if(a==3)
	{
		return gyro_x;
	}
	else if(a==4)
	{
		return gyro_y;
	}
	else if(a==5)
	{
		return gyro_z;
	}
	else if(a==6)
	{
		return magneto_x;
	}
	else if(a==7)
	{
		return magneto_y;
	}
	else if(a==8)
	{
		return magneto_z;
	}
	else if(a==9)
	{
		return proximity;
	}
	else if(a==10)
	{
		return temperature;
	}
	else if(a==11)
	{
		return sound_level;
	}
	else if(a==12)
	{
		return barometer;
	}
	else if(a==13)
	{
		return gps_longitude;
	}
	else if(a==14)
	{
		return gps_latitude;
	}
	else if(a==15)
	{
		return light;
	}
}

void SensorModule::processData()
{
	byte functionId =getDabbleInstance().getFunctionId();
	if(functionId == ACCELEROMETER || functionId == GYROSCOPE || functionId == MAGNETOMETER)
	{
        if(functionId == ACCELEROMETER)
		{
			accelo_x = getDabbleInstance().convertBytesToFloat(getDabbleInstance().getArgumentData(0));
			accelo_y = getDabbleInstance().convertBytesToFloat(getDabbleInstance().getArgumentData(1));
			accelo_z = getDabbleInstance().convertBytesToFloat(getDabbleInstance().getArgumentData(2));
		}
		else if(functionId == GYROSCOPE)
		{
			gyro_x = getDabbleInstance().convertBytesToFloat(getDabbleInstance().getArgumentData(0));
			gyro_y = getDabbleInstance().convertBytesToFloat(getDabbleInstance().getArgumentData(1));
			gyro_z = getDabbleInstance().convertBytesToFloat(getDabbleInstance().getArgumentData(2));
		}
		else if(functionId == MAGNETOMETER)
		{
			magneto_x = getDabbleInstance().convertBytesToFloat(getDabbleInstance().getArgumentData(0));
			magneto_y = getDabbleInstance().convertBytesToFloat(getDabbleInstance().getArgumentData(1));
			magneto_z = getDabbleInstance().convertBytesToFloat(getDabbleInstance().getArgumentData(2));
		}
	}
	
	if(functionId == PROXIMITY || functionId == TEMPERATURE || functionId == LIGHT || functionId == SOUND || functionId == BAROMETER)
	{
		if(functionId == PROXIMITY )
		{
			proximity = getDabbleInstance().convertBytesToFloat(getDabbleInstance().getArgumentData(0));
		}
		else if(functionId == TEMPERATURE)
		{
			temperature = getDabbleInstance().convertBytesToFloat(getDabbleInstance().getArgumentData(0));
		}
			else if(functionId == SOUND)
		{
			sound_level = getDabbleInstance().convertBytesToFloat(getDabbleInstance().getArgumentData(0));
		}
			else if(functionId == BAROMETER)
		{
			barometer = getDabbleInstance().convertBytesToFloat(getDabbleInstance().getArgumentData(0));
		}	
		else if(functionId == LIGHT)
		{
			light = getDabbleInstance().convertBytesToFloat(getDabbleInstance().getArgumentData(0));
		}
	}
	
	if(functionId == GPS)
		{
			
			gps_longitude = getDabbleInstance().convertBytesToFloat(getDabbleInstance().getArgumentData(1));
			gps_latitude =  getDabbleInstance().convertBytesToFloat(getDabbleInstance().getArgumentData(0));
		}	
	}


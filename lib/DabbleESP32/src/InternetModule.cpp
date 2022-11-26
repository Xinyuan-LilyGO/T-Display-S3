#define FROM_DABBLE_LIBRARY
#include "DabbleESP32.h"
#include "InternetModule.h"

//Constructor
InternetModule::InternetModule():ModuleParent(INTERNET_ID)
{}

/*//Read from Android
char InternetModule::read()
{
	if(buffer.remain()<=0)return -1;
	return buffer.pop();
	return 0;
}
//Flush buffer contents
void InternetModule::flush()
{
	while(read()!=-1);
}
//Check Data avialable in Buffer
int InternetModule::available()
{
	return buffer.remain();
}
//Read bytes from Buffer
int InternetModule::readBytes(char *arr, int length)
{
	int count = 0;
	 	while (count < length) {
	    int c = read();
	    if (c < 0) break;
	    *arr++ = (char)c;
	    count++;
 	}
  return count;
}
*/
//Terminal Incomming Data processing
void InternetModule::processData()
{
	byte functionID = getDabbleInstance().getFunctionId();
	byte dataLength = getDabbleInstance().getArgumentLength(0);
	byte dataLines =  getDabbleInstance().getArgumentNo();
	
	/*Serial.print("In processInput");
    Serial.println(functionID);
	Serial.println(dataLength);*/
	if(functionID == HTTP_RESPONSE && successState == 1 )
	{ 
		successState = 0;
		//Serial.println("Response");
        mainString = "";
		int counter = 0; 
		for(int i  = 0;i< dataLines; i++)
		{		
			for (int j=0; j<dataLength; j++)
			{
				//Serial.print("counter= " + String(counter) + "   ");
				//Serial.println(char (getDabbleInstance().getArgumentData(i)[j]));
				mainString = mainString + char(getDabbleInstance().getArgumentData(i)[j]);
				counter++;
			}
		}
		Serial.println(mainString);
	}
	if(functionID == HTTP_SUCCESS)
	{
		successState = 1;
		#ifdef DEBUG
		//Serial.println("Success");
		//Serial.println(getDabbleInstance().getArgumentData(0)[0]);
		#endif
	}
}

void InternetModule::sendGETRequest(String Urladd)
{
	Dabble.sendModuleFrame(INTERNET_ID,0,HTTP_GET,1,new FunctionArg(Urladd.length(),(byte *)&Urladd[0]));
}

void InternetModule::updateChannelFeed(String KEY,int noOfData, int value1, int value2, int value3, int value4, int value5, int value6, int value7, int value8)
{
	String channelFeedUrl = "";
	String baseUrl = "https://api.thingspeak.com/update?api_key=";
	channelFeedUrl = baseUrl + KEY;
	if(noOfData == 1)
	{
		channelFeedUrl = channelFeedUrl + "&field1=" + String(value1);
		#ifdef DEBUG 
		Serial.println(channelFeedUrl); 
		#endif
	}
	if(noOfData == 2)
	{
		channelFeedUrl = channelFeedUrl + "&field1=" + String(value1);
		channelFeedUrl = channelFeedUrl + "&field2=" + String(value2);
		#ifdef DEBUG 
		Serial.println(channelFeedUrl); 
		#endif
	}
	if(noOfData == 3)
	{
		channelFeedUrl = channelFeedUrl + "&field1=" + String(value1);
		channelFeedUrl = channelFeedUrl + "&field2=" + String(value2);
		channelFeedUrl = channelFeedUrl + "&field3=" + String(value3);
		#ifdef DEBUG 
		Serial.println(channelFeedUrl); 
		#endif
	}
	if(noOfData == 4)
	{
		channelFeedUrl = channelFeedUrl + "&field1=" + String(value1);
		channelFeedUrl = channelFeedUrl + "&field2=" + String(value2);
		channelFeedUrl = channelFeedUrl + "&field3=" + String(value3);
		channelFeedUrl = channelFeedUrl + "&field4=" + String(value4);
		#ifdef DEBUG 
		Serial.println(channelFeedUrl); 
		#endif
	}
	if(noOfData == 5)
	{
		channelFeedUrl = channelFeedUrl + "&field1=" + String(value1);
		channelFeedUrl = channelFeedUrl + "&field2=" + String(value2);
		channelFeedUrl = channelFeedUrl + "&field3=" + String(value3);
		channelFeedUrl = channelFeedUrl + "&field4=" + String(value4);
		channelFeedUrl = channelFeedUrl + "&field5=" + String(value5);
		#ifdef DEBUG 
		Serial.println(channelFeedUrl); 
		#endif
	}
	if(noOfData == 6)
	{
		channelFeedUrl = channelFeedUrl + "&field1=" + String(value1);
		channelFeedUrl = channelFeedUrl + "&field2=" + String(value2);
		channelFeedUrl = channelFeedUrl + "&field3=" + String(value3);
		channelFeedUrl = channelFeedUrl + "&field4=" + String(value4);
		channelFeedUrl = channelFeedUrl + "&field5=" + String(value5);
		channelFeedUrl = channelFeedUrl + "&field6=" + String(value6);
		#ifdef DEBUG
		Serial.println(channelFeedUrl);
		#endif
	}
	if(noOfData == 7)
	{
		channelFeedUrl = channelFeedUrl + "&field1=" + String(value1);
		channelFeedUrl = channelFeedUrl + "&field2=" + String(value2);
		channelFeedUrl = channelFeedUrl + "&field3=" + String(value3);
		channelFeedUrl = channelFeedUrl + "&field4=" + String(value4);
		channelFeedUrl = channelFeedUrl + "&field5=" + String(value5);
		channelFeedUrl = channelFeedUrl + "&field6=" + String(value6);
		channelFeedUrl = channelFeedUrl + "&field7=" + String(value7);
		#ifdef DEBUG 
		Serial.println(channelFeedUrl); 
		#endif		
	}
	if(noOfData == 8)
	{
		channelFeedUrl = channelFeedUrl + "&field1=" + String(value1);
		channelFeedUrl = channelFeedUrl + "&field2=" + String(value2);
		channelFeedUrl = channelFeedUrl + "&field3=" + String(value3);
		channelFeedUrl = channelFeedUrl + "&field4=" + String(value4);
		channelFeedUrl = channelFeedUrl + "&field5=" + String(value5);
		channelFeedUrl = channelFeedUrl + "&field6=" + String(value6);
		channelFeedUrl = channelFeedUrl + "&field7=" + String(value7);
		channelFeedUrl = channelFeedUrl + "&field8=" + String(value8);
		#ifdef DEBUG 
		Serial.println(channelFeedUrl); 
		#endif
		
	}
	if(channelFeedUrl.length() < 256)
	{
		Dabble.sendModuleFrame(INTERNET_ID,0,HTTP_GET,1,new FunctionArg(channelFeedUrl.length(),(byte *)&channelFeedUrl[0]));
	}
	else
	{
		#ifdef DEBUG
		Serial.println("Data too long");
		#endif
	}
}

void InternetModule::updateChannelField(String KEY, uint8_t fieldNumber,int data)
{
	String channelFeedUrl = "";
	String baseUrl = "https://api.thingspeak.com/update?api_key=";
	channelFeedUrl = baseUrl + KEY;
	channelFeedUrl = channelFeedUrl + "&field" + String(fieldNumber) + "=" + String(data);
	//Serial.println(channelFeedUrl);
	Dabble.sendModuleFrame(INTERNET_ID,0,HTTP_GET,1,new FunctionArg(channelFeedUrl.length(),(byte *)&channelFeedUrl[0]));
}
// https://api.thingspeak.com/channels/767508/fields/1.json?api_key=GHZVP14IXO9CINZO&results=2

float InternetModule::getFieldData(String KEY,uint8_t fieldNumber,long timeout)
{
	String fieldDataUrl = "";
	String baseUrl = "https://api.thingspeak.com/channels/767508/fields/";
	fieldDataUrl = baseUrl + String(fieldNumber) + ".json?api_key=" + KEY + "&results=2";
	/*#ifdef DEBUG
	Serial.println(fieldDataUrl);
	#endif*/
	Dabble.sendModuleFrame(INTERNET_ID,0,HTTP_GET,1,new FunctionArg(fieldDataUrl.length(),(byte *)&fieldDataUrl[0]));
	long lastTime =millis();
	//String mainString = ""; 
	
    Dabble.delay(10000);
	//Serial.println(mainString);
	int feedindex = mainString.indexOf("feeds");
	//Serial.println("feedindex" + String(feedindex));
    if(feedindex != -1)
	{
      int fieldindex = mainString.indexOf("field1", feedindex);
      if(fieldindex != -1 && fieldNumber == 1){
		float fieldValue1; 
        for (int k=9; k<100; k++){
          String value1string = mainString.substring(fieldindex+9,fieldindex+k+1);
          fieldValue1 = value1string.toFloat();
          k++;
        }
		return fieldValue1;
      }
      fieldindex = mainString.indexOf("field2", feedindex);
	  Serial.println(fieldindex);
      if(fieldindex != -1 && fieldNumber == 2){
        float fieldValue2;
		for (int k=9; k<100; k++){
          String value1string = mainString.substring(fieldindex+9,fieldindex+k+1);
          fieldValue2 = value1string.toFloat();
		  k++;
        }
		return fieldValue2;
      }
      fieldindex = mainString.indexOf("field3", feedindex);
       if(fieldindex != -1 && fieldNumber == 3){
        float fieldValue3;
		for (int k=9; k<100; k++){
          String value1string = mainString.substring(fieldindex+9,fieldindex+k+1);
          fieldValue3 = value1string.toFloat();
          k++;
        }
		return fieldValue3;
      }
      fieldindex = mainString.indexOf("field4", feedindex);
       if(fieldindex != -1 && fieldNumber == 4){
        float fieldValue4;
		for (int k=9; k<100; k++){
          String value1string = mainString.substring(fieldindex+9,fieldindex+k+1);
          fieldValue4 = value1string.toFloat();
          k++;
        }
		return fieldValue4;
      }
      fieldindex = mainString.indexOf("field5", feedindex);
       if(fieldindex != -1 && fieldNumber == 5){
		float fieldValue5;
	   for (int k=9; k<100; k++){
          String value1string = mainString.substring(fieldindex+9,fieldindex+k+1);
          fieldValue5 = value1string.toFloat();
          k++;
        }
		return fieldValue5;
      }
      fieldindex = mainString.indexOf("field6", feedindex);
       if(fieldindex != -1 && fieldNumber == 6){
        float fieldValue6;
		for (int k=9; k<100; k++){
          String value1string = mainString.substring(fieldindex+9,fieldindex+k+1);
          fieldValue6 = value1string.toFloat();
          k++;
        }
		return fieldValue6;
      }
      fieldindex = mainString.indexOf("field7", feedindex);
       if(fieldindex != -1 && fieldNumber == 7){
        float fieldValue7;
		for (int k=9; k<100; k++){
          String value1string = mainString.substring(fieldindex+9,fieldindex+k+1);
          fieldValue7 = value1string.toFloat();
          k++;
        }
		return fieldValue7;
      }
      fieldindex = mainString.indexOf("field8", feedindex);
       if(fieldindex != -1 && fieldNumber == 8){
        float fieldValue8;
		for (int k=9; k<100; k++){
          String value1string = mainString.substring(fieldindex+9,fieldindex+k+1);
          fieldValue8 = value1string.toFloat();
          k++;
        }
		return fieldValue8;
      }
	}
	else{
		return -100;
	}
}

void InternetModule::getWeatherData(String KEY, float lat, float lon)
{
	String latString = "";
	String longString = "";
	String appId = ""; 
	//"https:api.openweathermap.org/data/2.5/weather?lat={lat}&lon={lon}""
	String weatherURL = "";
	String baseUrl = "https://api.openweathermap.org/data/2.5/weather?";
	latString = "lat=" + String(lat);
	longString = "lon=" + String(lon);
	appId = "appid=" + KEY;
	weatherURL = baseUrl + latString + "&" + longString + "&" + appId ;
	Serial.println(weatherURL);
	
	Dabble.sendModuleFrame(INTERNET_ID,0,HTTP_GET,1,new FunctionArg(weatherURL.length(),(byte *)&weatherURL[0]));
	Dabble.delay(10000);
	
	Serial.println(mainString);
	int fieldindex;
	int feedindex;
	String tester = String("\"");
    
    Serial.println(mainString);

    fieldindex = mainString.indexOf("lon");
    if(fieldindex != -1){
      for (int k=9; k<20; k++){
        String value1string = mainString.substring(fieldindex+5,fieldindex+k+1);
        longitude = value1string.toFloat();
        k++;
      }
    }
    
    fieldindex = mainString.indexOf("lat");
    if(fieldindex != -1){
      for (int k=9; k<20; k++){
        String value1string = mainString.substring(fieldindex+5,fieldindex+k+1);
        latitude = value1string.toFloat();
        k++;
      }
    }

    fieldindex = mainString.indexOf("temp");
    if(fieldindex != -1){
      for (int k=9; k<20; k++){
        String value1string = mainString.substring(fieldindex+6,fieldindex+k+1);
        temperatureK = value1string.toFloat();
        k++;
      }
      temperatureC = (temperatureK- 273.15);
      temperatureF = (temperatureC *5/9) + 32;
    }

    fieldindex = mainString.indexOf("pressure");
    if(fieldindex != -1){
      for (int k=9; k<20; k++){
        String value1string = mainString.substring(fieldindex+10,fieldindex+k+1);
        pressure = value1string.toFloat();
        k++;
      }
    }

    fieldindex = mainString.indexOf("humidity");
    if(fieldindex != -1){
      for (int k=9; k<20; k++){
        String value1string = mainString.substring(fieldindex+10,fieldindex+k+1);
        humidity = value1string.toFloat();
        k++;
      }
    }

    fieldindex = mainString.indexOf("visibility");
    if(fieldindex != -1){
      for (int k=9; k<20; k++){
        String value1string = mainString.substring(fieldindex+12,fieldindex+k+1);
        visibility = value1string.toFloat();
        k++;
      }
    }

    fieldindex = mainString.indexOf("speed");
    if(fieldindex != -1){
      for (int k=9; k<20; k++){
        String value1string = mainString.substring(fieldindex+7,fieldindex+k+1);
        windSpeed = value1string.toFloat();
        k++;
      }
    }

    fieldindex = mainString.indexOf("deg");
    if(fieldindex != -1){
      for (int k=9; k<20; k++){
        String value1string = mainString.substring(fieldindex+5,fieldindex+k+1);
        windDirection = value1string.toFloat();
        k++;
      }
    }

    fieldindex = mainString.indexOf("all");
    if(fieldindex != -1){
      for (int k=9; k<20; k++){
        String value1string = mainString.substring(fieldindex+5,fieldindex+k+1);
        cloud = value1string.toFloat();
        k++;
      }
    }
    
    fieldindex = mainString.indexOf("dt");
    if(fieldindex != -1){
      for (int k=9; k<20; k++){
        String value1string = mainString.substring(fieldindex+4,fieldindex+k+1);
        dataTime = value1string.toFloat();
        k++;
      }
    //  dataTimeString = getStringDate(dataTime);
    }

    fieldindex = mainString.indexOf("sunrise");
    if(fieldindex != -1){
      for (int k=9; k<20; k++){
        String value1string = mainString.substring(fieldindex+9,fieldindex+k+1);
        sunRiseTime = value1string.toFloat();
        k++;
      }
      //sunRiseTimeString = getStringDate(sunRiseTime);
    }

    fieldindex = mainString.indexOf("sunset");
    if(fieldindex != -1){
      for (int k=9; k<20; k++){
        String value1string = mainString.substring(fieldindex+8,fieldindex+k+1);
        sunSetTime = value1string.toFloat();
        k++;
      }
    //sunSetTimeString = getStringDate(sunSetTime);
    }
    
    feedindex = mainString.indexOf("weather");
    if(feedindex != -1){
      fieldindex = mainString.indexOf("main", feedindex);
      int pos1 = mainString.indexOf(tester, fieldindex+6);
      int pos2 = mainString.indexOf(tester, fieldindex+9);
      weather = mainString.substring(pos1+1,pos2);
    }

    feedindex = mainString.indexOf("weather");
    if(feedindex != -1){
      fieldindex = mainString.indexOf("description", feedindex);
      int pos1 = mainString.indexOf(tester, fieldindex+13);
      int pos2 = mainString.indexOf(tester, fieldindex+16);
      weatherDescription = mainString.substring(pos1+1,pos2);
    }

    feedindex = mainString.indexOf("country");
    if(feedindex != -1){
      int pos1 = mainString.indexOf(tester, feedindex+9);
      int pos2 = mainString.indexOf(tester, feedindex+12);
      countryCode = mainString.substring(pos1+1,pos2);
    }
    else{
      countryCode = "Not Found";
    }

    feedindex = mainString.indexOf("name");
    if(feedindex != -1){
      int pos1 = mainString.indexOf(tester, feedindex+6);
      int pos2 = mainString.indexOf(tester, feedindex+9);
      cityName = mainString.substring(pos1+1,pos2);
    }
    else{
      cityName = "Not Found";
    }
	Serial.println("latitude: " + String(latitude));
	Serial.println("longitude: " + String(longitude));
	Serial.println("temperatureK: " + String(temperatureK));
	Serial.println("temperatureC: " + String(temperatureC));
	Serial.println("humidity: " + String(humidity));
	Serial.println("pressure: " + String(pressure));
	Serial.println("visibility: " + String(visibility));
	Serial.println("windSpeed: " + String(windSpeed));
	Serial.println("windDirection: " + String(windDirection));
	Serial.println("cloud: " + String(cloud));
	Serial.println("rain1hr: " + String(rain1hr));
	Serial.println("dataTime: " + String(dataTime));
	Serial.println("sunRiseTime: " + String(sunRiseTime));
	Serial.println("sunSetTime: " + String(sunSetTime));
}

float InternetModule::getLatitude()
{
	return latitude;
}

float InternetModule::getLongitude()
{
	return longitude;
}

float InternetModule::getPressure()
{
	return pressure;
}

float InternetModule::getTemperatureC()
{
	return temperatureC;
}

float InternetModule::getTemperatureF()
{
	return temperatureF;
}

float InternetModule::getTemperatureK()
{
	return temperatureK;
}


/*
 * This library is developed for Dabble app. https://thestempedia.com/product/dabble/
 * It includes modulewise sublibraries as per modules present in 
 * Dabble app.
 * Version 1.5.1
 * 
 * This library structure is derived from OneSheeld Library.
 *
 * This is licensed under GNU GPL V3 [http://www.gnu.org/licenses/gpl.txt].
 * Written by Dhrupal R Shah for ESP32, Agilo Research Pvt. Ltd
 * Created on: April 10, 2019
 * Updated on: 20190610
 * Contact: support@thestempedia.com
 * Copyright (c) 2018 Agilo Research Pvt. Ltd.  All rights reserved.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef DabbleESP32_h
#define DabbleESP32_h

#include "Stream.h"
#include <Arduino.h>
#include "esp32BLEUtilities.h"

#include "ModuleIds.h"
#include "ModuleSelection.h"
#include "ModuleIncludes.h"
#include "ModuleParent.h"
#include "ModuleInstantiation.h"
//#include "InternetModule.h"
//#include "HttpResponse.h"
//#include "HttpRequest.h"

#define ONE_SECOND 1000

//Start and End of packet sent
#define START_OF_FRAME  0xFF
#define END_OF_FRAME 	0x00
//Time between sending Frames
#define TIME_GAP		10UL

//#define DEBUG

//Output function ID's
#define CHECK_CONNECTION	0x01
#define BOARDID_REQUEST		0x03

#define SEND_LIBRARY_VERSION	0x01

#define CALLBACK_ENTERED		0x03
#define CALLBACK_EXITED			0x04
#define LIBRARY_TESTING_RESPONSE	0x05
//Input function ID's 
//Checking Bluetooth connection
#define CONNECTION_CHECK_FUNCTION 0x01
#define DISCONNECTION_CHECK_FUNCTION 0x02



//#define LIBRARY_VERSION_REQUEST	0x03
//#define LIBRARY_TESTING_REQUEST	0x05


//Number of Module
#define MODULE_NO	18
extern bool callBackForDataLogger;
extern void (*dataLoggerCallBack)(void);
//Class for Datalength and Data
class FunctionArg
{
private:
	byte length;
	byte * data;
	bool saveData;
public:
	FunctionArg(int l ,byte * d, bool _saveData=false)
	{
		saveData=_saveData;
		length=(l>0xff)?0xff:l;
		if(saveData)
		{
			data=(byte *)malloc(sizeof(byte)*length);
			memcpy(data,d,length);
		}
		else
		{
			data=d;
		}
	}
	byte getLength()
	{
		return length;
	}
	byte * getData()
	{
		return data;
	}
	~FunctionArg()
	{
		if(saveData)free(data);
	}

};

union FloatUnion{
	    float number;
	    byte floatBytes[sizeof(float)];
	  };



class DabbleClass
{


public:
    
	DabbleClass();
	//Blocking function
	void waitForAppConnection();
	//Check connection
	bool isAppConnected();
	void setOnAppConnected(void (*)(bool));
	//rx,tx pins for software
	uint8_t pin_rx,pin_tx;
	//Getters 
	byte getModuleId();
	byte getFunctionId();
	byte getArgumentNo();
	byte getScreenId();
	byte readModuleId();
	byte getArgumentLength(byte x);
	byte * getArgumentData(byte );
	void convertFloatToBytes(float , byte *);
	float convertBytesToFloat(byte * );
	//Processing Incomming Frames
	void processInput();
    void appWrite(byte command);	
	void begin(std::string bleName);
	//Adding objects in array 
	static void addToModulesArray(ModuleParent *);
	// #ifdef INTERNET_MODULE
	//static void addToUnSentRequestsArray(HttpRequest *);
	// #endif
	static bool isInitialized();
	static bool isSoftwareSerial();
	//Frame Sender
	void sendModuleFrame(byte , byte ,byte , byte , ...);
	void sendModuleFrame(byte , byte , byte , byte , FunctionArg ** );
	void setOnNewModuleFrame(void (*)(byte, byte, byte, byte *,byte **));
	void setOnNewSerialData(void (*)(byte));
	//PulseWidthModulation Getter 
	unsigned char analogRead(int );	
	void delay(unsigned long);
	bool isCallbacksInterruptsSet();
	void enableCallbacksInterrupts();
	void disableCallbacksInterrupts();
	byte getVerificationByte();
private:
	//Reserve Variables
	bool isArgumentsNumberMalloced;
	bool isArgumentLengthMalloced;
	bool isDabbleConnected;
	bool isAppConnectionCallBack;
	bool isModuleFrameCallback;
	bool isSerialDataCallback;
	bool dontDelay;
	static bool isFirstFrame;
	bool framestart;
	static bool inACallback;
	static bool callbacksInterrupts;
	//Data bytes
	byte numberOfDataMalloced;
	byte Module;
	byte verificationByte;
	byte functions;
	byte counter;
	byte screenId;
	byte readModuleID;
	byte argumentcounter;
	byte datalengthcounter;
	byte argumentnumber;
	byte argumentsize;
	byte **arguments;	
	byte *argumentL;            
	byte endFrame;
	//Module Counter 
	static byte ModulesCounter;
	//Requests Counter
	static byte requestsCounter;
	//Is constructor called
	static bool isInit;
	static bool isSws;
	//Checker variable 
	static unsigned long lastTimeFrameSent;

	static unsigned long argumentDataBytesTimeReceived;
	//Array of pointers to Parents
	static ModuleParent * ModulesArray[MODULE_NO];
	// #ifdef INTERNET_MODULE
	//Array of pointers to un sent requests
	//static HttpRequest ** requestsArray;
	// #endif
	//Send Incomming Data to Modules
	void sendToModules();
    //void begin(long baudRate);
	void init();
	void freeMemoryAllocated();
	void processFrame();
	void (*isAppConnectedCallBack)(bool);
	void (*ModuleFrameCallback)(byte, byte, byte, byte *,byte **);
	void (*serialDataCallback)(byte);
	void enteringACallback();
	void exitingACallback();
	bool isInACallback();
	void processInput(int);
	
friend class ModuleParent;
};
//Extern Object
extern DabbleClass Dabble;
#endif

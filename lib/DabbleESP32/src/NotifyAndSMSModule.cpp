#define FROM_DABBLE_LIBRARY
#include "DabbleESP32.h"
#include "NotifyAndSMSModule.h"

SMSModule::SMSModule():ModuleParent(SMS_ID)
{}

void SMSModule::sendMessage(String number, String content)
{
	uint8_t* numberInt = NULL;
	numberInt =  new uint8_t [number.length()];
	for(int i =0 ; i<number.length();i++)
	{
		numberInt[i] = (number.charAt(i) - 48);
		/*#ifdef DEBUG
			Serial.print(numberInt[i]);
		#endif*/
   }
  /* #ifdef DEBUG
		Serial.println();
   #endif*/
   Dabble.sendModuleFrame(SMS_ID,0,SENDNUMBER,1,new FunctionArg(number.length(), numberInt));
   Dabble.sendModuleFrame(SMS_ID,0,SENDCONTENT,1,new FunctionArg(content.length(),(byte*)&content[0]));
   delete [] numberInt;
}

NotificationModule::NotificationModule()
{
}


void NotificationModule::setTitle(String title)
{
	Dabble.sendModuleFrame(NOTIFICATION_ID,0,CREATE_NOTIFICATION, 1, new FunctionArg(title.length(),(byte*)&title[0]));
}

void NotificationModule::notifyPhone(String content)
{
	if(prevContent != content)
	Dabble.sendModuleFrame(NOTIFICATION_ID,0,UPDATE_NOTIFICATION, 1, new FunctionArg(content.length(),(byte*)&content[0]));
    
	prevContent = content;
}

void NotificationModule::clear()
{
	Dabble.sendModuleFrame(NOTIFICATION_ID,0,CLEAR_NOTIFICATION,0); 
	prevContent  = "";
}

/*void NotificationModule::notifyPhone(String title, String content)
{
	if(prevTitle != title)
	{
	create(title);
	}
	message(content);
	prevTitle = title;
}*/
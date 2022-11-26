#ifndef SMSModule_H_
#define SMSModule_H_
#include "ModuleParent.h"


//function -id
#define SENDNUMBER  01                     // number for SMS
#define SENDCONTENT 02                     // Content of Message
#define CREATE_NOTIFICATION	0x03           //for creating notification
#define UPDATE_NOTIFICATION	0x04           //for updating content
#define CLEAR_NOTIFICATION	0x05          //for clear current notification

class SMSModule:public ModuleParent
{
	public:
		SMSModule();	
		void sendMessage(String Number, String Content);
};
class NotificationModule
{
	public:
        NotificationModule();	
		void setTitle(String);
        void notifyPhone(String content);
		void clear();
	private:
		String prevContent="";
		
};
extern SMSModule SMS;
extern NotificationModule Notification;
#endif

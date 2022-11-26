
#define FROM_DABBLE_LIBRARY
#include "DabbleESP32.h"

DabbleClass * ModuleParent::DabbleInstance=NULL;
bool ModuleParent::DabbleInstanceAvailable=false;

ModuleParent::ModuleParent(byte ModuleNo)
{
	//Serial.println("FOUND NEW MODULE");
	ModuleID = ModuleNo ;
	/*#ifdef DEBUG
	Serial.println(ModuleID);
	#endif*/
	isCallBackSet=false;
	DabbleClass::addToModulesArray(this);
}

void ModuleParent::select()
{
	Dabble.sendModuleFrame(ModuleID,0x00,SELECT_MODULE,0x00);
}

void ModuleParent::deselect()
{
	Dabble.sendModuleFrame(ModuleID,0x00,DESELECT_MODULE,0x00);
}

void ModuleParent::setOnSelected(void (*userFunction)(void))
{
	Dabble.sendModuleFrame(ModuleID,0x00,QUERY_SELECTED,0x00);
	isCallBackSet=true;
	selectedCallBack=userFunction;
}

byte ModuleParent::getModuleId()
{
	return ModuleID;
}

void ModuleParent::enteringACallback()
{
  Dabble.enteringACallback();
}

void ModuleParent::exitingACallback()
{
  Dabble.exitingACallback();
}

bool ModuleParent::isInACallback()
{
  return Dabble.isInACallback();
}

void ModuleParent::setDabbleInstance(DabbleClass & instance)
{
	DabbleInstance=&instance;
	DabbleInstanceAvailable=true;
}

void ModuleParent::unSetDabbleInstance()
{
	DabbleInstance=NULL;
	DabbleInstanceAvailable=false;
}

DabbleClass & ModuleParent::getDabbleInstance()
{
	if(DabbleInstanceAvailable)return *DabbleInstance;
	else return Dabble;
}

void ModuleParent::processFrame()
{
	if(ModuleID!=getDabbleInstance().getModuleId())
	{
		return;
	}
	byte functionID = getDabbleInstance().getFunctionId();

	if(functionID == CHECK_SELECTED)
	{
		if(isCallBackSet && !isInACallback())
		{
				enteringACallback();
				(*selectedCallBack)();
				exitingACallback();
		}
	}
	else 
	{
		processData();
	}
}
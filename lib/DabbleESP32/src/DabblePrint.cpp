
#define FROM_DABBLE_LIBRARY
#include "DabbleESP32.h"
#include "DabblePrint.h"




//Constructor 
PrintClass::PrintClass(byte shid,byte writefnid, byte printfnid)
{
	ModuleId=shid;
	print_fn_id=printfnid;
	write_fn_id=writefnid;
}
//Write character 
void PrintClass::write(char data)
{
	Dabble.sendModuleFrame(ModuleId,0,write_fn_id,1,new FunctionArg(1,(byte *)&data));
}
//Print character
void PrintClass::print(char data)
{
	Dabble.sendModuleFrame(ModuleId,0,print_fn_id,1,new FunctionArg(1,(byte *)&data));
}
//Print integers
void PrintClass::print(int data)
{
	char stringPointer[7];
	snprintf(stringPointer,7,"%d",data);

	Dabble.sendModuleFrame(ModuleId,0,print_fn_id,1,new FunctionArg(strlen(stringPointer),(byte *)stringPointer));
}
//Print unsigned integers 
void PrintClass::print(unsigned int data)
{
	char stringPointer[6];
	snprintf(stringPointer,6,"%d",data);
	
	Dabble.sendModuleFrame(ModuleId,0,print_fn_id,1,new FunctionArg(strlen(stringPointer),(byte *)stringPointer));
}
//Print long integers
void PrintClass::print(long data)
{
	char stringPointer[12];
	snprintf(stringPointer,12,"%ld",data);
	
	Dabble.sendModuleFrame(ModuleId,0,print_fn_id,1,new FunctionArg(strlen(stringPointer),(byte *)stringPointer));
}
//Print unsigned long integers
void PrintClass::print(unsigned long data)
{
	char stringPointer[11];
	snprintf(stringPointer,11,"%lu",data);
	
	Dabble.sendModuleFrame(ModuleId,0,print_fn_id,1,new FunctionArg(strlen(stringPointer),(byte *)stringPointer));	
}
//Print string
void PrintClass::print(const char * stringData)
{
	//Serial.println("Print String");
	//Check length of string 
	int stringDataLength = strlen(stringData);
	#ifdef DEBUG
	Serial.println(stringDataLength);
	#endif
	if(!stringDataLength) return;
	Dabble.sendModuleFrame(ModuleId,0,print_fn_id,1,new FunctionArg(stringDataLength,(byte*)stringData));	
}

void PrintClass::print(String stringData)
{
	print(&stringData[0]);
}

//Print double 
//Unsupported by Intel Galileo board and Arduino Due
#if  !defined(ARDUINO_LINUX) && !defined(SAM3X8)
void PrintClass::print(double data , int precesion)
{
	char buffer[32]={'\0'};
	dtostrf(data,1,precesion,buffer);
	
	Dabble.sendModuleFrame(ModuleId,0,print_fn_id,1,new FunctionArg(strlen(buffer),(byte *) buffer));
}
#endif
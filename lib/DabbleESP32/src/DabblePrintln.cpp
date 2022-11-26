
#define FROM_DABBLE_LIBRARY
#include "DabbleESP32.h"
#include "DabblePrintln.h"

//Constructor
PrintlnClass::PrintlnClass(byte shid,byte writefnid, byte printfnid):PrintClass(shid,writefnid,printfnid)
{
}
//Print newline 
void PrintlnClass::println()
{
	char buffer[3];
	buffer[0]='\r';
	buffer[1]='\n';
	buffer[2]='\0';

	print(buffer);
}
//Print character with newline
void PrintlnClass::println(char data)
{
	char buffer[4];
	buffer[0]=data;
	buffer[1]='\r';
	buffer[2]='\n';
	buffer[3]='\0';
	
	print(buffer);
}
//Print integers with newline 
void PrintlnClass::println(int data)
{
	char stringPointer[10];
	snprintf(stringPointer,9,"%d",data);
	strcat(stringPointer,"\r\n");
	
	print(stringPointer);
	#ifdef DEBUG
	Serial.println(data);
	Serial.println(stringPointer);
	#endif	
}
//Print unsigned integers with newline
void PrintlnClass::println(unsigned int data)
{
	char stringPointer[9];
	snprintf(stringPointer,8,"%d",data);
	strcat(stringPointer,"\r\n");
	
	print(stringPointer);

}
//Print long with newline
void PrintlnClass::println(long data)
{
	char stringPointer[15];
	snprintf(stringPointer,14,"%ld",data);
	strcat(stringPointer,"\r\n");
	
	print(stringPointer);
}
//Print unsigned long with newline
void PrintlnClass::println(unsigned long data)
{
	char stringPointer[14];
	snprintf(stringPointer,13,"%lu",data);
	strcat(stringPointer,"\r\n");
	
	print(stringPointer);
}
//Print string with newline
void PrintlnClass::println(const char * stringData)
{
	char stringNewLine[strlen(stringData)+3];
	stringNewLine[0]='\0';
	strcat(stringNewLine,stringData);
	strcat(stringNewLine,"\r\n");

	print(stringNewLine);
}

//Support strings
void PrintlnClass::println(String stringData)
{
	println(&stringData[0]);
}

//Unsupported by Intel Galileo board and Arduino Due
#if  !defined(ARDUINO_LINUX) && !defined(SAM3X8)
void PrintlnClass::println(double data, int precesion)
{
	int i=0;
	char buffer[32]={'\0'};
	dtostrf(data,1,precesion,buffer);

	while(buffer[i]!='\0' && i<32)i++;

	if(i+2>32)return;
	buffer[i]='\r';
	buffer[i+1]='\n';
	buffer[i+2]='\0';

	print(buffer);

}
#endif


#ifndef TerminalModule_h
#define TerminalModule_h

#include "ModuleParent.h"

#include "DabblePrintln.h"
#include "CircularBuffer.h"


//Output Function ID's
#define TERMINAL_WRITE 0x02	
#define TERMINAL_PRINT 0x02

//Input Function ID
#define TERMINAL_READ  0x01

class TerminalModule : public PrintlnClass, public ModuleParent
{
public:
	//Constructor
	TerminalModule();
	//Checker
	int available();
	//Getter
	char read();
	//Getter
	int readBytes(char *, int);
	//Setter
	void flush();
	String readString();
	bool compareString(String text);
	int readNumber();
	 
private:
	//Instatiate Object from class CircularBuffer
	CircularBuffer<char,64> buffer;
	//CircularBuffer<char,64> buffer1;
	//Process Input data
	void processData();
};
//Extern Object
extern TerminalModule Terminal;
#endif
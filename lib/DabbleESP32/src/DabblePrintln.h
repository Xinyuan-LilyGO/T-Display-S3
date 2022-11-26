

#ifndef PrintlnClass_h
#define PrintlnClass_h

#include "DabblePrint.h"

class PrintlnClass: public PrintClass
{
public:
	PrintlnClass(byte, byte, byte);
	//Printing in new line 
	void println();
	void println(char);
	void println(int);
	void println(unsigned int);
	void println(long);
	void println(unsigned long);
	void println(const char *);
	void println(String);
	//Unsupported by Intel Galileo board and Arduino Due
	#if  !defined(ARDUINO_LINUX) && !defined(SAM3X8)
	void println(double , int = 3);
    #endif
};

#endif
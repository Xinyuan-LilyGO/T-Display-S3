

#ifndef PrintClass_h
#define PrintClass_h

//Defining decimal 
#define DEC	  10

class PrintClass
{
public:
	PrintClass(byte, byte, byte);
	//Write
	void write(char);
	//Printing functions
	void print(char);
	void print(int);
	void print(unsigned int);
	void print(long);
	void print(unsigned long);
	void print(const char *);
	void print(String );
	//Unsupported by Intel Galileo board and Arduino Due
	#if  !defined(ARDUINO_LINUX) && !defined(SAM3X8)
	void print(double ,int = 3 );
	#endif
private:
	//Reserve variables for function ID's
	byte print_fn_id;
	byte write_fn_id;
	//Reserve variable for the Module ID
	byte ModuleId;
};
extern PrintClass PinStatus;
#endif
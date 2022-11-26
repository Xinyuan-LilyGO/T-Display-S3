#ifndef DataLoggerModule_H_
#define DataLoggerModule_H_
#include "ModuleParent.h"

//functionId
#define FILENAME		0x01
#define COLUMNNAME		0x02
#define CLOSEFILE		0x03
#define	DATATYPE_CHAR	0x04
#define DATATYPE_INT	0x05
#define DATATYPE_FLOAT	0x06

class DataLoggerModule:public ModuleParent
{
public:
DataLoggerModule();
void createFile(String);
//void createFile(String,int,...);  //FileName, number of columns,column names
void createColumn(String colName);
void send(String,float);
void send(String,String);
void sendSettings(void(*)(void));
//void send(String,char);
//void send(int,...);
void stop();

//PictoBlox
void fileConfig(uint8_t,String);
String *columnName =NULL;
};
extern DataLoggerModule DataLogger;
#endif
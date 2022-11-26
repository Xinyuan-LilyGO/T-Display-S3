#define FROM_DABBLE_LIBRARY
#include "DabbleESP32.h"
#include "DataLoggerModule.h"
uint8_t columnNumber=0;
String *tempColName = NULL;
DataLoggerModule::DataLoggerModule():ModuleParent(DATALOGGER_ID)
{}

void DataLoggerModule::sendSettings(void(*function)(void))
{
	callBackForDataLogger = true;
	dataLoggerCallBack = function;
}
void DataLoggerModule::createFile(String FileName)
{
	columnNumber=0;
	Dabble.sendModuleFrame(DATALOGGER_ID,0,FILENAME,1,new FunctionArg(FileName.length(),(byte *)&FileName[0]));
}

void DataLoggerModule::createColumn(String colName)
{
	//Process to store FileNames starts
	if(columnNumber != 0)
	{
		if(columnNumber != 1)
		delete [] tempColName;
		tempColName = new String[columnNumber];
	}
	for(int i=0;i<columnNumber;i++)
	{
		tempColName[i] = columnName[i]; 
	}
	if(columnNumber !=0 )
	{
		delete[] columnName;
	}
	columnName = new String[columnNumber+1];   //+1 since number of columns start from 0;
	for(int i = 0;i < columnNumber;i++)
	{
		columnName[i] = tempColName[i];
	}
	columnName[columnNumber] = colName;
    #ifdef DEBUG
	for(int i=0;i<(columnNumber+1);i++)
	{
		Serial.println(columnName[i]);
	
	}
	#endif
	//end
	uint8_t a = columnNumber+1;
	String sendColumnName = columnName[columnNumber];
    Dabble.sendModuleFrame(DATALOGGER_ID,0,COLUMNNAME,2,new FunctionArg(1,&a),new FunctionArg(sendColumnName.length(),(byte *)&sendColumnName[0]));
	columnNumber++;
}

void DataLoggerModule::fileConfig(uint8_t type, String Name)
{
	if(type == 1)
	{
		createFile(Name);
	}
	else if(type == 2)
	{
		createColumn(Name);
	}
}

void DataLoggerModule::send(String col,float data)
{
	byte columnFlag=0;
	byte floatData[4];
	Dabble.convertFloatToBytes(data,floatData);

	for(int i =0;i<columnNumber;i++)
	{
		if(col == columnName[i])
		{
			columnFlag = i+1;
			#ifdef DEBUG
			//Serial.println(columnFlag);
			#endif
			break;
		}
	}
	if(columnFlag == 0)
	{
		#ifdef DEBUG
		//Serial.println("No match");
		#endif
		return;
	}

		Dabble.sendModuleFrame(DATALOGGER_ID,0,DATATYPE_FLOAT,2,new FunctionArg(1,&columnFlag),new FunctionArg(sizeof(float),floatData));

}

void DataLoggerModule::send(String col,String data)
{
	byte columnFlag=0;

	for(int i =0;i<columnNumber;i++)
	{
		if(col == columnName[i])
		{
			columnFlag = i+1;
			#ifdef DEBUG
			//Serial.println(columnFlag);
			#endif
			break;
		}
	}
	if(columnFlag == 0)
	{
		return;
	}
	
		Dabble.sendModuleFrame(DATALOGGER_ID,0,DATATYPE_CHAR,2,new FunctionArg(1,&columnFlag),new FunctionArg(data.length(),(byte *)&data[0]));
}

void DataLoggerModule::stop()
{
Dabble.sendModuleFrame(DATALOGGER_ID,0,CLOSEFILE,0);
}

/*void DataLoggerModule::createFile(String FileName,int count,...)
{
columnNumber = count;
va_list colName;
va_start(colName, count);
  char *str;
  columnName = new String[count];  
  for ( int i = 0; i < count; i++ ) // do "count" times
  {
	str = va_arg( colName, char* );
	columnName[i] = "";
	while (*str != '\0')
	{
		columnName[i] = String(columnName[i] + *str);
	  //columnName[i] = String(columnName[i] + *str);
	  str++;
	}
}
va_end(colName);
//columnName=&columnName[0];
#ifdef DEBUG
for(int i = 0;i<count;i++)
{
	//Serial.print(i);
	//Serial.print(" ");
	//Serial.println(columnName[i]);
}
#endif
//Serial.println();
byte fileNameLength = FileName.length();
Dabble.sendModuleFrame(DATALOGGER_ID,0,FILENAME,1,new FunctionArg(fileNameLength,&FileName[0]));
for(uint8_t i=0;i<count;i++)
{
	byte a = i+1;
	byte columnNameLength = columnName[i].length();
	Dabble.sendModuleFrame(DATALOGGER_ID,0,COLUMNNAME,2,new FunctionArg(1,&a),new FunctionArg(columnNameLength,&columnName[i][0]));
}
}*/
/*void DataLoggerModule::send(String col,int data)
{
byte columnFlag=0;

for(int i =0;i<columnNumber;i++)
{
	if(col == columnName[i])
	{
		columnFlag = i+1;
		//Serial.println(columnFlag);
		break;
	}
}
if(columnFlag == 0)
{
	return;
}
byte a[2];
a[0] = byte(data>>8);
a[1] = byte(data);
//Serial.print(a[0],BIN);
//Serial.print(" ");
//Serial.print(a[1],BIN);
//Serial.print(" ");
//Serial.println(data,BIN);
Dabble.sendModuleFrame(DATALOGGER_ID,0,DATATYPE_INT,2,new FunctionArg(1,&columnFlag),new FunctionArg(sizeof(int),a));
}*/

/*void send(int count,...)
{
String tempColName = "";
byte columnFlag = 0;
va_list columData;
va_start(colData, count);
	for(int i=0;i<count;i++)
	{
		if(i%2 == 0)
		{
			char *tempData;
			tempData = va_arg( colName, char* );
			while(*tempData != '\0')
			{
				tempColName = String(tempColName + *tempData);
			}
			for(int i =0;i<columnNumber;i++)
			{
				if(col == columnName[i])
				{
					columnFlag = i+1;
					//Serial.println(columnFlag);
					break;
				}
			}
			if(columnFlag == 0)
			{
				return;
			}
		}
		if(i%2 == 1)
		{
			
		}
	}
va_end(colData);


}*/


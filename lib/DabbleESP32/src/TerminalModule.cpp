#define FROM_DABBLE_LIBRARY
#include "DabbleESP32.h"
#include "TerminalModule.h"

//String comparestring= "";
String numberString= "";
String stringData= "";
String comparestring="";
bool stringclearflag1=0;
bool stringclearflag2=0;
bool stringclearflag3=0;

//Constructor
TerminalModule::TerminalModule():PrintlnClass(TERMINAL_ID,TERMINAL_WRITE,TERMINAL_PRINT),ModuleParent(TERMINAL_ID)
{}

//Read from Android
char TerminalModule::read()
{
	if(buffer.remain()<=0)return -1;
	return buffer.pop();
}
//Flush buffer contents
void TerminalModule::flush()
{
	while(read()!=-1);
}
//Check Data avialable in Buffer
int TerminalModule::available()
{
	return buffer.remain();
}
//Read bytes from Buffer
int TerminalModule::readBytes(char *arr, int length)
{
	int count = 0;
	 	while (count < length) {
	    int c = read();
	    if (c < 0) break;
	    *arr++ = (char)c;
	    count++;
 	}
  return count;
}

//Terminal Incomming Data processing
void TerminalModule::processData()
{
	byte functionID = getDabbleInstance().getFunctionId();
	byte dataLength = getDabbleInstance().getArgumentLength(0);
	
	if(functionID == TERMINAL_READ)
	{
		    
			for (int j=0; j<dataLength; j++)
			{
				buffer.push(getDabbleInstance().getArgumentData(0)[j]);
				//buffer1.push(getDabbleInstance().getArgumentData(0)[j]);
			}
	}
}
bool TerminalModule::compareString(String text)
{
	String compareData= "";
	readString();
    compareData = comparestring;
	//Serial.println("Terminal String" + comparestring);
	
    if(stringclearflag3 ==1)
	{
		compareData= "";
	}	
	if(compareData == text)
	{
	  stringclearflag3 =1;
	 return 1;
	}
	else
	{
     return 0;
	}
	
}


/*int TerminalModule::readNumber()
{
	int numberData;
	bool a =0;
	String numCheck=stringData;
	for(int i=0;i<numCheck.length();i++)
	{
		if(numCheck.charAt(i)>47 && numCheck.charAt(i)<58)
		{
			a=1;
		}			
	}
	if(a==1)
	{
		
		numberData= numCheck.toInt();
	}
	else
	{
		numberData=-30000;
	}
	return numberData;
}*/

String TerminalModule::readString()
{
	if(buffer.remain()>0)
	{
	stringclearflag1=0;
	stringclearflag2=0;
	stringclearflag3=0;
	//comparestring="";
	//numberstring="";
	stringData= "";
	while(buffer.remain()>0)
	{
     char a = buffer.pop();
	 stringData=String(stringData+a);
	 numberString=stringData;
	 comparestring=stringData;
	 //comparestring=String(comparestring+a);
	 //numberstring=String(numberstring+a);
	}
	}
	if(stringclearflag1==0)
	{
		stringclearflag1 =1;
		return stringData;
	}
	else
	{
	 stringData = "";
	 return stringData;
	}
}

int TerminalModule::readNumber()
{
	int numberData;
	bool a=0;
	readString();
	for(int i=0;i<numberString.length();i++)
	{
		if(numberString.charAt(i)>47 && numberString.charAt(i)<58)
		{
			a=1;
		}
        else
        {
			break;
		}			
	}
	if(a==1 && stringclearflag2 == 0)
	{
		stringclearflag2=1;
		numberData= numberString.toInt();
	}
	else
	{
		numberData=-100;
	}
	return numberData;
}




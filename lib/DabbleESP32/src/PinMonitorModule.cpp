#define FROM_DABBLE_LIBRARY
#include "DabbleESP32.h"
#include "PinMonitorModule.h"

PinMonitorModule::PinMonitorModule() : ModuleParent(PINMONITOR_ID)
{

}

/*void PinMonitorModule::write(char data)
{
	Dabble.sendModuleFrame(PINMONITOR_ID,0,function_id_d,1,new FunctionArg(1,(byte *)&data));
}*/

void PinMonitorModule::processData()
{
	//Serial.println(getDabbleInstance().getFunctionId());
	
}

void PinMonitorModule::sendDigitalData()
{
  digitaldata = new byte[n];
   for(int i=0;i<n;i++)
    {
	digitaldata[i]=0;
    }
     for(int j=0;j<n;j++)
   {
	for(int i=0;i<8;i++)
	{
		digitaldata[j] = digitaldata[j] + ((digitalRead(digital_pins[j][i]))<<i);
	}
   }
/*#ifdef DEBUG                     //Remove from final file
for(int j=0;j<n;j++)
{
	Serial.print(digitaldata[j],BIN);
	Serial.print("    ");
}
Serial.println();
#endif*/
  if(getDabbleInstance().getScreenId()==1 && getDabbleInstance().readModuleId()==3)
  {
   Dabble.sendModuleFrame(PINMONITOR_ID,0,function_id_d,1,new FunctionArg(n,digitaldata));
  }
delete [] digitaldata;
}

void PinMonitorModule::sendAnalogData()
{
 uint8_t counter=0;
 analog_data = new byte[m];
  for(int i=0;i<m;i++)
  {
	analog_data[i]=0;
  }
  for (int i = 0; i < m; i=i+2)
  {
	analog_data[i] = (analog_pin[counter]-30) << 4;
    analog_data[i] = analog_data[i] | ((analogRead(analog_pin[counter]) & 0x0F00) / 256); 
    analog_data[i+1]= (analogRead(analog_pin[counter]) & 0x00ff);
	counter++;  
  }
    /*#ifdef DEBUG                     //Remove from final file
	for(int j=0;j<m;j++)
	{
		Serial.print(analog_data[j],BIN);
		Serial.print("    ");
	}
	Serial.println();
	#endif*/
	//Dabble.sendModuleFrame(PINMONITOR_ID,0,function_id_a,1,new FunctionArg(m,analog_data));
  if(getDabbleInstance().getScreenId()==3 && getDabbleInstance().readModuleId()==3) 
  {
   Dabble.sendModuleFrame(PINMONITOR_ID,0,function_id_a,1,new FunctionArg(m,analog_data));
  }
delete [] analog_data;
}
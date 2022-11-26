#define FROM_DABBLE_LIBRARY
#include "DabbleESP32.h"
#include "ColorDetectorModule.h"
int checksettings=0;

float idealColorValue[5][3] ={  {255, 0, 0},         //RGB for red
							    {0, 255, 0},         //RGB for Green
								{0, 0, 255},         //RGB for Blue
								{255, 242, 0},       //RGB for Yellow
								{148, 0, 211},       //RGB for Violet
							 }; 

ColorDetectorModule::ColorDetectorModule():ModuleParent(COLORDETECTOR_ID),ColorPrediction()
{
}

void ColorDetectorModule::sendSettings(void(*function)(void))
{
	checksettingsCallBack=true;
	settingsCallBack = function;
}
void ColorDetectorModule::sendSettings(uint8_t Grid,uint8_t calcMode,uint8_t ColorScheme)
{
	checksettings =3;
	#ifdef DEBUG
	Serial.print("Settings Callback: ");
	Serial.println(checksettings);
	#endif
	if(Grid == 1)   //1x1
	{
		gridSize = 1;           
	}
	else if(Grid == 2)   //3x3
	{
		gridSize = 3;           
	}
	else if(Grid == 3)  //5x5
	{
		gridSize = 5;
	}
	if(calcMode == 1)   //Dominant
	{
		calculationMode = 1;
	}
	else if(calcMode == 2) // Average
	{
		calculationMode = 2;
	}
    if(ColorScheme == 6 )    //GRAYSCALE_1BIT
	{		
		colorScheme = 1 ;
	}
	else if(ColorScheme == 5 )    //GRAYSCALE_4BIT
	{		
		colorScheme = 4;
	}
	else if(ColorScheme == 4 )    //GRAYSCALE_8BIT
	{		
		colorScheme = 8 ;
	}
	else if(ColorScheme == 3 )    //Rgb_3bit
	{		
		colorScheme = 3 ;
	}
	else if(ColorScheme == 2 )    //Rgb_15bit
	{		
		colorScheme = 15 ;
	}
	else if(ColorScheme == 1)   //Rgb_24bit
	{
		colorScheme = 24;
	}
	
}
void ColorDetectorModule::processData()
{
	if(checksettings != 0)
	{ 
		if(checksettings == 3)
		{
			Dabble.sendModuleFrame(COLORDETECTOR_ID,0,COLOR_SCHEME,1,new FunctionArg(1,&colorScheme));
			checksettings = 2;
		}
		else if(checksettings == 2)
		{
			Dabble.sendModuleFrame(COLORDETECTOR_ID,0,GRID_SETTING,1,new FunctionArg(1,&gridSize));
			checksettings =1;
		}
		else if(checksettings == 1)
		{
			Dabble.sendModuleFrame(COLORDETECTOR_ID,0,COLOR_CALCULATION_TYPE,1,new FunctionArg(1,&calculationMode));
			checksettings =0;
		}
	}
	
	if(checksettingsCallBack == true)
	{
		checksettingsCallBack=false;
		(*settingsCallBack)();
	}
	
	byte functionID = getDabbleInstance().getFunctionId();
	 if(functionID == COLOR_DATA )
	{
		
		//Second arg line onwards color values are stored.
		if(currentArgnumber != getDabbleInstance().getArgumentNo())
		{
			if(currentArgnumber!=0)
			{
				for (int i = 0; i < currentArgnumber-1; i++)
                 {
                   delete [] colorArray[i];
                 }
              delete [] colorArray;
			}
			colorArray = new uint8_t*[getDabbleInstance().getArgumentNo()-1];
            for (int i = 0; i < getDabbleInstance().getArgumentNo()-1; i++)  //color values comes from second line
            {
              colorArray[i] = new uint8_t[3];
            }
        }
		currentArgnumber=getDabbleInstance().getArgumentNo();
		for (int i = 1; i < getDabbleInstance().getArgumentNo(); i++) //color values comes from second line
          {
            for (int j = 0; j < 3; j++)
               {
                 colorArray[i-1][j] = getDabbleInstance().getArgumentData(i)[j];
				 #ifdef DEBUG
				 Serial.print(colorArray[i-1][j]);
				 Serial.print(" ");
			     #endif
               }
			   #ifdef DEBUG
			   Serial.println();
			   #endif
		  }
	} 
}
void ColorDetectorModule::setColorScheme(byte bitScheme)
{
	Dabble.sendModuleFrame(COLORDETECTOR_ID,0,COLOR_SCHEME,1,new FunctionArg(1,&bitScheme));
}

void ColorDetectorModule::setGridSize(byte gridsize)
{
	Dabble.sendModuleFrame(COLORDETECTOR_ID,0,GRID_SETTING,1,new FunctionArg(1,&gridsize));
}

void ColorDetectorModule::setCalculationMode(byte calculationMode)
{
    Dabble.sendModuleFrame(COLORDETECTOR_ID,0,COLOR_CALCULATION_TYPE,1,new FunctionArg(1,&calculationMode));	
}

int ColorDetectorModule::getRedColor()
{
   if(currentArgnumber !=0 )
   {
		return colorArray[0][0];
   }
   else
   {
		return -1;
   }
}

int ColorDetectorModule::getGreenColor()
{
   if(currentArgnumber !=0 )
   {
		return colorArray[0][1];
   }
   else
   {
		return -1;
   }
}

int ColorDetectorModule::getBlueColor()
{
   if(currentArgnumber !=0 )
   {
		return colorArray[0][2];
   }
   else
   {
		return -1;
   }
}


int ColorDetectorModule::getRedColor(byte row, byte col)
{
	if(currentArgnumber!=0)
	{
		if(row == 0)
		{
			return colorArray[col][0];
		}
		else if(row == 1)
		{
			return colorArray[(currentArgnumber/gridSize)+col][0];
		}
		else if(row == 2)
		{
			return colorArray[((currentArgnumber/gridSize)*2)+col][0];
		}
		else if(row == 3)
		{
			return colorArray[((currentArgnumber/gridSize)*3)+col][0];
		}
		else if(row == 4)
		{
			return colorArray[((currentArgnumber/gridSize)*4)+col][0];
		}
	}
	else
	{
		return -1;
	}
}

int ColorDetectorModule::getGreenColor(byte row, byte col)
{
	if(currentArgnumber!=0)
	{
		if(row == 0)
		{
			return colorArray[col][1];
		}
		else if(row == 1)
		{
			return colorArray[(currentArgnumber/gridSize)+col][1];
		}
		else if(row == 2)
		{
			return colorArray[((currentArgnumber/gridSize)*2)+col][1];
		}
		else if(row == 3)
		{
			return colorArray[((currentArgnumber/gridSize)*3)+col][1];
		}
		else if(row == 4)
		{
			return colorArray[((currentArgnumber/gridSize)*4)+col][1];
		}
	}
	else
	{
		return -1;
	}
}
int ColorDetectorModule::getBlueColor(byte row, byte col)
{
	if(currentArgnumber!=0)
	{
		if(row == 0)
		{
			return colorArray[col][2];
		}
		else if(row == 1)
		{
			return colorArray[(currentArgnumber/gridSize)+col][2];
		}
		else if(row == 2)
		{
			return colorArray[((currentArgnumber/gridSize)*2)+col][2];
		}
		else if(row == 3)
		{
			return colorArray[((currentArgnumber/gridSize)*3)+col][2];
		}
		else if(row == 4)
		{
			return colorArray[((currentArgnumber/gridSize)*4)+col][2];
		}
	}
	else
	{
		return -1;
	}
}

uint8_t ColorDetectorModule::getGridSize()
{
	return getDabbleInstance().getArgumentData(0)[2];;
}

uint8_t ColorDetectorModule::getColorScheme()
{
	return getDabbleInstance().getArgumentData(0)[0];;
}

uint8_t ColorDetectorModule::getCalculationMode()
{
	return getDabbleInstance().getArgumentData(0)[1];;
}

int ColorDetectorModule::getColorValue(uint8_t colorName,uint8_t Row,uint8_t Col)
{
	if(gridSize == 3 && (Row < 3 && Col < 3))
	{
		if(colorName == 1)    //Red
		{
			return getRedColor(Row,Col);
		}
		else if(colorName == 2)    //Green
		{
			return getGreenColor(Row,Col);
		}
		else if(colorName == 3)    //Blue
		{
			return getBlueColor(Row,Col);
		}
		else if(colorName == 4)  //Black
		{
			if((colorScheme == GRAYSCALE_1BIT) || (colorScheme == GRAYSCALE_4BIT) || (colorScheme == GRAYSCALE_8BIT))
			return getRedColor(Row,Col);
		    else 
			return -1;
     	}
	}
	else if(gridSize == 5 && (Row<5 && Col<5))
	{
		if(colorName == 1)    //Red
		{
			return getRedColor(Row,Col);
		}
		else if(colorName == 2)    //Green
		{
			return getGreenColor(Row,Col);
		}
		else if(colorName == 3)    //Blue
		{
			return getBlueColor(Row,Col);
		}
		else if(colorName == 4)  //Black
		{
            if((colorScheme == GRAYSCALE_1BIT) || (colorScheme == GRAYSCALE_4BIT) || (colorScheme == GRAYSCALE_8BIT))
			return getRedColor(Row,Col);
		    else 
			return -1;
		}
	}
	else if(gridSize == 1 && (Row<1 && Col<1))
	{
		if(colorName == 1)    //Red
		{
			return getRedColor();
		}
		else if(colorName == 2)    //Green
		{
			return getGreenColor();
		}
		else if(colorName == 3)    //Blue
		{
			return getBlueColor();
		}
		else if(colorName == 4)  //Black
		{
			if((colorScheme == GRAYSCALE_1BIT) || (colorScheme == GRAYSCALE_4BIT) || (colorScheme == GRAYSCALE_8BIT))
			return getRedColor();
		    else 
			return -1;
        }
	}
	else
	{
		return -1;
	}
	
}

int ColorDetectorModule::getGrayScaleColor(byte row,byte col)
{
	if(currentArgnumber!=0 && (colorScheme == GRAYSCALE_1BIT || colorScheme == GRAYSCALE_4BIT || colorScheme == GRAYSCALE_8BIT))
	{
		if(row == 0)
		{
			return colorArray[col][0];
		}
		else if(row == 1)
		{
			return colorArray[(currentArgnumber/gridSize)+col][0];
		}
		else if(row == 2)
		{
			return colorArray[((currentArgnumber/gridSize)*2)+col][0];
		}
		else if(row == 3)
		{
			return colorArray[((currentArgnumber/gridSize)*3)+col][0];
		}
		else if(row == 4)
		{
			return colorArray[((currentArgnumber/gridSize)*4)+col][0];
		}
	}
	else
	{
		return -1;
	}
}

ColorPrediction::ColorPrediction(){
	min_deviation = 255;
}

bool ColorPrediction::checkColor(int *colorValue, uint8_t colorName)
{
	if((colorValue[0]==-1) || (colorValue[1]==-1) || (colorValue[2]==-1))
	{
		return 0;
	}
	else{
	min_deviation = 255;
	for(int i = 0;i<5;i++)
	{
		deviation = sqrt((sq(idealColorValue[i][0] - colorValue[0]) + sq(idealColorValue[i][1] - colorValue[1]) + sq(idealColorValue[i][2] - colorValue[2])) / 3);
		if(min_deviation>deviation)
		{
			min_deviation = deviation;
			colorFlag = i+1;
		}
	}
	if(colorFlag == colorName)
		return 1;
	else
		return 0;
	}
}

float ColorPrediction::getColorError(uint8_t *colorValue,uint8_t *referenceValue)
{
	if((colorValue[0]==-1) || (colorValue[1]==-1) || (colorValue[2]==-1))
	{
		return -1;
	}
	else
	{
	deviation = sqrt((sq(referenceValue[0] - colorValue[0]) + sq(referenceValue[1] - colorValue[1]) + sq(referenceValue[2] - colorValue[2])) / 3);
	return deviation;
	}
}

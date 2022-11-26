#ifndef ColorDetectorModule_H_
#define ColorDetectorModule_H_

#include "ModuleParent.h"

//Function Id
#define GRID_SETTING             0x01
#define COLOR_CALCULATION_TYPE   0x02    // for Dominant or average 
#define COLOR_SCHEME             0x03    // bit size of color
#define COLOR_DATA               0x04


//Literals
#define RGB_3BIT        3
#define RGB_15BIT       15
#define RGB_24BIT       24

#define GRAYSCALE_1BIT  1
#define GRAYSCALE_4BIT  4
#define GRAYSCALE_8BIT  8

//#define GRID_5x1                 0x04
#define GRID_3x3                 0x03
#define GRID_1x1                 0x01
#define GRID_5x5                 0x05

#define DOMINANT         0x01
#define AVERAGE           0x02

#define RED    0x01
#define GREEN  0x02
#define BLUE   0x03
#define YELLOW 0x04
#define VIOLET 0x05

class ColorPrediction
{
	public:
	ColorPrediction();
	bool checkColor(int *colorValue,uint8_t colorName);
	float  getColorError(uint8_t *colorValue,uint8_t *referenceValue);
	int colorFlag=0;
	float min_deviation = 255;
	float deviation = 0;
};

class ColorDetectorModule:public ModuleParent, public ColorPrediction
{
public:
ColorDetectorModule();
//Arduino
void setColorScheme(byte);
void setGridSize(byte);
void setCalculationMode(byte);
int getRedColor();
int getBlueColor();
int getGreenColor();
int getRedColor(byte,byte);   //for different blocks of grid
int getBlueColor(byte,byte);  //for different blocks of grid
int getGreenColor(byte,byte); //for different blocks of grid
uint8_t getGridSize();
uint8_t getColorScheme();
uint8_t getCalculationMode();
int getGrayScaleColor(byte,byte);
void sendSettings(void(*)(void));

//Pictoblox
void sendSettings(uint8_t,uint8_t,uint8_t);
int getColorValue(uint8_t,uint8_t,uint8_t);

private:
void processData();
void (*settingsCallBack)(void);
bool checksettingsCallBack;
uint8_t** colorArray;
uint8_t currentArgnumber=0;
uint8_t gridSize=0;
uint8_t colorScheme=0;
uint8_t calculationMode=0;
};
extern ColorDetectorModule ColorDetector;
#endif
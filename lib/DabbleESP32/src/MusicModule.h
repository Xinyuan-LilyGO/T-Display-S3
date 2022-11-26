#ifndef MusicModule_H_
#define MusicModule_H_
#include "ModuleParent.h"

#define ADDTOQUEUE	0x01
#define PLAYNOW     0x02
#define STOPMUSIC        0x03

class MusicModule:public ModuleParent
{
	public:
	MusicModule();
	void play(String);
	void addToQueue(String);
	void stop();
	void playMusic(uint8_t , String);
};
extern MusicModule Music;
#endif
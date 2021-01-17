#pragma once
#include "cHack.h"

/*struct sKeybind
{
	BYTE type = 0;    //1 byte
	BYTE vkCode;      //1 byte
	char pad[2];      //2 byte
	void (*func)();   //4 byte
};

struct sToggle
{
	BYTE type = 1;    //1 byte
	BYTE vkCode;      //1 bytes
	char pad[2];      //2 bytes
	bool* setting;    //4 bytes
	//char pad[1];
};*/    //THIS WAS THE WEIRD IDEA I HAD TO STORE THESE THINGS ALL IN ONE ARARY IGNORE IT 



struct sKeybind
{
	BYTE vkCode;
	SHORT bitMask;
	void (*func)();
};

struct sToggle
{
	BYTE vkCode;
	bool* settingPtr;
};

class cKeybinds
{
public:

	sKeybind keybindsTable[100];   //allocate an excess amount
	int keybindCount;

	sToggle togglesTable[100];   //allocate an excess amount
	int toggleCount;

	void checkKeybinds();

	void checkToggles();

	void loadDefaults();

	void replaceKeybind(sKeybind newBind);

	void replaceToggle(sToggle newToggle);
};

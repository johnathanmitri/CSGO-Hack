#pragma once
#include "cHack.h"

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

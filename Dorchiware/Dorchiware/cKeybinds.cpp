#include "pch.h"
#include "cKeybinds.h"
#include "cHack.h"
#include "MiscFunctions.h"

//extern cHack 
//extern cHack* hack;

//extern cKeybinds* keybinds;


void cKeybinds::loadDefaults()
{
	const sKeybind defaultKeybinds[] =     //1 for single click,  1<<15 for holding it down
	{
		{VK_UP, 1, MiscFunctions::increaseTriggerbotFov},
		{VK_DOWN, 1, MiscFunctions::decreaseTriggerbotFov},

		//{VK_END, 1, &changeBinding},
		//{VK_DELETE, 1, incrementSetting},
		//{VK_UP, 1, changeBinding2}
	};

	const sToggle defaultToggles[] =
	{
		{VK_KEY_N, &hack->settings.hookReady},
		{VK_KEY_Q, &hack->settings.aimbot},
		{VK_ADD, &hack->settings.teamChecking},
		{VK_KEY_O, &hack->settings.noRecoil},
		{VK_OEM_PLUS, &hack->settings.noViewPunch}, //equal sign
		{VK_KEY_L, &hack->settings.triggerbot},
		{VK_OEM_PERIOD, &hack->settings.tracers},
		{VK_KEY_U, &hack->settings.spinbot}

		//{VK_OEM_COMMA, &hack->settings.stacking},

	};

	int defaultKeybindCount = (sizeof(defaultKeybinds) / sizeof(sKeybind));
	int defaultToggleCount = (sizeof(defaultToggles) / sizeof(sToggle));


	memcpy(keybindsTable, defaultKeybinds, defaultKeybindCount * sizeof(sKeybind));
	keybindCount = defaultKeybindCount;

	memcpy(togglesTable, defaultToggles, defaultToggleCount * sizeof(sToggle));
	toggleCount = defaultToggleCount;
}


void cKeybinds::checkKeybinds()
{
	for (int i = 0; i < keybindCount; i++)
	{
		sKeybind currKeybind = keybindsTable[i];
		if (GetAsyncKeyState(currKeybind.vkCode) & currKeybind.bitMask)
		{
			(*currKeybind.func)();
		}
	}
}

void cKeybinds::checkToggles()
{
	for (int i = 0; i < toggleCount; i++)
	{
		sToggle currToggle = togglesTable[i];
		if (GetAsyncKeyState(currToggle.vkCode) & 1)
		{
			*currToggle.settingPtr = !*currToggle.settingPtr;
		}
	}
}

void cKeybinds::replaceKeybind(sKeybind newBind)
{
	bool replaced = false;
	for (int i = 0; i < keybindCount; i++)
	{
		if (keybindsTable[i].vkCode == newBind.vkCode)  //if the element we are looking at has the same button as the new one
		{
			keybindsTable[i] = newBind;                 //replace the element with the new one
			replaced = true;
		}
	}
	if (!replaced)
	{
		keybindsTable[keybindCount]; //sets the element thats at the end of the list, then increments count
		keybindCount++;
	}

}

void cKeybinds::replaceToggle(sToggle newToggle)
{
	bool replaced = false;
	for (int i = 0; i < toggleCount; i++)          //same as above
	{
		if (togglesTable[i].vkCode == newToggle.vkCode)
		{
			togglesTable[i] = newToggle;
			replaced = true;
		}
	}
	if (!replaced)
	{
		togglesTable[toggleCount]; //sets the element thats at the end of the list, then increments count
		toggleCount++;
	}
}
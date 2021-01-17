#pragma once
#include "pch.h"
class cHack
{
public:

    struct SettingsType
    {
        // bool windowedMode = true;

        bool aimbot = false;
        bool triggerbot = false;
        float halfTriggerbotFov = 10;
        bool tracers = true;
        bool teamChecking = true;
        bool noRecoil = false;
        //bool autoSnapping = true;
        bool noViewPunch = false;
        bool stacking = false;
        bool esp = false;
        bool testFlag = false;
        bool hookReady = false;
        bool spinbot = false;

        //int UIxPos = 0;
        //int UIyPos = 0;
    }settings;

	uintptr_t clientModuleBase;
	uintptr_t engineModuleBase;

	clientInfo* clientList;


    localPlayerEnt* localPlayer;
    playerEnt* targetPlayerEntity;
    int targetIndex = 1;

    Vector2AIM* aimAngles;

    Vector2AIM oldAimPunch = { 0, 0 };



	static void update();

	static playerEnt* getPlayerEntity(int index);

	//void __stdcall test();
};

extern cHack* hack;
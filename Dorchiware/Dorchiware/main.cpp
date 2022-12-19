#include "pch.h"
#include "main.h"
#include "dx.h"
#include "drawing.h"
#include "cHack.h"
#include "Calcs.h"
#include "cKeybinds.h"
#include "cHook.h"


#include <ctime>
#include <ratio>
#include <chrono>
#include <sstream>


int UIxPos = 0;
int UIyPos = 0;

struct FontsType
{
	CD3DFont* logo;
	CD3DFont* hackList;
	CD3DFont* esp;
	CD3DFont* name;

}fonts;

ID3DXLine* tracerLine;

bool hookWorking = false;   //this makes sure that we dont clear memory while the hook is still running.  -- maybe unecessary

cHack* hack;// = new cHack();
cKeybinds* keybinds;
Calcs* calcs;

int targetIndex = 2;

FILE* f = 0;
bool testFlag = false;

bool currentlyAbovePlayer = false;


playerEnt* stackingPlayer = nullptr;




extern LPDIRECT3DDEVICE9 pDevice = 0;

cHook* endSceneHook;

void* d3d9Device[119];



void displaySetting(int* index, const char* name, const char* value)
{
	int xPositionFirst = UIxPos + 0;
	int xPositionSecond = UIxPos + 210;
	int yPosition = UIyPos + 57 + (*index * 30);

	fastDrawText(name, xPositionFirst, yPosition, DT_LEFT, D3DCOLOR_ARGB(255, 255, 255, 255), fonts.hackList);
	fastDrawText(value, xPositionSecond, yPosition, DT_LEFT, D3DCOLOR_ARGB(255, 255, 255, 255), fonts.hackList);

	*index = *index + 1;
}
//endscene hook
void APIENTRY hkEndScene(LPDIRECT3DDEVICE9 o_pDevice)
{
	hookWorking = true;
	if (!pDevice)
		pDevice = o_pDevice;

	if (hack->settings.hookReady)
	{

		fonts.esp->BeginDrawing();

		if (hack->localPlayer != NULL)   //make sure that we are in game
		{
			if (hack->settings.tracers || hack->settings.esp) //if tracers are on, then draw them
			{
				playerEnt* currPlayer;
				for (int tempIndex = 1; tempIndex < 65; tempIndex++)
				{
					currPlayer = hack->getPlayerEntity(tempIndex);
					if (currPlayer != 0)
					{
						Vector2 entPosHead2D;
						Vector2 entPosHeadTop2D;
						Vector2 entPosBase2D;
						Vector3 targetPos3D = currPlayer->getBonePosition(8);
						bool inFront = calcs->WorldToScreen(targetPos3D, entPosHead2D);
						targetPos3D.z += 7;

						calcs->WorldToScreen(currPlayer->getOrigin(), entPosBase2D);

						float distance = calcs->getDistance(hack->localPlayer->getOrigin(), currPlayer->getOrigin());    //CALCULATE THE COLOR THAT WE WANT THEM TO APPEAR, TO SIGNIFY DISTANCE
						//150, 1500
						D3DCOLOR color;
						int min = 200;
						int max = 2248;
						if (distance <= min)
						{
							color = D3DCOLOR_ARGB(255, 255, 0, 0);
						}
						else if (distance >= max)
						{
							color = D3DCOLOR_ARGB(255, 0, 255, 0);
						}
						else
						{
							float a = distance - min;
							float greenness = a / (max - min);   //1350 is the difference between closest and farthest. When a is at 1350, greenness will be 1, meaning max green
							float redness = 1 - greenness;
							color = D3DCOLOR_ARGB(255, (BYTE)(redness * 255), (BYTE)(greenness * 255), 0);
						}


						if (inFront)   //if WorldToScreen returns true, meaning they are on the screen.
						{
							DrawEspBox3D(currPlayer, 3, color, tracerLine);
							std::string test = std::to_string(currPlayer->getHealth()) + "HP";
							fastDrawText(test.c_str(), entPosBase2D.x, entPosBase2D.y + 5, DT_CENTER, D3DCOLOR_ARGB(255, 255, 255, 255), fonts.esp);

						}
						else  //If its false, they are offscreen, and more work has to be done before tracres may be drawn
						{
							entPosHead2D.x = windowWidth - entPosHead2D.x;  //reverses them
							entPosHead2D.y = windowHeight - entPosHead2D.y; //


							float xDist = abs((halfWindowWidth)-entPosHead2D.x);
							float yDist = abs((halfWindowHeight)-entPosHead2D.y);

							if (xDist < halfWindowWidth && yDist < halfWindowHeight) //This check could be placed lower, and use the x and y ratios that are already calculated, but this is for performance reasons
							{
								//float slope = xDist / yDist;    //rise over run

								float xDistWindowWidthRatio = halfWindowWidth / xDist;
								float yDistWindowHeightRatio = halfWindowHeight / yDist;
								if (xDistWindowWidthRatio < yDistWindowHeightRatio)      //x is proportionally larger, because  1280 / 300 would be bigger, meaning the denominator is smaller
								{
									//now we need to set the xDist to halfWindowWidth, which really means making entPosHead2D.x 
									//(halfWindowWidth) = 500 + entPosHead2D.x

									entPosHead2D.x = halfWindowWidth - (halfWindowWidth - entPosHead2D.x) * xDistWindowWidthRatio;
									entPosHead2D.y = halfWindowHeight - ((halfWindowHeight - entPosHead2D.y) * xDistWindowWidthRatio);
								}
								else
								{
									entPosHead2D.x = halfWindowWidth - (halfWindowWidth - entPosHead2D.x) * yDistWindowHeightRatio;
									entPosHead2D.y = halfWindowHeight - ((halfWindowHeight - entPosHead2D.y) * yDistWindowHeightRatio);

								}
							}
						}

						//color = D3DCOLOR_ARGB(255, 255, 0, 0);
						DrawLine(entPosHead2D.x, entPosHead2D.y, halfWindowWidth, halfWindowHeight, 2, color, tracerLine);

					}
				}
			}

			// crosshair
			//*aimAngles = *aimAngles + oldAimPunch - (localPlayer->getAimPunchAngle() * 2);
			int xPos = halfWindowWidth - (int)((windowWidth / 90) * hack->localPlayer->getAimPunchAngle().x) - 2;
			int yPos = halfWindowHeight + (int)((windowHeight / 90) * hack->localPlayer->getAimPunchAngle().y) - 2;


		}

		fonts.esp->EndDrawing();

		int i = 0;

		fonts.logo->BeginDrawing();
		fastDrawText("Dorchiware", UIxPos, UIyPos + 20, 0, D3DCOLOR_ARGB(255, 255, 255, 255), fonts.logo);
		fonts.logo->EndDrawing();

		fonts.name->BeginDrawing();
		fastDrawText("Written by Johnathan", UIxPos, UIyPos + 40, 0, D3DCOLOR_ARGB(255, 255, 255, 255), fonts.name);
		fonts.name->EndDrawing();

		fonts.hackList->BeginDrawing();

		displaySetting(&i, "Aimbot (Q): ", hack->settings.aimbot ? "on" : "off");

		displaySetting(&i, "No view punch (=): ", hack->settings.noViewPunch ? "on" : "off");

		displaySetting(&i, "Triggerbot (L): ", hack->settings.triggerbot ? "on" : "off");

		displaySetting(&i, "Spinbot (U): ", hack->settings.spinbot ? "on" : "off");

		std::string str = std::to_string(hack->settings.halfTriggerbotFov * 2);
		str.erase(str.find_last_not_of('0') + 1, std::string::npos);
		str.erase(str.find_last_not_of('.') + 1, std::string::npos);

		displaySetting(&i, "TBot FOV (Up/Down): ", str.c_str());

		displaySetting(&i, "Tracers (.): ", hack->settings.tracers ? "on" : "off");

		//displaySetting(&i, "Stacking: ", settings.stacking ? "on" : "off");

		//displaySetting(&i, "On Someone: ", currentlyAbovePlayer ? "yes" : "no");

		displaySetting(&i, "test flag: ", testFlag ? "on" : "off");


		int leftoverMillis = GetTickCount64() % 100;
		if (leftoverMillis < 50)
			displaySetting(&i, "moving: ", "left");
		else
			displaySetting(&i, "moving: ", "right");

		fonts.hackList->EndDrawing();


	}
	hookWorking = false;

	((tEndScene)endSceneHook->jumpBackGateway)(pDevice);

}

playerEnt* findPlayerWereStandingOn()
{
	playerEnt* playerWereStandingOn = nullptr;
	Vector3 playerPos;
	int i = 1;
	for (; i < 65; i++)
	{
		playerWereStandingOn = hack->getPlayerEntity(i);
		if (playerWereStandingOn == nullptr)
			continue;
		playerPos = playerWereStandingOn->getOrigin();
		Vector3 playerHeadPos = playerWereStandingOn->getBonePosition(8);
		Vector3 localPos = hack->localPlayer->getOrigin();
		float xDist = abs(localPos.x - playerPos.x);
		float yDist = abs(localPos.y - playerPos.y);
		if ((localPos.z - playerHeadPos.z) > 0 && xDist < 45 && yDist < 45)            //if we are high enough above the other guy
		{
			break;
		}
	}
	return playerWereStandingOn;
}

DWORD WINAPI MainLoop()
{
	hack->localPlayer = localPlayerEnt::get();
	keybinds->checkToggles();

	if (hack->localPlayer != NULL && GetActiveWindow() == GetFocus())   //make sure that we are in game
	{
		hack->update();
		keybinds->checkKeybinds();
	}
	else
	{
		Sleep(20);
	}
	return 1;
}

DWORD WINAPI HackThread()
{//do one time setup here

	hack = new cHack();
	keybinds = new cKeybinds();
	keybinds->loadDefaults();
	calcs = new Calcs();

	hack->clientModuleBase = (uintptr_t)GetModuleHandle("client.dll");

	if (GetD3D9Device(d3d9Device, sizeof(d3d9Device)))
	{
		endSceneHook = new cHook((char*)d3d9Device[42], (char*)hkEndScene, 7);
		endSceneHook->applyHook();
	}

	Sleep(1000);

	fonts.logo = new CD3DFont("Verdana", 14, D3DFONT_BOLD);
	fonts.hackList = new CD3DFont("Verdana", 11, 0);
	fonts.esp = new CD3DFont("Verdana", 8, 0);
	fonts.name = new CD3DFont("Verdana", 7, 0);

	UIxPos = windowWidth - 400;
	UIyPos = windowHeight - 310;

	if (fonts.hackList->InitializeDeviceObjects(pDevice) == S_OK)
		fonts.hackList->RestoreDeviceObjects();

	if (fonts.logo->InitializeDeviceObjects(pDevice) == S_OK)
		fonts.logo->RestoreDeviceObjects();

	if (fonts.esp->InitializeDeviceObjects(pDevice) == S_OK)
		fonts.esp->RestoreDeviceObjects();

	if (fonts.name->InitializeDeviceObjects(pDevice) == S_OK)
		fonts.name->RestoreDeviceObjects();

	D3DXCreateLine(pDevice, &tracerLine);
	hack->settings.hookReady = true;

	while (!(GetAsyncKeyState(VK_END) & 1))  //unload DLL when user presses End Key
	{
		MainLoop();
	}

	cHook::unHookAll();

	return 0;
}
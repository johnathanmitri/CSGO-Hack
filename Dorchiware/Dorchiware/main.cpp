#include "pch.h"
#include "main.h"
#include "hook.h"
#include "dx.h"
#include "drawing.h"
#include "cHack.h"
#include "Calcs.h"
#include "cKeybinds.h"


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
	//ID3DXFont* logo;
	//ID3DXFont* logo;

}fonts;


//keybinds = new cKeybinds();

ID3DXLine* tracerLine;

//CD3DFont* aa;

bool hookWorking = false;   //this makes sure that we dont clear memory while the hook is still running.  -- maybe unecessary

cHack* hack;// = new cHack();
cKeybinds* keybinds;
Calcs* calcs;

int targetIndex = 2;

FILE* f = 0;
bool testFlag = false;

bool currentlyAbovePlayer = false;


//uintptr_t clientModuleBase;
//uintptr_t engineModuleBase;




playerEnt* stackingPlayer = nullptr;




extern LPDIRECT3DDEVICE9 pDevice = 0;


void* d3d9Device[119];
BYTE EndSceneBytes[7]{ 0 };
tEndScene oEndScene = nullptr;


void displaySetting(int* index, const char* name, const char* value)
{
	int xPositionFirst = UIxPos + 0;
	int xPositionSecond = UIxPos + 210;
	int yPosition = UIyPos + 50 + (*index * 30);

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
						//calcs->WorldToScreen(targetPos3D, entPosHeadTop2D);               //SLIHGTLY ABOVE HEAD (+=7)
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

							//slowDrawText("lol", entPosBase2D.x, entPosBase2D.y, DT_LEFT, color, "Arial", 12);
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

			//DrawFilledRect(xPos, yPos, 4, 4, D3DCOLOR_ARGB(255, 176, 5, 255));
		}

		fonts.esp->EndDrawing();

		//DrawFilledRect(settings.UIxPos, settings.UIyPos, 200, 200, D3DCOLOR_ARGB(50, 1, 1, 1));
		//DrawText("Dorchiware 1.0", settings.UIxPos, settings.UIyPos, DT_LEFT, D3DCOLOR_ARGB(255, 255, 255, 255), fonts.logo);

	  //  std::string settingName   
		int i = 0;

		//DrawFilledBox(UIxPos, UIyPos, 200, 200, D3DCOLOR_ARGB(50, 50, 50, 50));

		fonts.logo->BeginDrawing();

		fastDrawText("Dorchiware 2.0", UIxPos, UIyPos + 20, 0, D3DCOLOR_ARGB(255, 255, 255, 255), fonts.logo);

		fonts.logo->EndDrawing();

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
		/*if (localPlayer != nullptr)
		{
			Vector3 viewPunchAngle = *(Vector3*)(localPlayer + hazedumper::netvars::m_viewPunchAngle);
			displaySetting(&i, "VP Angle: ", std::to_string(viewPunchAngle.x).c_str());

			displaySetting(&i, "VP Angle: ", std::to_string(viewPunchAngle.y).c_str());
		}*/
		// call og function

	}
	hookWorking = false;
	oEndScene(pDevice);

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

DWORD WINAPI MainLoop() //RETURN 0 TO UNLOAD HACK DLL
{
	hack->localPlayer = localPlayerEnt::get();
	keybinds->checkToggles();

	if (hack->localPlayer != NULL)   //make sure that we are in game
	{

		hack->update();

		keybinds->checkKeybinds();

		//keybinds.update or something


		/*if (GetAsyncKeyState(VK_KEY_N) & 1)
		{
			//testFlag = !testFlag;
			hookReady = !hookReady;
		}*/
		/*if (GetAsyncKeyState(VK_KEY_Q) & 1)  //key q
		{
			hack->settings.aimbot = !hack->settings.aimbot;
		}

		if (GetAsyncKeyState(VK_KEY_F) & 1)  //key f
		{
			//autoSnapping = true;
			targetIndex++;
			if (targetIndex > 65)
			{
				targetIndex = 1;
			}
		}
		if (GetAsyncKeyState(VK_ADD) & 1)  //numpad plus
		{
			hack->settings.isTeamChecking = !hack->settings.isTeamChecking;

			//std::cout << "Checking team: " << std::boolalpha << settings.isTeamChecking << std::endl;
		}*/


		/*if (GetAsyncKeyState(VK_KEY_O) & 1) //key O
		{
			settings.noRecoil = !settings.noRecoil;
			//std::cout << "No recoil: " << std::boolalpha << settings.noRecoil;
		}
		if (GetAsyncKeyState(VK_OEM_PLUS) & 1) //keyg
		{
			settings.noViewPunch = !settings.noViewPunch;
			//std::cout << "No recoil: " << std::boolalpha << settings.noRecoil;
		}
		if (GetAsyncKeyState(VK_KEY_L) & 1) //keyL
		{
			settings.triggerbot = !settings.triggerbot;
		}

		if (GetAsyncKeyState(VK_OEM_PERIOD) & 1)
		{
			settings.tracers = !settings.tracers;
		}
		if (GetAsyncKeyState(VK_OEM_COMMA) & 1)
		{
			settings.stacking = !settings.stacking;
		}

		if (GetAsyncKeyState(VK_UP) & 1)
		{
			if (settings.halfTriggerbotFov != 20)
				settings.halfTriggerbotFov++;
		}
		if (GetAsyncKeyState(VK_DOWN) & 1)
		{
			if (settings.halfTriggerbotFov != 1)
				settings.halfTriggerbotFov--; //decrease by 1
		}*/

		/*if (GetAsyncKeyState(VK_KEY_O) & 1) //Pressing o, snap to nearest playerdw
		{
			//autoSnapping = false;

			float lowestDistance = 999999;
			int lowestDistIndex = 0;
			playerEnt* tempPlayerEntity;
			for (int tempIndex = 1; tempIndex < 65; tempIndex++)
			{
				tempPlayerEntity = getPlayerEntity(tempIndex);
				if (tempPlayerEntity != 0)
				{
					float distToTempPlayer = getDistance(localPlayer->getOrigin(), tempPlayerEntity->getOrigin());

					if (distToTempPlayer < lowestDistance)
					{
						lowestDistance = distToTempPlayer;
						lowestDistIndex = tempIndex;
					}
				}
			}
			if (lowestDistIndex != 0)
			{
				targetIndex = lowestDistIndex;
				targetPlayerEntity = getPlayerEntity(lowestDistIndex);
			}

		}*/




		/*
		if (settings.noRecoil && !settings.aimbot)
		{
			if (localPlayer->getShotsFired() > 1)
				*aimAngles = *aimAngles + oldAimPunch - (localPlayer->getAimPunchAngle() * 2);
			oldAimPunch = localPlayer->getAimPunchAngle() * 2;
		}*/



		/*if (settings.stacking)
		{
			int i = 1;

			if (!currentlyAbovePlayer)
			{
				stackingPlayer = findPlayerWereStandingOn();
				if (stackingPlayer != nullptr)                //if we actually are ontop of someone, change it to true
					currentlyAbovePlayer = true;
			}

			if (currentlyAbovePlayer)    //If were still on someone, or if we just found someone in the if-statement above
			{
				Vector3 playerPos = stackingPlayer->getOrigin();
				Vector3 playerHeadPos = stackingPlayer->getBonePosition(8);
				Vector3 localPos = localPlayer->getOrigin();
				float xDist = localPos.x - playerPos.x;
				float yDist = localPos.y - playerPos.y;
				if (((localPos.z - playerHeadPos.z) > 0 && abs(xDist) < 45 && abs(yDist) < 45))  //if we are still standing on this person
				{

					Vector2 currAngle = *aimAngles;// = stackingPlayer->getViewAngles();
					//east = 0deg = positive x
					//north = 90deg = positive y

					//0-90 = +x, +y
					//90-180 = -x, + y
					//-90 - 0 = +x, -y
					//-180 - -90 = -x, -y

					int eastDir = 1;
					int northDir = 1;

					if (abs(currAngle.x) < 90)
						eastDir = -eastDir;
					if (currAngle.x < 0)
						northDir = -northDir;

					if (xDist > 10) //xDist > 10 means that we are currently 10 units greater than the other player. We must travel east
					{
						//nothing must be done to eastdir
					}
					else if (xDist < -10)
					{
						eastDir = -eastDir;
					}

					if (yDist > 10)
					{
						//nothing must be done
					}
					else if (yDist < -10)
					{
						northDir = -northDir;
					}

					*(BYTE*)(clientModuleBase + hazedumper::signatures::dwForceForward) &= ~(1 << 0); //reset all the movement really quickly while we set the new ones
					*(BYTE*)(clientModuleBase + hazedumper::signatures::dwForceBackward) &= ~(1 << 0);
					*(BYTE*)(clientModuleBase + hazedumper::signatures::dwForceLeft) &= ~(1 << 0);
					*(BYTE*)(clientModuleBase + hazedumper::signatures::dwForceRight) &= ~(1 << 0);

					if (eastDir > 0)
						*(BYTE*)(clientModuleBase + hazedumper::signatures::dwForceRight) |= (1 << 0);
					else
						*(BYTE*)(clientModuleBase + hazedumper::signatures::dwForceLeft) |= (1 << 0);

					if (northDir > 0)
						*(BYTE*)(clientModuleBase + hazedumper::signatures::dwForceForward) |= (1 << 0);
					else
						*(BYTE*)(clientModuleBase + hazedumper::signatures::dwForceBackward) |= (1 << 0);
				}
				else   //if we aren't on them anymore
				{
					currentlyAbovePlayer = false;
				}
			}
		}*/




	}
	else
	{
		Sleep(20);
	}
	/*catch (std::exception e)
	{
		std::cout << "\n\n    EXCEPTION CAUGHT: " << "\n";
		system("PAUSE");
		return 0;
	}*/
	return 1;
}

DWORD WINAPI HackThread()
{//do one time setup here

	hack = new cHack();
	keybinds = new cKeybinds();
	keybinds->loadDefaults();
	calcs = new Calcs();

	hack->clientModuleBase = (uintptr_t)GetModuleHandle("client.dll");

	if (GetD3D9Device(d3d9Device, sizeof(d3d9Device))) {
		memcpy(EndSceneBytes, (char*)d3d9Device[42], 7);

		oEndScene = (tEndScene)TrampHook((char*)d3d9Device[42], (char*)hkEndScene, 7);
	}


	bool testConsole = false;

	if (testConsole)
	{
		AllocConsole();
		freopen_s(&f, "CONOUT$", "w", stdout);
		std::cout << "Injected\n\n";
	}


	Sleep(1000);
	//D3DXCreateFont(pDevice, 24, 0, FW_NORMAL, 1, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Verdana", &fonts.logo);
	//D3DXCreateFont(pDevice, 18, 0, FW_NORMAL, 1, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Verdana", &fonts.hackList);

	fonts.logo = new CD3DFont("Verdana", 14, D3DFONT_BOLD);
	fonts.hackList = new CD3DFont("Verdana", 11, 0);
	fonts.esp = new CD3DFont("Verdana", 8, 0);
	
	UIxPos = windowWidth - 400;
	UIyPos = windowHeight - 300;

	//aa = new CD3DFont("Arial", 11, D3DFONT_ZENABLE);

	if (fonts.hackList->InitializeDeviceObjects(pDevice) == S_OK)
		fonts.hackList->RestoreDeviceObjects();

	if (fonts.logo->InitializeDeviceObjects(pDevice) == S_OK)
		fonts.logo->RestoreDeviceObjects();

	if (fonts.esp->InitializeDeviceObjects(pDevice) == S_OK)
		fonts.esp->RestoreDeviceObjects();

	D3DXCreateLine(pDevice, &tracerLine);
	hack->settings.hookReady = true;

	while (!(GetAsyncKeyState(VK_END) & 1))
	{
		/*if ()
		auto t1 = std::chrono::high_resolution_clock::now();
		stop = MainLoop();
		auto t2 = std::chrono::high_resolution_clock::now();

		auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();

		std::cout << duration;

		continue;*/
		MainLoop();
	}

	//while (hookWorking) {}    //this just waits until the hook isn't running so that we can patch it without breaking anything

	Patch((BYTE*)d3d9Device[42], EndSceneBytes, 7);   //unhook EndScene()

	if (testConsole)
	{
		fclose(f);
		FreeConsole();
	}

	return 0;
}
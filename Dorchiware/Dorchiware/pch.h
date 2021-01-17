// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

// add headers that you want to pre-compile here
#include "framework.h"

#include <d3d9.h>
#include <d3dx9.h>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#include <vector>
#include <iostream>

#include <TlHelp32.h>


#include "csgo.hpp"
#define _USE_MATH_DEFINES
#include <math.h>
#include <cstdint>

#include "keycodes.h"
#include <string>

#include "D3DFont.h"

float toRad(float dec);

struct Vector2AIM
{
	float y, x; //REVERSED FOR CSGO ONLY

	Vector2AIM operator+ (Vector2AIM a)
	{
		return { y + a.y, x + a.x };
	}
	Vector2AIM operator- (Vector2AIM a)
	{
		return { y - a.y, x - a.x };
	}
	Vector2AIM operator* (float a)
	{
		return { y * a, x * a };
	}
	Vector2AIM operator/ (float a)
	{
		return { y / a, x / a };
	}

	/*Vector2()
	{

	}*/

	void Normalize()
	{
		if (y < -89)
			y = -89;
		else if (y > 89)
			y = 89;
		while (x < -180)
			x += 360;
		while (x > 180)
			x -= 360;
		if (isnan(x) || isnan(y))
		{
			x = 0;
			y = 0;
		}
	}
};

struct Vector2
{
	float x, y; //REVERSED FOR CSGO ONLY

	Vector2AIM operator+ (Vector2AIM a)
	{
		return { y + a.y, x + a.x };
	}
	Vector2AIM operator- (Vector2AIM a)
	{
		return { y - a.y, x - a.x };
	}
	Vector2AIM operator* (float a)
	{
		return { y * a, x * a };
	}
	Vector2AIM operator/ (float a)
	{
		return { y / a, x / a };
	}

	/*Vector2()
	{

	}*/

	void Normalize()
	{
		if (y < -89)
			y = -89;
		else if (y > 89)
			y = 89;
		while (x < -180)
			x += 360;
		while (x > 180)
			x -= 360;
		if (isnan(x) || isnan(y))
		{
			x = 0;
			y = 0;
		}
	}
};

struct Vector3
{
	float x, y, z;
	Vector3 operator+ (Vector3 a)
	{
		return { x + a.x, y + a.y, z + a.z };
	}
	Vector3 operator- (Vector3 a)
	{
		return { x - a.x, y - a.y, z - a.z };
	}
	Vector3 operator* (float a)
	{
		return { x * a, y * a, z * a };
	}
};

struct Vector4
{
	float x, y, z, w;
	Vector4 operator+ (Vector4 a)
	{
		return { x + a.x, y + a.y, z + a.z, w + a.w };
	}
	Vector4 operator- (Vector4 a)
	{
		return { x - a.x, y - a.y, z - a.z, w - a.w };
	}
	Vector4 operator* (float a)
	{
		return { x * a, y * a, z * a, w * a };
	}
};


class clientInfo
{
public:
	class ent* entptr; //0x0000
	int32_t N000006CD; //0x0004
	class clientInfo* blink; //0x0008
	class clientInfo* flink; //0x000C
}; //Size: 0x0010
static_assert(sizeof(clientInfo) == 0x10, "Client Info Broke");


class entList
{
public:
	class clientInfo entityArray[11]; //0x0000
};


class playerEnt
{
public:
	int getHealth()
	{
		return *(int*)(this + hazedumper::netvars::m_iHealth);
	}
	int getTeam()
	{
		return *(int*)(this + hazedumper::netvars::m_iTeamNum);
	}
	bool getbDormant()
	{
		return *(bool*)(this + hazedumper::signatures::m_bDormant);
	}
	int getShotsFired()
	{
		return *(int*)(this + hazedumper::netvars::m_iShotsFired);
	}
	Vector2AIM getAimPunchAngle()
	{
		return *(Vector2AIM*)(this + hazedumper::netvars::m_aimPunchAngle);
	}
	Vector3 getOrigin()
	{
		return *(Vector3*)(this + hazedumper::netvars::m_vecOrigin);
	}
	Vector2AIM getViewAngles()
	{
		return *(Vector2AIM*)(this + hazedumper::netvars::m_angEyeAnglesX); //says x, actually is the address of the y, followed by the x.
		//return *(Vector2AIM*)(this + 0x3B8);  //hardcoded offset, bad
	}
	Vector3 getBonePosition(int boneId)
	{
		Vector3 bonePos;
		uintptr_t boneMatrixBase = *(uintptr_t*)(this + hazedumper::netvars::m_dwBoneMatrix);
		bonePos.x = *(float*)(boneMatrixBase + 0x30 * boneId + 0x0C);
		bonePos.y = *(float*)(boneMatrixBase + 0x30 * boneId + 0x1C);
		bonePos.z = *(float*)(boneMatrixBase + 0x30 * boneId + 0x2C);
		return bonePos;
	}
	Vector3 getViewPosition()
	{
		Vector3 viewOffset = *(Vector3*)(this + hazedumper::netvars::m_vecViewOffset);
		Vector3 viewPos = getOrigin() + viewOffset;
		return viewPos;
	}
	//0x26A8
	Vector3 getVelocity()
	{
		return *(Vector3*)(this + hazedumper::netvars::m_vecVelocity);
	}

};

class localPlayerEnt : public playerEnt
{

public:
	static localPlayerEnt* get()
	{
		uintptr_t clientModuleBase = (uintptr_t)GetModuleHandle("client.dll");
		uintptr_t localPlayerAddr = *(uintptr_t*)(clientModuleBase + hazedumper::signatures::dwLocalPlayer);

		localPlayerEnt* localPlayer = (localPlayerEnt*)localPlayerAddr;
		return localPlayer;
	}


};

#endif //PCH_H

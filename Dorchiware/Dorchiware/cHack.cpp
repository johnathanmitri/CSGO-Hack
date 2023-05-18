#include "pch.h"
#include "main.h"
#include "Calcs.h"
#include "cKeybinds.h"
#include "cHack.h"


//Vector2 cHack::oldAimPunch = 
//int cHack::targetIndex = 1;

#define square(x) (x)*(x)

void cHack::update()
{
    
    hack->engineModuleBase = (uintptr_t)GetModuleHandle("engine.dll");
    hack->clientList = (clientInfo*)(hack->clientModuleBase + hazedumper::signatures::dwEntityList - 0x10);
    hack->aimAngles = (Vector2AIM*)(*(uintptr_t*)(hack->engineModuleBase + hazedumper::signatures::dwClientState) + hazedumper::signatures::dwClientState_ViewAngles);
    //uintptr_t localPlayerPtr = *(uintptr_t*)(hack->clientModuleBase + hazedumper::signatures::dwLocalPlayer);

    memcpy(&calcs->viewMatrix, (PBYTE*)(hack->clientModuleBase + hazedumper::signatures::dwViewMatrix), sizeof(calcs->viewMatrix));
    
    if (hack->settings.noViewPunch)
    {
        Vector3* viewPunchAngle = (Vector3*)(hack->localPlayer + hazedumper::netvars::m_viewPunchAngle);

        viewPunchAngle->x = 0;
        viewPunchAngle->y = 0;
    }

    if (GetAsyncKeyState(VK_SPACE) & (1 << 15)) //check most significant bit to see if space is down
    {
        // BITFIELD HERE
        BYTE FORCE_BITMASK = 0b0000'0001;
        BYTE FL_ONGROUND = 0b0000'0001;

        BYTE flags = *(BYTE*)(hack->localPlayer + hazedumper::netvars::m_fFlags);
        if (flags & FL_ONGROUND)
        {
            *(BYTE*)(hack->clientModuleBase + hazedumper::signatures::dwForceJump) |= FORCE_BITMASK; //sets the bit
            Sleep(30);
            *(BYTE*)(hack->clientModuleBase + hazedumper::signatures::dwForceJump) &= ~FORCE_BITMASK;
        }
        else if (hack->settings.testFlag)
        {
            *(BYTE*)(hack->clientModuleBase + hazedumper::signatures::dwForceForward) &= ~FORCE_BITMASK;

            int leftoverMillis = GetTickCount64() % 100;
            if (leftoverMillis < 50)   //Left
            {
                *(BYTE*)(hack->clientModuleBase + hazedumper::signatures::dwForceRight) &= ~FORCE_BITMASK;  //UNSET RIGHT

                *(BYTE*)(hack->clientModuleBase + hazedumper::signatures::dwForceLeft) |= FORCE_BITMASK; //SET LEFT

                Vector2AIM newAngle = *hack->aimAngles;
                newAngle.x += 0.0015;
                newAngle.Normalize();
                *hack->aimAngles = newAngle;
            }
            else  //Right
            {
                *(BYTE*)(hack->clientModuleBase + hazedumper::signatures::dwForceLeft) &= ~FORCE_BITMASK;  //UNSET RIGHT

                *(BYTE*)(hack->clientModuleBase + hazedumper::signatures::dwForceRight) |= FORCE_BITMASK; //SET LEFT

                Vector2AIM newAngle = *hack->aimAngles;
                newAngle.x -= 0.0015;
                newAngle.Normalize();
                *hack->aimAngles = newAngle;
            }


        }

    }

    if (hack->settings.aimbot)
    {
        hack->targetPlayerEntity = getPlayerEntity(hack->targetIndex);
        if (hack->targetPlayerEntity == 0) //if it failed the test
        {
            hack->targetIndex++;
            if (hack->targetIndex > 65)
            {
                hack->targetIndex = 1;
            }
            goto breakIf;
        }

        Vector2AIM targetAngle = calcs->calcAngle(hack->localPlayer->getViewPosition(), hack->targetPlayerEntity->getBonePosition(8));

        if (!isnan(targetAngle.x) && !isnan(targetAngle.y))
        {
            targetAngle = targetAngle - hack->localPlayer->getAimPunchAngle() * 2;
            targetAngle.Normalize();
            *hack->aimAngles = targetAngle;
        }

    }

    if (hack->settings.spinbot)
    {
        Vector2AIM newAim = *hack->aimAngles;
        newAim.x += 5;
        newAim.Normalize();

        *hack->aimAngles = newAim;

    }


    if (hack->settings.triggerbot && GetAsyncKeyState(VK_LBUTTON))
    {

        Vector2AIM closestPlayerAngle;
        float lowestDistance = 100;
        for (int tempIndex = 1; tempIndex < 65; tempIndex++)
        {
            playerEnt* tempPlayerEntity = getPlayerEntity(tempIndex);
            if (tempPlayerEntity != 0)
            {
                Vector2AIM angle = calcs->calcAngle(hack->localPlayer->getViewPosition(), tempPlayerEntity->getBonePosition(8));
                if (isnan(angle.x) || isnan(angle.y))
                {
                    continue;
                }
                angle = angle - hack->localPlayer->getAimPunchAngle() * 2;
                float distFromCrosshair = sqrt(square(angle.x - hack->aimAngles->x) + square(angle.y - hack->aimAngles->y)); //{ abs(angle.x - hack->aimAngles->x), abs(angle.y - hack->aimAngles->y)};
                if (distFromCrosshair < hack->settings.halfTriggerbotFov) //if hes within the fov
                {
                    if (distFromCrosshair < lowestDistance)
                    {
                        lowestDistance = distFromCrosshair;
                        closestPlayerAngle = angle;
                    }
                    
                }
            }
        }
        if (lowestDistance != 100)   //after looping, if the lowest value has changed, then we found someone
        {
            closestPlayerAngle.Normalize();
            *hack->aimAngles = closestPlayerAngle;

        }


    }

    if (GetAsyncKeyState(VK_KEY_C)) //key c being Held Down
    {
        if (abs(hack->localPlayer->getVelocity().z) < 7)
        {
            Vector2AIM closestPlayerAngle;
            float lowestDistance = 100;
            for (int tempIndex = 1; tempIndex < 65; tempIndex++)
            {
                playerEnt* tempPlayerEntity = getPlayerEntity(tempIndex);
                if (tempPlayerEntity != 0)
                {
                    Vector2AIM angle = calcs->calcAngle(hack->localPlayer->getViewPosition(), tempPlayerEntity->getBonePosition(8));
                    if (isnan(angle.x) || isnan(angle.y))
                    {
                        continue;
                    }
                    angle = angle - hack->localPlayer->getAimPunchAngle() * 2;
                    float distFromCrosshair = sqrt(square(angle.x - hack->aimAngles->x) + square(angle.y - hack->aimAngles->y)); //{ abs(angle.x - hack->aimAngles->x), abs(angle.y - hack->aimAngles->y)};
                    if (distFromCrosshair < hack->settings.halfTriggerbotFov) //if hes within the fov
                    {
                        if (distFromCrosshair < lowestDistance)
                        {
                            lowestDistance = distFromCrosshair;
                            closestPlayerAngle = angle;
                        }

                    }
                }
            }
            if (lowestDistance != 100)   //after looping, if the lowest value has changed, then we found someone
            {
                closestPlayerAngle.Normalize();
                *hack->aimAngles = closestPlayerAngle;
                *(int*)(hack->clientModuleBase + hazedumper::signatures::dwForceAttack) = 5;
                Sleep(20);
                *(int*)(hack->clientModuleBase + hazedumper::signatures::dwForceAttack) = 4;
            }
        }

        /*//std::cout << "Vert Velocity: " << localPlayer->getVelocity().z << std::endl;
        if (abs(hack->localPlayer->getVelocity().z) < 7)
        {
            float lowestDistance = 999999;
            int lowestDistIndex = 0;
            playerEnt* tempPlayerEntity;
            for (int tempIndex = 1; tempIndex < 65; tempIndex++)
            {
                tempPlayerEntity = getPlayerEntity(tempIndex);
                if (tempPlayerEntity != 0)
                {
                    Vector2AIM angle = calcs->calcAngle(hack->localPlayer->getViewPosition(), tempPlayerEntity->getBonePosition(8));
                    if (isnan(angle.x) || isnan(angle.y))
                    {
                        continue;
                    }
                    angle = angle - hack->localPlayer->getAimPunchAngle() * 2;
                    if (abs(angle.x - hack->aimAngles->x) < hack->settings.halfTriggerbotFov && abs(angle.y - hack->aimAngles->y) < hack->settings.halfTriggerbotFov)
                    {

                        angle.Normalize();
                        *hack->aimAngles = angle;
                        *(int*)(hack->clientModuleBase + hazedumper::signatures::dwForceAttack) = 5;
                        break;
                    }
                }
            }

            Sleep(20);
            *(int*)(hack->clientModuleBase + hazedumper::signatures::dwForceAttack) = 4;
        }
        */
    }
    
breakIf:
    Sleep(1);
}


playerEnt* cHack::getPlayerEntity(int index)
{
    playerEnt* target = 0;
    target = (playerEnt*)hack->clientList[index].entptr;
    if (target == nullptr)
    {
        return 0;
    }
    else if (target == hack->localPlayer)
    {
        return 0;
    }
    else if (target->getbDormant() == true)
    {
        return 0;
    }
    else if (target->getHealth() <= 0 || target->getHealth() > 100)
    {
        return 0;
    }
    else if (hack->settings.teamChecking && target->getTeam() == hack->localPlayer->getTeam())
    {
        return 0;
    }


    return target;
}
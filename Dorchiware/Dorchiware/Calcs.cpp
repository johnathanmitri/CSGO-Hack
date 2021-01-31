#include "pch.h"
#include "Calcs.h"
#include "dx.h"

Vector2AIM Calcs::calcAngle(Vector3 localPlayerCoords, Vector3 targetPlayerCoords)
{
    float xDisp = localPlayerCoords.x - targetPlayerCoords.x;
    float yDisp = localPlayerCoords.z - targetPlayerCoords.z; //z is y and y is z because i mixed up the order in the CODE. the Player object is the right order. Z is vertical, x,y is the plane
    float zDisp = localPlayerCoords.y - targetPlayerCoords.y;

    if (isnan(xDisp) || isnan(zDisp))
    {
        return { NAN,NAN };
    }

    float lateralHypotenuse = sqrt(xDisp * xDisp + zDisp * zDisp);

    float angleFromEast = acos(xDisp / lateralHypotenuse) * 180 / (float)M_PI;

    angleFromEast -= 180;

    if (zDisp < 0)
        angleFromEast = -angleFromEast;

    float verticalHypotenuse = sqrt(lateralHypotenuse * lateralHypotenuse + yDisp * yDisp);

    float vertAngle = acos(lateralHypotenuse / verticalHypotenuse) * 180 / (float)M_PI;

    if (yDisp < 0)
        vertAngle = -vertAngle;

    return { vertAngle, angleFromEast };

}

float Calcs::getDistance(Vector3 localPlayer, Vector3 target)
{
    float xDisp = localPlayer.x - target.x;
    float yDisp = localPlayer.z - target.z; //z is y and y is z because i mixed up the order in the CODE. the Player object is the right order. Z is vertical, x,y is the plane
    float zDisp = localPlayer.y - target.y;
    float lateralHypotenuse = sqrt(xDisp * xDisp + zDisp * zDisp);

    return sqrt(lateralHypotenuse * lateralHypotenuse + yDisp * yDisp);  //now take vertical distance into account
}

bool Calcs::WorldToScreen(Vector3 pos, Vector2& screen)
{
    bool status = true;
    Vector4 clipCoords;
    clipCoords.x = pos.x * viewMatrix[0] + pos.y * viewMatrix[1] + pos.z * viewMatrix[2] + viewMatrix[3];
    clipCoords.y = pos.x * viewMatrix[4] + pos.y * viewMatrix[5] + pos.z * viewMatrix[6] + viewMatrix[7];
    clipCoords.z = pos.x * viewMatrix[8] + pos.y * viewMatrix[9] + pos.z * viewMatrix[10] + viewMatrix[11];
    clipCoords.w = pos.x * viewMatrix[12] + pos.y * viewMatrix[13] + pos.z * viewMatrix[14] + viewMatrix[15];

    if (clipCoords.w < 0.0)
        status = false;

    Vector3 NDC;
    NDC.x = clipCoords.x / clipCoords.w;
    NDC.y = clipCoords.y / clipCoords.w;
    NDC.z = clipCoords.z / clipCoords.w;

    screen.x = (halfWindowWidth * NDC.x) + (NDC.x + halfWindowWidth);
    screen.y = -(halfWindowHeight * NDC.y) + (NDC.y + halfWindowHeight);
    return status;
}
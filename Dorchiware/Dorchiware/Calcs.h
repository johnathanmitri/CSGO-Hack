#pragma once


class Calcs
{
public:
    float viewMatrix[16];

    Vector2AIM calcAngle(Vector3 localPlayerCoords, Vector3 targetPlayerCoords);

    float getDistance(Vector3 localPlayer, Vector3 target);

    bool WorldToScreen(Vector3 pos, Vector2& screen);

};

extern Calcs* calcs;
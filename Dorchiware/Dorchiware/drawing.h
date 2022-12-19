#pragma once

void DrawFilledRect(int x, int y, int w, int h, D3DCOLOR col);

void DrawFilledBox(int x, int y, int width, int height, D3DCOLOR color);

void DrawLine(int x1, int y1, int x2, int y2, int thickness, D3DCOLOR color, ID3DXLine* LineL);

//TOP LEFT AND BOTTOM RIGHT CORNERS
void DrawEspBox(Vector2 topLeft, Vector2 bottomRight, int thickness, D3DCOLOR color, ID3DXLine* LineL);

void DrawEspBox3D(playerEnt* player, int thickness, D3DCOLOR color, ID3DXLine* LineL);

void slowDrawText(const char* text, float x, float y, int justification, D3DCOLOR color, const char* fontName, int fontSize);

void LDrawText(const char* text, float x, float y, int justification, D3DCOLOR color, ID3DXFont* font);

void fastDrawText(const char* text, float x, float y, int justification, D3DCOLOR color, CD3DFont* font);
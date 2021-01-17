#pragma once

typedef HRESULT(APIENTRY* tEndScene)(LPDIRECT3DDEVICE9 pDevice);

bool GetD3D9Device(void** pTable, size_t size);

static HWND window;

HWND GetProcessWindow();

extern int windowHeight, windowWidth;

extern float halfWindowHeight, halfWindowWidth;

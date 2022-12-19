// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "main.h"

#define CONSOLE false

DWORD WINAPI setupHackThread(HMODULE hModule)
{

#if CONSOLE
    FILE* f = 0;
    AllocConsole();
    freopen_s(&f, "CONOUT$", "w", stdout);
    std::cout << "Injected\n\n";
#endif

    HackThread();

    MessageBeep(MB_ICONWARNING);

#if CONSOLE
    fclose(f);
    FreeConsole();
#endif
    FreeLibraryAndExitThread(hModule, 0);
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)setupHackThread, hModule, 0, nullptr));
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}


#include "framework.h"

DWORD Initialize(LPVOID)
{
    AllocConsole();
    FILE* File;
    freopen_s(&File, "Aeon.log", "w+", stdout);

    TArray<ULocalPlayer*>& LocalPlayers = UWorld::GetWorld()->OwningGameInstance->LocalPlayers;
    LocalPlayers[0]->PlayerController->SwitchLevel(L"Athena_Terrain");
    LocalPlayers.Remove(0);

    GameModeHooks::Initialize();

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ulReason, LPVOID lpReserved)
{
    switch (ulReason)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(0, 0, Initialize, 0, 0, 0);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}


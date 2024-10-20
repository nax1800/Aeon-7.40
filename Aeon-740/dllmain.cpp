#include "framework.h"

DWORD InputThread(LPVOID)
{
    while (true)
    {
        if (GetAsyncKeyState(VK_F6) & 0x01)
        {
            UKismetSystemLibrary::ExecuteConsoleCommand(UWorld::GetWorld(), L"startaircraft", nullptr);
        }
    }
}

void Initialize()
{
    AllocConsole();
    FILE* File;
    freopen_s(&File, "CONOUT$", "w+", stdout);

    MH_Initialize();

    TArray<ULocalPlayer*>& LocalPlayers = UWorld::GetWorld()->OwningGameInstance->LocalPlayers;
    LocalPlayers[0]->PlayerController->SwitchLevel(L"Athena_Terrain");
    LocalPlayers.Remove(0);
    LocalPlayers.Free();

    for (uintptr_t ByteOffset : GOffsets::BytesToPatch)
    {
        Memory::PatchByte(ByteOffset);
    }

    *(bool*)Memory::GetAddress(GOffsets::GIsClient) = false;

    Hooks::Server::Initialize();
    Hooks::GameMode::Initialize();
    Hooks::PlayerController::Initialize();
    Hooks::Pawn::Initialize();
    Hooks::Abilities::Initialize();
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ulReason, LPVOID lpReserved)
{
    switch (ulReason)
    {
    case DLL_PROCESS_ATTACH:
        Initialize();
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}


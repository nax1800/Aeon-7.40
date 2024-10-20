#pragma once

#include <windows.h>
#include <iostream>
#include <format>
#include <vector>

#include "SDK.hpp"
#include "MinHook/include/MinHook.h"

using namespace SDK;
using namespace std;

// 7FF7104E0000

#define LogInfo(...) { cout << "AeonLog: " << format(__VA_ARGS__) << endl; }

#include "Offsets.h"
#include "HookingManager.h"
#include "Memory.h"
#include "Globals.h"
#include "Defines.h"

#include "SDKUtils.h"

#include "AbilitiesHandler.h"
#include "GameModeHandler.h"
#include "InventoryHandler.h"
#include "LootHandler.h"
#include "ServerHandler.h"

#include "AbilitiesHooks.h"
#include "ActorHooks.h"
#include "ServerHooks.h"
#include "GameModeHooks.h"
#include "PawnHooks.h"
#include "PlayerControllerHooks.h"

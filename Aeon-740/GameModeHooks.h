#pragma once
#include "framework.h"

namespace Hooks
{
	namespace GameMode
	{
		bool hkReadyToStartMatch(AFortGameModeAthena* GameMode)
		{
			auto GameState = static_cast<AFortGameStateAthena*>(GameMode->GameState);

			if (!Globals::bPlaylistSetup)
			{
				Globals::bPlaylistSetup = true;
				auto Playlist = SDKUtils::FindObject<UFortPlaylistAthena>("/Game/Athena/Playlists/Playlist_DefaultSolo.Playlist_DefaultSolo");
				if (Playlist)
				{
					LogInfo("GameMode::ReadyToStartMatch: Playlist is setting up...");
					GameState->CurrentPlaylistId = Playlist->PlaylistId;
					FPlaylistPropertyArray& PlaylistInfo = GameState->CurrentPlaylistInfo;
					PlaylistInfo.BasePlaylist = Playlist;
					PlaylistInfo.OverridePlaylist = Playlist;
					PlaylistInfo.PlaylistReplicationKey++;
					PlaylistInfo.MarkArrayDirty();

					GameMode->CurrentPlaylistName = Playlist->PlaylistName;
					GameMode->CurrentPlaylistId = Playlist->PlaylistId;
					GameState->OnRep_CurrentPlaylistId();
					GameState->OnRep_CurrentPlaylistInfo();
				}
			}

			if (!GameState->MapInfo)
				return false;

			GameMode->DefaultPawnClass = SDK::APlayerPawn_Athena_C::StaticClass();

			GameModeHandler::ShowFoundation(SDKUtils::FindObject<ABuildingFoundation>("/Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.LF_Athena_POI_25x36"));
			GameModeHandler::ShowFoundation(SDKUtils::FindObject<ABuildingFoundation>("/Game/Athena/Maps/Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.ShopsNew")); 

			if (!Globals::bListening)
			{
				ServerHandler::Listen();
				Hooks::Actor::Initialize();
				LootHandler::Initialize();

				GameMode->GameSession->MaxPlayers = 100;
				GameMode->WarmupRequiredPlayerCount = 1;

				GameState->OnRep_CurrentPlaylistInfo();
			}

			GameMode->bWorldIsReady = true;

			return Defines::GameMode::ReadyToStartMatch(GameMode);
		}

		APawn* hkSpawnDefaultPawnFor(AGameMode* GameMode, AController* NewPlayer, AActor* StartSpot)
		{
			if (NewPlayer && StartSpot)
			{
				auto Transform = StartSpot->GetTransform();
				return GameMode->SpawnDefaultPawnAtTransform(NewPlayer, Transform);
			}

			return nullptr;
		}

		void Initialize()
		{
			HookingManager::CreateMinHook(Memory::GetAddress(GOffsets::GameMode::ReadyToStartMatch), hkReadyToStartMatch, (void**)&Defines::GameMode::ReadyToStartMatch);
			HookingManager::CreateMinHook(Memory::GetAddress(GOffsets::GameMode::SpawnDefaultPawnFor), hkSpawnDefaultPawnFor);
		}
	}
}
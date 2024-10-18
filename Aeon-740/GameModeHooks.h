#pragma once
#include "framework.h"

namespace GameModeHooks
{
	bool (*oReadyToStartMatch)(AGameMode* GameMode);
	bool hkReadyToStartMatch(AFortGameMode* GameMode)
	{
		auto GameState = static_cast<AFortGameStateAthena*>(GameMode->GameState);

		if (!Globals::bPlaylistSetup)
		{
			Globals::bPlaylistSetup = true;
			auto Playlist = SDKUtils::FindObject<UFortPlaylistAthena>("/Game/Athena/Playlists/Playlist_DefaultSolo.Playlist_DefaultSolo");
			if (Playlist)
			{
				GameState->CurrentPlaylistId = Playlist->PlaylistId;
				FPlaylistPropertyArray& PlaylistInfo = GameState->CurrentPlaylistInfo;
				PlaylistInfo.BasePlaylist = Playlist;
				PlaylistInfo.OverridePlaylist = Playlist;
				PlaylistInfo.PlaylistReplicationKey++;

				GameState->OnRep_CurrentPlaylistId();
				GameState->OnRep_CurrentPlaylistInfo();
			}
		}

		if (!GameState->MapInfo)
			return false;

		if (!Globals::bListening)
		{
			Globals::bListening = true;
		}

		return oReadyToStartMatch(GameMode);
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

	}
}
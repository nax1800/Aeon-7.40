#pragma once
#include "framework.h"

namespace ServerHandler
{
	void Listen()
	{
		if (Globals::bListening)
			return;

		Globals::bListening = true;

		auto World = UWorld::GetWorld();

		UNetDriver* NetDriver = Defines::Server::CreateNetDriver(UEngine::GetEngine(), World, FName(282));
		if (!NetDriver)
		{
			LogInfo("Net Driver not created??!");
			return;
		}
		else
			LogInfo("NetDriver: {}", NetDriver->GetFullName());

		World->NetDriver = NetDriver;
		NetDriver->World = World;
		NetDriver->NetDriverName = FName(282);

		FString Error;
		FURL URL = FURL();
		URL.Port = 7777;

		Defines::Server::InitListen(NetDriver, World, URL, false, Error);
		Defines::Server::SetWorld(NetDriver, World);

		World->LevelCollections[0].NetDriver = NetDriver;
		World->LevelCollections[1].NetDriver = NetDriver;

		auto vft = *(void***)NetDriver->ReplicationDriver;
		Defines::Server::ServerReplicateActors = decltype(Defines::Server::ServerReplicateActors)(vft[0x57]);

		LogInfo("ServerHandler::Listen: Server Listening on port {}", 7777);
	}
}
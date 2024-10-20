#pragma once
#include "framework.h"

namespace GameModeHandler
{
	static void ShowFoundation(ABuildingFoundation* BuildingFoundation, bool bShow = true)
	{
		if (!BuildingFoundation)
			return;

		BuildingFoundation->bServerStreamedInLevel = bShow;
		BuildingFoundation->bFoundationEnabled = bShow;
		BuildingFoundation->DynamicFoundationType = EDynamicFoundationType::Static;
		BuildingFoundation->SetDynamicFoundationEnabled(bShow);
		BuildingFoundation->OnRep_ServerStreamedInLevel();
		BuildingFoundation->OnRep_LevelToStream();
	}
}
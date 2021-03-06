#include <API/ARK/Ark.h>

#include "StructureHealth.h"
#include "StructureHealthCommands.h"
#include "StructureHealthConfig.h"

#pragma comment(lib, "ArkApi.lib")
#pragma comment(lib, "Permissions.lib")
void Init()
{
	Log::Get().Init("StructureHealth");
	InitConfig();
	InitCommands();
	Log::GetLog()->info("StructureHealth Loaded.");
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		Init();
		break;
	case DLL_PROCESS_DETACH:
		RemoveCommands();
		break;
	}
	return TRUE;
}

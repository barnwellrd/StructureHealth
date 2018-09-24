#pragma once

#include <Permissions.h>

#include "StructureHealth.h"

inline void ChangeStructureHealth(AShooterPlayerController* player, FString* message, int mode)
{
	if (!player || !player->PlayerStateField() || RequiresAdmin && !player->bIsAdmin()())
		return;

	const uint64 steam_id = ArkApi::IApiUtils::GetSteamIdFromController(player);
	if (RequiresPermission && !Permissions::IsPlayerHasPermission(steam_id, "InvincibleStructures"))
	{
		ArkApi::GetApiUtils().SendServerMessage(player, FLinearColor(255, 0, 0),
		                                        "You don't have permissions to use this command");
		return;
	}

	TArray<FString> Parsed;
	message->ParseIntoArray(Parsed, L" ", true);

	if (Parsed.Num() < 2 )
	{
		ArkApi::GetApiUtils().SendServerMessage(player, FLinearColor(255, 0, 0), "Incorrect Syntax: /sh <CommandNum> <CommandArg(optional)>");
		return;
	}

	int CommandNum = -1;
	int CommandArg = 0;

	try
	{
		if (Parsed.Num() == 2)
		{
			CommandNum = std::stoi(Parsed[1].ToString().c_str());
			CommandArg = std::stoi(Parsed[2].ToString().c_str());
		}
		else
		{
			CommandNum = std::stoi(Parsed[1].ToString().c_str());
		}
		
	}
	catch (...)
	{
		ArkApi::GetApiUtils().SendServerMessage(player, FLinearColor(255, 0, 0), "Incorrect Syntax: /sh <CommandNum> <CommandArg(optional)>");
	}

	if (CommandNum < 0 || CommandNum > 4)
	{
		ArkApi::GetApiUtils().SendServerMessage(player, FLinearColor(255, 0, 0), "<CommandNum> must be between 0 and 4!");
		return;
	}

	if (CommandArg < 0 || CommandArg > 100)
	{
		ArkApi::GetApiUtils().SendServerMessage(player, FLinearColor(255, 0, 0), "<CommandArg> must be between 0 and 100!");
		return;
	}
	
	AActor* Actor = player->GetPlayerCharacter()->GetAimedActor(ECC_GameTraceChannel2, nullptr, 0.0, 0.0, nullptr, nullptr,
	                                                            false, false);
	if (Actor && Actor->IsA(APrimalStructure::GetPrivateStaticClass()))
	{
		APrimalStructure* Structure = static_cast<APrimalStructure*>(Actor);
		AShooterPlayerState* ASPS = static_cast<AShooterPlayerState*>(player->PlayerStateField());

		if (ASPS && ASPS->MyPlayerDataStructField())
		{
			float Args[] = {0};
			const int teamId = Structure->TargetingTeamField();

			if (teamId != player->TargetingTeamField() || ASPS->MyTribeDataField() &&
				teamId != ASPS->MyTribeDataField()->TribeIDField())
			{
				ArkApi::GetApiUtils().SendServerMessage(player, FLinearColor(255, 0, 0), "That is not your structure");
				return;
			}

			if (CommandNum == 0)
			{
				FString StructName = Structure->DescriptiveNameField();
				Structure->Demolish(player);
				ArkApi::GetApiUtils().SendServerMessage(player, FLinearColor(0, 255, 0), "{} demolished structure: {}",
					player->GetPlayerCharacter()->PlayerNameField().ToString().c_str(), StructName.ToString().c_str());
			}

			if (CommandNum == 1)
			{
				Structure->MaxHealthField() = 500000;
				Structure->HealthField() = 500000;
				Structure->UpdatedHealth(true);
				TArray<APrimalStructure*> linkedStructures = Structure->LinkedStructuresField();

				for(int cnt = 0; cnt < CommandArg; cnt++)
				{
					TArray<APrimalStructure*> tempLS;

					for (APrimalStructure* ls : linkedStructures)
					{
						tempLS.Append(ls->LinkedStructuresField());

					}
					for (APrimalStructure* templs : tempLS)
					{
						linkedStructures.AddUnique(templs);
					}
				}

				for (APrimalStructure* ls : linkedStructures)
				{
					ls->MaxHealthField() = 500000;
					ls->HealthField() = 500000;
					ls->UpdatedHealth(true);
					FString StructName = ls->DescriptiveNameField(); 
					ArkApi::GetApiUtils().SendServerMessage(player, FLinearColor(0, 255, 0), "{} updated structure health: {}",
						player->GetPlayerCharacter()->PlayerNameField().ToString().c_str(), StructName.ToString().c_str());
				}
			}
				
			if (CommandNum == 2)
			{
				APrimalStructure* default_structure = static_cast<APrimalStructure*>(Structure->ClassField()->GetDefaultObject(true));


				Structure->MaxHealthField() = default_structure->HealthField();
				Structure->HealthField() = default_structure->HealthField();
				Structure->UpdatedHealth(true);
				TArray<APrimalStructure*> linkedStructures = Structure->LinkedStructuresField();

				for (int cnt = 0; cnt < CommandArg; cnt++)
				{
					TArray<APrimalStructure*> tempLS;

					for (APrimalStructure* ls : linkedStructures)
					{
						tempLS.Append(ls->LinkedStructuresField());

					}

					for (APrimalStructure* templs : tempLS)
					{
						linkedStructures.AddUnique(templs);
					}
				}

				for (APrimalStructure* ls : linkedStructures)
				{
					default_structure = static_cast<APrimalStructure*>(ls->ClassField()->GetDefaultObject(true));
					ls->MaxHealthField() = default_structure->HealthField();
					ls->HealthField() = default_structure->HealthField();
					ls->UpdatedHealth(true);
					FString StructName = ls->DescriptiveNameField(); 
					ArkApi::GetApiUtils().SendServerMessage(player, FLinearColor(0, 255, 0), "{} updated structure health: {}",
						player->GetPlayerCharacter()->PlayerNameField().ToString().c_str(), StructName.ToString().c_str());

				}
			}
				
			if (CommandNum == 3)
			{

				/**
					* \brief Finds all Structures owned by team
					*/
				TArray<AActor*> AllStructures;
				UGameplayStatics::GetAllActorsOfClass(reinterpret_cast<UObject*>(ArkApi::GetApiUtils().GetWorld()), APrimalStructure::GetPrivateStaticClass(), &AllStructures);
				TArray<APrimalStructure*> FoundStructures;
				APrimalStructure* Struc;

				for (AActor* StructActor : AllStructures)
				{
					if (!StructActor || StructActor->TargetingTeamField() != teamId || !Actor->IsA(APrimalStructure::GetPrivateStaticClass())) continue;
					Struc = static_cast<APrimalStructure*>(StructActor);
					FoundStructures.Add(Struc);
				}

				for (APrimalStructure* st : FoundStructures)
				{
					st->MaxHealthField() = 500000;
					st->HealthField() = 500000;
					st->UpdatedHealth(true);
					FString StructName = st->DescriptiveNameField();
					ArkApi::GetApiUtils().SendServerMessage(player, FLinearColor(0, 255, 0), "{} updated structure health: {}",
						player->GetPlayerCharacter()->PlayerNameField().ToString().c_str(), StructName.ToString().c_str());
				}
					
			}

			if (CommandNum == 4)
			{
				APrimalStructure* default_structure;
				TArray<AActor*> AllStructures;
				UGameplayStatics::GetAllActorsOfClass(reinterpret_cast<UObject*>(ArkApi::GetApiUtils().GetWorld()), APrimalStructure::GetPrivateStaticClass(), &AllStructures);
				TArray<APrimalStructure*> FoundStructures;
				APrimalStructure* Struc;

				for (AActor* StructActor : AllStructures)
				{
					if (!StructActor || StructActor->TargetingTeamField() != teamId || !Actor->IsA(APrimalStructure::GetPrivateStaticClass())) continue;
					Struc = static_cast<APrimalStructure*>(StructActor);
					FoundStructures.Add(Struc);
				}

				for (APrimalStructure* st : FoundStructures)
				{
					default_structure = static_cast<APrimalStructure*>(st->ClassField()->GetDefaultObject(true));
					st->MaxHealthField() = default_structure->HealthField();
					st->HealthField() = default_structure->HealthField();
					st->UpdatedHealth(true);
					FString StructName = st->DescriptiveNameField();
					ArkApi::GetApiUtils().SendServerMessage(player, FLinearColor(0, 255, 0), "{} updated structure health: {}",
						player->GetPlayerCharacter()->PlayerNameField().ToString().c_str(), StructName.ToString().c_str());
				}
			}
		}
	}
	else
	{
		ArkApi::GetApiUtils().SendServerMessage(player, FLinearColor(255, 0, 0),
		                                        "Please face the middle of your screen towards the Structure you want to change.");
	}
}

inline void InitCommands()
{
	ArkApi::GetCommands().AddChatCommand("/sh", &ChangeStructureHealth);
}

inline void RemoveCommands()
{
	ArkApi::GetCommands().RemoveChatCommand("/sh");
}

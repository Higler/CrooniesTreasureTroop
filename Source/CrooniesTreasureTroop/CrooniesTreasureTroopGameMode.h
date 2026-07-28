// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CrooniesTreasureTroopInteractables.h"
#include "CrooniesTreasureTroopGameMode.generated.h"

UCLASS(minimalapi)
class ACrooniesTreasureTroopGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ACrooniesTreasureTroopGameMode();

	virtual void BeginPlay() override;

private:
	UPROPERTY()
	class UStaticMesh* CubeMesh;

	UPROPERTY()
	class UStaticMesh* SphereMesh;

	UPROPERTY()
	class UStaticMesh* ConeMesh;

	UPROPERTY()
	class UMaterialInterface* BaseMaterial;

	UPROPERTY()
	TArray<class UStaticMesh*> TreeMeshes;

	UPROPERTY()
	class UTexture2D* HubCaveEntranceTexture;

	UPROPERTY()
	class UTexture2D* HubTaskBoardTexture;

	UPROPERTY()
	class UTexture2D* HubGearStoreTexture;

	UPROPERTY()
	class UTexture2D* HubPotionShopTexture;

	UPROPERTY()
	class UTexture2D* HubSpecialAreaTexture;

	UPROPERTY()
	class UStaticMesh* OldAtticHouseMesh;

	UPROPERTY()
	class UTexture2D* AtticBoxesTexture;

	UPROPERTY()
	class UTexture2D* AtticLadderTexture;

	class AStaticMeshActor* SpawnBlock(const FVector& Location, const FVector& Scale, const FLinearColor& Color, FName Name);
	class AStaticMeshActor* SpawnDecorBlock(const FVector& Location, const FVector& Scale, const FLinearColor& Color, FName Name);
	class AStaticMeshActor* SpawnProp(class UStaticMesh* Mesh, const FVector& Location, const FVector& Scale, const FLinearColor& Color, FName Name);
	class AStaticMeshActor* SpawnTree(const FVector& Location, float Scale, int32 Variant, FName Name);
	class ACovePickup* SpawnPickup(const FVector& Location, ECovePickupKind Kind, FName FriendId = NAME_None);
	void SpawnLevelZone(const FVector& Location, int32 LevelNumber, const FString& Title, const FString& Style, const FString& Objective);
	class ACoveHubPortal* SpawnHubPortal(const FVector& Location, int32 LevelNumber, const FString& Label);
	void SpawnHubImageSign(const FVector& Location, class UTexture2D* Texture, float Scale, FName Name);
	void SpawnGate(const FVector& Location, FName RequiredFriend, const FString& LockedMessage);
	void SpawnTrap(const FVector& Location, ECoveTrapKind TrapKind = ECoveTrapKind::Skeleton);
	void SpawnEnemy(const FVector& Location, FName EnemyId, int32 LevelNumber);
	void SpawnCollectibleSet(const FVector& Base, int32 Count, ECovePickupKind Kind, float Spacing);
	void SpawnProceduralRunContent(const FVector& Base, const FVector& RoomFloorScale, int32 LevelNumber);
	void SpawnAreaLight(const FVector& Location, float Radius, float Intensity, FName Name);
	void SpawnLighting();
	void MovePlayerToTutorial();
	void SpawnTreeBorder(const FVector& Center, const FVector& FloorScale, int32 Seed, bool bDense = false);
	void SpawnHubGrassDetail(const FVector& Center, const FVector& FloorScale);
	void SpawnHubPet(const FVector& Location, bool bDog, float Scale, FName Name);
	void SpawnHubPets(const FVector& Center);
	void SpawnTemporaryLayoutGrid(const FVector& Center, const FVector& FloorScale, const FString& MapName);
	void SpawnHub();
	void SpawnStoryLevels();
	void SpawnBonusLevels();
	void SpawnCrewChallengeArena();
	void SpawnWorld();
};





// Copyright Epic Games, Inc. All Rights Reserved.

#include "CrooniesTreasureTroopGameMode.h"
#include "CrooniesTreasureTroopHUD.h"
#include "CrooniesTreasureTroopInteractables.h"
#include "CrooniesTreasureTroopPlayerController.h"
#include "CrooniesTreasureTroopCharacter.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/Texture2D.h"
#include "Engine/TextRenderActor.h"
#include "Engine/World.h"
#include "Engine/DirectionalLight.h"
#include "Engine/SkyLight.h"
#include "Engine/PointLight.h"
#include "Engine/ExponentialHeightFog.h"
#include "Components/LightComponent.h"
#include "Components/SkyLightComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "Components/BillboardComponent.h"
#include "Components/TextRenderComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Misc/DateTime.h"
#include "UObject/ConstructorHelpers.h"

ACrooniesTreasureTroopGameMode::ACrooniesTreasureTroopGameMode()
{
	PlayerControllerClass = ACrooniesTreasureTroopPlayerController::StaticClass();
	DefaultPawnClass = ACrooniesTreasureTroopCharacter::StaticClass();
	HUDClass = ACrooniesTreasureTroopHUD::StaticClass();

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeAsset(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeAsset.Succeeded())
	{
		CubeMesh = CubeAsset.Object;
	}

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereAsset(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (SphereAsset.Succeeded())
	{
		SphereMesh = SphereAsset.Object;
	}

	static ConstructorHelpers::FObjectFinder<UStaticMesh> ConeAsset(TEXT("/Engine/BasicShapes/Cone.Cone"));
	if (ConeAsset.Succeeded())
	{
		ConeMesh = ConeAsset.Object;
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialAsset(TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
	if (MaterialAsset.Succeeded())
	{
		BaseMaterial = MaterialAsset.Object;
	}

	OldAtticHouseMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Game/CityPark/Meshes/Buildings/SM_House02_1.SM_House02_1"));
	HubCaveEntranceTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Cove/UIIcons/Cave_Entrance_Transparent.Cave_Entrance_Transparent"));
	if (!HubCaveEntranceTexture)
	{
		HubCaveEntranceTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Cove/UIIcons/Cave_Entrance_Adventure_Transparent.Cave_Entrance_Adventure_Transparent"));
	}
	HubTaskBoardTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Cove/UIIcons/Task_Board_Transparent.Task_Board_Transparent"));
	HubGearStoreTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Cove/UIIcons/Hardware_Gadget_Store_Transparent.Hardware_Gadget_Store_Transparent"));
	HubPotionShopTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Cove/UIIcons/Potion_Time_Magic_Shop_Transparent.Potion_Time_Magic_Shop_Transparent"));
	HubSpecialAreaTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Cove/UIIcons/Magical_Specialty_Area_Fixed.Magical_Specialty_Area_Fixed"));
	if (!HubSpecialAreaTexture)
	{
		HubSpecialAreaTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Cove/UIIcons/Magical_Specialty_Area.Magical_Specialty_Area"));
	}
	AtticBoxesTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Cove/UIIcons/Cardboard_Boxes_Transparent.Cardboard_Boxes_Transparent"));
	AtticLadderTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Cove/UIIcons/Wooden_Ladder_Transparent.Wooden_Ladder_Transparent"));
}

void ACrooniesTreasureTroopGameMode::BeginPlay()
{
	Super::BeginPlay();
	SpawnWorld();
}

AStaticMeshActor* ACrooniesTreasureTroopGameMode::SpawnBlock(const FVector& Location, const FVector& Scale, const FLinearColor& Color, FName Name)
{
	UWorld* World = GetWorld();
	if (!World || !CubeMesh)
	{
		return nullptr;
	}

	AStaticMeshActor* Actor = World->SpawnActor<AStaticMeshActor>(Location, FRotator::ZeroRotator);
#if WITH_EDITOR
	Actor->SetActorLabel(Name.ToString());
#endif
	Actor->GetStaticMeshComponent()->SetStaticMesh(CubeMesh);
	Actor->GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
	Actor->GetStaticMeshComponent()->SetCollisionProfileName(TEXT("BlockAll"));
	Actor->SetActorScale3D(Scale);

	if (BaseMaterial)
	{
		UMaterialInstanceDynamic* Mat = UMaterialInstanceDynamic::Create(BaseMaterial, Actor);
		Mat->SetVectorParameterValue(TEXT("Color"), Color);
		Mat->SetVectorParameterValue(TEXT("BaseColor"), Color);
		Actor->GetStaticMeshComponent()->SetMaterial(0, Mat);
	}

	return Actor;
}

AStaticMeshActor* ACrooniesTreasureTroopGameMode::SpawnDecorBlock(const FVector& Location, const FVector& Scale, const FLinearColor& Color, FName Name)
{
	AStaticMeshActor* Actor = SpawnBlock(Location, Scale, Color, Name);
	if (Actor)
	{
		Actor->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	return Actor;
}

void ACrooniesTreasureTroopGameMode::SpawnTemporaryLayoutGrid(const FVector& Center, const FVector& FloorScale, const FString& MapName)
{
	constexpr bool bShowTemporaryLayoutGrid = true;
	if (!bShowTemporaryLayoutGrid)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	const FLinearColor GridLineColor(0.72f, 0.72f, 0.72f, 0.45f);
	const FLinearColor AxisLineColor(0.88f, 0.88f, 0.88f, 0.60f);
	const float HalfX = FloorScale.X * 50.f;
	const float HalfY = FloorScale.Y * 50.f;
	const float GridHalfX = FMath::Max(100.f, HalfX + 120.f);
	const float GridHalfY = FMath::Max(100.f, HalfY + 120.f);
	const float GridStepX = GridHalfX / 20.f;
	const float GridStepY = GridHalfY / 20.f;
	const float LineZ = 31.f;
	const float LabelZ = 84.f;
	const FString Prefix = FString::Printf(TEXT("TEMP_LAYOUT_GRID_%s"), *MapName);

	for (int32 Index = -20; Index <= 20; ++Index)
	{
		const bool bAxis = Index == 0;
		const FLinearColor LineColor = bAxis ? AxisLineColor : GridLineColor;
		const float Thickness = bAxis ? 0.028f : 0.014f;
		SpawnDecorBlock(Center + FVector(Index * GridStepX, 0.f, LineZ), FVector(Thickness, HalfY / 50.f, 0.018f), LineColor, FName(*FString::Printf(TEXT("%s_XLine_%+03d"), *Prefix, Index)));
		SpawnDecorBlock(Center + FVector(0.f, Index * GridStepY, LineZ + 0.5f), FVector(HalfX / 50.f, Thickness, 0.018f), LineColor, FName(*FString::Printf(TEXT("%s_YLine_%+03d"), *Prefix, Index)));
	}

	auto SpawnGridLabel = [&](const FVector& Location, const FString& Text, const FString& LabelName)
	{
		ATextRenderActor* LabelActor = World->SpawnActor<ATextRenderActor>(Location, FRotator(62.f, 180.f, 0.f));
		if (!LabelActor || !LabelActor->GetTextRender())
		{
			return;
		}
#if WITH_EDITOR
		LabelActor->SetActorLabel(LabelName);
#endif
		const bool bCellLabel = LabelName.Contains(TEXT("_Cell_"));
		UTextRenderComponent* Label = LabelActor->GetTextRender();
		Label->SetText(FText::FromString(Text));
		Label->SetTextRenderColor(bCellLabel ? FColor(255, 235, 20, 255) : FColor(255, 255, 255, 255));
		Label->SetHorizontalAlignment(EHTA_Center);
		Label->SetVerticalAlignment(EVRTA_TextCenter);
		Label->SetWorldSize(bCellLabel ? 44.f : 72.f);
		Label->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	};

	for (int32 Index = -20; Index <= 20; ++Index)
	{
		const FString Value = Index == 0 ? TEXT("0") : FString::Printf(TEXT("%+d"), Index);
		SpawnGridLabel(Center + FVector(Index * GridStepX, -GridHalfY - 95.f, LabelZ), Value, FString::Printf(TEXT("%s_XLabel_%+03d"), *Prefix, Index));
		SpawnGridLabel(Center + FVector(GridHalfX + 95.f, Index * GridStepY, LabelZ), Value, FString::Printf(TEXT("%s_YLabel_%+03d"), *Prefix, Index));
	}

	for (int32 GridX = -20; GridX < 20; ++GridX)
	{
		for (int32 GridY = -20; GridY < 20; ++GridY)
		{
			const float CellCenterX = (GridX + 0.5f) * GridStepX;
			const float CellCenterY = (GridY + 0.5f) * GridStepY;
			const FString CellText = FString::Printf(TEXT("%d,%d"), GridX, GridY);
			SpawnGridLabel(Center + FVector(CellCenterX, CellCenterY, LabelZ + 1.f), CellText, FString::Printf(TEXT("%s_Cell_%+03d_%+03d"), *Prefix, GridX, GridY));
		}
	}
}

AStaticMeshActor* ACrooniesTreasureTroopGameMode::SpawnProp(UStaticMesh* Mesh, const FVector& Location, const FVector& Scale, const FLinearColor& Color, FName Name)
{
	UWorld* World = GetWorld();
	if (!World || !Mesh)
	{
		return nullptr;
	}

	AStaticMeshActor* Actor = World->SpawnActor<AStaticMeshActor>(Location, FRotator::ZeroRotator);
#if WITH_EDITOR
	Actor->SetActorLabel(Name.ToString());
#endif
	Actor->GetStaticMeshComponent()->SetStaticMesh(Mesh);
	Actor->GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
	Actor->GetStaticMeshComponent()->SetCollisionProfileName(TEXT("BlockAll"));
	Actor->SetActorScale3D(Scale);

	if (BaseMaterial)
	{
		UMaterialInstanceDynamic* Mat = UMaterialInstanceDynamic::Create(BaseMaterial, Actor);
		Mat->SetVectorParameterValue(TEXT("Color"), Color);
		Mat->SetVectorParameterValue(TEXT("BaseColor"), Color);
		Actor->GetStaticMeshComponent()->SetMaterial(0, Mat);
	}

	return Actor;
}

AStaticMeshActor* ACrooniesTreasureTroopGameMode::SpawnTree(const FVector& Location, float Scale, int32 Variant, FName Name)
{
	if (!CubeMesh)
	{
		return nullptr;
	}

	const float VariantTone = FMath::Frac(FMath::Abs(Variant) * 0.173f);
	const FLinearColor LeafA(0.08f + VariantTone * 0.06f, 0.52f + VariantTone * 0.14f, 0.16f, 1.f);
	const FLinearColor LeafB(0.04f, 0.36f + VariantTone * 0.10f, 0.10f, 1.f);
	const FLinearColor Trunk(0.34f, 0.18f, 0.08f, 1.f);

	AStaticMeshActor* TrunkActor = SpawnDecorBlock(Location + FVector(0.f, 0.f, 70.f * Scale), FVector(0.34f * Scale, 0.34f * Scale, 1.35f * Scale), Trunk, Name);
	SpawnDecorBlock(Location + FVector(0.f, 0.f, 178.f * Scale), FVector(1.22f * Scale, 1.22f * Scale, 0.72f * Scale), LeafA, FName(*FString::Printf(TEXT("%s_CanopyA"), *Name.ToString())));
	SpawnDecorBlock(Location + FVector(-36.f * Scale, 30.f * Scale, 226.f * Scale), FVector(0.78f * Scale, 0.78f * Scale, 0.52f * Scale), LeafB, FName(*FString::Printf(TEXT("%s_CanopyB"), *Name.ToString())));
	SpawnDecorBlock(Location + FVector(42.f * Scale, -34.f * Scale, 238.f * Scale), FVector(0.66f * Scale, 0.66f * Scale, 0.46f * Scale), LeafA, FName(*FString::Printf(TEXT("%s_CanopyC"), *Name.ToString())));
	return TrunkActor;
}

ACovePickup* ACrooniesTreasureTroopGameMode::SpawnPickup(const FVector& Location, ECovePickupKind Kind, FName FriendId)
{
	if (UWorld* World = GetWorld())
	{
		const FTransform SpawnTransform(FRotator::ZeroRotator, Location);
		ACovePickup* Pickup = World->SpawnActorDeferred<ACovePickup>(ACovePickup::StaticClass(), SpawnTransform);
		if (!Pickup)
		{
			return nullptr;
		}
		Pickup->Kind = Kind;
		Pickup->FriendId = FriendId;
		Pickup->FinishSpawning(SpawnTransform);
		return Pickup;
	}
	return nullptr;
}

void ACrooniesTreasureTroopGameMode::SpawnLevelZone(const FVector& Location, int32 LevelNumber, const FString& Title, const FString& Style, const FString& Objective)
{
	if (UWorld* World = GetWorld())
	{
		ACoveLevelZone* Zone = World->SpawnActor<ACoveLevelZone>(Location, FRotator::ZeroRotator);
		Zone->LevelNumber = LevelNumber;
		Zone->LevelTitle = Title;
		Zone->LevelStyle = Style;
		Zone->Objective = Objective;
	}
}

ACoveHubPortal* ACrooniesTreasureTroopGameMode::SpawnHubPortal(const FVector& Location, int32 LevelNumber, const FString& Label)
{
	if (UWorld* World = GetWorld())
	{
		ACoveHubPortal* Portal = World->SpawnActor<ACoveHubPortal>(Location, FRotator::ZeroRotator);
		if (!Portal)
		{
			return nullptr;
		}
		Portal->TargetLevelNumber = LevelNumber;
		Portal->PortalLabel = Label;
		if (LevelNumber == 0)
		{
			Portal->SetActorScale3D(FVector(0.25f));
			if (Portal->IconSprite)
			{
				Portal->IconSprite->SetVisibility(false);
				Portal->IconSprite->SetHiddenInGame(true);
			}
			if (Portal->PortalText)
			{
				Portal->PortalText->SetVisibility(false);
				Portal->PortalText->SetHiddenInGame(true);
			}
		}
		return Portal;
	}
	return nullptr;
}

void ACrooniesTreasureTroopGameMode::SpawnHubImageSign(const FVector& Location, UTexture2D* Texture, float Scale, FName Name)
{
	UWorld* World = GetWorld();
	if (!World || !Texture)
	{
		return;
	}

	AActor* Sign = World->SpawnActor<AActor>(Location, FRotator::ZeroRotator);
	if (!Sign)
	{
		return;
	}
#if WITH_EDITOR
	Sign->SetActorLabel(Name.ToString());
#endif
	UBillboardComponent* Billboard = NewObject<UBillboardComponent>(Sign, TEXT("HubLandmarkImage"));
	if (!Billboard)
	{
		return;
	}
	Billboard->SetSprite(Texture);
	Billboard->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Billboard->bIsScreenSizeScaled = false;
	Billboard->SetWorldScale3D(FVector(Scale));
	Sign->SetRootComponent(Billboard);
	Billboard->RegisterComponent();
}

void ACrooniesTreasureTroopGameMode::SpawnGate(const FVector& Location, FName RequiredFriend, const FString& LockedMessage)
{
	if (UWorld* World = GetWorld())
	{
		ACoveGate* Gate = World->SpawnActor<ACoveGate>(Location, FRotator::ZeroRotator);
		Gate->RequiredFriend = RequiredFriend;
		Gate->LockedMessage = LockedMessage;
	}
}

void ACrooniesTreasureTroopGameMode::SpawnTrap(const FVector& Location, ECoveTrapKind TrapKind)
{
	if (UWorld* World = GetWorld())
	{
		ACoveTrap* Trap = World->SpawnActor<ACoveTrap>(Location, FRotator::ZeroRotator);
		if (Trap)
		{
			Trap->TrapKind = TrapKind;
		}
	}
}

void ACrooniesTreasureTroopGameMode::SpawnEnemy(const FVector& Location, FName EnemyId, int32 LevelNumber)
{
	if (UWorld* World = GetWorld())
	{
		const FTransform SpawnTransform(FRotator::ZeroRotator, Location);
		ACoveEnemy* Enemy = World->SpawnActorDeferred<ACoveEnemy>(ACoveEnemy::StaticClass(), SpawnTransform);
		if (!Enemy)
		{
			return;
		}
		Enemy->EnemyId = EnemyId;
		Enemy->LevelNumber = LevelNumber;
		Enemy->FinishSpawning(SpawnTransform);
	}
}

void ACrooniesTreasureTroopGameMode::SpawnCollectibleSet(const FVector& Base, int32 Count, ECovePickupKind Kind, float Spacing)
{
	for (int32 Index = 0; Index < Count; ++Index)
	{
		const FVector Offset(0.f, (Index - Count / 2) * Spacing, 0.f);
		SpawnPickup(Base + Offset, Kind);
	}
}

void ACrooniesTreasureTroopGameMode::SpawnProceduralRunContent(const FVector& Base, const FVector& RoomFloorScale, int32 LevelNumber)
{
	if (LevelNumber < 2)
	{
		return;
	}

	const float HalfX = RoomFloorScale.X * 50.f;
	const float HalfY = RoomFloorScale.Y * 50.f;
	FRandomStream Stream(static_cast<int32>((FDateTime::Now().GetTicks() & 0x7fffffff) ^ (LevelNumber * 7919)));
	const int32 Difficulty = FMath::Clamp(LevelNumber - 1, 1, 11);
	const int32 LootCount = 3 + Difficulty / 2;
	const int32 TrapCount = 2 + Difficulty / 3;
	const int32 EnemyCount = 1 + Difficulty / 4;
	const int32 BoundedEnemyCount = 1 + Difficulty / 3;
	static const FName EnemyCycle[] = { TEXT("Jake"), TEXT("Francis"), TEXT("Mama") };
	static const FName BoundedEnemyCycle[] = { TEXT("Rat"), TEXT("Bat") };

	auto PickPoint = [&]()
	{
		return Base + FVector(
			Stream.FRandRange(-HalfX + 320.f, HalfX - 320.f),
			Stream.FRandRange(-HalfY + 320.f, HalfY - 320.f),
			85.f);
	};

	for (int32 Index = 0; Index < LootCount; ++Index)
	{
		SpawnPickup(PickPoint(), ECovePickupKind::TreasureLoot, FName(*FString::Printf(TEXT("L%dLoot%d"), LevelNumber, Index + 1)));
	}
	for (int32 Index = 0; Index < FMath::Max(1, Difficulty / 3); ++Index)
	{
		SpawnPickup(PickPoint(), ECovePickupKind::StaminaBoost, FName(*FString::Printf(TEXT("L%dStamina%d"), LevelNumber, Index + 1)));
	}
	static const ECoveTrapKind TrapCycle[] = { ECoveTrapKind::Skeleton, ECoveTrapKind::Fire, ECoveTrapKind::Gas, ECoveTrapKind::Hole };
	for (int32 Index = 0; Index < TrapCount; ++Index)
	{
		FVector TrapLocation = PickPoint();
		TrapLocation.Z = 25.f;
		SpawnTrap(TrapLocation, TrapCycle[(LevelNumber + Index) % UE_ARRAY_COUNT(TrapCycle)]);
	}
	for (int32 Index = 0; Index < EnemyCount; ++Index)
	{
		SpawnEnemy(PickPoint(), EnemyCycle[(LevelNumber + Index) % UE_ARRAY_COUNT(EnemyCycle)], LevelNumber);
	}
	for (int32 Index = 0; Index < BoundedEnemyCount; ++Index)
	{
		FVector CritterLocation = PickPoint();
		CritterLocation.Z = BoundedEnemyCycle[(LevelNumber + Index) % UE_ARRAY_COUNT(BoundedEnemyCycle)] == TEXT("Bat") ? 150.f : 70.f;
		SpawnEnemy(CritterLocation, BoundedEnemyCycle[(LevelNumber + Index) % UE_ARRAY_COUNT(BoundedEnemyCycle)], LevelNumber);
	}
}
void ACrooniesTreasureTroopGameMode::SpawnAreaLight(const FVector& Location, float Radius, float Intensity, FName Name)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	APointLight* Light = World->SpawnActor<APointLight>(Location, FRotator::ZeroRotator);
	if (!Light)
	{
		return;
	}
#if WITH_EDITOR
	Light->SetActorLabel(Name.ToString());
#endif
	if (UPointLightComponent* PointLight = Cast<UPointLightComponent>(Light->GetLightComponent()))
	{
		PointLight->SetAttenuationRadius(Radius);
		PointLight->SetIntensity(Intensity);
		PointLight->SetLightColor(FLinearColor(1.f, 0.86f, 0.55f));
	}
}

void ACrooniesTreasureTroopGameMode::SpawnLighting()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	ADirectionalLight* Sun = World->SpawnActor<ADirectionalLight>(FVector(-1200.f, -900.f, 1800.f), FRotator(-48.f, -35.f, 0.f));
	if (Sun)
	{
		Sun->SetActorScale3D(FVector(1.f));
		Sun->GetLightComponent()->SetIntensity(0.0f);
		Sun->GetLightComponent()->SetLightColor(FLinearColor(1.f, 0.91f, 0.74f));
	}

	ASkyLight* Sky = World->SpawnActor<ASkyLight>(FVector::ZeroVector, FRotator::ZeroRotator);
	if (Sky)
	{
		Sky->GetLightComponent()->SetIntensity(0.0f);
		Sky->GetLightComponent()->SetLightColor(FLinearColor(0.58f, 0.72f, 1.f));
	}

	AExponentialHeightFog* Fog = World->SpawnActor<AExponentialHeightFog>(FVector::ZeroVector, FRotator::ZeroRotator);
	if (Fog)
	{
		Fog->GetComponent()->SetFogDensity(0.004f);
		Fog->GetComponent()->SetFogHeightFalloff(0.18f);
		Fog->GetComponent()->SetFogInscatteringColor(FLinearColor(0.28f, 0.42f, 0.62f));
	}
}

void ACrooniesTreasureTroopGameMode::MovePlayerToTutorial()
{
	APawn* Pawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (Pawn)
	{
		Pawn->SetActorLocation(FVector(35.f, 620.f, 140.f), false, nullptr, ETeleportType::TeleportPhysics);
		Pawn->SetActorRotation(FRotator::ZeroRotator);

		if (ACrooniesTreasureTroopCharacter* Character = Cast<ACrooniesTreasureTroopCharacter>(Pawn))
		{
			Character->EnterLevel(1, TEXT("The Old Attic"), TEXT("Tutorial"), TEXT("Follow the glowing attic path: recruit Quip, switch Croonies, solve the switch path, and collect the treasure map."));
		}
	}
}

void ACrooniesTreasureTroopGameMode::SpawnTreeBorder(const FVector& Center, const FVector& FloorScale, int32 Seed, bool bDense)
{
	const float HalfX = FloorScale.X * 50.f;
	const float HalfY = FloorScale.Y * 50.f;
	const int32 CountX = bDense ? 9 : 5;
	const int32 CountY = bDense ? 7 : 4;
	const float GridStepX = (HalfX + 120.f) / 20.f;
	const float GridStepY = (HalfY + 120.f) / 20.f;
	auto ShouldSkipHubTreeCell = [&](const FVector& LocalOffset)
	{
		if (!(bDense && Seed == 11))
		{
			return false;
		}

		const int32 GridX = FMath::FloorToInt(LocalOffset.X / GridStepX);
		const int32 GridY = FMath::FloorToInt(LocalOffset.Y / GridStepY);
		return (GridX == -14 && (GridY == -18 || GridY == 17))
			|| (GridY == 18 && (GridX == -10 || GridX == -9));
	};

	for (int32 Index = 0; Index < CountX; ++Index)
	{
		const float Alpha = CountX <= 1 ? 0.5f : static_cast<float>(Index) / static_cast<float>(CountX - 1);
		const float X = FMath::Lerp(-HalfX + 360.f, HalfX - 360.f, Alpha);
		if (bDense && Seed == 11 && X < -3170.f)
		{
			continue;
		}
		const float Jitter = FMath::Sin((Index + Seed) * 1.73f) * 130.f;
		const float Scale = 0.85f + FMath::Frac((Index + Seed) * 0.318f) * 0.35f;
		const FVector NorthOffset(X, -HalfY + 185.f + Jitter, 0.f);
		const FVector SouthOffset(X, HalfY - 185.f - Jitter, 0.f);
		if (!ShouldSkipHubTreeCell(NorthOffset))
		{
			SpawnTree(Center + NorthOffset, Scale, Seed + Index, FName(*FString::Printf(TEXT("TreeNorth_%d_%d"), Seed, Index)));
		}
		if (!ShouldSkipHubTreeCell(SouthOffset))
		{
			SpawnTree(Center + SouthOffset, Scale * 0.94f, Seed + Index + 41, FName(*FString::Printf(TEXT("TreeSouth_%d_%d"), Seed, Index)));
		}
	}

	for (int32 Index = 0; Index < CountY; ++Index)
	{
		const float Alpha = CountY <= 1 ? 0.5f : static_cast<float>(Index) / static_cast<float>(CountY - 1);
		const float Y = FMath::Lerp(-HalfY + 420.f, HalfY - 420.f, Alpha);
		const float Jitter = FMath::Cos((Index + Seed) * 1.29f) * 120.f;
		const float Scale = 0.75f + FMath::Frac((Index + Seed) * 0.271f) * 0.32f;
		if (!(bDense && Seed == 11))
		{
			SpawnTree(Center + FVector(-HalfX + 185.f + Jitter, Y, 0.f), Scale, Seed + Index + 83, FName(*FString::Printf(TEXT("TreeWest_%d_%d"), Seed, Index)));
		}
		SpawnTree(Center + FVector(HalfX - 185.f - Jitter, Y, 0.f), Scale * 1.02f, Seed + Index + 127, FName(*FString::Printf(TEXT("TreeEast_%d_%d"), Seed, Index)));
	}
}

void ACrooniesTreasureTroopGameMode::SpawnHubGrassDetail(const FVector& Center, const FVector& FloorScale)
{
	const float HalfX = FloorScale.X * 50.f;
	const float HalfY = FloorScale.Y * 50.f;
	const FLinearColor GrassLight(0.18f, 0.54f, 0.27f, 1.f);
	const FLinearColor GrassMid(0.11f, 0.43f, 0.21f, 1.f);
	const FLinearColor GrassDark(0.06f, 0.28f, 0.15f, 1.f);

	for (int32 Index = 0; Index < 120; ++Index)
	{
		const float X = -HalfX + 260.f + FMath::Fmod(Index * 379.f, HalfX * 2.f - 520.f);
		const float Y = -HalfY + 260.f + FMath::Fmod(Index * 233.f, HalfY * 2.f - 520.f);

		// Keep paths and main hub landmarks readable while giving the flat grass a low-poly texture.
		if (FMath::Abs(Y) < 190.f || (Y < -1120.f && FMath::Abs(X) < 3700.f) || (Y > 1180.f && FMath::Abs(X) < 2900.f))
		{
			continue;
		}

		const FLinearColor PatchColor = (Index % 5 == 0) ? GrassLight : ((Index % 3 == 0) ? GrassDark : GrassMid);
		const float PatchX = 0.10f + FMath::Fmod(Index * 0.037f, 0.12f);
		const float PatchY = 0.025f + FMath::Fmod(Index * 0.019f, 0.05f);
		SpawnDecorBlock(Center + FVector(X, Y, 7.f), FVector(PatchX, PatchY, 0.018f), PatchColor, FName(*FString::Printf(TEXT("HubGrassBlade_%d"), Index)));
		SpawnDecorBlock(Center + FVector(X + 22.f, Y + 12.f, 8.f), FVector(PatchY * 2.0f, PatchX * 0.28f, 0.018f), PatchColor, FName(*FString::Printf(TEXT("HubGrassBladeSide_%d"), Index)));
	}

	for (int32 Patch = 0; Patch < 18; ++Patch)
	{
		const float X = -HalfX + 500.f + FMath::Fmod(Patch * 719.f, HalfX * 2.f - 1000.f);
		const float Y = -HalfY + 500.f + FMath::Fmod(Patch * 487.f, HalfY * 2.f - 1000.f);
		if (FMath::Abs(Y) < 240.f)
		{
			continue;
		}
		SpawnDecorBlock(Center + FVector(X, Y, 5.f), FVector(0.95f, 0.42f, 0.012f), FLinearColor(0.07f, 0.37f, 0.18f, 1.f), FName(*FString::Printf(TEXT("HubLowPolyGrassPatch_%d"), Patch)));
	}
}

void ACrooniesTreasureTroopGameMode::SpawnHubPet(const FVector& Location, bool bDog, float Scale, FName Name)
{
	const FLinearColor BodyColor = bDog ? FLinearColor(0.62f, 0.36f, 0.16f, 1.f) : FLinearColor(0.18f, 0.20f, 0.22f, 1.f);
	const FLinearColor AccentColor = bDog ? FLinearColor(0.93f, 0.76f, 0.46f, 1.f) : FLinearColor(0.88f, 0.52f, 0.24f, 1.f);
	const FLinearColor EyeColor(0.02f, 0.025f, 0.03f, 1.f);
	const FString BaseName = Name.ToString();

	SpawnDecorBlock(Location + FVector(0.f, 0.f, 34.f * Scale), FVector(0.54f * Scale, 0.34f * Scale, 0.26f * Scale), BodyColor, FName(*FString::Printf(TEXT("%s_Body"), *BaseName)));
	SpawnDecorBlock(Location + FVector(-38.f * Scale, 0.f, 58.f * Scale), FVector(0.28f * Scale, 0.26f * Scale, 0.28f * Scale), AccentColor, FName(*FString::Printf(TEXT("%s_Head"), *BaseName)));
	SpawnDecorBlock(Location + FVector(-54.f * Scale, -15.f * Scale, 74.f * Scale), FVector(0.08f * Scale, 0.08f * Scale, 0.18f * Scale), BodyColor, FName(*FString::Printf(TEXT("%s_EarL"), *BaseName)));
	SpawnDecorBlock(Location + FVector(-54.f * Scale, 15.f * Scale, 74.f * Scale), FVector(0.08f * Scale, 0.08f * Scale, 0.18f * Scale), BodyColor, FName(*FString::Printf(TEXT("%s_EarR"), *BaseName)));
	SpawnDecorBlock(Location + FVector(-62.f * Scale, -8.f * Scale, 64.f * Scale), FVector(0.035f * Scale, 0.035f * Scale, 0.035f * Scale), EyeColor, FName(*FString::Printf(TEXT("%s_EyeL"), *BaseName)));
	SpawnDecorBlock(Location + FVector(-62.f * Scale, 8.f * Scale, 64.f * Scale), FVector(0.035f * Scale, 0.035f * Scale, 0.035f * Scale), EyeColor, FName(*FString::Printf(TEXT("%s_EyeR"), *BaseName)));
	SpawnDecorBlock(Location + FVector(56.f * Scale, 0.f, 54.f * Scale), bDog ? FVector(0.30f * Scale, 0.06f * Scale, 0.08f * Scale) : FVector(0.08f * Scale, 0.08f * Scale, 0.42f * Scale), BodyColor, FName(*FString::Printf(TEXT("%s_Tail"), *BaseName)));

	for (int32 Leg = 0; Leg < 4; ++Leg)
	{
		const float LegX = (Leg < 2 ? -20.f : 28.f) * Scale;
		const float LegY = (Leg % 2 == 0 ? -18.f : 18.f) * Scale;
		SpawnDecorBlock(Location + FVector(LegX, LegY, 14.f * Scale), FVector(0.08f * Scale, 0.065f * Scale, 0.20f * Scale), BodyColor, FName(*FString::Printf(TEXT("%s_Leg_%d"), *BaseName, Leg)));
	}
}

void ACrooniesTreasureTroopGameMode::SpawnHubPets(const FVector& Center)
{
	SpawnHubPet(Center + FVector(-2650.f, 820.f, 25.f), true, 1.10f, TEXT("HubDogBuddy"));
	SpawnHubPet(Center + FVector(1880.f, 980.f, 25.f), false, 0.95f, TEXT("HubCatMango"));
	SpawnHubPet(Center + FVector(-3150.f, -760.f, 25.f), false, 0.88f, TEXT("HubCatShadow"));
	SpawnHubPet(Center + FVector(2860.f, -1040.f, 25.f), true, 1.00f, TEXT("HubDogScout"));
	SpawnHubPet(Center + FVector(850.f, 2120.f, 25.f), false, 0.82f, TEXT("HubCatPorch"));
}

void ACrooniesTreasureTroopGameMode::SpawnHub()
{
	const FVector HubCenter(-9000.f, 0.f, 0.f);
	const FVector HubFloorScale(90.f, 60.f, 0.50f);
	const float HalfX = HubFloorScale.X * 50.f;
	const float HalfY = HubFloorScale.Y * 50.f;
	const FLinearColor Grass(0.08f, 0.32f, 0.18f);
	const FLinearColor Path(0.50f, 0.40f, 0.26f);
	const FLinearColor Road(0.015f, 0.017f, 0.020f);
	const FLinearColor Sidewalk(0.70f, 0.70f, 0.66f);
	const FLinearColor Wall(0.10f, 0.20f, 0.18f);

	SpawnAreaLight(HubCenter + FVector(0.f, 0.f, 900.f), 6500.f, 90000.f, TEXT("HubNeighborhoodAreaLight"));
	SpawnBlock(HubCenter + FVector(0.f, 0.f, -25.f), HubFloorScale, Grass, TEXT("NeighborhoodHubOneLayerFloor"));
	SpawnTemporaryLayoutGrid(HubCenter, HubFloorScale, TEXT("Hub"));
	SpawnBlock(HubCenter + FVector(0.f, -HalfY, 100.f), FVector(HubFloorScale.X + 0.8f, 0.60f, 2.25f), Wall, TEXT("NeighborhoodNorthBoundary"));
	SpawnBlock(HubCenter + FVector(0.f, HalfY, 100.f), FVector(HubFloorScale.X + 0.8f, 0.60f, 2.25f), Wall, TEXT("NeighborhoodSouthBoundary"));
	SpawnBlock(HubCenter + FVector(-HalfX, 0.f, 100.f), FVector(0.60f, HubFloorScale.Y, 2.25f), Wall, TEXT("NeighborhoodWestBoundary"));
	SpawnBlock(HubCenter + FVector(HalfX, 0.f, 100.f), FVector(0.60f, HubFloorScale.Y, 2.25f), Wall, TEXT("NeighborhoodEastBoundary"));
	SpawnTreeBorder(HubCenter, HubFloorScale, 11, true);
	SpawnHubGrassDetail(HubCenter, HubFloorScale);
	SpawnHubPets(HubCenter);

	const float HubHouseSideRoadEdgeY = 360.f;
	const float HubCaveSideRoadEdgeY = -1100.f;
	const float HubCurbLawnWidth = 55.f;
	const float HubSidewalkWidth = 90.f;
	const float HubRoadStoreEndX = 2850.f;
	const float HubRoadCenterY = (HubHouseSideRoadEdgeY + HubCaveSideRoadEdgeY) * 0.5f;
	const float HubRoadYScale = (HubHouseSideRoadEdgeY - HubCaveSideRoadEdgeY) / 100.f;
	const float HubHouseCurbCenterY = HubHouseSideRoadEdgeY + (HubCurbLawnWidth * 0.5f);
	const float HubCaveCurbCenterY = HubCaveSideRoadEdgeY - (HubCurbLawnWidth * 0.5f);
	const float HubHouseSidewalkCenterY = HubHouseSideRoadEdgeY + HubCurbLawnWidth + (HubSidewalkWidth * 0.5f);
	const float HubCaveSidewalkCenterY = HubCaveSideRoadEdgeY - HubCurbLawnWidth - (HubSidewalkWidth * 0.5f);
	const float HubRoadCapCenterX = HubRoadStoreEndX + (HubSidewalkWidth * 0.5f);
	const float HubRoadCapYScale = ((HubHouseSidewalkCenterY + (HubSidewalkWidth * 0.5f)) - (HubCaveSidewalkCenterY - (HubSidewalkWidth * 0.5f))) / 100.f;
	const float HubDockGrassEdgeX = -2860.f;
	const float HubDockTurnRoadMinX = HubDockGrassEdgeX + HubSidewalkWidth + HubCurbLawnWidth;
	const float HubDockTurnRoadMaxX = HubDockTurnRoadMinX + (HubHouseSideRoadEdgeY - HubCaveSideRoadEdgeY);
	const float HubDockTurnRoadCenterX = (HubDockTurnRoadMinX + HubDockTurnRoadMaxX) * 0.5f;
	const float HubDockTurnRoadStartY = HubHouseSideRoadEdgeY;
	const float HubDockTurnRoadEndY = HalfY;
	const float HubDockTurnRoadCenterY = (HubDockTurnRoadStartY + HubDockTurnRoadEndY) * 0.5f;
	const float HubDockTurnRoadLengthScale = (HubDockTurnRoadEndY - HubDockTurnRoadStartY) / 100.f;
	const float HubDockTurnDockCurbCenterX = HubDockGrassEdgeX + HubSidewalkWidth + (HubCurbLawnWidth * 0.5f);
	const float HubDockTurnDockSidewalkCenterX = HubDockGrassEdgeX + (HubSidewalkWidth * 0.5f);
	const float HubDockTurnOuterCurbCenterX = HubDockTurnRoadMaxX + (HubCurbLawnWidth * 0.5f);
	const float HubDockTurnOuterSidewalkCenterX = HubDockTurnRoadMaxX + HubCurbLawnWidth + (HubSidewalkWidth * 0.5f);
	const float HubDockCornerRoadCenterX = HubDockTurnRoadCenterX;
	const float HubDockCornerRoadCenterY = HubRoadCenterY;
	const float HubDockCornerRoadScale = (HubHouseSideRoadEdgeY - HubCaveSideRoadEdgeY) / 100.f;
	const float HubRoadStartX = HubDockTurnRoadMaxX;
	const float HubRoadCenterX = (HubRoadStartX + HubRoadStoreEndX) * 0.5f;
	const float HubRoadLengthScale = (HubRoadStoreEndX - HubRoadStartX) / 100.f;
	const float HubHouseSidewalkStartX = HubRoadStartX + (HubSidewalkWidth * 0.65f);
	const float HubHouseSidewalkCenterX = (HubHouseSidewalkStartX + HubRoadStoreEndX) * 0.5f;
	const float HubHouseSidewalkLengthScale = (HubRoadStoreEndX - HubHouseSidewalkStartX) / 100.f;
	const float HubDockTurnOuterSidewalkStartY = HubHouseSidewalkCenterY + (HubSidewalkWidth * 0.5f);
	const float HubDockTurnOuterSidewalkCenterY = (HubDockTurnOuterSidewalkStartY + HubDockTurnRoadEndY) * 0.5f;
	const float HubDockTurnOuterSidewalkLengthScale = (HubDockTurnRoadEndY - HubDockTurnOuterSidewalkStartY) / 100.f;

	SpawnDecorBlock(HubCenter + FVector(HubRoadCenterX, HubRoadCenterY, 7.f), FVector(HubRoadLengthScale, HubRoadYScale, 0.075f), Road, TEXT("FullLengthWideAsphaltStreet"));
	SpawnDecorBlock(HubCenter + FVector(HubRoadCenterX, HubCaveCurbCenterY, 8.f), FVector(HubRoadLengthScale, HubCurbLawnWidth / 100.f, 0.055f), Grass, TEXT("LeftCurbLawnStrip"));
	SpawnDecorBlock(HubCenter + FVector(HubRoadCenterX, HubHouseCurbCenterY, 8.f), FVector(HubRoadLengthScale, HubCurbLawnWidth / 100.f, 0.055f), Grass, TEXT("RightCurbLawnStrip"));
	SpawnDecorBlock(HubCenter + FVector(HubRoadCenterX, HubCaveSidewalkCenterY, 10.f), FVector(HubRoadLengthScale + (HubSidewalkWidth / 100.f), HubSidewalkWidth / 100.f, 0.09f), Sidewalk, TEXT("LeftFullLengthSidewalk"));
	SpawnDecorBlock(HubCenter + FVector(HubHouseSidewalkCenterX, HubHouseSidewalkCenterY, 10.f), FVector(HubHouseSidewalkLengthScale, HubSidewalkWidth / 100.f, 0.09f), Sidewalk, TEXT("RightFullLengthSidewalk"));
	SpawnDecorBlock(HubCenter + FVector(HubRoadCapCenterX, HubRoadCenterY, 11.f), FVector(HubSidewalkWidth / 100.f, HubRoadCapYScale, 0.10f), Sidewalk, TEXT("HubStoreRoadEndUSidewalkCap"));
	SpawnDecorBlock(HubCenter + FVector(HubRoadStoreEndX - 24.f, HubRoadCenterY, 12.f), FVector(0.24f, HubRoadYScale - 0.38f, 0.035f), Road, TEXT("HubStoreRoadRoundedEndFace"));
	const float HubGridStepX = ((HubFloorScale.X * 50.f) + 120.f) / 20.f;
	const float HubGridStepY = ((HubFloorScale.Y * 50.f) + 120.f) / 20.f;
	for (int32 GridX = -10; GridX <= 11; ++GridX)
	{
		if (GridX == -10)
		{
			continue;
		}
		const FVector HubDashGridRow(GridX + 0.5f, -3.f + 0.5f, 0.f);
		SpawnDecorBlock(HubCenter + FVector(HubDashGridRow.X * HubGridStepX, HubDashGridRow.Y * HubGridStepY, 21.f), FVector(0.58f, 0.055f, 0.025f), FLinearColor(1.f, 0.82f, 0.18f), FName(*FString::Printf(TEXT("HubDashGrid_%d_Neg3"), GridX)));
	}
	for (int32 GridY = -3; GridY <= 18; ++GridY)
	{
		if (GridY == -3)
		{
			continue;
		}
		const FVector HubDashGridColumn(-10.f + 0.5f, GridY + 0.5f, 0.f);
		SpawnDecorBlock(HubCenter + FVector(HubDashGridColumn.X * HubGridStepX, HubDashGridColumn.Y * HubGridStepY, 21.f), FVector(0.055f, 0.58f, 0.025f), FLinearColor(1.f, 0.82f, 0.18f), FName(*FString::Printf(TEXT("HubDashGrid_Neg10_%d"), GridY)));
	}
	SpawnDecorBlock(HubCenter + FVector(HubDockCornerRoadCenterX, HubDockCornerRoadCenterY, 8.f), FVector(HubDockCornerRoadScale, HubDockCornerRoadScale, 0.075f), Road, TEXT("HubDockEndDedicatedRoadCorner"));
	SpawnDecorBlock(HubCenter + FVector(HubDockCornerRoadCenterX, HubCaveCurbCenterY, 9.f), FVector(HubDockCornerRoadScale, HubCurbLawnWidth / 100.f, 0.055f), Grass, TEXT("HubDockEndCornerCaveCurbLawn"));
	SpawnDecorBlock(HubCenter + FVector(HubDockCornerRoadCenterX, HubCaveSidewalkCenterY, 12.f), FVector(HubDockCornerRoadScale, HubSidewalkWidth / 100.f, 0.09f), Sidewalk, TEXT("HubDockEndCornerCaveSidewalk"));
	SpawnDecorBlock(HubCenter + FVector(HubDockTurnDockCurbCenterX, HubDockCornerRoadCenterY, 9.f), FVector(HubCurbLawnWidth / 100.f, HubDockCornerRoadScale, 0.055f), Grass, TEXT("HubDockEndCornerDockCurbLawn"));
	SpawnDecorBlock(HubCenter + FVector(HubDockTurnDockSidewalkCenterX, HubDockCornerRoadCenterY, 12.f), FVector(HubSidewalkWidth / 100.f, HubDockCornerRoadScale, 0.09f), Sidewalk, TEXT("HubDockEndCornerDockSidewalk"));
	SpawnDecorBlock(HubCenter + FVector(HubDockTurnDockSidewalkCenterX + 56.f, HubCaveSidewalkCenterY, 13.f), FVector((HubSidewalkWidth + 112.f) / 100.f, HubSidewalkWidth / 100.f, 0.09f), Sidewalk, TEXT("HubDockEndCornerSidewalkConnector"));
	SpawnDecorBlock(HubCenter + FVector(HubDockTurnRoadCenterX, HubDockTurnRoadCenterY, 9.f), FVector(HubRoadYScale, HubDockTurnRoadLengthScale, 0.08f), Road, TEXT("HubDockEndRoadAtticTurnStraight"));
	SpawnDecorBlock(HubCenter + FVector(HubDockTurnDockCurbCenterX, HubDockTurnRoadCenterY, 10.f), FVector(HubCurbLawnWidth / 100.f, HubDockTurnRoadLengthScale, 0.055f), Grass, TEXT("HubDockEndTurnDockCurbLawn"));
	SpawnDecorBlock(HubCenter + FVector(HubDockTurnOuterCurbCenterX, HubDockTurnRoadCenterY, 10.f), FVector(HubCurbLawnWidth / 100.f, HubDockTurnRoadLengthScale, 0.055f), Grass, TEXT("HubDockEndTurnOuterCurbLawn"));
	SpawnDecorBlock(HubCenter + FVector(HubDockTurnDockSidewalkCenterX, HubDockTurnRoadCenterY, 12.f), FVector(HubSidewalkWidth / 100.f, HubDockTurnRoadLengthScale, 0.09f), Sidewalk, TEXT("HubDockEndTurnDockAlignedSidewalk"));
	SpawnDecorBlock(HubCenter + FVector(HubDockTurnOuterSidewalkCenterX, HubDockTurnOuterSidewalkCenterY, 12.f), FVector(HubSidewalkWidth / 100.f, HubDockTurnOuterSidewalkLengthScale, 0.09f), Sidewalk, TEXT("HubDockEndTurnOuterSidewalk"));
	SpawnDecorBlock(HubCenter + FVector(0.f, -1225.f, 3.f), FVector(62.f, 0.70f, 0.05f), FLinearColor(0.86f, 0.66f, 0.22f), TEXT("StoryPortalGuideLine"));
	SpawnDecorBlock(HubCenter + FVector(0.f, 1425.f, 3.f), FVector(48.f, 0.70f, 0.05f), FLinearColor(0.24f, 0.62f, 0.88f), TEXT("BonusPortalGuideLine"));
	SpawnDecorBlock(HubCenter + FVector(0.f, -2500.f, 3.f), FVector(58.f, 0.52f, 0.05f), FLinearColor(0.96f, 0.78f, 0.28f), TEXT("CollectionShowcaseGuideLine"));
	SpawnDecorBlock(HubCenter + FVector(0.f, 2450.f, 3.f), FVector(46.f, 0.52f, 0.05f), FLinearColor(0.22f, 0.72f, 0.46f), TEXT("CrewUpgradeGuideLine"));

	const FLinearColor DockWood(0.74f, 0.54f, 0.32f);
	const FLinearColor DockWoodDark(0.20f, 0.10f, 0.045f);
	const FLinearColor HubWater(0.02f, 0.42f, 0.58f);
	const float HubDockWidth = 310.f;
	const float HubDockWaterEdgeX = HubDockGrassEdgeX - HubDockWidth;
	const float HubDockCenterX = (HubDockGrassEdgeX + HubDockWaterEdgeX) * 0.5f;
	const float HubWaterInnerEdgeX = HubDockWaterEdgeX;
	const float HubWaterOuterEdgeX = -HalfX;
	const float HubWaterCenterX = (HubWaterInnerEdgeX + HubWaterOuterEdgeX) * 0.5f;
	const float HubWaterWidth = HubWaterInnerEdgeX - HubWaterOuterEdgeX;
	const float HubCrossLengthScale = HubFloorScale.Y + 0.20f;
	SpawnDecorBlock(HubCenter + FVector(HubWaterCenterX, 0.f, 16.f), FVector(HubWaterWidth / 100.f, HubCrossLengthScale, 0.055f), HubWater, TEXT("FullLengthHubWaterChannel"));
	SpawnBlock(HubCenter + FVector(HubDockCenterX, 0.f, 19.f), FVector(HubDockWidth / 100.f, HubCrossLengthScale, 0.12f), DockWood, TEXT("FullLengthWalkableWoodenDock"));
	SpawnDecorBlock(HubCenter + FVector(HubDockWaterEdgeX - 12.f, 0.f, 24.f), FVector(0.24f, HubCrossLengthScale, 0.045f), DockWoodDark, TEXT("HubDockWaterOverhangLip"));
	for (int32 Plank = -22; Plank <= 22; ++Plank)
	{
		SpawnDecorBlock(HubCenter + FVector(HubDockCenterX, Plank * 135.f, 28.f), FVector(HubDockWidth / 100.f, 0.035f, 0.035f), DockWoodDark, TEXT("HubDockEndToEndPlankSeam"));
	}
	for (int32 Wave = -20; Wave <= 20; ++Wave)
	{
		const float WaveX = HubWaterCenterX + ((Wave % 2 == 0) ? -150.f : 135.f);
		const float WaveY = Wave * 145.f;
		SpawnDecorBlock(HubCenter + FVector(WaveX, WaveY, 11.f), FVector(0.045f, 0.72f, 0.025f), FLinearColor(0.08f, 0.68f, 0.86f), TEXT("HubWaterSurfaceRipple"));
	}

	struct FHubGateSpec
	{
		int32 TargetLevel;
		const TCHAR* Label;
		FVector Offset;
	};

	const FHubGateSpec MainGates[] = {
		{1, TEXT("Old Attic Tutorial"), FVector(-700.f, 850.f, 55.f)},
		{-1, TEXT("Caverns Entrance"), FVector(-1200.f, -1550.f, 55.f)},
		{-4, TEXT("Daily Tasks"), FVector(0.f, -1550.f, 55.f)},
		{-3, TEXT("Local Gear Store"), FVector(1200.f, -1550.f, 55.f)},
		{-2, TEXT("Special Levels"), FVector(2400.f, -1550.f, 55.f)},
		{-5, TEXT("Adventure Shop"), FVector(3400.f, -800.f, 55.f)}
	};

	for (int32 Index = 0; Index < UE_ARRAY_COUNT(MainGates); ++Index)
	{
		const FVector PortalLocation = HubCenter + MainGates[Index].Offset;
		ACoveHubPortal* Portal = SpawnHubPortal(PortalLocation, MainGates[Index].TargetLevel, MainGates[Index].Label);
		UTexture2D* PortalTexture = nullptr;
		if (MainGates[Index].TargetLevel == -1)
		{
			PortalTexture = HubCaveEntranceTexture;
		}
		else if (MainGates[Index].TargetLevel == -4)
		{
			PortalTexture = HubTaskBoardTexture;
		}
		else if (MainGates[Index].TargetLevel == -3)
		{
			PortalTexture = HubGearStoreTexture;
		}
		else if (MainGates[Index].TargetLevel == -2)
		{
			PortalTexture = HubSpecialAreaTexture;
		}
		else if (MainGates[Index].TargetLevel == -5)
		{
			PortalTexture = HubPotionShopTexture;
		}
		if (PortalTexture)
		{
			if (Portal)
			{
				Portal->PortalIconTexture = PortalTexture;
				if (Portal->IconSprite)
				{
					Portal->IconSprite->SetSprite(PortalTexture);
					Portal->IconSprite->SetVisibility(true);
					Portal->IconSprite->SetHiddenInGame(false);
				}
			}
		}
		if (MainGates[Index].TargetLevel == 1)
		{
			if (OldAtticHouseMesh)
			{
				AStaticMeshActor* House = GetWorld() ? GetWorld()->SpawnActor<AStaticMeshActor>(PortalLocation + FVector(0.f, -40.f, 0.f), FRotator(0.f, -205.f, 0.f)) : nullptr;
				if (House)
				{
					#if WITH_EDITOR
					House->SetActorLabel(TEXT("OldAtticCartoonHousePortalLandmark"));
					#endif
					House->GetStaticMeshComponent()->SetStaticMesh(OldAtticHouseMesh);
					House->GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
					House->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
					House->SetActorScale3D(FVector(0.78f));
				}
			}
			else
			{
				const FVector HouseBase = PortalLocation + FVector(0.f, -40.f, 0.f);
				SpawnDecorBlock(HouseBase + FVector(0.f, 0.f, 92.f), FVector(3.4f, 2.8f, 1.75f), FLinearColor(0.92f, 0.58f, 0.26f), TEXT("OldAtticFallbackCartoonHouseBody"));
				SpawnDecorBlock(HouseBase + FVector(0.f, 0.f, 218.f), FVector(3.9f, 3.3f, 0.58f), FLinearColor(0.42f, 0.16f, 0.075f), TEXT("OldAtticFallbackCartoonHouseRoof"));
				SpawnDecorBlock(HouseBase + FVector(-106.f, -138.f, 68.f), FVector(0.78f, 0.10f, 0.92f), FLinearColor(0.26f, 0.11f, 0.045f), TEXT("OldAtticFallbackCartoonHouseDoor"));
				SpawnDecorBlock(HouseBase + FVector(92.f, -142.f, 96.f), FVector(0.62f, 0.08f, 0.52f), FLinearColor(0.56f, 0.82f, 1.f), TEXT("OldAtticFallbackCartoonHouseWindow"));
			}
		}
	}

	const FVector CrewChallengeGridCell(2.5f, 5.5f, 0.f);
	const FVector CrewChallengeLocation = HubCenter + FVector(CrewChallengeGridCell.X * HubGridStepX, CrewChallengeGridCell.Y * HubGridStepY, 55.f);
	ACoveHubPortal* CrewChallengePortal = SpawnHubPortal(CrewChallengeLocation, 201, TEXT("Crew Challenge"));
	if (CrewChallengePortal)
	{
		CrewChallengePortal->SetActorScale3D(FVector(0.82f));
		CrewChallengePortal->PortalIconTexture = nullptr;
		if (CrewChallengePortal->IconSprite)
		{
			CrewChallengePortal->IconSprite->SetVisibility(false);
			CrewChallengePortal->IconSprite->SetHiddenInGame(true);
		}
	}
	SpawnDecorBlock(CrewChallengeLocation + FVector(0.f, 0.f, -28.f), FVector(1.60f, 0.28f, 0.24f), FLinearColor(0.07f, 0.08f, 0.10f), TEXT("CrewChallengeBattleStandardBase"));
	SpawnDecorBlock(CrewChallengeLocation + FVector(-72.f, 0.f, 58.f), FVector(0.10f, 0.12f, 1.28f), FLinearColor(0.18f, 0.10f, 0.04f), TEXT("CrewChallengeBattleStandardBluePole"));
	SpawnDecorBlock(CrewChallengeLocation + FVector(72.f, 0.f, 58.f), FVector(0.10f, 0.12f, 1.28f), FLinearColor(0.18f, 0.10f, 0.04f), TEXT("CrewChallengeBattleStandardRedPole"));
	SpawnDecorBlock(CrewChallengeLocation + FVector(-42.f, -12.f, 132.f), FVector(0.56f, 0.045f, 0.58f), FLinearColor(0.05f, 0.22f, 0.85f), TEXT("CrewChallengeBlueCrewBanner"));
	SpawnDecorBlock(CrewChallengeLocation + FVector(42.f, -12.f, 132.f), FVector(0.56f, 0.045f, 0.58f), FLinearColor(0.82f, 0.08f, 0.08f), TEXT("CrewChallengeRedCrewBanner"));
	SpawnDecorBlock(CrewChallengeLocation + FVector(-42.f, -18.f, 166.f), FVector(0.40f, 0.035f, 0.05f), FLinearColor(0.95f, 0.78f, 0.20f), TEXT("CrewChallengeBlueBannerGoldTrim"));
	SpawnDecorBlock(CrewChallengeLocation + FVector(42.f, -18.f, 166.f), FVector(0.40f, 0.035f, 0.05f), FLinearColor(0.95f, 0.78f, 0.20f), TEXT("CrewChallengeRedBannerGoldTrim"));
	SpawnDecorBlock(CrewChallengeLocation + FVector(0.f, -18.f, 122.f), FVector(0.28f, 0.045f, 0.24f), FLinearColor(0.96f, 0.72f, 0.12f), TEXT("CrewChallengeCenterTrophyCup"));
	SpawnDecorBlock(CrewChallengeLocation + FVector(0.f, -18.f, 152.f), FVector(0.42f, 0.045f, 0.10f), FLinearColor(0.96f, 0.72f, 0.12f), TEXT("CrewChallengeCenterTrophyCrown"));
	SpawnDecorBlock(CrewChallengeLocation + FVector(-24.f, -20.f, 152.f), FVector(0.07f, 0.035f, 0.20f), FLinearColor(0.96f, 0.72f, 0.12f), TEXT("CrewChallengeTrophyLeftPoint"));
	SpawnDecorBlock(CrewChallengeLocation + FVector(24.f, -20.f, 152.f), FVector(0.07f, 0.035f, 0.20f), FLinearColor(0.96f, 0.72f, 0.12f), TEXT("CrewChallengeTrophyRightPoint"));
	SpawnDecorBlock(CrewChallengeLocation + FVector(0.f, -20.f, 84.f), FVector(0.92f, 0.04f, 0.08f), FLinearColor(0.95f, 0.95f, 0.92f), TEXT("CrewChallengeVersusBar"));

	SpawnDecorBlock(HubCenter + FVector(-2000.f, -2500.f, 22.f), FVector(3.4f, 0.42f, 0.42f), FLinearColor(1.f, 0.72f, 0.06f), TEXT("CoinCollectionShowcase"));
	SpawnDecorBlock(HubCenter + FVector(-1000.f, -2500.f, 22.f), FVector(3.4f, 0.42f, 0.42f), FLinearColor(1.f, 0.48f, 0.12f), TEXT("MapCollectionShowcase"));
	SpawnDecorBlock(HubCenter + FVector(0.f, -2500.f, 22.f), FVector(3.4f, 0.42f, 0.42f), FLinearColor(0.90f, 0.76f, 0.34f), TEXT("ArtifactCollectionShowcase"));
	SpawnDecorBlock(HubCenter + FVector(1000.f, -2500.f, 22.f), FVector(3.4f, 0.42f, 0.42f), FLinearColor(1.f, 0.20f, 0.52f), TEXT("FriendshipCollectionShowcase"));
	SpawnDecorBlock(HubCenter + FVector(2000.f, -2500.f, 22.f), FVector(3.4f, 0.42f, 0.42f), FLinearColor(0.56f, 0.28f, 1.f), TEXT("UpgradeCollectionShowcase"));

	SpawnPickup(HubCenter + FVector(-2000.f, 2450.f, 80.f), ECovePickupKind::Friend, TEXT("Mouth"));
	SpawnPickup(HubCenter + FVector(-1400.f, 2450.f, 80.f), ECovePickupKind::Friend, TEXT("Data"));
	SpawnPickup(HubCenter + FVector(-800.f, 2450.f, 80.f), ECovePickupKind::Friend, TEXT("Brand"));
	SpawnPickup(HubCenter + FVector(-200.f, 2450.f, 80.f), ECovePickupKind::Upgrade, TEXT("StarterFlashlight"));
	SpawnPickup(HubCenter + FVector(400.f, 2450.f, 80.f), ECovePickupKind::Upgrade, TEXT("PatchworkBackpack"));
}

void ACrooniesTreasureTroopGameMode::SpawnStoryLevels()
{
	const FLinearColor CaveFloor(0.05f, 0.10f, 0.11f);
	const FLinearColor Wall(0.18f, 0.20f, 0.19f);
	const FLinearColor Water(0.02f, 0.24f, 0.36f);
	const FLinearColor Crystal(0.18f, 0.85f, 0.95f);
	const FLinearColor ShipWood(0.32f, 0.16f, 0.08f);

	struct FLevelSpec
	{
		int32 Number;
		float X;
		const TCHAR* Title;
		const TCHAR* Style;
		const TCHAR* Objective;
	};

	const FLevelSpec Levels[] = {
		{1, 650.f, TEXT("The Old Attic"), TEXT("Tutorial switches"), TEXT("Practice crew switching, collect the attic map, and open the first trapdoor.")},
		{2, 6000.f, TEXT("The Rusty Lighthouse"), TEXT("Vertical climb"), TEXT("Use Data's repairs, dodge debris, and find the hidden friend room.")},
		{3, 12000.f, TEXT("The Flooded Tunnels"), TEXT("Water exploration"), TEXT("Drain water paths, push crates with Brand, and recruit Stef.")},
		{4, 18000.f, TEXT("The Collapsing Mines"), TEXT("Isometric exploration"), TEXT("Cross timed platforms and survive the first Grumpelli chase beat.")},
		{5, 24000.f, TEXT("The Bat Caverns"), TEXT("Dark flashlight maze"), TEXT("Use flashlight upgrades to scatter bat swarms and read echo clues.")},
		{6, 30000.f, TEXT("Crystal Caverns"), TEXT("Light reflection puzzle"), TEXT("Bounce light through crystals and break fragile treasure walls.")},
		{7, 36000.f, TEXT("The Mirror Maze"), TEXT("Grid puzzle dungeon"), TEXT("Solve moving mirror paths and unlock the Super Flashlight.")},
		{8, 42000.f, TEXT("Ancient Pirate Outpost"), TEXT("Trap gauntlet"), TEXT("Clear spikes, boulders, rope hooks, and a full-team treasure trial.")},
		{9, 48000.f, TEXT("The Great Chasm"), TEXT("Chase platforming"), TEXT("Swing over gaps while the Grumpelli crew closes in.")},
		{10, 54000.f, TEXT("The Underground River"), TEXT("Raft and cave route"), TEXT("Push logs, read currents, and steer the raft path.")},
		{11, 60000.f, TEXT("Neighborhood Getaway"), TEXT("Auto-runner chase"), TEXT("Dodge street obstacles, collect coins, and use teamwork boosts.")},
		{12, 66000.f, TEXT("Cyclops Captain's Shipwreck"), TEXT("Final boss puzzle"), TEXT("Survive the tilting ship, open the treasure room, and beat the boss trial.")}
	};

	for (const FLevelSpec& Level : Levels)
	{
		const float StoryLevelScale = Level.Number == 1 ? 1.f : 10.f;
		const FVector Base(Level.Number == 1 ? 650.f : Level.X * StoryLevelScale, 0.f, 0.f);
		auto ScaledStoryOffset = [StoryLevelScale](const FVector& Offset)
		{
			return FVector(Offset.X * StoryLevelScale, Offset.Y * StoryLevelScale, Offset.Z);
		};
		const FLinearColor FloorColor = Level.Number >= 12 ? ShipWood : (Level.Number == 3 || Level.Number == 10 ? Water : CaveFloor);
		if (Level.Number != 1)
		{
			const bool bWideLevel = Level.Number == 9 || Level.Number == 10 || Level.Number == 11 || Level.Number == 12;
			const FVector RoomFloorScale((bWideLevel ? 40.0f : 32.0f) * StoryLevelScale, (bWideLevel ? 22.0f : 20.0f) * StoryLevelScale, 0.50f);
			const float HalfX = RoomFloorScale.X * 50.f;
			const float HalfY = RoomFloorScale.Y * 50.f;
			SpawnAreaLight(Base + FVector(0.f, 0.f, 760.f), (bWideLevel ? 3600.f : 3000.f) * StoryLevelScale, (bWideLevel ? 42000.f : 34000.f) * StoryLevelScale, FName(*FString::Printf(TEXT("Level%dAreaLight"), Level.Number)));
			SpawnBlock(Base + FVector(0.f, 0.f, -25.f), RoomFloorScale, FloorColor, TEXT("SealedLevelOnePieceFloor"));
			SpawnTemporaryLayoutGrid(Base, RoomFloorScale, FString::Printf(TEXT("Level%02d"), Level.Number));
			SpawnBlock(Base + FVector(0.f, -HalfY, 100.f), FVector(RoomFloorScale.X + 0.8f, 0.60f, 2.25f), Wall, TEXT("SealedLevelNorthWall"));
			SpawnBlock(Base + FVector(0.f, HalfY, 100.f), FVector(RoomFloorScale.X + 0.8f, 0.60f, 2.25f), Wall, TEXT("SealedLevelSouthWall"));
			SpawnBlock(Base + FVector(-HalfX, 0.f, 100.f), FVector(0.60f, RoomFloorScale.Y, 2.25f), Wall, TEXT("SealedLevelWestWall"));
			SpawnBlock(Base + FVector(HalfX, 0.f, 100.f), FVector(0.60f, RoomFloorScale.Y, 2.25f), Wall, TEXT("SealedLevelEastWall"));
			if (Level.Number == 2 || Level.Number == 9 || Level.Number == 11)
			{
				SpawnTreeBorder(Base, RoomFloorScale, Level.Number * 19, false);
			}
		}
		if (Level.Number != 1)
		{
			SpawnLevelZone(Base + ScaledStoryOffset(FVector(-420.f, 0.f, 80.f)), Level.Number, Level.Title, Level.Style, Level.Objective);
			const bool bProceduralWideLevel = Level.Number == 9 || Level.Number == 10 || Level.Number == 11 || Level.Number == 12;
			const FVector ProceduralFloorScale((bProceduralWideLevel ? 40.0f : 32.0f) * StoryLevelScale, (bProceduralWideLevel ? 22.0f : 20.0f) * StoryLevelScale, 0.50f);
			SpawnProceduralRunContent(Base, ProceduralFloorScale, Level.Number);
		}

		if (Level.Number >= 2 && Level.Number <= 12)
		{
			SpawnPickup(Base + ScaledStoryOffset(FVector(210.f, 260.f, 80.f)), ECovePickupKind::Artifact, FName(*FString::FromInt(Level.Number)));
		}
		if (Level.Number >= 2 && Level.Number <= 8)
		{
			SpawnPickup(Base + ScaledStoryOffset(FVector(560.f, 260.f, 80.f)), ECovePickupKind::FriendshipToken);
		}

		if (Level.Number >= 2 && Level.Number <= 12)
		{
			const int32 TargetMapLevel = FMath::Clamp(Level.Number + 1, 2, 12);
			const FVector MapBase = Base + ScaledStoryOffset(FVector(-330.f, -250.f, 85.f));
			for (int32 Piece = 0; Piece < 4; ++Piece)
			{
				const FVector Offset = ScaledStoryOffset(FVector(Piece * 170.f, (Piece % 2 == 0) ? 0.f : 185.f, 0.f));
				const FName FragmentId(*FString::Printf(TEXT("%d.%d"), TargetMapLevel, Piece + 1));
				const FVector MapPieceLocation = MapBase + Offset;
				SpawnPickup(MapPieceLocation, ECovePickupKind::MapPiece, FragmentId);
				if (Piece == 3)
				{
					const FVector ExitBase = MapPieceLocation + ScaledStoryOffset(FVector(76.f, 0.f, 0.f));
					SpawnBlock(ExitBase + FVector(-90.f, -80.f, 8.f), FVector(0.80f, 1.35f, 0.12f), Wall, FName(*FString::Printf(TEXT("Level%dHubExitStairStep1"), Level.Number)));
					SpawnBlock(ExitBase + FVector(-30.f, -80.f, 30.f), FVector(0.80f, 1.35f, 0.12f), Wall, FName(*FString::Printf(TEXT("Level%dHubExitStairStep2"), Level.Number)));
					SpawnBlock(ExitBase + FVector(30.f, -80.f, 52.f), FVector(0.80f, 1.35f, 0.12f), Wall, FName(*FString::Printf(TEXT("Level%dHubExitStairStep3"), Level.Number)));
					SpawnHubPortal(ExitBase + FVector(90.f, -80.f, 70.f), 0, TEXT("Neighborhood Hub"));
				}
			}
		}

		if (Level.Number >= 2 && Level.Number <= 12)
		{
			static const FName EnemyCycle[] = {
				TEXT("Jake"), TEXT("Francis"), TEXT("Mama")
			};
			SpawnEnemy(Base + ScaledStoryOffset(FVector(-120.f, -320.f, 90.f)), EnemyCycle[(Level.Number - 2) % UE_ARRAY_COUNT(EnemyCycle)], Level.Number);
		}

		if (Level.Number == 1)
		{
			const FVector AtticCenter(300.f, 0.f, 0.f);
			const FVector RoomOrigin = Base + AtticCenter;
			const FVector AtticFloorScale(34.4f, 23.0f, 0.50f);
			const float HalfX = AtticFloorScale.X * 50.f;
			const float HalfY = AtticFloorScale.Y * 50.f;
			const FLinearColor AtticFloor(0.31f, 0.19f, 0.085f);
			const FLinearColor AtticWall(0.34f, 0.23f, 0.14f);
			const FLinearColor AtticBeam(0.17f, 0.085f, 0.035f);
			const FLinearColor Cardboard(0.58f, 0.36f, 0.17f);
			const FLinearColor CardboardDark(0.34f, 0.20f, 0.085f);
			SpawnBlock(RoomOrigin + FVector(0.f, 0.f, -25.f), AtticFloorScale, AtticFloor, TEXT("OldAtticOneSolidWoodFloor"));
			SpawnTemporaryLayoutGrid(RoomOrigin, AtticFloorScale, TEXT("OldAttic"));

			const float ShelfHalfY = HalfY * 0.5f;
			const float ShelfInnerY = -HalfY - 8.f;
			const float ShelfOuterY = ShelfInnerY - (ShelfHalfY * 2.f);
			const float ShelfCenterX = 0.f;
			const float ShelfCenterY = (ShelfOuterY + ShelfInnerY) * 0.5f;
			const float ShelfLeftX = -HalfX + 18.f;
			const float ShelfRightX = HalfX - 18.f;
			const float LadderX = -HalfX * 0.10f;
			const float LadderGapHalfWidth = 165.f;
			const float SharedWallGapLeft = LadderX - LadderGapHalfWidth;
			const float SharedWallGapRight = LadderX + LadderGapHalfWidth;

			SpawnBlock(RoomOrigin + FVector((-HalfX + SharedWallGapLeft) * 0.5f, -HalfY, 45.f), FVector((SharedWallGapLeft - (-HalfX)) / 100.f, 0.60f, 1.10f), AtticWall, TEXT("OldAtticNorthConnectedWoodWallLeftSegment"));
			SpawnBlock(RoomOrigin + FVector((SharedWallGapRight + HalfX) * 0.5f, -HalfY, 45.f), FVector((HalfX - SharedWallGapRight) / 100.f, 0.60f, 1.10f), AtticWall, TEXT("OldAtticNorthConnectedWoodWallRightSegment"));
			SpawnBlock(RoomOrigin + FVector(0.f, HalfY, 226.f), FVector(AtticFloorScale.X + 0.8f, 0.60f, 4.72f), AtticWall, TEXT("OldAtticSouthConnectedWoodWall"));
			SpawnBlock(RoomOrigin + FVector(-HalfX, 0.f, 226.f), FVector(0.60f, AtticFloorScale.Y, 4.72f), AtticWall, TEXT("OldAtticWestConnectedWoodWall"));
			SpawnBlock(RoomOrigin + FVector(HalfX, 0.f, 226.f), FVector(0.60f, AtticFloorScale.Y, 4.72f), AtticWall, TEXT("OldAtticEastConnectedWoodWall"));
			for (int32 Line = -6; Line <= 6; ++Line)
			{
				SpawnDecorBlock(RoomOrigin + FVector(0.f, Line * 150.f, 8.f), FVector(28.2f, 0.035f, 0.045f), AtticBeam, TEXT("OldAtticFloorPlankSeam"));
			}
			for (int32 Beam = -4; Beam <= 4; ++Beam)
			{
				SpawnDecorBlock(RoomOrigin + FVector(Beam * 300.f, -HalfY + 18.f, 190.f), FVector(0.055f, 0.18f, 1.40f), AtticBeam, TEXT("OldAtticNorthWallStud"));
				SpawnDecorBlock(RoomOrigin + FVector(Beam * 300.f, HalfY - 18.f, 190.f), FVector(0.055f, 0.18f, 1.40f), AtticBeam, TEXT("OldAtticSouthWallStud"));
			}

			SpawnBlock(RoomOrigin + FVector(ShelfCenterX, ShelfCenterY, 225.f), FVector((HalfX / 50.f) - 0.35f, ShelfHalfY / 50.f, 0.34f), AtticFloor, TEXT("OldAtticRaisedShelfDiagramWalkableFloor"));
			SpawnBlock(RoomOrigin + FVector(ShelfCenterX, ShelfOuterY, 352.f), FVector((HalfX / 50.f) - 0.35f, 0.60f, 2.20f), AtticWall, TEXT("OldAtticRaisedShelfOuterWall"));
			SpawnBlock(RoomOrigin + FVector(-HalfX, ShelfCenterY, 352.f), FVector(0.60f, ShelfHalfY / 50.f, 2.20f), AtticWall, TEXT("OldAtticRaisedShelfLeftEndWall"));
			SpawnBlock(RoomOrigin + FVector(HalfX, ShelfCenterY, 352.f), FVector(0.60f, ShelfHalfY / 50.f, 2.20f), AtticWall, TEXT("OldAtticRaisedShelfRightEndWall"));
			SpawnBlock(RoomOrigin + FVector((-HalfX + SharedWallGapLeft) * 0.5f, ShelfInnerY, 297.f), FVector((SharedWallGapLeft - (-HalfX)) / 100.f, 0.36f, 1.10f), AtticWall, TEXT("OldAtticRaisedShelfInnerWallLeftSegment"));
			SpawnBlock(RoomOrigin + FVector((SharedWallGapRight + HalfX) * 0.5f, ShelfInnerY, 297.f), FVector((HalfX - SharedWallGapRight) / 100.f, 0.36f, 1.10f), AtticWall, TEXT("OldAtticRaisedShelfInnerWallRightSegment"));
			const float LadderStartY = -HalfY + 360.f;
			const float LadderStepSpacing = 82.f;
			const float LadderStepTopZ[] = { 44.f, 84.f, 124.f, 164.f, 204.f, 242.f };
			for (int32 StepIndex = 0; StepIndex < UE_ARRAY_COUNT(LadderStepTopZ); ++StepIndex)
			{
				const float StepY = LadderStartY - (StepIndex * LadderStepSpacing);
				const float StepTop = LadderStepTopZ[StepIndex];
				SpawnBlock(RoomOrigin + FVector(LadderX, StepY, StepTop - 8.f), FVector(1.85f, 0.62f, 0.16f), AtticBeam, FName(*FString::Printf(TEXT("OldAtticShelfLadderStep%d"), StepIndex + 1)));
			}
			SpawnBlock(RoomOrigin + FVector(LadderX, ShelfInnerY - 42.f, 235.f), FVector(1.85f, 0.92f, 0.16f), AtticBeam, TEXT("OldAtticShelfLadderLanding"));
			auto SpawnCardboardBox = [&](const FVector& Local, const FVector& Scale, FName Name)
			{
				SpawnBlock(RoomOrigin + Local, Scale, Cardboard, Name);
				SpawnDecorBlock(RoomOrigin + Local + FVector(0.f, 0.f, Scale.Z * 52.f), FVector(Scale.X * 0.72f, 0.025f, 0.035f), CardboardDark, TEXT("OldAtticBoxTape"));
			};
			auto SpawnMapPuzzlePen = [&](const FVector& Center, int32 EntrySide, const TCHAR* Prefix)
			{
				const float WallZ = Center.Z + 112.f;
				const float LowZ = Center.Z + 58.f;
				const FVector WallBlocks[] = {
					FVector(-150.f, 0.f, WallZ), FVector(150.f, 0.f, WallZ), FVector(0.f, -150.f, WallZ), FVector(0.f, 150.f, WallZ),
					FVector(-150.f, -150.f, WallZ), FVector(150.f, -150.f, WallZ), FVector(-150.f, 150.f, WallZ), FVector(150.f, 150.f, WallZ)
				};
				const int32 EntryIndex = EntrySide == 0 ? 3 : 0;
				for (int32 Index = 0; Index < UE_ARRAY_COUNT(WallBlocks); ++Index)
				{
					if (Index == EntryIndex)
					{
						continue;
					}
					SpawnCardboardBox(FVector(Center.X, Center.Y, 0.f) + WallBlocks[Index], FVector(0.72f, 0.72f, 2.20f), FName(*FString::Printf(TEXT("%sTallBlocker%d"), Prefix, Index)));
				}
				const FVector EntryOffset = EntrySide == 0 ? FVector(0.f, 150.f, LowZ) : FVector(-150.f, 0.f, LowZ);
				SpawnCardboardBox(FVector(Center.X, Center.Y, 0.f) + EntryOffset, FVector(0.72f, 0.72f, 0.92f), FName(*FString::Printf(TEXT("%sJumpableEntryBlock"), Prefix)));
			};
			auto SpawnShelfMapFragmentPen = [&](const FVector& Center, const TCHAR* Prefix, bool bEntryOnNegativeY)
			{
				const float BlockZ = 360.f;
				const FVector BlockScale(0.72f, 0.72f, 2.15f);
				const FVector LowEntryOffset = bEntryOnNegativeY ? FVector(170.f, -170.f, 310.f) : FVector(0.f, 170.f, 310.f);
				const FVector Blocks[] = {
					FVector(-170.f, -170.f, BlockZ), FVector(0.f, -170.f, BlockZ), FVector(170.f, -170.f, BlockZ),
					FVector(-170.f, 0.f, BlockZ), FVector(170.f, 0.f, BlockZ),
					FVector(-170.f, 170.f, BlockZ), FVector(170.f, 170.f, BlockZ)
				};
				for (int32 Index = 0; Index < UE_ARRAY_COUNT(Blocks); ++Index)
				{
					if (FMath::IsNearlyEqual(Blocks[Index].X, LowEntryOffset.X) && FMath::IsNearlyEqual(Blocks[Index].Y, LowEntryOffset.Y))
					{
						continue;
					}
					SpawnCardboardBox(Center + Blocks[Index], BlockScale, FName(*FString::Printf(TEXT("%sCardboardBlocker%d"), Prefix, Index)));
				}
				SpawnCardboardBox(Center + LowEntryOffset, FVector(0.64f, 0.64f, 1.05f), FName(*FString::Printf(TEXT("%sLowEntryBlock"), Prefix)));
			};

			SpawnMapPuzzlePen(FVector(HalfX - 310.f, HalfY - 310.f, 0.f), 1, TEXT("OldAtticMainTopRightMapPuzzle"));
			SpawnMapPuzzlePen(FVector(0.f, HalfY - 360.f, 0.f), 0, TEXT("OldAtticMainBottomCenterMapPuzzle"));
			SpawnCardboardBox(FVector(ShelfLeftX + 260.f, ShelfOuterY + 250.f, 260.f), FVector(0.68f, 0.68f, 0.52f), TEXT("OldAtticShelfTopTallBoxA"));
			SpawnCardboardBox(FVector(ShelfLeftX + 470.f, ShelfOuterY + 250.f, 260.f), FVector(0.58f, 0.58f, 0.44f), TEXT("OldAtticShelfTopShortBoxA"));
			SpawnCardboardBox(FVector(ShelfRightX - 270.f, ShelfInnerY - 270.f, 260.f), FVector(0.68f, 0.68f, 0.52f), TEXT("OldAtticShelfBottomTallBoxB"));
			SpawnCardboardBox(FVector(ShelfRightX - 490.f, ShelfInnerY - 270.f, 260.f), FVector(0.58f, 0.58f, 0.44f), TEXT("OldAtticShelfBottomShortBoxB"));
			SpawnCardboardBox(FVector(-HalfX + 520.f, HalfY - 530.f, 34.f), FVector(0.7f, 0.64f, 0.52f), TEXT("OldAtticStartBoxesA"));
			SpawnCardboardBox(FVector(-HalfX + 610.f, HalfY - 440.f, 34.f), FVector(0.52f, 0.52f, 0.44f), TEXT("OldAtticStartBoxesB"));
			if (AtticBoxesTexture)
			{
				SpawnHubImageSign(RoomOrigin + FVector(-HalfX + 570.f, HalfY - 490.f, 105.f), AtticBoxesTexture, 0.32f, TEXT("OldAtticCardboardBoxStackSprite"));
				SpawnHubImageSign(RoomOrigin + FVector(ShelfLeftX + 390.f, ShelfOuterY + 290.f, 335.f), AtticBoxesTexture, 0.26f, TEXT("OldAtticShelfBoxesSprite"));
			}
			SpawnBlock(RoomOrigin + FVector(-HalfX + 740.f, 220.f, 5.f), FVector(0.7f, 1.2f, 0.35f), FLinearColor(0.45f, 0.28f, 0.14f), TEXT("AtticTrunk"));
			SpawnBlock(RoomOrigin + FVector(-HalfX + 645.f, 520.f, 10.f), FVector(0.42f, 2.0f, 0.16f), FLinearColor(0.9f, 0.68f, 0.20f), TEXT("TutorialSwitchPath"));
			SpawnBlock(RoomOrigin + FVector(300.f, 440.f, 30.f), FVector(1.2f, 0.25f, 0.7f), FLinearColor(0.62f, 0.42f, 0.22f), TEXT("TinyJumpPlatform"));
			const FVector ExitSpot = RoomOrigin + FVector(-HalfX + 190.f, HalfY - 190.f, 0.f);
			SpawnBlock(ExitSpot + FVector(-55.f, 0.f, 8.f), FVector(0.58f, 0.92f, 0.10f), FLinearColor(0.16f, 0.08f, 0.035f), TEXT("AtticExitStairStep1"));
			SpawnBlock(ExitSpot + FVector(-8.f, 0.f, 30.f), FVector(0.58f, 0.92f, 0.10f), FLinearColor(0.20f, 0.10f, 0.045f), TEXT("AtticExitStairStep2"));
			SpawnBlock(ExitSpot + FVector(38.f, 0.f, 52.f), FVector(0.58f, 0.92f, 0.10f), FLinearColor(0.24f, 0.12f, 0.055f), TEXT("AtticExitStairStep3"));
			const FVector SwitchSpot = RoomOrigin + FVector(-HalfX + 460.f, HalfY - 520.f, 58.f);
			SpawnBlock(SwitchSpot + FVector(0.f, 0.f, -8.f), FVector(0.62f, 0.24f, 0.46f), FLinearColor(0.94f, 0.74f, 0.24f), TEXT("OldAtticVisibleSwitchBackplate"));
			if (SphereMesh)
			{
				SpawnProp(SphereMesh, SwitchSpot + FVector(940.f, -480.f, 420.f), FVector(0.28f, 0.28f, 0.28f), FLinearColor(1.f, 0.86f, 0.42f), TEXT("OldAtticVisibleHangingBulbFallback"));
			}
			SpawnDecorBlock(SwitchSpot + FVector(940.f, -480.f, 520.f), FVector(0.035f, 0.035f, 1.15f), FLinearColor(0.06f, 0.045f, 0.025f), TEXT("OldAtticLightBulbCord"));
			if (UWorld* World = GetWorld())
			{
				World->SpawnActor<ACoveAtticLightSwitch>(SwitchSpot, FRotator::ZeroRotator);
			}
			SpawnPickup(RoomOrigin + FVector(-HalfX + 520.f, HalfY - 600.f, 80.f), ECovePickupKind::Friend, TEXT("Mouth"));
			SpawnPickup(RoomOrigin + FVector(-HalfX + 760.f, 410.f, 90.f), ECovePickupKind::Artifact, TEXT("1"));
			SpawnPickup(RoomOrigin + FVector(HalfX - 310.f, HalfY - 310.f, 95.f), ECovePickupKind::MapPiece, TEXT("2.1"));
			SpawnPickup(RoomOrigin + FVector(0.f, HalfY - 360.f, 95.f), ECovePickupKind::MapPiece, TEXT("2.2"));
			SpawnShelfMapFragmentPen(FVector(ShelfLeftX + 310.f, ShelfOuterY + 260.f, 0.f), TEXT("OldAtticShelfTopMapFragmentPen"), false);
			SpawnShelfMapFragmentPen(FVector(ShelfRightX - 310.f, ShelfInnerY - 260.f, 0.f), TEXT("OldAtticShelfBottomMapFragmentPen"), true);
			SpawnPickup(RoomOrigin + FVector(ShelfLeftX + 310.f, ShelfOuterY + 260.f, 295.f), ECovePickupKind::MapPiece, TEXT("2.3"));
			SpawnPickup(RoomOrigin + FVector(ShelfRightX - 310.f, ShelfInnerY - 260.f, 295.f), ECovePickupKind::MapPiece, TEXT("2.4"));
			SpawnHubPortal(ExitSpot + FVector(42.f, 0.f, 55.f), 0, TEXT("Neighborhood Hub"));
		}
		else if (Level.Number == 2)
		{
			SpawnPickup(Base + ScaledStoryOffset(FVector(520.f, 120.f, 80.f)), ECovePickupKind::Friend, TEXT("Sloth"));
			SpawnGate(Base + ScaledStoryOffset(FVector(0.f, 0.f, 40.f)), TEXT("Data"), TEXT("Data can rebuild the lighthouse bridge."));
			SpawnProp(ConeMesh, Base + ScaledStoryOffset(FVector(280.f, -150.f, 220.f)), FVector(1.4f, 1.4f, 2.2f), FLinearColor(0.68f, 0.62f, 0.44f), TEXT("LighthouseStack"));
		}
		else if (Level.Number == 3)
		{
			SpawnGate(Base + ScaledStoryOffset(FVector(0.f, 0.f, 40.f)), TEXT("Brand"), TEXT("Brand can push the drainage crate."));
			SpawnPickup(Base + ScaledStoryOffset(FVector(520.f, -60.f, 80.f)), ECovePickupKind::Friend, TEXT("Stef"));
		}
		else if (Level.Number == 5)
		{
			SpawnGate(Base + ScaledStoryOffset(FVector(0.f, 0.f, 40.f)), TEXT("Data"), TEXT("Data can tune the flashlight to clear the bat swarm."));
			SpawnPickup(Base + ScaledStoryOffset(FVector(520.f, 0.f, 80.f)), ECovePickupKind::Friend, TEXT("Data"));
		}
		else if (Level.Number == 6)
		{
			SpawnProp(ConeMesh, Base + ScaledStoryOffset(FVector(130.f, 120.f, 90.f)), FVector(0.8f, 0.8f, 1.4f), Crystal, TEXT("GlowCrystal"));
			SpawnProp(ConeMesh, Base + ScaledStoryOffset(FVector(350.f, -190.f, 90.f)), FVector(1.0f, 1.0f, 1.7f), Crystal, TEXT("GlowCrystal"));
		}
		else if (Level.Number == 7)
		{
			SpawnPickup(Base + ScaledStoryOffset(FVector(520.f, 0.f, 80.f)), ECovePickupKind::Upgrade, TEXT("SuperFlashlight"));
			SpawnBlock(Base + ScaledStoryOffset(FVector(120.f, 120.f, 10.f)), FVector(0.35f, 2.2f, 0.8f), FLinearColor(0.75f, 0.9f, 1.f), TEXT("MirrorWall"));
		}
		else if (Level.Number == 8)
		{
			SpawnPickup(Base + ScaledStoryOffset(FVector(520.f, 0.f, 80.f)), ECovePickupKind::Friend, TEXT("Brand"));
			SpawnTrap(Base + ScaledStoryOffset(FVector(120.f, -180.f, 25.f)));
			SpawnTrap(Base + ScaledStoryOffset(FVector(340.f, 160.f, 25.f)));
		}
		else if (Level.Number == 9)
		{
			SpawnGate(Base + ScaledStoryOffset(FVector(0.f, 0.f, 40.f)), TEXT("Brand"), TEXT("Brand can rig the rope swing over the chasm."));
			SpawnTrap(Base + ScaledStoryOffset(FVector(200.f, -110.f, 25.f)));
		}
		else if (Level.Number == 10)
		{
			SpawnPickup(Base + ScaledStoryOffset(FVector(520.f, 0.f, 80.f)), ECovePickupKind::Friend, TEXT("Mikey"));
			SpawnGate(Base + ScaledStoryOffset(FVector(0.f, 0.f, 40.f)), TEXT("Mikey"), TEXT("Mikey can read the current markers."));
		}
		else if (Level.Number == 11)
		{
			SpawnTrap(Base + ScaledStoryOffset(FVector(-120.f, -120.f, 25.f)));
			SpawnTrap(Base + ScaledStoryOffset(FVector(140.f, 120.f, 25.f)));
			SpawnTrap(Base + ScaledStoryOffset(FVector(390.f, -40.f, 25.f)));
		}
		else if (Level.Number == 12)
		{
			SpawnBlock(Base + ScaledStoryOffset(FVector(120.f, 0.f, 90.f)), FVector(3.4f, 2.4f, 0.7f), ShipWood, TEXT("ShipDeck"));
			SpawnBlock(Base + ScaledStoryOffset(FVector(480.f, 0.f, 170.f)), FVector(1.8f, 0.35f, 2.4f), FLinearColor(0.18f, 0.10f, 0.06f), TEXT("ShipMast"));
			SpawnGate(Base + ScaledStoryOffset(FVector(-40.f, 0.f, 40.f)), TEXT("Mikey"), TEXT("Mikey must coordinate the final treasure-room puzzle."));
			SpawnPickup(Base + ScaledStoryOffset(FVector(650.f, 0.f, 100.f)), ECovePickupKind::FinalTreasure);
		}
	}
}

void ACrooniesTreasureTroopGameMode::SpawnBonusLevels()
{
	const FVector BonusBase(6000.f, 9000.f, 0.f);
	const TCHAR* Titles[] = {
		TEXT("Secret Treehouse"), TEXT("Ghost Ship Trial"), TEXT("Endless Caverns"), TEXT("Neighborhood Mayhem")
	};

	for (int32 Index = 0; Index < 4; ++Index)
	{
		const FVector Base = BonusBase + FVector(Index * 8000.f, 0.f, 0.f);
		const FVector BonusFloorScale(28.f, 16.f, 0.50f);
		const float HalfX = BonusFloorScale.X * 50.f;
		const float HalfY = BonusFloorScale.Y * 50.f;
		SpawnAreaLight(Base + FVector(0.f, 0.f, 720.f), 2800.f, 30000.f, FName(*FString::Printf(TEXT("Bonus%dAreaLight"), Index + 1)));
		SpawnBlock(Base + FVector(0.f, 0.f, -25.f), BonusFloorScale, FLinearColor(0.10f, 0.22f, 0.12f), TEXT("BonusOnePieceFloor"));
		SpawnTemporaryLayoutGrid(Base, BonusFloorScale, FString::Printf(TEXT("Bonus%02d"), Index + 1));
		SpawnBlock(Base + FVector(0.f, -HalfY, 100.f), FVector(BonusFloorScale.X + 0.8f, 0.60f, 2.25f), FLinearColor(0.08f, 0.16f, 0.10f), TEXT("BonusNorthWall"));
		SpawnBlock(Base + FVector(0.f, HalfY, 100.f), FVector(BonusFloorScale.X + 0.8f, 0.60f, 2.25f), FLinearColor(0.08f, 0.16f, 0.10f), TEXT("BonusSouthWall"));
		SpawnBlock(Base + FVector(-HalfX, 0.f, 100.f), FVector(0.60f, BonusFloorScale.Y, 2.25f), FLinearColor(0.08f, 0.16f, 0.10f), TEXT("BonusWestWall"));
		SpawnBlock(Base + FVector(HalfX, 0.f, 100.f), FVector(0.60f, BonusFloorScale.Y, 2.25f), FLinearColor(0.08f, 0.16f, 0.10f), TEXT("BonusEastWall"));
		if (Index == 0 || Index == 3)
		{
			SpawnTreeBorder(Base, BonusFloorScale, 210 + Index * 31, false);
		}
		SpawnLevelZone(Base + FVector(-180.f, 0.f, 80.f), 101 + Index, Titles[Index], TEXT("Bonus challenge"), TEXT("Optional challenge: practice movement and earn collection rewards."));
		if (Index == 1)
		{
			SpawnPickup(Base + FVector(430.f, 0.f, 80.f), ECovePickupKind::FriendshipToken);
		}
	}
}


void ACrooniesTreasureTroopGameMode::SpawnCrewChallengeArena()
{
	const FVector Base(760000.f, 25000.f, 0.f);
	const FVector FloorScale(210.f, 132.f, 0.50f);
	const float HalfX = FloorScale.X * 50.f;
	const float HalfY = FloorScale.Y * 50.f;
	const FLinearColor FloorColor(0.08f, 0.12f, 0.13f);
	const FLinearColor WallColor(0.10f, 0.20f, 0.18f);
	const FLinearColor LaneColor(0.88f, 0.72f, 0.24f);

	SpawnAreaLight(Base + FVector(0.f, 0.f, 1200.f), 12600.f, 90000.f, TEXT("CrewChallengeArenaLight"));
	SpawnBlock(Base + FVector(0.f, 0.f, -25.f), FloorScale, FloorColor, TEXT("CrewChallengeArenaFloor"));
	SpawnBlock(Base + FVector(0.f, -HalfY, 100.f), FVector(FloorScale.X + 0.8f, 0.60f, 2.25f), WallColor, TEXT("CrewChallengeNorthWall"));
	SpawnBlock(Base + FVector(0.f, HalfY, 100.f), FVector(FloorScale.X + 0.8f, 0.60f, 2.25f), WallColor, TEXT("CrewChallengeSouthWall"));
	SpawnBlock(Base + FVector(-HalfX, 0.f, 100.f), FVector(0.60f, FloorScale.Y, 2.25f), WallColor, TEXT("CrewChallengeWestWall"));
	SpawnBlock(Base + FVector(HalfX, 0.f, 100.f), FVector(0.60f, FloorScale.Y, 2.25f), WallColor, TEXT("CrewChallengeEastWall"));
	SpawnLevelZone(Base + FVector(0.f, 0.f, 80.f), 201, TEXT("Crew Challenge Cove"), TEXT("4v4 Crew Match"), TEXT("Find five keys, open chests, and claim three before the rival crew."));

	for (int32 X = -9; X <= 9; ++X)
	{
		SpawnDecorBlock(Base + FVector(X * 700.f, 0.f, 12.f), FVector(1.9f, 0.08f, 0.035f), LaneColor, FName(*FString::Printf(TEXT("CrewChallengeCenterDashX%d"), X)));
	}
	for (int32 Y = -6; Y <= 6; ++Y)
	{
		SpawnDecorBlock(Base + FVector(0.f, Y * 700.f, 13.f), FVector(0.08f, 1.9f, 0.035f), LaneColor, FName(*FString::Printf(TEXT("CrewChallengeCenterDashY%d"), Y)));
	}

	const FVector CrewChallengeTrapOffsets[] = {
		FVector(-7600.f, -4200.f, 25.f), FVector(-5200.f, 1800.f, 25.f), FVector(-2400.f, -5100.f, 25.f),
		FVector(1200.f, 4200.f, 25.f), FVector(4400.f, -2600.f, 25.f), FVector(7600.f, 3300.f, 25.f),
		FVector(8600.f, -4700.f, 25.f), FVector(-8800.f, 4800.f, 25.f)
	};
	const ECoveTrapKind CrewChallengeTrapKinds[] = {
		ECoveTrapKind::Skeleton, ECoveTrapKind::Fire, ECoveTrapKind::Gas, ECoveTrapKind::Hole,
		ECoveTrapKind::Fire, ECoveTrapKind::Gas, ECoveTrapKind::Skeleton, ECoveTrapKind::Hole
	};
	for (int32 Index = 0; Index < UE_ARRAY_COUNT(CrewChallengeTrapOffsets); ++Index)
	{
		SpawnTrap(Base + CrewChallengeTrapOffsets[Index], CrewChallengeTrapKinds[Index]);
	}

	const FVector RatOffsets[] = {
		FVector(-6800.f, -1200.f, 70.f), FVector(-3600.f, 3800.f, 70.f), FVector(-800.f, -4300.f, 70.f),
		FVector(3100.f, 2100.f, 70.f), FVector(6100.f, -3600.f, 70.f), FVector(8600.f, 600.f, 70.f)
	};
	for (int32 Index = 0; Index < UE_ARRAY_COUNT(RatOffsets); ++Index)
	{
		SpawnEnemy(Base + RatOffsets[Index], TEXT("Rat"), 201);
	}

	const FVector BatOffsets[] = {
		FVector(-8200.f, 4200.f, 150.f), FVector(-4300.f, -3500.f, 150.f), FVector(600.f, 5200.f, 150.f),
		FVector(4700.f, -900.f, 150.f), FVector(8200.f, -5200.f, 150.f)
	};
	for (int32 Index = 0; Index < UE_ARRAY_COUNT(BatOffsets); ++Index)
	{
		SpawnEnemy(Base + BatOffsets[Index], TEXT("Bat"), 201);
	}
}
void ACrooniesTreasureTroopGameMode::SpawnWorld()
{
	SpawnLighting();
	SpawnHub();
	SpawnStoryLevels();
	SpawnBonusLevels();
	SpawnCrewChallengeArena();
	MovePlayerToTutorial();
}







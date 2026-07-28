// Copyright Epic Games, Inc. All Rights Reserved.

#include "CrooniesTreasureTroopCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/BillboardComponent.h"
#include "Components/PointLightComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/World.h"
#include "Engine/Engine.h"

namespace
{
	FName NormalizeFriendId(FName FriendId)
	{
		if (FriendId == TEXT("Scout") || FriendId == TEXT("MikeyLeader"))
		{
			return TEXT("Mikey");
		}
		if (FriendId == TEXT("Quip") || FriendId == TEXT("MouthSarcastic"))
		{
			return TEXT("Mouth");
		}
		if (FriendId == TEXT("Inventor") || FriendId == TEXT("DataInventor"))
		{
			return TEXT("Data");
		}
		if (FriendId == TEXT("Flashlight"))
		{
			return TEXT("Data");
		}
		if (FriendId == TEXT("Strong") || FriendId == TEXT("BrandBigBrother") || FriendId == TEXT("Rope"))
		{
			return TEXT("Brand");
		}
		if (FriendId == TEXT("Smart"))
		{
			return TEXT("Mikey");
		}
		if (FriendId == TEXT("StefTomboy"))
		{
			return TEXT("Stef");
		}
		return FriendId;
	}

	bool IsStaticFriend(FName FriendId)
	{
		return NormalizeFriendId(FriendId) == TEXT("Sloth");
	}

	FString GetFriendDisplayName(FName FriendId)
	{
		const FName Id = NormalizeFriendId(FriendId);
		if (Id == TEXT("Stef"))
		{
			return TEXT("Stef");
		}
		if (Id == TEXT("Mouth"))
		{
			return TEXT("Mouth");
		}
		if (Id == TEXT("Mikey"))
		{
			return TEXT("Mikey");
		}
		if (Id == TEXT("Data"))
		{
			return TEXT("Data");
		}
		if (Id == TEXT("Brand"))
		{
			return TEXT("Brand");
		}
		if (Id == TEXT("Sloth"))
		{
			return TEXT("Sloth");
		}
		return Id.ToString();
	}

	FString GetFriendRole(FName FriendId)
	{
		const FName Id = NormalizeFriendId(FriendId);
		if (Id == TEXT("Stef"))
		{
			return TEXT("Tomboy");
		}
		if (Id == TEXT("Mouth"))
		{
			return TEXT("Sarcastic");
		}
		if (Id == TEXT("Mikey"))
		{
			return TEXT("Leader");
		}
		if (Id == TEXT("Data"))
		{
			return TEXT("Inventor");
		}
		if (Id == TEXT("Brand"))
		{
			return TEXT("Big Brother");
		}
		if (Id == TEXT("Sloth"))
		{
			return TEXT("Special Friend");
		}
		return TEXT("Friend");
	}

	int32 GetMaxCourageForFriendLevel(FName FriendId, int32 Level)
	{
		if (IsStaticFriend(FriendId))
		{
			return 30;
		}
		return 10 + (FMath::Clamp(Level, 1, 10) - 1) * 3;
	}

	int32 GetFriendXpRequiredForNextLevel(int32 CurrentLevel)
	{
		return FMath::Clamp(CurrentLevel, 1, 9) * 2;
	}

	FCrooniesFriendProgress MakeFriendProgress(FName FriendId)
	{
		const FName Id = NormalizeFriendId(FriendId);
		FCrooniesFriendProgress Progress;
		Progress.FriendId = Id;
		Progress.DisplayName = GetFriendDisplayName(Id);
		Progress.Role = GetFriendRole(Id);
		Progress.Level = 1;
		Progress.Experience = 0;
		Progress.bStaticLevel = IsStaticFriend(Id);
		Progress.MaxCourage = GetMaxCourageForFriendLevel(Id, Progress.Level);
		return Progress;
	}
}

ACrooniesTreasureTroopCharacter::ACrooniesTreasureTroopCharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = false;
	GetCharacterMovement()->bSnapToPlaneAtStart = false;
	GetCharacterMovement()->JumpZVelocity = 540.f;
	GetCharacterMovement()->AirControl = 0.35f;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 1650.f;
	CameraBoom->SetRelativeRotation(FRotator(-52.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	BaseCharacterMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseCharacterMesh"));
	BaseCharacterMesh->SetupAttachment(RootComponent);
	BaseCharacterMesh->SetRelativeLocation(FVector(0.f, 0.f, -94.f));
	BaseCharacterMesh->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	BaseCharacterMesh->SetRelativeScale3D(FVector(0.78f));
	BaseCharacterMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ExplorerBody"));
	BodyMesh->SetupAttachment(RootComponent);
	BodyMesh->SetRelativeLocation(FVector(0.f, 0.f, -34.f));
	BodyMesh->SetRelativeScale3D(FVector(0.58f, 0.38f, 0.78f));
	BodyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	HeadMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ExplorerHead"));
	HeadMesh->SetupAttachment(RootComponent);
	HeadMesh->SetRelativeLocation(FVector(0.f, 0.f, 46.f));
	HeadMesh->SetRelativeScale3D(FVector(0.42f, 0.42f, 0.42f));
	HeadMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	HairMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ExplorerHair"));
	HairMesh->SetupAttachment(RootComponent);
	HairMesh->SetRelativeLocation(FVector(-4.f, 0.f, 74.f));
	HairMesh->SetRelativeScale3D(FVector(0.46f, 0.46f, 0.18f));
	HairMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	BackpackMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ExplorerBackpack"));
	BackpackMesh->SetupAttachment(RootComponent);
	BackpackMesh->SetRelativeLocation(FVector(-30.f, 0.f, -22.f));
	BackpackMesh->SetRelativeScale3D(FVector(0.22f, 0.46f, 0.62f));
	BackpackMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	FootMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ExplorerFeet"));
	FootMesh->SetupAttachment(RootComponent);
	FootMesh->SetRelativeLocation(FVector(12.f, 0.f, -104.f));
	FootMesh->SetRelativeScale3D(FVector(0.48f, 0.54f, 0.10f));
	FootMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	CharacterSprite = CreateDefaultSubobject<UBillboardComponent>(TEXT("CharacterPortraitFallback"));
	CharacterSprite->SetupAttachment(RootComponent);
	CharacterSprite->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CharacterSprite->bIsScreenSizeScaled = false;
	CharacterSprite->ScreenSize = 0.0025f;
	CharacterSprite->SetRelativeLocation(FVector(0.f, 0.f, 16.f));
	CharacterSprite->SetRelativeScale3D(FVector(0.70f));

	PlayerAuraLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("PlayerAuraLight"));
	PlayerAuraLight->SetupAttachment(RootComponent);
	PlayerAuraLight->SetRelativeLocation(FVector(0.f, 0.f, 80.f));
	PlayerAuraLight->SetAttenuationRadius(135.f);
	PlayerAuraLight->SetIntensity(4200.f);
	PlayerAuraLight->SetLightColor(FLinearColor(1.f, 0.82f, 0.46f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeAsset(TEXT("/Engine/BasicShapes/Cube.Cube"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereAsset(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> ScoutSpriteAsset(TEXT("/Game/Cove/UIIcons/Mikey_Leader.Mikey_Leader"));
	if (ScoutSpriteAsset.Succeeded())
	{
		CharacterSprite->SetSprite(ScoutSpriteAsset.Object);
	}
	const bool bHasTedBoyModel = false;
	if (CubeAsset.Succeeded())
	{
		BodyMesh->SetStaticMesh(CubeAsset.Object);
		BackpackMesh->SetStaticMesh(CubeAsset.Object);
		FootMesh->SetStaticMesh(CubeAsset.Object);
	}
	if (SphereAsset.Succeeded())
	{
		HeadMesh->SetStaticMesh(SphereAsset.Object);
		HairMesh->SetStaticMesh(SphereAsset.Object);
	}
	BaseCharacterMesh->SetVisibility(bHasTedBoyModel);
	BodyMesh->SetVisibility(true);
	HeadMesh->SetVisibility(true);
	HairMesh->SetVisibility(true);
	BackpackMesh->SetVisibility(true);
	FootMesh->SetVisibility(true);
	CharacterSprite->SetVisibility(true);

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void ACrooniesTreasureTroopCharacter::BeginPlay()
{
	Super::BeginPlay();
	Friends.Add(TEXT("Mikey"));
	FriendProgress.FindOrAdd(TEXT("Mikey")) = MakeFriendProgress(TEXT("Mikey"));
	SelectedCrewmate = NormalizeFriendId(SelectedCrewmate);

	const bool bUsingTedBoyModel = BaseCharacterMesh && BaseCharacterMesh->GetStaticMesh() != nullptr;
	BaseCharacterMesh->SetVisibility(bUsingTedBoyModel);
	BodyMesh->SetVisibility(true);
	HeadMesh->SetVisibility(true);
	HairMesh->SetVisibility(true);
	BackpackMesh->SetVisibility(true);
	FootMesh->SetVisibility(true);
	CharacterSprite->SetVisibility(true);

	if (UMaterialInstanceDynamic* Mat = BodyMesh->CreateAndSetMaterialInstanceDynamic(0))
	{
		Mat->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.95f, 0.16f, 0.10f));
		Mat->SetVectorParameterValue(TEXT("BaseColor"), FLinearColor(0.95f, 0.16f, 0.10f));
	}
	if (UMaterialInstanceDynamic* Mat = HeadMesh->CreateAndSetMaterialInstanceDynamic(0))
	{
		Mat->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.72f, 0.46f, 0.29f));
		Mat->SetVectorParameterValue(TEXT("BaseColor"), FLinearColor(0.72f, 0.46f, 0.29f));
	}
	if (UMaterialInstanceDynamic* Mat = HairMesh->CreateAndSetMaterialInstanceDynamic(0))
	{
		Mat->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.12f, 0.07f, 0.03f));
		Mat->SetVectorParameterValue(TEXT("BaseColor"), FLinearColor(0.12f, 0.07f, 0.03f));
	}
	if (UMaterialInstanceDynamic* Mat = BackpackMesh->CreateAndSetMaterialInstanceDynamic(0))
	{
		Mat->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.16f, 0.28f, 0.12f));
		Mat->SetVectorParameterValue(TEXT("BaseColor"), FLinearColor(0.16f, 0.28f, 0.12f));
	}
	if (UMaterialInstanceDynamic* Mat = FootMesh->CreateAndSetMaterialInstanceDynamic(0))
	{
		Mat->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.05f, 0.04f, 0.035f));
		Mat->SetVectorParameterValue(TEXT("BaseColor"), FLinearColor(0.05f, 0.04f, 0.035f));
	}

	GrantTestMapPiecesThroughLevel(11);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Cyan, TEXT("Croonies build 0.1.50 - attic props, light switch, visible player fallback."));
	}
}

void ACrooniesTreasureTroopCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!GetCharacterMovement())
	{
		return;
	}

	const float WeightSlowdown = FMath::Clamp(CarryWeight * 0.010f, 0.f, 0.38f);
	GetCharacterMovement()->MaxWalkSpeed = 600.f * (1.f - WeightSlowdown);

	if (bRunActive && RunStamina > 0.f)
	{
		const float Speed = GetVelocity().Size2D();
		const float MovementDrain = Speed > 25.f ? 0.48f : 0.18f;
		const float WeightDrain = CarryWeight * 0.006f;
		SpendRunStamina((MovementDrain + WeightDrain) * DeltaSeconds);
	}
}

void ACrooniesTreasureTroopCharacter::RecruitFriend(FName FriendId)
{
	FriendId = NormalizeFriendId(FriendId);
	if (!Friends.Contains(FriendId))
	{
		Friends.Add(FriendId);
		FriendProgress.FindOrAdd(FriendId) = MakeFriendProgress(FriendId);
		if (Friends.Num() == 1)
		{
			SelectedCrewmate = FriendId;
		}
		if (GEngine)
		{
			const FCrooniesFriendProgress& Progress = FriendProgress[FriendId];
			GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Green, FString::Printf(TEXT("%s joined the crew! %s starts at Level %d with Max Courage %d."),
				*Progress.DisplayName, *Progress.Role, Progress.Level, Progress.MaxCourage));
		}
		if (CurrentLevelNumber == 1 && FriendId == TEXT("Mouth"))
		{
			TutorialStep = FMath::Max(TutorialStep, 3);
		}
	}
}

bool ACrooniesTreasureTroopCharacter::LevelUpFriend(FName FriendId)
{
	FriendId = NormalizeFriendId(FriendId);
	if (!Friends.Contains(FriendId))
	{
		return false;
	}

	FCrooniesFriendProgress& Progress = FriendProgress.FindOrAdd(FriendId);
	if (Progress.FriendId.IsNone())
	{
		Progress = MakeFriendProgress(FriendId);
	}
	if (Progress.bStaticLevel)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor(255, 205, 80), FString::Printf(TEXT("%s is a special friend and does not level up."), *Progress.DisplayName));
		}
		return false;
	}
	if (Progress.Level >= 10)
	{
		Progress.Level = 10;
		Progress.MaxCourage = GetMaxCourageForFriendLevel(FriendId, Progress.Level);
		return false;
	}

	++Progress.Level;
	Progress.Experience = 0;
	Progress.MaxCourage = GetMaxCourageForFriendLevel(FriendId, Progress.Level);
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::Green, FString::Printf(TEXT("%s leveled up to %d! Max Courage is now %d."),
			*Progress.DisplayName, Progress.Level, Progress.MaxCourage));
	}
	return true;
}

bool ACrooniesTreasureTroopCharacter::AddFriendExperience(FName FriendId, int32 Amount)
{
	FriendId = NormalizeFriendId(FriendId);
	if (!Friends.Contains(FriendId) || Amount <= 0)
	{
		return false;
	}

	FCrooniesFriendProgress& Progress = FriendProgress.FindOrAdd(FriendId);
	if (Progress.FriendId.IsNone())
	{
		Progress = MakeFriendProgress(FriendId);
	}
	if (Progress.bStaticLevel || Progress.Level >= 10)
	{
		return false;
	}

	Progress.Experience += Amount;
	const int32 Needed = GetFriendXpRequiredForNextLevel(Progress.Level);
	if (Progress.Experience >= Needed)
	{
		return LevelUpFriend(FriendId);
	}
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor(130, 220, 255), FString::Printf(TEXT("%s friendship XP: %d/%d"),
			*Progress.DisplayName, Progress.Experience, Needed));
	}
	return true;
}

void ACrooniesTreasureTroopCharacter::ApplyCharacterCustomization(bool bInMale, int32 InSkinTone, int32 InHairStyle, int32 InHairColor, int32 InEyeColor)
{
	bMale = bInMale;
	SkinTone = FMath::Clamp(InSkinTone, 0, 4);
	HairStyle = FMath::Clamp(InHairStyle, 0, 4);
	HairColor = FMath::Clamp(InHairColor, 0, 5);
	EyeColor = FMath::Clamp(InEyeColor, 0, 4);

	static const FLinearColor SkinColors[] = {
		FLinearColor(0.92f, 0.68f, 0.48f),
		FLinearColor(0.72f, 0.46f, 0.29f),
		FLinearColor(0.48f, 0.29f, 0.18f),
		FLinearColor(0.98f, 0.79f, 0.58f),
		FLinearColor(0.32f, 0.19f, 0.12f)
	};

	if (UMaterialInstanceDynamic* Mat = BodyMesh->CreateAndSetMaterialInstanceDynamic(0))
	{
		const FLinearColor Shirt = bMale ? FLinearColor(0.95f, 0.16f, 0.10f) : FLinearColor(0.08f, 0.38f, 0.96f);
		Mat->SetVectorParameterValue(TEXT("Color"), Shirt);
		Mat->SetVectorParameterValue(TEXT("BaseColor"), Shirt);
	}
	if (UMaterialInstanceDynamic* Mat = HeadMesh->CreateAndSetMaterialInstanceDynamic(0))
	{
		const FLinearColor Color = SkinColors[SkinTone];
		Mat->SetVectorParameterValue(TEXT("Color"), Color);
		Mat->SetVectorParameterValue(TEXT("BaseColor"), Color);
	}
	if (UMaterialInstanceDynamic* Mat = HairMesh->CreateAndSetMaterialInstanceDynamic(0))
	{
		static const FLinearColor HairColors[] = {
			FLinearColor(0.08f, 0.045f, 0.02f),
			FLinearColor(0.38f, 0.20f, 0.09f),
			FLinearColor(0.85f, 0.58f, 0.20f),
			FLinearColor(0.05f, 0.05f, 0.05f),
			FLinearColor(0.70f, 0.16f, 0.08f),
			FLinearColor(0.82f, 0.82f, 0.74f)
		};
		const FLinearColor Color = HairColors[HairColor];
		Mat->SetVectorParameterValue(TEXT("Color"), Color);
		Mat->SetVectorParameterValue(TEXT("BaseColor"), Color);
	}
}

void ACrooniesTreasureTroopCharacter::AddArtifact()
{
	AddArtifactForLevel(CurrentLevelNumber);
}

void ACrooniesTreasureTroopCharacter::AddArtifactForLevel(int32 SourceLevel)
{
	SourceLevel = FMath::Clamp(SourceLevel, 1, 12);
	if (ArtifactLevels.Contains(SourceLevel))
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.5f, FColor(255, 205, 80), FString::Printf(TEXT("Level %d artifact is already collected."), SourceLevel));
		}
		return;
	}

	ArtifactLevels.Add(SourceLevel);
	ArtifactCount = ArtifactLevels.Num();
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, FString::Printf(TEXT("Level %d artifact found: %d/12"), SourceLevel, ArtifactCount));
	}
}

bool ACrooniesTreasureTroopCharacter::HasArtifactForLevel(int32 SourceLevel) const
{
	return ArtifactLevels.Contains(FMath::Clamp(SourceLevel, 1, 12));
}

void ACrooniesTreasureTroopCharacter::AddMapPiece()
{
	AddMapPieceForLevel(FMath::Clamp(CurrentLevelNumber + 1, 2, 12));
}

void ACrooniesTreasureTroopCharacter::AddMapPieceForLevel(int32 TargetLevel)
{
	TargetLevel = FMath::Clamp(TargetLevel, 2, 12);
	int32& Pieces = MapPiecesByLevel.FindOrAdd(TargetLevel);
	if (Pieces >= 4)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.5f, FColor(255, 205, 80), FString::Printf(TEXT("Map L%d is already complete."), TargetLevel));
		}
		return;
	}

	Pieces++;
	MapPieceCount++;
	if (CurrentLevelNumber == 1)
	{
		TutorialStep = FMath::Max(TutorialStep, 6);
	}
	if (GEngine)
	{
		const FString CompleteText = Pieces >= 4 ? TEXT(" MAP COMPLETE") : TEXT("");
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Orange, FString::Printf(TEXT("Map L%d piece found: %d/4.%s"), TargetLevel, Pieces, *CompleteText));
	}
}

void ACrooniesTreasureTroopCharacter::GrantTestMapPiecesThroughLevel(int32 HighestTargetLevel)
{
	HighestTargetLevel = FMath::Clamp(HighestTargetLevel, 2, 12);
	for (int32 TargetLevel = 2; TargetLevel <= HighestTargetLevel; ++TargetLevel)
	{
		MapPiecesByLevel.FindOrAdd(TargetLevel) = 4;
	}

	MapPieceCount = 0;
	for (const TPair<int32, int32>& Entry : MapPiecesByLevel)
	{
		MapPieceCount += FMath::Clamp(Entry.Value, 0, 4);
	}
}
void ACrooniesTreasureTroopCharacter::AddSpecialItem(FName ItemId)
{
	if (ItemId.IsNone())
	{
		return;
	}
	if (!SpecialItems.Contains(ItemId))
	{
		SpecialItems.Add(ItemId);
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor(110, 230, 255), FString::Printf(TEXT("Special item found: %s"), *ItemId.ToString()));
		}
	}
}

void ACrooniesTreasureTroopCharacter::AddCoins(int32 Amount)
{
	CoinCount += Amount;
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor(255, 205, 60), FString::Printf(TEXT("+%d gold coins"), Amount));
	}
}

void ACrooniesTreasureTroopCharacter::AddCourage(int32 Amount)
{
	if (Amount <= 0)
	{
		return;
	}

	const int32 OldCourage = Courage;
	Courage = FMath::Clamp(Courage + Amount, 0, 1000);
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor(255, 185, 70),
			FString::Printf(TEXT("Liquid Courage restored +%d Courage. Courage: %d/1000"), Courage - OldCourage, Courage));
	}
}

bool ACrooniesTreasureTroopCharacter::SpendCourage(int32 Amount)
{
	if (Amount <= 0)
	{
		return true;
	}
	if (Courage < Amount)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor(255, 205, 80),
				FString::Printf(TEXT("Need %d Courage. You have %d."), Amount, Courage));
		}
		return false;
	}
	Courage -= Amount;
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.5f, FColor(255, 185, 70),
			FString::Printf(TEXT("Spent %d Courage. Courage: %d/1000"), Amount, Courage));
	}
	return true;
}

void ACrooniesTreasureTroopCharacter::AddRunLoot(int32 Value, float Weight)
{
	if (Value <= 0 && Weight <= 0.f)
	{
		return;
	}

	RunLootValue += FMath::Max(0, Value);
	CarryWeight = FMath::Max(0.f, CarryWeight + FMath::Max(0.f, Weight));
	++RunLootCount;
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor(255, 220, 90),
			FString::Printf(TEXT("Loot packed: +%d value, +%.1f weight. Run loot: %d value / %.1f weight."), Value, Weight, RunLootValue, CarryWeight));
	}
}

void ACrooniesTreasureTroopCharacter::RestoreRunStamina(float Amount)
{
	if (Amount <= 0.f)
	{
		return;
	}
	const float OldStamina = RunStamina;
	RunStamina = FMath::Clamp(RunStamina + Amount, 0.f, MaxRunStamina);
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.5f, FColor(110, 240, 180),
			FString::Printf(TEXT("Stamina restored +%.0f. Stamina: %.0f/%.0f"), RunStamina - OldStamina, RunStamina, MaxRunStamina));
	}
}

void ACrooniesTreasureTroopCharacter::SpendRunStamina(float Amount)
{
	if (Amount <= 0.f || !bRunActive)
	{
		return;
	}

	const float OldStamina = RunStamina;
	RunStamina = FMath::Clamp(RunStamina - Amount, 0.f, MaxRunStamina);
	if (OldStamina > 0.f && RunStamina <= 0.f && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor(255, 90, 60), TEXT("Stamina is empty. Extract or use a stamina item before the run collapses."));
	}
}

void ACrooniesTreasureTroopCharacter::ResetRunStateForLevel(int32 LevelNumber)
{
	bRunActive = LevelNumber >= 2;
	MaxRunStamina = 100.f + FMath::Clamp(LevelNumber - 2, 0, 10) * 6.f;
	RunStamina = MaxRunStamina;
	CarryWeight = 0.f;
	RunLootValue = 0;
	RunLootCount = 0;
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = 600.f;
	}
}

bool ACrooniesTreasureTroopCharacter::SpendCoins(int32 Amount)
{
	if (Amount <= 0)
	{
		return true;
	}
	if (CoinCount < Amount)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor(255, 205, 80), FString::Printf(TEXT("Need %d coins. You have %d."), Amount, CoinCount));
		}
		return false;
	}
	CoinCount -= Amount;
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor(255, 205, 60), FString::Printf(TEXT("Spent %d coins."), Amount));
	}
	return true;
}

bool ACrooniesTreasureTroopCharacter::CompleteDailyTask(int32 TaskIndex)
{
	if (TaskIndex < 0 || TaskIndex >= GetDailyTaskCount())
	{
		return false;
	}
	if (CompletedDailyTasks.Contains(TaskIndex))
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.5f, FColor(255, 205, 80), FString::Printf(TEXT("Daily task %d is already complete."), TaskIndex + 1));
		}
		return false;
	}
	if (Courage <= 0)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor(255, 145, 90), TEXT("No Courage left. Come back tomorrow or use a future rest item."));
		}
		return false;
	}

	Courage--;
	CompletedDailyTasks.Add(TaskIndex);
	AddCoins(1);
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor(120, 255, 145), FString::Printf(TEXT("Daily task %d complete: +1 coin. Courage left: %d"), TaskIndex + 1, Courage));
	}
	return true;
}

int32 ACrooniesTreasureTroopCharacter::GetCompletedDailyTaskCount() const
{
	return CompletedDailyTasks.Num();
}

bool ACrooniesTreasureTroopCharacter::IsDailyTaskComplete(int32 TaskIndex) const
{
	return CompletedDailyTasks.Contains(TaskIndex);
}

void ACrooniesTreasureTroopCharacter::AddFriendshipToken()
{
	FriendshipTokenCount++;
	if (Friends.Contains(SelectedCrewmate))
	{
		AddFriendExperience(SelectedCrewmate, 1);
	}
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor(255, 100, 180), FString::Printf(TEXT("Friendship token: %d/8"), FriendshipTokenCount));
	}
}

void ACrooniesTreasureTroopCharacter::AddUpgrade(FName UpgradeId)
{
	if (!Upgrades.Contains(UpgradeId))
	{
		Upgrades.Add(UpgradeId);
		UpgradeCount = Upgrades.Num();
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor(170, 120, 255), FString::Printf(TEXT("Upgrade unlocked: %s"), *UpgradeId.ToString()));
		}
	}
}

void ACrooniesTreasureTroopCharacter::ClaimFinalTreasure()
{
	if (!bFoundFinalTreasure && HasCompletedMapForLevel(12) && ArtifactCount >= 12 && FriendshipTokenCount >= 8)
	{
		bFoundFinalTreasure = true;
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor(80, 255, 150), TEXT("Final treasure recovered! The crew saved Anchor Cove and brought the neighborhood together."));
		}
	}
	else if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor(190, 190, 190), TEXT("The ship vault needs the completed Shipwreck map, 12 artifacts, and 8 friendship tokens."));
	}
}

void ACrooniesTreasureTroopCharacter::HitTrap()
{
	Courage = FMath::Max(0, Courage - 1);
	SpendRunStamina(12.f);
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("A trap rattled the crew. Stamina and Courage took a hit."));
	}
}

void ACrooniesTreasureTroopCharacter::EnterLevel(int32 LevelNumber, const FString& Title, const FString& Style, const FString& Objective)
{
	CurrentLevelNumber = LevelNumber;
	ResetRunStateForLevel(LevelNumber);
	CurrentLevelTitle = Title;
	CurrentLevelStyle = Style;
	CurrentObjective = Objective;
	if (CurrentLevelNumber == 1)
	{
		SetAtticLightOn(false);
	}
	else
	{
		SetAtticLightOn(true);
	}
	if (CurrentLevelNumber == 1 && TutorialStep == 0)
	{
		TutorialStep = 1;
	}
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("Level %d: %s - %s"), LevelNumber, *Title, *Style));
	}
}

void ACrooniesTreasureTroopCharacter::SelectCrewmate(int32 Slot)
{
	static const FName CrewOrder[] = {
		TEXT("Mikey"), TEXT("Stef"), TEXT("Mouth"), TEXT("Data"),
		TEXT("Brand"), TEXT("Sloth")
	};

	if (Slot >= 0 && Slot < UE_ARRAY_COUNT(CrewOrder))
	{
		const FName Choice = NormalizeFriendId(CrewOrder[Slot]);
		if (Choice == TEXT("Mikey") || HasFriend(Choice))
		{
			SelectedCrewmate = Choice;
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::White, FString::Printf(TEXT("Active friend: %s"), *GetFriendProgressSummary(Choice)));
			}
			if (CurrentLevelNumber == 1 && Choice == TEXT("Mouth"))
			{
				TutorialStep = FMath::Max(TutorialStep, 4);
			}
		}
	}
}

void ACrooniesTreasureTroopCharacter::SetAtticLightOn(bool bInLightOn)
{
	bAtticLightOn = bInLightOn;
	if (PlayerAuraLight)
	{
		PlayerAuraLight->SetAttenuationRadius(bAtticLightOn ? 260.f : 135.f);
		PlayerAuraLight->SetIntensity(bAtticLightOn ? 1200.f : 4200.f);
	}
}

void ACrooniesTreasureTroopCharacter::NotifyMovedForward()
{
	if (CurrentLevelNumber == 1 && TutorialStep <= 1)
	{
		bPressedForwardInTutorial = true;
		TutorialStep = 2;
	}
}

void ACrooniesTreasureTroopCharacter::NotifyInteractPressed()
{
	if (CurrentLevelNumber == 1)
	{
		bPressedInteractInTutorial = true;
		if (TutorialStep < 3)
		{
			TutorialStep = 3;
		}
		else if (TutorialStep == 4)
		{
			TutorialStep = 5;
		}
	}
}

bool ACrooniesTreasureTroopCharacter::HasFriend(FName FriendId) const
{
	FriendId = NormalizeFriendId(FriendId);
	return FriendId == TEXT("Mikey") || Friends.Contains(FriendId);
}

int32 ACrooniesTreasureTroopCharacter::GetFriendLevel(FName FriendId) const
{
	FriendId = NormalizeFriendId(FriendId);
	if (const FCrooniesFriendProgress* Progress = FriendProgress.Find(FriendId))
	{
		return Progress->Level;
	}
	return 1;
}

int32 ACrooniesTreasureTroopCharacter::GetFriendMaxCourage(FName FriendId) const
{
	FriendId = NormalizeFriendId(FriendId);
	if (const FCrooniesFriendProgress* Progress = FriendProgress.Find(FriendId))
	{
		return Progress->MaxCourage;
	}
	return GetMaxCourageForFriendLevel(FriendId, 1);
}

bool ACrooniesTreasureTroopCharacter::IsFriendStaticLevel(FName FriendId) const
{
	FriendId = NormalizeFriendId(FriendId);
	if (const FCrooniesFriendProgress* Progress = FriendProgress.Find(FriendId))
	{
		return Progress->bStaticLevel;
	}
	return IsStaticFriend(FriendId);
}

FString ACrooniesTreasureTroopCharacter::GetFriendProgressSummary(FName FriendId) const
{
	FriendId = NormalizeFriendId(FriendId);
	const FCrooniesFriendProgress* Progress = FriendProgress.Find(FriendId);
	if (!Progress)
	{
		const FCrooniesFriendProgress DefaultProgress = MakeFriendProgress(FriendId);
		return FString::Printf(TEXT("%s (%s) Lv%d Max Courage %d"),
			*DefaultProgress.DisplayName, *DefaultProgress.Role, DefaultProgress.Level, DefaultProgress.MaxCourage);
	}

	if (Progress->bStaticLevel)
	{
		return FString::Printf(TEXT("%s (%s) Static Max Courage %d"),
			*Progress->DisplayName, *Progress->Role, Progress->MaxCourage);
	}

	const int32 Needed = Progress->Level >= 10 ? 0 : GetFriendXpRequiredForNextLevel(Progress->Level);
	return Progress->Level >= 10
		? FString::Printf(TEXT("%s (%s) Lv10 Max Courage %d"), *Progress->DisplayName, *Progress->Role, Progress->MaxCourage)
		: FString::Printf(TEXT("%s (%s) Lv%d XP %d/%d Max Courage %d"), *Progress->DisplayName, *Progress->Role, Progress->Level, Progress->Experience, Needed, Progress->MaxCourage);
}

int32 ACrooniesTreasureTroopCharacter::GetMapPiecesForLevel(int32 TargetLevel) const
{
	if (const int32* Pieces = MapPiecesByLevel.Find(TargetLevel))
	{
		return FMath::Clamp(*Pieces, 0, 4);
	}
	return 0;
}

bool ACrooniesTreasureTroopCharacter::HasCompletedMapForLevel(int32 TargetLevel) const
{
	return GetMapPiecesForLevel(TargetLevel) >= 4;
}

bool ACrooniesTreasureTroopCharacter::HasSpecialItem(FName ItemId) const
{
	return ItemId.IsNone() || SpecialItems.Contains(ItemId);
}

FName ACrooniesTreasureTroopCharacter::GetRequiredSpecialItemIdForLevel(int32 TargetLevel) const
{
	switch (TargetLevel)
	{
	case 2: return TEXT("GrapplingHook");
	case 3: return TEXT("DrainCrank");
	case 4: return TEXT("MineKey");
	case 5: return TEXT("BatCharm");
	case 6: return TEXT("CrystalLens");
	case 7: return TEXT("MirrorCompass");
	case 8: return TEXT("PirateSeal");
	case 9: return TEXT("RopeHarness");
	case 10: return TEXT("RiverOar");
	case 11: return TEXT("GetawayWhistle");
	case 12: return TEXT("ShipWheelKey");
	default: return NAME_None;
	}
}

FString ACrooniesTreasureTroopCharacter::GetRequiredSpecialItemNameForLevel(int32 TargetLevel) const
{
	switch (TargetLevel)
	{
	case 2: return TEXT("Grappling Hook");
	case 3: return TEXT("Drain Crank");
	case 4: return TEXT("Mine Key");
	case 5: return TEXT("Bat Charm");
	case 6: return TEXT("Crystal Lens");
	case 7: return TEXT("Mirror Compass");
	case 8: return TEXT("Pirate Seal");
	case 9: return TEXT("Rope Harness");
	case 10: return TEXT("River Oar");
	case 11: return TEXT("Getaway Whistle");
	case 12: return TEXT("Ship Wheel Key");
	default: return TEXT("No special item");
	}
}

bool ACrooniesTreasureTroopCharacter::HasRequiredSpecialItemForLevel(int32 TargetLevel) const
{
	return HasSpecialItem(GetRequiredSpecialItemIdForLevel(TargetLevel));
}

FString ACrooniesTreasureTroopCharacter::GetPartySummary() const
{
	TArray<FString> Names;
	for (const FName& Friend : Friends)
	{
		Names.Add(GetFriendDisplayName(Friend));
	}
	Names.Sort();
	return Names.Num() > 0 ? FString::Join(Names, TEXT(", ")) : TEXT("Solo");
}

FString ACrooniesTreasureTroopCharacter::GetCrewProgressSummary() const
{
	TArray<FString> Lines;
	for (const FName& Friend : Friends)
	{
		Lines.Add(GetFriendProgressSummary(Friend));
	}
	Lines.Sort();
	return Lines.Num() > 0 ? FString::Join(Lines, TEXT(" | ")) : TEXT("No friends recruited yet");
}

FString ACrooniesTreasureTroopCharacter::GetSelectedCrewmate() const
{
	return GetFriendProgressSummary(SelectedCrewmate);
}

FString ACrooniesTreasureTroopCharacter::GetObjectiveText() const
{
	if (bFoundFinalTreasure)
	{
		return TEXT("Victory: the final treasure is safe and the neighborhood is saved.");
	}
	if (!HasFriend(TEXT("Mouth")))
	{
		return CurrentLevelNumber == 1
			? TEXT("Tutorial: walk to the green friend marker and recruit Mouth so you can practice switching.")
			: TEXT("Hub: recruit Mouth near the treehouse and enter Level 1.");
	}
	if (CurrentLevelNumber == 1 && !HasCompletedMapForLevel(2))
	{
		return TEXT("Tutorial: collect all four Rusty Lighthouse map pieces, then earn 10 coins from Daily Tasks and buy the Grappling Hook at the hub store.");
	}
	if (!HasFriend(TEXT("Data")))
	{
		return TEXT("Find Data at the lighthouse workshop for bridge repairs.");
	}
	if (!HasFriend(TEXT("Brand")))
	{
		return TEXT("Find Brand near the flooded tunnel crates.");
	}
	if (CurrentLevelNumber == 0)
	{
		return TEXT("Neighborhood hub: complete daily tasks for coins, buy required gear, then choose an adventure marker.");
	}
	if (ArtifactCount < 12 || !HasCompletedMapForLevel(12) || FriendshipTokenCount < 8)
	{
		return CurrentObjective;
	}
	return TEXT("All collections ready. Enter the final shipwreck vault and recover the treasure.");
}

FString ACrooniesTreasureTroopCharacter::GetTutorialPromptText() const
{
	if (CurrentLevelNumber != 1 || HasCompletedMapForLevel(2))
	{
		return TEXT("");
	}
	if (TutorialStep <= 1)
	{
		return TEXT("Step 1: Press W to walk forward to the glowing green friend marker.");
	}
	if (TutorialStep == 2)
	{
		return TEXT("Step 2: Stand on the green marker, then press G to recruit Mouth.");
	}
	if (!HasFriend(TEXT("Mouth")))
	{
		return TEXT("Step 2: Touch the green friend marker and press G to add Mouth to your crew.");
	}
	if (TutorialStep == 3)
	{
		return TEXT("Step 3: Press 3 to switch from Mikey to Mouth.");
	}
	if (TutorialStep == 4)
	{
		return TEXT("Step 4: Walk along the gold path with W/A/S/D to the attic switch.");
	}
	if (TutorialStep == 5)
	{
		return TEXT("Step 5: Press G at the attic switch to turn on the hanging bulb, then collect the four Rusty Lighthouse map pieces.");
	}
	return TEXT("Step 6: Find all 4 map pieces, return to the hub, complete Daily Tasks, and buy the Grappling Hook.");
}




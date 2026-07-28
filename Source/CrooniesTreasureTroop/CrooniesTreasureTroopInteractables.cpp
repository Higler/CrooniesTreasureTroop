#include "CrooniesTreasureTroopInteractables.h"
#include "CrooniesTreasureTroopCharacter.h"
#include "CrooniesTreasureTroopPlayerController.h"
#include "Components/BoxComponent.h"
#include "Components/BillboardComponent.h"
#include "Components/SphereComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/PointLightComponent.h"
#include "Engine/Engine.h"
#include "Engine/Texture2D.h"
#include "GameFramework/PlayerController.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

namespace
{
	void Tint(UStaticMeshComponent* Mesh, const FLinearColor& Color)
	{
		if (Mesh)
		{
			if (UMaterialInstanceDynamic* Mat = Mesh->CreateAndSetMaterialInstanceDynamic(0))
			{
				Mat->SetVectorParameterValue(TEXT("Color"), Color);
			}
		}
	}

	void ConfigureText(UTextRenderComponent* Text, const FString& Value, const FLinearColor& Color, float Size = 58.f)
	{
		if (!Text)
		{
			return;
		}
		Text->SetText(FText::FromString(Value));
		Text->SetTextRenderColor(Color.ToFColor(true));
		Text->SetWorldSize(Size);
		Text->SetHorizontalAlignment(EHTA_Center);
		Text->SetVerticalAlignment(EVRTA_TextCenter);
		Text->SetRelativeLocation(FVector(0.f, 0.f, 185.f));
		Text->SetRelativeRotation(FRotator::ZeroRotator);
	}

	void HidePart(UStaticMeshComponent* Part)
	{
		if (Part)
		{
			Part->SetVisibility(false);
		}
	}

	void ConfigurePart(UStaticMeshComponent* Part, UStaticMesh* StaticMesh, const FVector& Location, const FVector& Scale, const FLinearColor& Color, const FRotator& Rotation = FRotator::ZeroRotator)
	{
		if (!Part || !StaticMesh)
		{
			return;
		}
		Part->SetVisibility(true);
		Part->SetStaticMesh(StaticMesh);
		Part->SetRelativeLocation(Location);
		Part->SetRelativeRotation(Rotation);
		Part->SetRelativeScale3D(Scale);
		Tint(Part, Color);
	}

	const TCHAR* TexturePathForCharacter(FName CharacterId)
	{
		if (CharacterId == TEXT("Scout") || CharacterId == TEXT("Mikey"))
		{
			return TEXT("/Game/Cove/UIIcons/Mikey_Leader.Mikey_Leader");
		}
		if (CharacterId == TEXT("Quip") || CharacterId == TEXT("Mouth"))
		{
			return TEXT("/Game/Cove/UIIcons/Quip_Friend_Transparent.Quip_Friend_Transparent");
		}
		if (CharacterId == TEXT("Inventor") || CharacterId == TEXT("Data"))
		{
			return TEXT("/Game/Cove/UIIcons/Data_Inventor_Kid.Data_Inventor_Kid");
		}
		if (CharacterId == TEXT("Strong") || CharacterId == TEXT("Brand"))
		{
			return TEXT("/Game/Cove/UIIcons/Brand_Big_Brother.Brand_Big_Brother");
		}
		if (CharacterId == TEXT("Swimmer") || CharacterId == TEXT("Stef"))
		{
			return TEXT("/Game/Cove/UIIcons/Stef_Tomboy.Stef_Tomboy");
		}
		if (CharacterId == TEXT("Flashlight") || CharacterId == TEXT("Andy"))
		{
			return TEXT("/Game/Cove/UIIcons/Andy_Cheerleader_Updated.Andy_Cheerleader_Updated");
		}
		if (CharacterId == TEXT("Rope") || CharacterId == TEXT("Chunk"))
		{
			return TEXT("/Game/Cove/UIIcons/Chunk_Clumsy_Kid.Chunk_Clumsy_Kid");
		}
		if (CharacterId == TEXT("FriendlyGiant") || CharacterId == TEXT("Sloth"))
		{
			return TEXT("/Game/Cove/UIIcons/Sloth_Fratelli_Friendly_Giant.Sloth_Fratelli_Friendly_Giant");
		}
		if (CharacterId == TEXT("Mama") || CharacterId == TEXT("GrumpelliMama"))
		{
			return TEXT("/Game/Cove/UIIcons/Mama_Fratelli_Leader_Villain.Mama_Fratelli_Leader_Villain");
		}
		if (CharacterId == TEXT("Francis") || CharacterId == TEXT("GrumpelliFrancis"))
		{
			return TEXT("/Game/Cove/UIIcons/Francis_Fratelli_Short_Angry_Villain.Francis_Fratelli_Short_Angry_Villain");
		}
		if (CharacterId == TEXT("Jake") || CharacterId == TEXT("GrumpelliJake"))
		{
			return TEXT("/Game/Cove/UIIcons/Jake_Fratelli_Tall_Goofy_Villainjpg.Jake_Fratelli_Tall_Goofy_Villainjpg");
		}
		if (CharacterId == TEXT("Bat"))
		{
			return TEXT("/Game/Cove/UIIcons/batsflashlight.batsflashlight");
		}
		return TEXT("/Game/Cove/UIIcons/Mikey_Leader.Mikey_Leader");
	}

	UTexture2D* LoadCharacterTexture(FName CharacterId)
	{
		return LoadObject<UTexture2D>(nullptr, TexturePathForCharacter(CharacterId));
	}

	const TCHAR* TexturePathForPickup(ECovePickupKind Kind)
	{
		switch (Kind)
		{
		case ECovePickupKind::Artifact:
			return TEXT("/Game/Cove/UIIcons/Skull_Pirate_Symbol_Transparent.Skull_Pirate_Symbol_Transparent");
		case ECovePickupKind::MapPiece:
			return TEXT("/Game/Cove/UIIcons/Treasure_Map.Treasure_Map");
		case ECovePickupKind::Coin:
			return TEXT("/Game/Cove/UIIcons/goldcoin.goldcoin");
		case ECovePickupKind::FriendshipToken:
			return TEXT("/Game/Cove/UIIcons/heartfriendship.heartfriendship");
		case ECovePickupKind::Upgrade:
			return TEXT("/Game/Cove/UIIcons/flashlight.flashlight");
		case ECovePickupKind::SpecialItem:
			return TEXT("/Game/Cove/UIIcons/grapplinghook.grapplinghook");
		case ECovePickupKind::TreasureLoot:
			return TEXT("/Game/Cove/UIIcons/Golden_Treasure_Chest.Golden_Treasure_Chest");
		case ECovePickupKind::StaminaBoost:
			return TEXT("/Game/Cove/UIIcons/Potion_Time_Magic_Shop_Transparent.Potion_Time_Magic_Shop_Transparent");
		case ECovePickupKind::CrewChallengeKey:
			return TEXT("/Game/Cove/UIIcons/compass.compass");
		case ECovePickupKind::CrewChallengeChest:
			return TEXT("/Game/Cove/UIIcons/Golden_Treasure_Chest.Golden_Treasure_Chest");
		case ECovePickupKind::FinalTreasure:
			return TEXT("/Game/Cove/UIIcons/Golden_Treasure_Chest.Golden_Treasure_Chest");
		default:
			return TEXT("/Game/Cove/UIIcons/compass.compass");
		}
	}

	UTexture2D* LoadPickupTexture(ECovePickupKind Kind)
	{
		return LoadObject<UTexture2D>(nullptr, TexturePathForPickup(Kind));
	}

	int32 ParseMapTargetLevel(FName PickupId, int32 FallbackLevel)
	{
		if (PickupId.IsNone())
		{
			return FallbackLevel;
		}
		const FString Raw = PickupId.ToString();
		FString LevelPart;
		FString FragmentPart;
		if (Raw.Split(TEXT("."), &LevelPart, &FragmentPart) || Raw.Split(TEXT(":"), &LevelPart, &FragmentPart))
		{
			return FCString::Atoi(*LevelPart);
		}
		return FCString::Atoi(*Raw);
	}

	int32 ParseMapFragmentIndex(FName PickupId)
	{
		const FString Raw = PickupId.ToString();
		FString LevelPart;
		FString FragmentPart;
		if (Raw.Split(TEXT("."), &LevelPart, &FragmentPart) || Raw.Split(TEXT(":"), &LevelPart, &FragmentPart))
		{
			return FMath::Clamp(FCString::Atoi(*FragmentPart), 1, 4);
		}
		return 1;
	}

	UTexture2D* LoadMapFragmentTexture(FName PickupId)
	{
		const int32 Fragment = ParseMapFragmentIndex(PickupId);
		const FString Path = FString::Printf(TEXT("/Game/Cove/UIIcons/mapfragment%d_Transparent.mapfragment%d_Transparent"), Fragment, Fragment);
		if (UTexture2D* FragmentTexture = LoadObject<UTexture2D>(nullptr, *Path))
		{
			return FragmentTexture;
		}
		return LoadPickupTexture(ECovePickupKind::MapPiece);
	}

	void ApplySpriteSheetFrame(UBillboardComponent* Billboard, float TimeSeconds)
	{
		if (!Billboard || !Billboard->Sprite)
		{
			return;
		}

		const int32 TextureWidth = Billboard->Sprite->GetSizeX();
		const int32 TextureHeight = Billboard->Sprite->GetSizeY();
		if (TextureWidth <= 0 || TextureHeight <= 0)
		{
			return;
		}

		const int32 FrameCount = 4;
		const int32 FrameWidth = FMath::Max(1, TextureWidth / FrameCount);
		const int32 FrameIndex = FMath::Clamp(FMath::FloorToInt(TimeSeconds * 3.0f) % FrameCount, 0, FrameCount - 1);
		Billboard->SetUV(FrameIndex * FrameWidth, FrameWidth, 0, TextureHeight);
	}

	void FaceTextToCamera(UTextRenderComponent* Text, UWorld* World)
	{
		if (!Text || !World)
		{
			return;
		}

		APlayerController* PC = World->GetFirstPlayerController();
		if (!PC || !PC->PlayerCameraManager)
		{
			return;
		}

		const FVector ToCamera = PC->PlayerCameraManager->GetCameraLocation() - Text->GetComponentLocation();
		if (!ToCamera.IsNearlyZero())
		{
			FRotator Facing = ToCamera.Rotation();
			Facing.Roll = 0.f;
			Text->SetWorldRotation(Facing);
		}
	}
}

ACovePickup::ACovePickup()
{
	PrimaryActorTick.bCanEverTick = true;

	Trigger = CreateDefaultSubobject<USphereComponent>(TEXT("Trigger"));
	Trigger->InitSphereRadius(95.f);
	Trigger->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	RootComponent = Trigger;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	AccentMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AccentMesh"));
	AccentMesh->SetupAttachment(RootComponent);
	AccentMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	DetailMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DetailMesh"));
	DetailMesh->SetupAttachment(RootComponent);
	DetailMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	MarkerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MarkerMesh"));
	MarkerMesh->SetupAttachment(RootComponent);
	MarkerMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	PortraitSprite = CreateDefaultSubobject<UBillboardComponent>(TEXT("PortraitSprite"));
	PortraitSprite->SetupAttachment(RootComponent);
	PortraitSprite->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PortraitSprite->SetVisibility(false);
	PortraitSprite->bIsScreenSizeScaled = false;
	PortraitSprite->ScreenSize = 0.0025f;
	PortraitSprite->SetRelativeLocation(FVector(0.f, 0.f, 68.f));
	PortraitSprite->SetRelativeScale3D(FVector(1.35f));

	ItemSprite = CreateDefaultSubobject<UBillboardComponent>(TEXT("ItemSprite"));
	ItemSprite->SetupAttachment(RootComponent);
	ItemSprite->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ItemSprite->SetVisibility(false);
	ItemSprite->bIsScreenSizeScaled = false;
	ItemSprite->ScreenSize = 0.0025f;
	ItemSprite->SetRelativeLocation(FVector(0.f, 0.f, 64.f));
	ItemSprite->SetRelativeScale3D(FVector(0.95f));

	Label = CreateDefaultSubobject<UTextRenderComponent>(TEXT("PickupLabel"));
	Label->SetupAttachment(RootComponent);
	Label->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (SphereMesh.Succeeded())
	{
		Mesh->SetStaticMesh(SphereMesh.Object);
		AccentMesh->SetStaticMesh(SphereMesh.Object);
		DetailMesh->SetStaticMesh(SphereMesh.Object);
		MarkerMesh->SetStaticMesh(SphereMesh.Object);
	}
	Mesh->SetRelativeScale3D(FVector(0.45f));
	AccentMesh->SetRelativeScale3D(FVector(0.18f));
	AccentMesh->SetRelativeLocation(FVector(0.f, 0.f, 46.f));
}

void ACovePickup::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!Label)
	{
		return;
	}

	FaceTextToCamera(Label, GetWorld());
	if (Kind == ECovePickupKind::Friend && PortraitSprite && PortraitSprite->IsVisible())
	{
		ApplySpriteSheetFrame(PortraitSprite, GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f);
	}
}

void ACovePickup::BeginPlay()
{
	Super::BeginPlay();
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &ACovePickup::OnOverlap);

	UStaticMesh* Sphere = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	UStaticMesh* Cube = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
	UStaticMesh* Cone = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cone.Cone"));
	UStaticMesh* Cylinder = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));

	HidePart(Mesh);
	HidePart(AccentMesh);
	HidePart(DetailMesh);
	HidePart(MarkerMesh);
	if (Label)
	{
		Label->SetVisibility(true);
	}
	if (ItemSprite)
	{
		ItemSprite->SetVisibility(false);
	}

	switch (Kind)
	{
	case ECovePickupKind::Friend:
		if (PortraitSprite)
		{
			PortraitSprite->SetSprite(LoadCharacterTexture(FriendId));
			PortraitSprite->SetVisibility(PortraitSprite->Sprite != nullptr);
			ApplySpriteSheetFrame(PortraitSprite, GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f);
		}
		ConfigurePart(Mesh, Cube, FVector(0.f, 0.f, -42.f), FVector(0.72f, 0.54f, 0.12f), FLinearColor(0.03f, 0.28f, 0.14f));
		ConfigurePart(AccentMesh, Sphere, FVector(0.f, 0.f, 8.f), FVector(0.22f), FLinearColor(0.10f, 0.88f, 0.34f));
		ConfigureText(Label, FString::Printf(TEXT("FRIEND\n%s"), FriendId.IsNone() ? TEXT("Crew") : *FriendId.ToString()), FLinearColor(0.68f, 1.f, 0.72f), 82.f);
		break;
	case ECovePickupKind::Artifact:
		if (ItemSprite)
		{
			ItemSprite->SetSprite(LoadPickupTexture(Kind));
			ItemSprite->SetVisibility(ItemSprite->Sprite != nullptr);
			ItemSprite->SetRelativeScale3D(FVector(0.90f));
		}
		ConfigurePart(Mesh, Cube, FVector(0.f, 0.f, -30.f), FVector(0.54f, 0.54f, 0.10f), FLinearColor(0.25f, 0.12f, 0.04f));
		ConfigurePart(AccentMesh, Cylinder, FVector(0.f, 0.f, 0.f), FVector(0.30f, 0.30f, 0.20f), FLinearColor(0.68f, 0.36f, 0.12f));
		ConfigurePart(DetailMesh, Cone, FVector(0.f, 0.f, 48.f), FVector(0.42f, 0.42f, 0.54f), FLinearColor(0.98f, 0.48f, 0.12f));
		ConfigurePart(MarkerMesh, Sphere, FVector(0.f, 0.f, 103.f), FVector(0.17f), FLinearColor(1.f, 0.92f, 0.28f));
		ConfigureText(Label, FString::Printf(TEXT("ARTIFACT\nL%d"), FriendId.IsNone() ? 0 : FCString::Atoi(*FriendId.ToString())), FLinearColor(1.f, 0.76f, 0.34f), 86.f);
		break;
	case ECovePickupKind::MapPiece:
		if (ItemSprite)
		{
			ItemSprite->SetSprite(LoadMapFragmentTexture(FriendId));
			ItemSprite->SetVisibility(ItemSprite->Sprite != nullptr);
			ItemSprite->SetHiddenInGame(false);
			ItemSprite->SetRelativeLocation(FVector(0.f, 0.f, 58.f));
			ItemSprite->SetRelativeScale3D(FVector(0.58f));
		}
		ConfigurePart(Mesh, Cube, FVector(0.f, 0.f, 0.f), FVector(0.78f, 0.48f, 0.055f), FLinearColor(0.98f, 0.76f, 0.42f), FRotator(0.f, 0.f, 9.f));
		ConfigurePart(AccentMesh, Cube, FVector(-12.f, 0.f, 18.f), FVector(0.52f, 0.055f, 0.030f), FLinearColor(0.28f, 0.12f, 0.04f), FRotator(0.f, 0.f, 9.f));
		ConfigurePart(DetailMesh, Cube, FVector(22.f, 0.f, 29.f), FVector(0.26f, 0.055f, 0.030f), FLinearColor(0.50f, 0.26f, 0.08f), FRotator(0.f, 0.f, -14.f));
		ConfigurePart(MarkerMesh, Cylinder, FVector(0.f, -31.f, 18.f), FVector(0.07f, 0.07f, 0.52f), FLinearColor(0.22f, 0.10f, 0.04f), FRotator(90.f, 0.f, 0.f));
		ConfigureText(Label, FString::Printf(TEXT("MAP %d/4\nL%d"), ParseMapFragmentIndex(FriendId), ParseMapTargetLevel(FriendId, 0)), FLinearColor(1.f, 0.84f, 0.42f), 48.f);
		break;
	case ECovePickupKind::Coin:
		if (ItemSprite)
		{
			ItemSprite->SetSprite(LoadPickupTexture(Kind));
			ItemSprite->SetVisibility(ItemSprite->Sprite != nullptr);
			ItemSprite->SetRelativeScale3D(FVector(0.80f));
		}
		ConfigurePart(Mesh, Cylinder, FVector(0.f, 0.f, 12.f), FVector(0.34f, 0.34f, 0.08f), FLinearColor(1.f, 0.72f, 0.05f), FRotator(90.f, 0.f, 0.f));
		ConfigurePart(AccentMesh, Sphere, FVector(0.f, 0.f, 26.f), FVector(0.10f), FLinearColor(1.f, 0.92f, 0.36f));
		ConfigureText(Label, TEXT("COIN"), FLinearColor(1.f, 0.86f, 0.22f), 82.f);
		break;
	case ECovePickupKind::TreasureLoot:
		if (ItemSprite)
		{
			ItemSprite->SetSprite(LoadPickupTexture(Kind));
			ItemSprite->SetVisibility(ItemSprite->Sprite != nullptr);
			ItemSprite->SetRelativeScale3D(FVector(0.86f));
		}
		ConfigurePart(Mesh, Cube, FVector(0.f, 0.f, 0.f), FVector(0.50f, 0.38f, 0.24f), FLinearColor(0.38f, 0.18f, 0.06f));
		ConfigurePart(AccentMesh, Cube, FVector(0.f, 0.f, 36.f), FVector(0.54f, 0.42f, 0.10f), FLinearColor(0.92f, 0.68f, 0.18f));
		ConfigurePart(DetailMesh, Sphere, FVector(22.f, -18.f, 52.f), FVector(0.10f), FLinearColor(0.35f, 0.90f, 1.f));
		ConfigureText(Label, TEXT("LOOT"), FLinearColor(1.f, 0.82f, 0.36f), 78.f);
		break;
	case ECovePickupKind::StaminaBoost:
		if (ItemSprite)
		{
			ItemSprite->SetSprite(LoadPickupTexture(Kind));
			ItemSprite->SetVisibility(ItemSprite->Sprite != nullptr);
			ItemSprite->SetRelativeScale3D(FVector(0.76f));
		}
		ConfigurePart(Mesh, Cylinder, FVector(0.f, 0.f, 24.f), FVector(0.18f, 0.18f, 0.34f), FLinearColor(0.10f, 0.85f, 0.62f));
		ConfigurePart(AccentMesh, Sphere, FVector(0.f, 0.f, 66.f), FVector(0.12f), FLinearColor(0.78f, 1.f, 0.86f));
		ConfigureText(Label, TEXT("STAMINA"), FLinearColor(0.66f, 1.f, 0.78f), 72.f);
		break;
	case ECovePickupKind::CrewChallengeKey:
		if (ItemSprite)
		{
			ItemSprite->SetSprite(LoadPickupTexture(Kind));
			ItemSprite->SetVisibility(ItemSprite->Sprite != nullptr);
			ItemSprite->SetRelativeScale3D(FVector(0.78f));
		}
		ConfigurePart(Mesh, Cylinder, FVector(0.f, 0.f, 12.f), FVector(0.16f, 0.16f, 0.42f), FLinearColor(1.f, 0.78f, 0.18f), FRotator(0.f, 0.f, 18.f));
		ConfigurePart(AccentMesh, Cube, FVector(20.f, 0.f, 42.f), FVector(0.34f, 0.08f, 0.08f), FLinearColor(0.88f, 0.58f, 0.08f));
		ConfigureText(Label, TEXT("CREW\nKEY"), FLinearColor(1.f, 0.88f, 0.34f), 72.f);
		break;
	case ECovePickupKind::CrewChallengeChest:
		if (ItemSprite)
		{
			ItemSprite->SetSprite(LoadPickupTexture(Kind));
			ItemSprite->SetVisibility(ItemSprite->Sprite != nullptr);
			ItemSprite->SetRelativeScale3D(FVector(0.92f));
		}
		ConfigurePart(Mesh, Cube, FVector(0.f, 0.f, 0.f), FVector(0.72f, 0.48f, 0.26f), FLinearColor(0.34f, 0.16f, 0.055f));
		ConfigurePart(AccentMesh, Cube, FVector(0.f, 0.f, 38.f), FVector(0.78f, 0.52f, 0.12f), FLinearColor(0.96f, 0.70f, 0.12f));
		ConfigurePart(DetailMesh, Sphere, FVector(0.f, -32.f, 58.f), FVector(0.12f), FLinearColor(0.10f, 0.86f, 1.f));
		ConfigureText(Label, TEXT("CREW\nCHEST"), FLinearColor(1.f, 0.86f, 0.34f), 72.f);
		break;
	case ECovePickupKind::FriendshipToken:
		if (ItemSprite)
		{
			ItemSprite->SetSprite(LoadPickupTexture(Kind));
			ItemSprite->SetVisibility(ItemSprite->Sprite != nullptr);
			ItemSprite->SetRelativeScale3D(FVector(0.82f));
		}
		ConfigurePart(Mesh, Sphere, FVector(-18.f, 0.f, 24.f), FVector(0.24f), FLinearColor(1.f, 0.22f, 0.54f));
		ConfigurePart(AccentMesh, Sphere, FVector(18.f, 0.f, 24.f), FVector(0.24f), FLinearColor(1.f, 0.36f, 0.64f));
		ConfigurePart(DetailMesh, Cube, FVector(0.f, 0.f, -2.f), FVector(0.30f, 0.30f, 0.30f), FLinearColor(1.f, 0.16f, 0.44f), FRotator(0.f, 0.f, 45.f));
		ConfigurePart(MarkerMesh, Sphere, FVector(0.f, 0.f, 60.f), FVector(0.09f), FLinearColor(1.f, 0.80f, 0.92f));
		ConfigureText(Label, TEXT("FRIENDSHIP\nTOKEN"), FLinearColor(1.f, 0.68f, 0.88f), 78.f);
		break;
	case ECovePickupKind::Upgrade:
		if (ItemSprite)
		{
			ItemSprite->SetSprite(LoadPickupTexture(Kind));
			ItemSprite->SetVisibility(ItemSprite->Sprite != nullptr);
			ItemSprite->SetRelativeScale3D(FVector(0.90f));
		}
		ConfigurePart(Mesh, Cube, FVector(0.f, 0.f, 10.f), FVector(0.44f, 0.34f, 0.48f), FLinearColor(0.48f, 0.24f, 0.92f));
		ConfigurePart(AccentMesh, Cylinder, FVector(0.f, -22.f, 48.f), FVector(0.16f, 0.16f, 0.16f), FLinearColor(0.24f, 0.84f, 1.f), FRotator(90.f, 0.f, 0.f));
		ConfigurePart(DetailMesh, Sphere, FVector(24.f, 0.f, 28.f), FVector(0.10f), FLinearColor(1.f, 0.92f, 0.24f));
		ConfigurePart(MarkerMesh, Cube, FVector(-26.f, 0.f, 28.f), FVector(0.06f, 0.36f, 0.26f), FLinearColor(0.16f, 0.08f, 0.32f));
		ConfigureText(Label, FString::Printf(TEXT("UPGRADE\n%s"), FriendId.IsNone() ? TEXT("Gear") : *FriendId.ToString()), FLinearColor(0.82f, 0.72f, 1.f), 74.f);
		break;
	case ECovePickupKind::SpecialItem:
		if (ItemSprite)
		{
			ItemSprite->SetSprite(LoadPickupTexture(Kind));
			ItemSprite->SetVisibility(ItemSprite->Sprite != nullptr);
			ItemSprite->SetRelativeScale3D(FVector(0.92f));
		}
		ConfigurePart(Mesh, Cylinder, FVector(0.f, 0.f, 20.f), FVector(0.18f, 0.18f, 0.62f), FLinearColor(0.12f, 0.85f, 1.f), FRotator(0.f, 0.f, 25.f));
		ConfigurePart(AccentMesh, Cone, FVector(34.f, 0.f, 58.f), FVector(0.24f, 0.24f, 0.34f), FLinearColor(0.86f, 1.f, 1.f), FRotator(0.f, 0.f, -65.f));
		ConfigurePart(DetailMesh, Sphere, FVector(-28.f, 0.f, -14.f), FVector(0.14f), FLinearColor(0.06f, 0.36f, 0.42f));
		ConfigureText(Label, TEXT("SPECIAL\nGEAR"), FLinearColor(0.72f, 0.96f, 1.f), 78.f);
		break;
	case ECovePickupKind::FinalTreasure:
		if (ItemSprite)
		{
			ItemSprite->SetSprite(LoadPickupTexture(Kind));
			ItemSprite->SetVisibility(ItemSprite->Sprite != nullptr);
			ItemSprite->SetRelativeScale3D(FVector(1.05f));
		}
		ConfigurePart(Mesh, Cube, FVector(0.f, 0.f, 0.f), FVector(0.86f, 0.56f, 0.26f), FLinearColor(0.44f, 0.20f, 0.06f));
		ConfigurePart(AccentMesh, Cube, FVector(0.f, 0.f, 36.f), FVector(0.92f, 0.60f, 0.16f), FLinearColor(1.f, 0.78f, 0.08f));
		ConfigurePart(DetailMesh, Sphere, FVector(0.f, -34.f, 56.f), FVector(0.16f), FLinearColor(0.12f, 0.88f, 1.f));
		ConfigurePart(MarkerMesh, Cube, FVector(0.f, -54.f, 8.f), FVector(0.16f, 0.05f, 0.16f), FLinearColor(1.f, 0.92f, 0.30f));
		ConfigureText(Label, TEXT("TREASURE"), FLinearColor(1.f, 0.90f, 0.24f), 92.f);
		break;
	default:
		ConfigurePart(Mesh, Cone, FVector(0.f, 0.f, 32.f), FVector(0.38f), FLinearColor(0.15f, 0.9f, 1.f));
		ConfigurePart(AccentMesh, Sphere, FVector(0.f, 0.f, 78.f), FVector(0.16f), FLinearColor(1.f, 0.92f, 0.42f));
		ConfigureText(Label, TEXT("PICKUP"), FLinearColor(1.f, 0.76f, 0.34f), 82.f);
		break;
	}
}

void ACovePickup::OnOverlap(UPrimitiveComponent*, AActor* OtherActor, UPrimitiveComponent*, int32, bool, const FHitResult&)
{
	if (ACrooniesTreasureTroopCharacter* Character = Cast<ACrooniesTreasureTroopCharacter>(OtherActor))
	{
		TryCollect(Character);
	}
}

bool ACovePickup::TryCollect(ACrooniesTreasureTroopCharacter* Character)
{
	if (!Character || IsActorBeingDestroyed())
	{
		return false;
	}

	if (Kind == ECovePickupKind::Friend)
	{
		Character->RecruitFriend(FriendId);
	}
	else if (Kind == ECovePickupKind::Artifact)
	{
		const int32 SourceLevel = FriendId.IsNone() ? Character->GetCurrentLevel() : FCString::Atoi(*FriendId.ToString());
		if (Character->HasArtifactForLevel(SourceLevel))
		{
			Destroy();
			return true;
		}
		Character->AddArtifactForLevel(SourceLevel);
	}
	else if (Kind == ECovePickupKind::MapPiece)
	{
		const int32 TargetLevel = ParseMapTargetLevel(FriendId, Character->GetCurrentLevel() + 1);
		Character->AddMapPieceForLevel(TargetLevel);
	}
	else if (Kind == ECovePickupKind::Coin)
	{
		Character->AddCoins(10);
	}
	else if (Kind == ECovePickupKind::FriendshipToken)
	{
		Character->AddFriendshipToken();
	}
	else if (Kind == ECovePickupKind::TreasureLoot)
	{
		Character->AddRunLoot(35, 8.f);
	}
	else if (Kind == ECovePickupKind::StaminaBoost)
	{
		Character->RestoreRunStamina(30.f);
	}
	else if (Kind == ECovePickupKind::CrewChallengeKey)
	{
		if (ACrooniesTreasureTroopPlayerController* TroopPC = Cast<ACrooniesTreasureTroopPlayerController>(Character->GetController()))
		{
			TroopPC->CollectCrewChallengeKey();
		}
	}
	else if (Kind == ECovePickupKind::CrewChallengeChest)
	{
		ACrooniesTreasureTroopPlayerController* TroopPC = Cast<ACrooniesTreasureTroopPlayerController>(Character->GetController());
		if (!TroopPC || !TroopPC->TryOpenCrewChallengeChest())
		{
			return false;
		}
	}
	else if (Kind == ECovePickupKind::Upgrade)
	{
		Character->AddUpgrade(FriendId);
	}
	else if (Kind == ECovePickupKind::SpecialItem)
	{
		Character->AddSpecialItem(FriendId);
	}
	else
	{
		Character->ClaimFinalTreasure();
		if (!Character->HasWon())
		{
			return false;
		}
	}
	Destroy();
	return true;
}

ACoveEnemy::ACoveEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	Trigger->SetBoxExtent(FVector(92.f, 92.f, 135.f));
	Trigger->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	RootComponent = Trigger;

	EnemySprite = CreateDefaultSubobject<UBillboardComponent>(TEXT("EnemySprite"));
	EnemySprite->SetupAttachment(RootComponent);
	EnemySprite->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	EnemySprite->bIsScreenSizeScaled = false;
	EnemySprite->ScreenSize = 0.004f;
	EnemySprite->SetRelativeLocation(FVector(0.f, 0.f, 78.f));
	EnemySprite->SetRelativeScale3D(FVector(6.0f));

	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EnemyBody"));
	BodyMesh->SetupAttachment(RootComponent);
	BodyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BodyMesh->SetVisibility(false);

	AccentMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EnemyAccent"));
	AccentMesh->SetupAttachment(RootComponent);
	AccentMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AccentMesh->SetVisibility(false);

	Label = CreateDefaultSubobject<UTextRenderComponent>(TEXT("EnemyLabel"));
	Label->SetupAttachment(RootComponent);
	Label->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Label->SetRelativeLocation(FVector(0.f, 0.f, 220.f));
	Label->SetHorizontalAlignment(EHTA_Center);
	Label->SetVerticalAlignment(EVRTA_TextCenter);
	Label->SetWorldSize(72.f);
	Label->SetTextRenderColor(FColor(255, 90, 70));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ConeMesh(TEXT("/Engine/BasicShapes/Cone.Cone"));
	if (SphereMesh.Succeeded())
	{
		BodyMesh->SetStaticMesh(SphereMesh.Object);
	}
	if (ConeMesh.Succeeded())
	{
		AccentMesh->SetStaticMesh(ConeMesh.Object);
	}
}

void ACoveEnemy::BeginPlay()
{
	Super::BeginPlay();
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &ACoveEnemy::OnOverlap);
	SpawnOrigin = GetActorLocation();

	if (EnemyId.IsNone())
	{
		EnemyId = TEXT("Jake");
	}
	if (EnemySprite)
	{
		EnemySprite->SetSprite(LoadCharacterTexture(EnemyId));
		EnemySprite->SetVisibility(EnemyId != TEXT("Rat"));
		EnemySprite->SetRelativeLocation(EnemyId == TEXT("Bat") ? FVector(0.f, 0.f, 126.f) : FVector(0.f, 0.f, 78.f));
		EnemySprite->SetRelativeScale3D(EnemyId == TEXT("Bat") ? FVector(2.8f) : FVector(6.0f));
		ApplySpriteSheetFrame(EnemySprite, GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f);
	}
	if (EnemyId == TEXT("Rat"))
	{
		ConfigurePart(BodyMesh, BodyMesh ? BodyMesh->GetStaticMesh() : nullptr, FVector(0.f, 0.f, -38.f), FVector(0.42f, 0.22f, 0.16f), FLinearColor(0.18f, 0.18f, 0.17f));
		ConfigurePart(AccentMesh, AccentMesh ? AccentMesh->GetStaticMesh() : nullptr, FVector(34.f, 0.f, -26.f), FVector(0.16f, 0.16f, 0.20f), FLinearColor(0.08f, 0.08f, 0.075f), FRotator(0.f, 90.f, 0.f));
	}
	else
	{
		HidePart(BodyMesh);
		HidePart(AccentMesh);
	}
	if (EnemyId == TEXT("Bat"))
	{
		Trigger->SetBoxExtent(FVector(82.f, 82.f, 92.f));
	}
	else if (EnemyId == TEXT("Rat"))
	{
		Trigger->SetBoxExtent(FVector(70.f, 50.f, 45.f));
	}
	ConfigureText(Label, FString::Printf(TEXT("ENEMY\n%s"), *EnemyId.ToString()), EnemyId == TEXT("Bat") ? FLinearColor(0.74f, 0.58f, 1.f) : FLinearColor(1.f, 0.28f, 0.18f), EnemyId == TEXT("Rat") ? 62.f : 82.f);
}

void ACoveEnemy::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bRecentlyHit)
	{
		HitCooldown -= DeltaSeconds;
		if (HitCooldown <= 0.f)
		{
			bRecentlyHit = false;
			HitCooldown = 0.f;
		}
	}

	if (UWorld* World = GetWorld())
	{
		if (APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(World, 0))
		{
			const FVector Current = GetActorLocation();
			const FVector Player = PlayerPawn->GetActorLocation();
			FVector Target = SpawnOrigin;
			float MoveSpeed = 70.f + FMath::Clamp(LevelNumber, 1, 12) * 8.f;

			if (IsBoundedEnemy())
			{
				const float Time = World->GetTimeSeconds();
				const float Radius = GetBoundedEnemyRadius();
				MoveSpeed = GetBoundedEnemySpeed();
				const FVector PatrolOffset(
					FMath::Cos(Time * (EnemyId == TEXT("Bat") ? 0.85f : 0.45f) + LevelNumber) * Radius,
					FMath::Sin(Time * (EnemyId == TEXT("Bat") ? 1.15f : 0.70f) + LevelNumber * 0.47f) * Radius,
					0.f);
				Target = SpawnOrigin + PatrolOffset;

				const float DetectRadius = Radius * 0.75f;
				if (FVector::Dist2D(SpawnOrigin, Player) <= DetectRadius)
				{
					Target = ClampToBoundedArea(Player);
				}
			}
			else
			{
				const float DetectRadius = 720.f + FMath::Clamp(LevelNumber, 1, 12) * 45.f;
				if (FVector::Dist2D(Current, Player) <= DetectRadius)
				{
					Target = Player;
				}
				else
				{
					const float PatrolOffset = FMath::Sin(World->GetTimeSeconds() * 0.55f + LevelNumber) * 180.f;
					Target = SpawnOrigin + FVector(0.f, PatrolOffset, 0.f);
				}
			}

			FVector Direction = Target - Current;
			Direction.Z = 0.f;
			if (!Direction.IsNearlyZero())
			{
				FVector Next = Current + Direction.GetSafeNormal() * MoveSpeed * DeltaSeconds;
				if (IsBoundedEnemy())
				{
					Next = ClampToBoundedArea(Next);
				}
				SetActorLocation(Next, true);
			}
		}
	}

	if (EnemySprite)
	{
		ApplySpriteSheetFrame(EnemySprite, GetWorld() ? GetWorld()->GetTimeSeconds() + LevelNumber : 0.f);
	}
	FaceTextToCamera(Label, GetWorld());
}


bool ACoveEnemy::IsBoundedEnemy() const
{
	return EnemyId == TEXT("Rat") || EnemyId == TEXT("Bat");
}

float ACoveEnemy::GetBoundedEnemyRadius() const
{
	return EnemyId == TEXT("Bat") ? 520.f : 340.f;
}

float ACoveEnemy::GetBoundedEnemySpeed() const
{
	const int32 Difficulty = FMath::Clamp(LevelNumber, 1, 12);
	return EnemyId == TEXT("Bat") ? 145.f + Difficulty * 6.f : 92.f + Difficulty * 4.f;
}

FVector ACoveEnemy::ClampToBoundedArea(const FVector& Location) const
{
	FVector Offset = Location - SpawnOrigin;
	Offset.Z = 0.f;
	const float Radius = GetBoundedEnemyRadius();
	if (Offset.SizeSquared2D() > FMath::Square(Radius))
	{
		Offset = Offset.GetSafeNormal() * Radius;
	}

	FVector Clamped = SpawnOrigin + Offset;
	Clamped.Z = EnemyId == TEXT("Bat") ? SpawnOrigin.Z + 70.f : SpawnOrigin.Z;
	return Clamped;
}
void ACoveEnemy::OnOverlap(UPrimitiveComponent*, AActor* OtherActor, UPrimitiveComponent*, int32, bool, const FHitResult&)
{
	if (bRecentlyHit)
	{
		return;
	}

	if (ACrooniesTreasureTroopCharacter* Character = Cast<ACrooniesTreasureTroopCharacter>(OtherActor))
	{
		bRecentlyHit = true;
		HitCooldown = 2.5f;
		Character->HitTrap();
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.5f, FColor(255, 80, 70), FString::Printf(TEXT("%s caught the crew! Use movement and teamwork to slip past."), *EnemyId.ToString()));
		}
	}
}

ACoveLevelZone::ACoveLevelZone()
{
	PrimaryActorTick.bCanEverTick = false;

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	Trigger->SetBoxExtent(FVector(260.f, 260.f, 120.f));
	Trigger->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	RootComponent = Trigger;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ZoneMarker"));
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		Mesh->SetStaticMesh(CubeMesh.Object);
	}
	Mesh->SetRelativeScale3D(FVector(2.4f, 2.4f, 0.08f));
}

void ACoveLevelZone::BeginPlay()
{
	Super::BeginPlay();
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &ACoveLevelZone::OnOverlap);
	const float Hue = FMath::Fmod(LevelNumber * 0.113f, 1.f);
	Tint(Mesh, FLinearColor::MakeFromHSV8(static_cast<uint8>(Hue * 255.f), 190, 220));
}

void ACoveLevelZone::OnOverlap(UPrimitiveComponent*, AActor* OtherActor, UPrimitiveComponent*, int32, bool, const FHitResult&)
{
	if (ACrooniesTreasureTroopCharacter* Character = Cast<ACrooniesTreasureTroopCharacter>(OtherActor))
	{
		Character->EnterLevel(LevelNumber, LevelTitle, LevelStyle, Objective);
	}
}

ACoveHubPortal::ACoveHubPortal()
{
	PrimaryActorTick.bCanEverTick = false;

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	Trigger->SetBoxExtent(FVector(280.f, 260.f, 150.f));
	Trigger->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	RootComponent = Trigger;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PortalPad"));
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetVisibility(false);
	Mesh->SetHiddenInGame(true);

	IconSprite = CreateDefaultSubobject<UBillboardComponent>(TEXT("PortalIconSprite"));
	IconSprite->SetupAttachment(RootComponent);
	IconSprite->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	IconSprite->bIsScreenSizeScaled = false;
	IconSprite->ScreenSize = 0.0025f;
	IconSprite->SetRelativeLocation(FVector(0.f, 0.f, 122.f));
	IconSprite->SetRelativeScale3D(FVector(1.35f));
	IconSprite->SetVisibility(false);
	IconSprite->SetHiddenInGame(false);

	PortalText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("PortalText"));
	PortalText->SetupAttachment(RootComponent);
	PortalText->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PortalText->SetRelativeLocation(FVector(0.f, -175.f, 48.f));
	PortalText->SetHorizontalAlignment(EHTA_Center);
	PortalText->SetVerticalAlignment(EVRTA_TextCenter);
	PortalText->SetWorldSize(24.f);
	PortalText->SetTextRenderColor(FColor(255, 214, 92));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		Mesh->SetStaticMesh(CubeMesh.Object);
	}
	Mesh->SetRelativeScale3D(FVector(5.0f, 4.4f, 0.12f));
}

void ACoveHubPortal::BeginPlay()
{
	Super::BeginPlay();
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &ACoveHubPortal::OnOverlap);
	Mesh->SetVisibility(false);
	Mesh->SetHiddenInGame(true);
	if (TargetLevelNumber == 0)
	{
		if (IconSprite)
		{
			IconSprite->SetVisibility(false);
			IconSprite->SetHiddenInGame(true);
		}
		if (PortalText)
		{
			PortalText->SetVisibility(false);
			PortalText->SetHiddenInGame(true);
		}
		return;
	}
	if (IconSprite && PortalIconTexture)
	{
		IconSprite->SetSprite(PortalIconTexture);
		IconSprite->SetVisibility(true);
		IconSprite->SetHiddenInGame(false);
	}
	ConfigureText(PortalText, PortalLabel.ToUpper(), FLinearColor(1.f, 0.80f, 0.24f), 22.f);
}

void ACoveHubPortal::OnOverlap(UPrimitiveComponent*, AActor* OtherActor, UPrimitiveComponent*, int32, bool, const FHitResult&)
{
	ACrooniesTreasureTroopCharacter* Character = Cast<ACrooniesTreasureTroopCharacter>(OtherActor);
	if (!Character)
	{
		return;
	}

	if (ACrooniesTreasureTroopPlayerController* TroopPC = Cast<ACrooniesTreasureTroopPlayerController>(Character->GetController()))
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.5f, FColor(255, 220, 90), FString::Printf(TEXT("Entering %s"), *PortalLabel));
		}
		if (TargetLevelNumber == -1)
		{
			TroopPC->OpenCavernsMenu();
		}
		else if (TargetLevelNumber == -2)
		{
			TroopPC->OpenSpecialLevelsMenu();
		}
		else if (TargetLevelNumber == -3)
		{
			TroopPC->OpenLocalStoreMenu();
		}
		else if (TargetLevelNumber == -4)
		{
			TroopPC->OpenDailyTasksMenu();
		}
		else if (TargetLevelNumber == -5)
		{
			TroopPC->OpenPremiumStoreMenu();
		}
		else if (TargetLevelNumber == 0)
		{
			TroopPC->TeleportToHub();
		}
		else
		{
			TroopPC->TeleportToLevelNumber(TargetLevelNumber);
		}
	}
}

ACoveGate::ACoveGate()
{
	PrimaryActorTick.bCanEverTick = false;

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	Trigger->SetBoxExtent(FVector(180.f, 80.f, 120.f));
	Trigger->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	RootComponent = Trigger;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GateMesh"));
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetCollisionProfileName(TEXT("BlockAll"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		Mesh->SetStaticMesh(CubeMesh.Object);
	}
	Mesh->SetRelativeScale3D(FVector(3.f, 0.6f, 1.4f));
}

void ACoveGate::BeginPlay()
{
	Super::BeginPlay();
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &ACoveGate::OnOverlap);
	Tint(Mesh, FLinearColor(0.55f, 0.38f, 0.22f));
}

void ACoveGate::OnOverlap(UPrimitiveComponent*, AActor* OtherActor, UPrimitiveComponent*, int32, bool, const FHitResult&)
{
	if (ACrooniesTreasureTroopCharacter* Character = Cast<ACrooniesTreasureTroopCharacter>(OtherActor))
	{
		TryUse(Character);
	}
}

bool ACoveGate::TryUse(ACrooniesTreasureTroopCharacter* Character)
{
	if (!Character || IsActorBeingDestroyed())
	{
		return false;
	}

	if (RequiredFriend.IsNone() || Character->HasFriend(RequiredFriend))
	{
		if (GEngine)
		{
			const FString Who = RequiredFriend.IsNone() ? TEXT("The crew") : RequiredFriend.ToString();
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("%s handled the obstacle."), *Who));
		}
		Destroy();
		return true;
	}
	if (GEngine)
	{
		const FString Message = LockedMessage.IsEmpty()
			? FString::Printf(TEXT("This obstacle needs %s."), *RequiredFriend.ToString())
			: LockedMessage;
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor(190, 190, 190), Message);
	}
	return false;
}

ACoveAtticLightSwitch::ACoveAtticLightSwitch()
{
	PrimaryActorTick.bCanEverTick = false;

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	Trigger->SetBoxExtent(FVector(135.f, 135.f, 110.f));
	Trigger->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	RootComponent = Trigger;

	SwitchSprite = CreateDefaultSubobject<UBillboardComponent>(TEXT("SwitchSprite"));
	SwitchSprite->SetupAttachment(RootComponent);
	SwitchSprite->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SwitchSprite->bIsScreenSizeScaled = false;
	SwitchSprite->ScreenSize = 0.0022f;
	SwitchSprite->SetRelativeLocation(FVector(0.f, 0.f, 92.f));
	SwitchSprite->SetRelativeScale3D(FVector(1.05f));

	BulbSprite = CreateDefaultSubobject<UBillboardComponent>(TEXT("BulbSprite"));
	BulbSprite->SetupAttachment(RootComponent);
	BulbSprite->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BulbSprite->bIsScreenSizeScaled = false;
	BulbSprite->ScreenSize = 0.0025f;
	BulbSprite->SetRelativeLocation(FVector(940.f, -480.f, 420.f));
	BulbSprite->SetRelativeScale3D(FVector(1.25f));

	AtticLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("AtticPointLight"));
	AtticLight->SetupAttachment(RootComponent);
	AtticLight->SetRelativeLocation(FVector(940.f, -480.f, 330.f));
	AtticLight->SetAttenuationRadius(2300.f);
	AtticLight->SetIntensity(0.f);
	AtticLight->SetLightColor(FLinearColor(1.f, 0.78f, 0.38f));

	Label = CreateDefaultSubobject<UTextRenderComponent>(TEXT("SwitchLabel"));
	Label->SetupAttachment(RootComponent);
	Label->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Label->SetRelativeLocation(FVector(0.f, 0.f, 185.f));
	Label->SetHorizontalAlignment(EHTA_Center);
	Label->SetVerticalAlignment(EVRTA_TextCenter);
	Label->SetWorldSize(42.f);
	Label->SetTextRenderColor(FColor(255, 220, 96));
}

void ACoveAtticLightSwitch::BeginPlay()
{
	Super::BeginPlay();
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &ACoveAtticLightSwitch::OnOverlap);
	if (SwitchSprite)
	{
		SwitchSprite->SetSprite(LoadObject<UTexture2D>(nullptr, TEXT("/Game/Cove/UIIcons/Light_Switch_Transparent.Light_Switch_Transparent")));
	}
	if (BulbSprite)
	{
		BulbSprite->SetSprite(LoadObject<UTexture2D>(nullptr, TEXT("/Game/Cove/UIIcons/Hanging_Light_Bulb_Transparent.Hanging_Light_Bulb_Transparent")));
	}
	ConfigureText(Label, TEXT("LIGHT SWITCH\nG: toggle"), FLinearColor(1.f, 0.82f, 0.32f), 42.f);
}

void ACoveAtticLightSwitch::OnOverlap(UPrimitiveComponent*, AActor* OtherActor, UPrimitiveComponent*, int32, bool, const FHitResult&)
{
	if (Cast<ACrooniesTreasureTroopCharacter>(OtherActor) && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor(255, 220, 90), TEXT("Press G to toggle the attic light."));
	}
}

bool ACoveAtticLightSwitch::TryUse(ACrooniesTreasureTroopCharacter* Character)
{
	if (!Character)
	{
		return false;
	}

	bLightOn = !bLightOn;
	if (AtticLight)
	{
		AtticLight->SetIntensity(bLightOn ? 12000.f : 0.f);
	}
	Character->SetAtticLightOn(bLightOn);
	if (Label)
	{
		ConfigureText(Label, bLightOn ? TEXT("ATTIC LIGHT\nON") : TEXT("LIGHT SWITCH\nG: toggle"), FLinearColor(1.f, 0.82f, 0.32f), 42.f);
	}
	Character->NotifyInteractPressed();
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.5f, FColor(255, 235, 150), bLightOn ? TEXT("The attic light clicks on.") : TEXT("The attic light clicks off."));
	}
	return true;
}

ACoveTrap::ACoveTrap()
{
	PrimaryActorTick.bCanEverTick = false;

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	Trigger->SetBoxExtent(FVector(120.f, 120.f, 40.f));
	Trigger->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	RootComponent = Trigger;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TrapMesh"));
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	AccentMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TrapAccent"));
	AccentMesh->SetupAttachment(RootComponent);
	AccentMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	DetailMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TrapDetail"));
	DetailMesh->SetupAttachment(RootComponent);
	DetailMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Label = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TrapLabel"));
	Label->SetupAttachment(RootComponent);
	Label->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Label->SetRelativeLocation(FVector(0.f, 0.f, 155.f));
	Label->SetHorizontalAlignment(EHTA_Center);
	Label->SetVerticalAlignment(EVRTA_TextCenter);
	Label->SetWorldSize(46.f);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ConeMesh(TEXT("/Engine/BasicShapes/Cone.Cone"));
	if (CubeMesh.Succeeded())
	{
		Mesh->SetStaticMesh(CubeMesh.Object);
	}
	if (SphereMesh.Succeeded())
	{
		AccentMesh->SetStaticMesh(SphereMesh.Object);
	}
	if (ConeMesh.Succeeded())
	{
		DetailMesh->SetStaticMesh(ConeMesh.Object);
	}
}

void ACoveTrap::BeginPlay()
{
	Super::BeginPlay();
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &ACoveTrap::OnOverlap);

	HidePart(Mesh);
	HidePart(AccentMesh);
	HidePart(DetailMesh);
	if (Label)
	{
		Label->SetVisibility(true);
	}

	switch (TrapKind)
	{
	case ECoveTrapKind::Fire:
		Trigger->SetBoxExtent(FVector(120.f, 120.f, 70.f));
		ConfigurePart(Mesh, Mesh ? Mesh->GetStaticMesh() : nullptr, FVector(0.f, 0.f, -26.f), FVector(1.55f, 1.55f, 0.08f), FLinearColor(0.18f, 0.04f, 0.02f));
		ConfigurePart(AccentMesh, AccentMesh ? AccentMesh->GetStaticMesh() : nullptr, FVector(0.f, 0.f, 10.f), FVector(0.42f, 0.42f, 0.72f), FLinearColor(1.f, 0.23f, 0.02f));
		ConfigurePart(DetailMesh, DetailMesh ? DetailMesh->GetStaticMesh() : nullptr, FVector(0.f, 0.f, 62.f), FVector(0.38f, 0.38f, 0.62f), FLinearColor(1.f, 0.78f, 0.10f));
		ConfigureText(Label, TEXT("FIRE"), FLinearColor(1.f, 0.42f, 0.12f), 50.f);
		break;
	case ECoveTrapKind::Gas:
		Trigger->SetBoxExtent(FVector(155.f, 155.f, 92.f));
		ConfigurePart(Mesh, Mesh ? Mesh->GetStaticMesh() : nullptr, FVector(0.f, 0.f, -30.f), FVector(1.80f, 1.80f, 0.06f), FLinearColor(0.10f, 0.20f, 0.09f));
		ConfigurePart(AccentMesh, AccentMesh ? AccentMesh->GetStaticMesh() : nullptr, FVector(-40.f, -24.f, 36.f), FVector(0.52f), FLinearColor(0.36f, 0.82f, 0.22f, 0.72f));
		ConfigurePart(DetailMesh, DetailMesh ? DetailMesh->GetStaticMesh() : nullptr, FVector(50.f, 30.f, 58.f), FVector(0.42f, 0.42f, 0.36f), FLinearColor(0.70f, 1.f, 0.36f, 0.62f));
		ConfigureText(Label, TEXT("GAS"), FLinearColor(0.62f, 1.f, 0.36f), 50.f);
		break;
	case ECoveTrapKind::Hole:
		Trigger->SetBoxExtent(FVector(145.f, 145.f, 36.f));
		ConfigurePart(Mesh, Mesh ? Mesh->GetStaticMesh() : nullptr, FVector(0.f, 0.f, -34.f), FVector(1.85f, 1.85f, 0.04f), FLinearColor(0.01f, 0.008f, 0.006f));
		ConfigurePart(AccentMesh, AccentMesh ? AccentMesh->GetStaticMesh() : nullptr, FVector(0.f, 0.f, -25.f), FVector(0.82f, 0.82f, 0.04f), FLinearColor(0.13f, 0.10f, 0.07f));
		ConfigureText(Label, TEXT("HOLE"), FLinearColor(0.42f, 0.35f, 0.25f), 46.f);
		break;
	case ECoveTrapKind::Skeleton:
	default:
		Trigger->SetBoxExtent(FVector(120.f, 120.f, 58.f));
		ConfigurePart(Mesh, Mesh ? Mesh->GetStaticMesh() : nullptr, FVector(0.f, 0.f, -30.f), FVector(1.55f, 1.55f, 0.08f), FLinearColor(0.20f, 0.16f, 0.11f));
		ConfigurePart(AccentMesh, AccentMesh ? AccentMesh->GetStaticMesh() : nullptr, FVector(0.f, 0.f, 30.f), FVector(0.34f), FLinearColor(0.86f, 0.82f, 0.64f));
		ConfigurePart(DetailMesh, DetailMesh ? DetailMesh->GetStaticMesh() : nullptr, FVector(44.f, 0.f, 0.f), FVector(0.16f, 0.16f, 0.72f), FLinearColor(0.76f, 0.72f, 0.55f), FRotator(0.f, 90.f, 0.f));
		ConfigureText(Label, TEXT("SKELETON"), FLinearColor(0.92f, 0.86f, 0.62f), 44.f);
		break;
	}
}

void ACoveTrap::OnOverlap(UPrimitiveComponent*, AActor* OtherActor, UPrimitiveComponent*, int32, bool, const FHitResult&)
{
	if (!bSpent)
	{
		if (ACrooniesTreasureTroopCharacter* Character = Cast<ACrooniesTreasureTroopCharacter>(OtherActor))
		{
			bSpent = true;
			Character->HitTrap();
			Tint(Mesh, FLinearColor(0.15f, 0.15f, 0.15f));
			Tint(AccentMesh, FLinearColor(0.15f, 0.15f, 0.15f));
			Tint(DetailMesh, FLinearColor(0.15f, 0.15f, 0.15f));
		}
	}
}




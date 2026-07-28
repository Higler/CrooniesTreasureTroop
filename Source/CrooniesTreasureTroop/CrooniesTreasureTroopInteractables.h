#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CrooniesTreasureTroopInteractables.generated.h"

UENUM()
enum class ECovePickupKind : uint8
{
	Friend,
	Artifact,
	MapPiece,
	Coin,
	FriendshipToken,
	Upgrade,
	SpecialItem,
	TreasureLoot,
	StaminaBoost,
	CrewChallengeKey,
	CrewChallengeChest,
	FinalTreasure
};

UCLASS()
class ACovePickup : public AActor
{
	GENERATED_BODY()

public:
	ACovePickup();

	virtual void Tick(float DeltaSeconds) override;

	bool TryCollect(class ACrooniesTreasureTroopCharacter* Character);

	UPROPERTY(EditAnywhere, Category = "Croonies")
	ECovePickupKind Kind = ECovePickupKind::Artifact;

	UPROPERTY(EditAnywhere, Category = "Croonies")
	FName FriendId;

	UPROPERTY(VisibleAnywhere)
	class USphereComponent* Trigger;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* AccentMesh;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* DetailMesh;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* MarkerMesh;

	UPROPERTY(VisibleAnywhere)
	class UBillboardComponent* PortraitSprite;

	UPROPERTY(VisibleAnywhere)
	class UBillboardComponent* ItemSprite;

	UPROPERTY(VisibleAnywhere)
	class UTextRenderComponent* Label;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};

UCLASS()
class ACoveEnemy : public AActor
{
	GENERATED_BODY()

public:
	ACoveEnemy();

	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Category = "Croonies")
	FName EnemyId;

	UPROPERTY(EditAnywhere, Category = "Croonies")
	int32 LevelNumber = 1;

	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* Trigger;

	UPROPERTY(VisibleAnywhere)
	class UBillboardComponent* EnemySprite;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* BodyMesh;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* AccentMesh;

	UPROPERTY(VisibleAnywhere)
	class UTextRenderComponent* Label;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	bool IsBoundedEnemy() const;
	float GetBoundedEnemyRadius() const;
	float GetBoundedEnemySpeed() const;
	FVector ClampToBoundedArea(const FVector& Location) const;

	bool bRecentlyHit = false;
	float HitCooldown = 0.f;
	FVector SpawnOrigin = FVector::ZeroVector;
};

UCLASS()
class ACoveLevelZone : public AActor
{
	GENERATED_BODY()

public:
	ACoveLevelZone();

	UPROPERTY(EditAnywhere, Category = "Croonies")
	int32 LevelNumber = 0;

	UPROPERTY(EditAnywhere, Category = "Croonies")
	FString LevelTitle;

	UPROPERTY(EditAnywhere, Category = "Croonies")
	FString LevelStyle;

	UPROPERTY(EditAnywhere, Category = "Croonies")
	FString Objective;

	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* Trigger;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* Mesh;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};

UCLASS()
class ACoveHubPortal : public AActor
{
	GENERATED_BODY()

public:
	ACoveHubPortal();

	UPROPERTY(EditAnywhere, Category = "Croonies")
	int32 TargetLevelNumber = 1;

	UPROPERTY(EditAnywhere, Category = "Croonies")
	FString PortalLabel;

	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* Trigger;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere)
	class UBillboardComponent* IconSprite;

	UPROPERTY(VisibleAnywhere)
	class UTextRenderComponent* PortalText;

	UPROPERTY(EditAnywhere, Category = "Croonies")
	class UTexture2D* PortalIconTexture;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};

UCLASS()
class ACoveGate : public AActor
{
	GENERATED_BODY()

public:
	ACoveGate();

	bool TryUse(class ACrooniesTreasureTroopCharacter* Character);

	UPROPERTY(EditAnywhere, Category = "Croonies")
	FName RequiredFriend;

	UPROPERTY(EditAnywhere, Category = "Croonies")
	FString LockedMessage;

	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* Trigger;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* Mesh;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};

UCLASS()
class ACoveAtticLightSwitch : public AActor
{
	GENERATED_BODY()

public:
	ACoveAtticLightSwitch();

	bool TryUse(class ACrooniesTreasureTroopCharacter* Character);

	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* Trigger;

	UPROPERTY(VisibleAnywhere)
	class UBillboardComponent* SwitchSprite;

	UPROPERTY(VisibleAnywhere)
	class UBillboardComponent* BulbSprite;

	UPROPERTY(VisibleAnywhere)
	class UPointLightComponent* AtticLight;

	UPROPERTY(VisibleAnywhere)
	class UTextRenderComponent* Label;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	bool bLightOn = false;
};

UENUM()
enum class ECoveTrapKind : uint8
{
	Skeleton,
	Fire,
	Gas,
	Hole
};

UCLASS()
class ACoveTrap : public AActor
{
	GENERATED_BODY()

public:
	ACoveTrap();

	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* Trigger;

	UPROPERTY(EditAnywhere, Category = "Croonies")
	ECoveTrapKind TrapKind = ECoveTrapKind::Skeleton;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* AccentMesh;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* DetailMesh;

	UPROPERTY(VisibleAnywhere)
	class UTextRenderComponent* Label;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	bool bSpent = false;
};

// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CrooniesTreasureTroopCharacter.generated.h"

class ACrooniesTreasureTroopGameMode;

USTRUCT(BlueprintType)
struct FCrooniesFriendProgress
{
	GENERATED_BODY()

	UPROPERTY()
	FName FriendId = NAME_None;

	UPROPERTY()
	FString DisplayName;

	UPROPERTY()
	FString Role;

	UPROPERTY()
	int32 Level = 1;

	UPROPERTY()
	int32 Experience = 0;

	UPROPERTY()
	int32 MaxCourage = 10;

	UPROPERTY()
	bool bStaticLevel = false;
};

UCLASS(Blueprintable)
class ACrooniesTreasureTroopCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ACrooniesTreasureTroopCharacter();

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;

	void RecruitFriend(FName FriendId);
	bool LevelUpFriend(FName FriendId);
	bool AddFriendExperience(FName FriendId, int32 Amount);
	void AddArtifact();
	void AddArtifactForLevel(int32 SourceLevel);
	void AddMapPiece();
	void AddMapPieceForLevel(int32 TargetLevel);
	void GrantTestMapPiecesThroughLevel(int32 HighestTargetLevel);
	void AddSpecialItem(FName ItemId);
	bool SpendCoins(int32 Amount);
	bool CompleteDailyTask(int32 TaskIndex);
	void AddCoins(int32 Amount);
	void AddCourage(int32 Amount);
	bool SpendCourage(int32 Amount);
	void AddRunLoot(int32 Value, float Weight);
	void RestoreRunStamina(float Amount);
	void SpendRunStamina(float Amount);
	void ResetRunStateForLevel(int32 LevelNumber);
	void AddFriendshipToken();
	void AddUpgrade(FName UpgradeId);
	void ClaimFinalTreasure();
	void HitTrap();
	void EnterLevel(int32 LevelNumber, const FString& Title, const FString& Style, const FString& Objective);
	void SelectCrewmate(int32 Slot);
	void ApplyCharacterCustomization(bool bInMale, int32 InSkinTone, int32 InHairStyle, int32 InHairColor, int32 InEyeColor);
	bool HasFriend(FName FriendId) const;
	int32 GetFriendLevel(FName FriendId) const;
	int32 GetFriendMaxCourage(FName FriendId) const;
	bool IsFriendStaticLevel(FName FriendId) const;
	FString GetFriendProgressSummary(FName FriendId) const;
	FString GetCrewProgressSummary() const;
	FString GetPartySummary() const;
	FString GetSelectedCrewmate() const;
	FString GetObjectiveText() const;
	FString GetTutorialPromptText() const;
	FString GetLevelTitle() const { return CurrentLevelTitle; }
	FString GetLevelStyle() const { return CurrentLevelStyle; }
	int32 GetArtifactCount() const { return ArtifactCount; }
	bool HasArtifactForLevel(int32 SourceLevel) const;
	int32 GetMapPieceCount() const { return MapPieceCount; }
	int32 GetMapPiecesForLevel(int32 TargetLevel) const;
	bool HasCompletedMapForLevel(int32 TargetLevel) const;
	bool HasSpecialItem(FName ItemId) const;
	bool HasRequiredSpecialItemForLevel(int32 TargetLevel) const;
	FName GetRequiredSpecialItemIdForLevel(int32 TargetLevel) const;
	FString GetRequiredSpecialItemNameForLevel(int32 TargetLevel) const;
	int32 GetCoinCount() const { return CoinCount; }
	int32 GetDailyTaskCount() const { return 10; }
	int32 GetCompletedDailyTaskCount() const;
	bool IsDailyTaskComplete(int32 TaskIndex) const;
	int32 GetFriendshipTokenCount() const { return FriendshipTokenCount; }
	int32 GetUpgradeCount() const { return UpgradeCount; }
	int32 GetCurrentLevel() const { return CurrentLevelNumber; }
	int32 GetCourage() const { return Courage; }
	float GetRunStamina() const { return RunStamina; }
	float GetMaxRunStamina() const { return MaxRunStamina; }
	float GetCarryWeight() const { return CarryWeight; }
	int32 GetRunLootValue() const { return RunLootValue; }
	int32 GetRunLootCount() const { return RunLootCount; }
	bool IsRunActive() const { return bRunActive; }
	bool HasWon() const { return bFoundFinalTreasure; }
	bool IsMale() const { return bMale; }
	int32 GetSkinTone() const { return SkinTone; }
	int32 GetHairStyle() const { return HairStyle; }
	int32 GetHairColor() const { return HairColor; }
	int32 GetEyeColor() const { return EyeColor; }
	bool IsAtticLightOn() const { return bAtticLightOn; }
	void SetAtticLightOn(bool bInLightOn);
	void NotifyMovedForward();
	void NotifyInteractPressed();

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = "Croonies")
	class UStaticMeshComponent* BaseCharacterMesh;

	UPROPERTY(VisibleAnywhere, Category = "Croonies")
	class UStaticMeshComponent* BodyMesh;

	UPROPERTY(VisibleAnywhere, Category = "Croonies")
	class UStaticMeshComponent* HeadMesh;

	UPROPERTY(VisibleAnywhere, Category = "Croonies")
	class UStaticMeshComponent* HairMesh;

	UPROPERTY(VisibleAnywhere, Category = "Croonies")
	class UStaticMeshComponent* BackpackMesh;

	UPROPERTY(VisibleAnywhere, Category = "Croonies")
	class UStaticMeshComponent* FootMesh;

	UPROPERTY(VisibleAnywhere, Category = "Croonies")
	class UBillboardComponent* CharacterSprite;

	UPROPERTY(VisibleAnywhere, Category = "Croonies")
	class UPointLightComponent* PlayerAuraLight;

	UPROPERTY()
	TSet<FName> Friends;

	UPROPERTY()
	TMap<FName, FCrooniesFriendProgress> FriendProgress;

	UPROPERTY()
	TSet<FName> Upgrades;

	UPROPERTY()
	TSet<FName> SpecialItems;

	UPROPERTY()
	TSet<int32> ArtifactLevels;

	UPROPERTY()
	TMap<int32, int32> MapPiecesByLevel;

	UPROPERTY()
	TSet<int32> CompletedDailyTasks;

	int32 ArtifactCount = 0;
	int32 MapPieceCount = 0;
	int32 CoinCount = 1000;
	int32 FriendshipTokenCount = 0;
	int32 UpgradeCount = 0;
	int32 CurrentLevelNumber = 0;
	int32 Courage = 1000;
	float MaxRunStamina = 100.f;
	float RunStamina = 100.f;
	float CarryWeight = 0.f;
	int32 RunLootValue = 0;
	int32 RunLootCount = 0;
	bool bRunActive = false;
	bool bFoundFinalTreasure = false;
	FName SelectedCrewmate = TEXT("Mikey");
	FString CurrentLevelTitle = TEXT("The Neighborhood");
	FString CurrentLevelStyle = TEXT("Hub");
	FString CurrentObjective = TEXT("Recruit friends, buy upgrades, and choose the next adventure.");
	bool bMale = true;
	int32 SkinTone = 1;
	int32 HairStyle = 0;
	int32 HairColor = 1;
	int32 EyeColor = 1;
	int32 TutorialStep = 0;
	bool bPressedForwardInTutorial = false;
	bool bPressedInteractInTutorial = false;
	bool bAtticLightOn = false;
};



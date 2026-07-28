// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "CrooniesTreasureTroopInteractables.h"
#include "GameFramework/PlayerController.h"
#include "CrooniesTreasureTroopPlayerController.generated.h"

/** Forward declaration to improve compiling times */
class UNiagaraSystem;
class UInputMappingContext;
class UInputAction;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UENUM()
enum class ECrooniesAppState : uint8
{
	Login,
	CharacterCreation,
	Playing
};

UENUM()
enum class ECrooniesChatChannel : uint8
{
	World,
	Team,
	Private
};

USTRUCT()
struct FCrooniesChatMessage
{
	GENERATED_BODY()

	UPROPERTY()
	ECrooniesChatChannel Channel = ECrooniesChatChannel::World;

	UPROPERTY()
	FString SenderName;

	UPROPERTY()
	FString TargetName;

	UPROPERTY()
	FString Body;

	UPROPERTY()
	int32 TeamId = 0;

	UPROPERTY()
	bool bSystemMessage = false;
};

UENUM()
enum class ECrooniesIdentityStatus : uint8
{
	LocalGuest,
	PendingPlatformValidation,
	Verified,
	Failed
};

USTRUCT()
struct FCrooniesCrewChallengeState
{
	GENERATED_BODY()

	UPROPERTY()
	bool bActive = false;

	UPROPERTY()
	bool bFinished = false;

	UPROPERTY()
	FString OpponentCrewName = TEXT("Lantern League");

	UPROPERTY()
	FString OpponentCrewTag = TEXT("LITE");

	UPROPERTY()
	FName SelectedMember = TEXT("Mikey");

	UPROPERTY()
	int32 KeysFound = 0;

	UPROPERTY()
	int32 ChestsOpened = 0;

	UPROPERTY()
	int32 OpponentChestsOpened = 0;

	UPROPERTY()
	float SecondsRemaining = 0.f;

	UPROPERTY()
	FString ResultText;
};

UCLASS()
class ACrooniesTreasureTroopPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ACrooniesTreasureTroopPlayerController();

	bool IsPauseMenuOpen() const { return bPauseMenuOpen; }
	bool IsSettingsOpen() const { return bSettingsOpen; }
	bool IsHubMenuOpen() const { return bHubMenuOpen; }
	int32 GetHubMenuMode() const { return HubMenuMode; }
	ECrooniesAppState GetAppState() const { return AppState; }
	FString GetLoginProvider() const { return LoginProvider; }
	bool IsMaleChoice() const { return bMaleChoice; }
	int32 GetSkinToneChoice() const { return SkinToneChoice; }
	int32 GetHairStyleChoice() const { return HairStyleChoice; }
	int32 GetHairColorChoice() const { return HairColorChoice; }
	int32 GetEyeColorChoice() const { return EyeColorChoice; }
	FString GetCharacterSummary() const;
	bool IsMusicEnabled() const { return bMusicEnabled; }
	bool IsSoundEffectsEnabled() const { return bSoundEffectsEnabled; }
	bool IsSubtitlesEnabled() const { return bSubtitlesEnabled; }
	bool IsVibrationEnabled() const { return bVibrationEnabled; }
	float GetMusicVolume() const { return MusicVolume; }
	float GetSoundEffectsVolume() const { return SoundEffectsVolume; }
	bool AreLoginOptionsOpen() const { return bLoginOptionsOpen; }
	FString GetPlatformSignInLabel() const;
	void TeleportToLevelNumber(int32 LevelNumber);
	bool CanTeleportToAdventureLevel(int32 LevelNumber, FString& LockedReason) const;
	int32 GetSpecialItemPriceForLevel(int32 LevelNumber) const;
	int32 GetSpecialLevelsAccessPrice() const;
	bool HasSpecialLevelsAccess() const;
	void OpenCavernsMenu();
	void OpenLocalStoreMenu();
	void OpenPremiumStoreMenu();
	void OpenSpecialLevelsMenu();
	void OpenDailyTasksMenu();
	void OpenCrewChallengeMenu();
	void BuySpecialItemForLevel(int32 LevelNumber);
	void BuySpecialLevelsAccessItem();
	void BuyLiquidCouragePotion();
	void UseLiquidCouragePotion();
	int32 GetLiquidCouragePotionCount() const { return LiquidCouragePotionCount; }
	FString GetLiquidCouragePotionPrice() const { return TEXT("$0.99"); }
	FString GetLiquidCourageProductId() const { return TEXT("croonies_liquid_courage_small_099"); }
	bool HasDailyLoginRewardNotice() const { return bDailyLoginRewardNotice; }
	FString GetDailyLoginRewardText() const { return DailyLoginRewardText; }
	int32 GetDailyLoginStreak() const { return DailyLoginStreak; }
	int32 GetPremiumStoreTab() const { return PremiumStoreTab; }
	void SetPremiumStoreTab(int32 NewTab);
	void CompleteDailyTask(int32 TaskIndex);
	void TeleportToHub();
	bool IsChatOpen() const { return bChatOpen; }
	const FString& GetChatDraft() const { return ChatDraft; }
	ECrooniesChatChannel GetActiveChatChannel() const { return ActiveChatChannel; }
	const TArray<FCrooniesChatMessage>& GetChatHistory() const { return ChatHistory; }
	FString GetChatChannelLabel() const;
	FString GetChatHelpText() const;
	int32 GetTeamId() const { return TeamId; }
	bool IsParentalChatEnabled() const { return bParentalChatEnabled; }
	bool IsProfanityFilterEnabled() const { return bProfanityFilterEnabled; }
	bool IsPrivateChatEnabled() const { return bPrivateChatEnabled; }
	bool IsReportingEnabled() const { return bReportingEnabled; }
	FString GetIdentityStatusLabel() const;
	FString GetCrewName() const { return CrewName; }
	FString GetCrewTag() const { return CrewTag; }
	FString GetCrewDisplayName() const;
	const TArray<FName>& GetCrewChallengeRoster() const { return CrewChallengeRoster; }
	int32 GetSelectedCrewChallengeMemberIndex() const { return SelectedCrewChallengeMemberIndex; }
	int32 GetCrewChallengeCourageCost() const { return CrewChallengeCourageCost; }
	const FCrooniesCrewChallengeState& GetCrewChallengeState() const { return CrewChallengeState; }
	void CollectCrewChallengeKey();
	bool TryOpenCrewChallengeChest();

	/** Time Threshold to know if it was a short press */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	float ShortPressThreshold;

	/** FX Class that we will spawn when clicking */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UNiagaraSystem* FXCursor;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;
	
	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* SetDestinationClickAction;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* SetDestinationTouchAction;

protected:
	/** True if the controlled character should navigate to the mouse cursor. */
	uint32 bMoveToMouseCursor : 1;

	virtual void SetupInputComponent() override;
	virtual void PlayerTick(float DeltaTime) override;
	virtual bool InputKey(const FInputKeyParams& Params) override;
	
	// To add mapping context
	virtual void BeginPlay();

	/** Input handlers for SetDestination action. */
	void OnInputStarted();
	void OnSetDestinationTriggered();
	void OnSetDestinationReleased();
	void OnTouchTriggered();
	void OnTouchReleased();
	void QuitGame();
	void TogglePauseMenu();
	void SignIn(const FString& Provider);
	void AdvanceCharacterCreation();
	void ChangeCharacterOption(int32 Delta);
	void ApplyCustomizationPreview();
	void SaveProfile();
	void TryGrantDailyLoginReward();
	bool HandleLoginClick();
	bool HandleCharacterCreationClick();
	bool HandleSettingsClick();
	void TeleportToOldAtticTutorial();
	void ToggleSettings();
	void ToggleHubMenu();
	bool HandleHubMenuClick();
	void PerformAction();
	void PerformJump();
	bool IsInSettingsButton(float MouseX, float MouseY, int32 SizeX, int32 SizeY) const;
	bool IsInExitButton(float MouseX, float MouseY, int32 SizeX, int32 SizeY) const;
	bool IsInActionButton(float MouseX, float MouseY, int32 SizeX, int32 SizeY) const;
	bool IsInJumpButton(float MouseX, float MouseY, int32 SizeX, int32 SizeY) const;
	void OpenChat(ECrooniesChatChannel Channel);
	void CloseChat();
	void CycleChatChannel();
	void SubmitChatDraft();
	void AppendChatCharacter(const FString& Text);
	bool TryHandleChatKey(const FKey& Key);
	FString GetChatDisplayName() const;
	bool IsChatRecipientMatch(const FString& RequestedName) const;
	bool IsChatAllowed(ECrooniesChatChannel Channel, FString& DeniedReason) const;
	FString ApplyProfanityFilter(const FString& RawMessage) const;
	void AddLocalSystemChatMessage(const FString& Body);
	void ReportPlayerFromChat(const FString& TargetName, const FString& Reason);
	void ValidatePlatformIdentity();
	bool HandleSafetySettingsClick(float PanelX, float PanelY, float MouseX, float MouseY);
	void SelectCrewChallengeMember(int32 Index);
	void StartCrewChallengeMatch();
	void SpawnCrewChallengePickups();
	class ACovePickup* SpawnCrewChallengePickup(const FVector& Location, ECovePickupKind Kind, FName PickupId);
	void FinishCrewChallengeMatch(bool bWon, const FString& Reason);

	UFUNCTION(Server, Reliable)
	void ServerSendChatMessage(ECrooniesChatChannel Channel, const FString& Body, const FString& TargetName);

	UFUNCTION(Client, Reliable)
	void ClientReceiveChatMessage(const FCrooniesChatMessage& Message);

	UFUNCTION(Server, Reliable)
	void ServerReportPlayer(const FString& ReporterName, const FString& TargetName, const FString& Reason, const TArray<FCrooniesChatMessage>& RecentContext);

private:
	FVector CachedDestination;

	bool bIsTouch; // Is it a touch device
	float FollowTime; // For how long it has been pressed
	bool bPauseMenuOpen = false;
	bool bSettingsOpen = false;
	bool bHubMenuOpen = false;
	bool bLoginOptionsOpen = false;
	int32 HubMenuMode = 0;
	ECrooniesAppState AppState = ECrooniesAppState::Login;
	FString LoginProvider = TEXT("Not signed in");
	int32 CreationField = 0;
	bool bMaleChoice = true;
	int32 SkinToneChoice = 1;
	int32 HairStyleChoice = 0;
	int32 HairColorChoice = 1;
	int32 EyeColorChoice = 1;
	bool bMusicEnabled = true;
	bool bSoundEffectsEnabled = true;
	bool bSubtitlesEnabled = true;
	bool bVibrationEnabled = true;
	float MusicVolume = 0.8f;
	float SoundEffectsVolume = 0.85f;
	bool bChatOpen = false;
	FString ChatDraft;
	ECrooniesChatChannel ActiveChatChannel = ECrooniesChatChannel::World;
	FString PrivateChatTarget = TEXT("Player");
	int32 TeamId = 1;
	TArray<FCrooniesChatMessage> ChatHistory;
	int32 LiquidCouragePotionCount = 0;
	FString LastDailyLoginRewardDate;
	int32 DailyLoginStreak = 0;
	bool bDailyLoginRewardNotice = false;
	FString DailyLoginRewardText;
	int32 PremiumStoreTab = 0;
	bool bParentalChatEnabled = true;
	bool bProfanityFilterEnabled = true;
	bool bPrivateChatEnabled = true;
	bool bReportingEnabled = true;
	ECrooniesIdentityStatus IdentityStatus = ECrooniesIdentityStatus::LocalGuest;
	FString CrewName = TEXT("Cavern Club");
	FString CrewTag = TEXT("CAVE");
	TArray<FName> CrewChallengeRoster;
	int32 SelectedCrewChallengeMemberIndex = 0;
	int32 CrewChallengeCourageCost = 4;
	FCrooniesCrewChallengeState CrewChallengeState;
	float CrewChallengeOpponentScoreTimer = 0.f;
};




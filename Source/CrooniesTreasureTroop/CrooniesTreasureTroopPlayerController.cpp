// Copyright Epic Games, Inc. All Rights Reserved.

#include "CrooniesTreasureTroopPlayerController.h"
#include "GameFramework/Pawn.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "CrooniesTreasureTroopCharacter.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "Engine/Engine.h"
#include "CrooniesTreasureTroopCharacter.h"
#include "CrooniesTreasureTroopInteractables.h"
#include "CrooniesTreasureTroopSaveGame.h"
#include "EngineUtils.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerState.h"
#include "InputCoreTypes.h"
#include "Kismet/GameplayStatics.h"
#include "HAL/PlatformFileManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

ACrooniesTreasureTroopPlayerController::ACrooniesTreasureTroopPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	CachedDestination = FVector::ZeroVector;
	FollowTime = 0.f;
	PrimaryActorTick.bTickEvenWhenPaused = true;
	bShouldPerformFullTickWhenPaused = true;
	CrewChallengeRoster = { TEXT("Mikey"), TEXT("Mouth"), TEXT("Data"), TEXT("Brand"), TEXT("Stef"), TEXT("Chunk"), TEXT("Andy"), TEXT("Sloth") };
	CrewChallengeState.OpponentCrewName = TEXT("Lantern League");
	CrewChallengeState.OpponentCrewTag = TEXT("LITE");
	CrewChallengeState.SelectedMember = CrewChallengeRoster.Num() > 0 ? CrewChallengeRoster[0] : TEXT("Mikey");
}

void ACrooniesTreasureTroopPlayerController::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	if (UGameplayStatics::DoesSaveGameExist(TEXT("CrooniesProfile"), 0))
	{
		if (UCrooniesTreasureTroopSaveGame* Save = Cast<UCrooniesTreasureTroopSaveGame>(UGameplayStatics::LoadGameFromSlot(TEXT("CrooniesProfile"), 0)))
		{
			LoginProvider = Save->PlatformProvider;
			bMaleChoice = Save->bMale;
			SkinToneChoice = Save->SkinTone;
			HairStyleChoice = Save->HairStyle;
			HairColorChoice = Save->HairColor;
			EyeColorChoice = Save->EyeColor;
			bParentalChatEnabled = Save->bParentalChatEnabled;
			bProfanityFilterEnabled = Save->bProfanityFilterEnabled;
			bPrivateChatEnabled = Save->bPrivateChatEnabled;
			bReportingEnabled = Save->bReportingEnabled;
			LiquidCouragePotionCount = Save->LiquidCouragePotionCount;
			LastDailyLoginRewardDate = Save->LastDailyLoginRewardDate;
			DailyLoginStreak = Save->DailyLoginStreak;
			ValidatePlatformIdentity();
			TryGrantDailyLoginReward();
		}
	}
	SetPause(true);
}

void ACrooniesTreasureTroopPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	// Add Input Mapping Context when the project has one configured.
	if (DefaultMappingContext)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		// Setup mouse input events
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Started, this, &ACrooniesTreasureTroopPlayerController::OnInputStarted);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Triggered, this, &ACrooniesTreasureTroopPlayerController::OnSetDestinationTriggered);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Completed, this, &ACrooniesTreasureTroopPlayerController::OnSetDestinationReleased);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Canceled, this, &ACrooniesTreasureTroopPlayerController::OnSetDestinationReleased);

		// Setup touch input events
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Started, this, &ACrooniesTreasureTroopPlayerController::OnInputStarted);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Triggered, this, &ACrooniesTreasureTroopPlayerController::OnTouchTriggered);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Completed, this, &ACrooniesTreasureTroopPlayerController::OnTouchReleased);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Canceled, this, &ACrooniesTreasureTroopPlayerController::OnTouchReleased);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ACrooniesTreasureTroopPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn)
	{
		return;
	}

	if (CrewChallengeState.bActive && !CrewChallengeState.bFinished)
	{
		CrewChallengeState.SecondsRemaining = FMath::Max(0.f, CrewChallengeState.SecondsRemaining - DeltaTime);
		CrewChallengeOpponentScoreTimer += DeltaTime;
		if (CrewChallengeOpponentScoreTimer >= 42.f)
		{
			CrewChallengeOpponentScoreTimer = 0.f;
			CrewChallengeState.OpponentChestsOpened = FMath::Clamp(CrewChallengeState.OpponentChestsOpened + 1, 0, 3);
			if (CrewChallengeState.OpponentChestsOpened >= 3)
			{
				FinishCrewChallengeMatch(false, TEXT("Opponent crew opened 3 chests first."));
			}
		}
		if (CrewChallengeState.SecondsRemaining <= 0.f)
		{
			FinishCrewChallengeMatch(false, TEXT("Timer expired."));
		}
	}

	if (WasInputKeyJustPressed(EKeys::Escape))
	{
		if (bChatOpen)
		{
			CloseChat();
			return;
		}
		if (bHubMenuOpen)
		{
			bHubMenuOpen = false;
			HubMenuMode = 0;
			if (AppState == ECrooniesAppState::Playing && !bPauseMenuOpen && !bSettingsOpen)
			{
				SetPause(false);
			}
			return;
		}
		if (bSettingsOpen)
		{
			bSettingsOpen = false;
			if (AppState == ECrooniesAppState::Playing && !bPauseMenuOpen)
			{
				SetPause(false);
			}
			return;
		}
		QuitGame();
		return;
	}

	if (WasInputKeyJustPressed(EKeys::LeftMouseButton))
	{
		int32 SizeX = 0;
		int32 SizeY = 0;
		float MouseX = 0.f;
		float MouseY = 0.f;
		GetViewportSize(SizeX, SizeY);
		if (GetMousePosition(MouseX, MouseY))
		{
			if (bSettingsOpen)
			{
				if (HandleSettingsClick())
				{
					return;
				}
			}
			else if (bHubMenuOpen)
			{
				if (HandleHubMenuClick())
				{
					return;
				}
			}
			else if (IsInExitButton(MouseX, MouseY, SizeX, SizeY))
			{
				QuitGame();
				return;
			}
			else if (IsInSettingsButton(MouseX, MouseY, SizeX, SizeY))
			{
				ToggleSettings();
				return;
			}
			else if (AppState == ECrooniesAppState::Playing && IsInActionButton(MouseX, MouseY, SizeX, SizeY))
			{
				PerformAction();
				return;
			}
			else if (AppState == ECrooniesAppState::Playing && IsInJumpButton(MouseX, MouseY, SizeX, SizeY))
			{
				PerformJump();
				return;
			}
		}
	}

	if (AppState == ECrooniesAppState::Login)
	{
		if (WasInputKeyJustPressed(EKeys::LeftMouseButton))
		{
			HandleLoginClick();
		}
		else if (WasInputKeyJustPressed(EKeys::Enter) || WasInputKeyJustPressed(EKeys::SpaceBar))
		{
			bLoginOptionsOpen = true;
		}
		else if (bLoginOptionsOpen && (WasInputKeyJustPressed(EKeys::G) || WasInputKeyJustPressed(EKeys::A)))
		{
			SignIn(GetPlatformSignInLabel());
		}
		else if (WasInputKeyJustPressed(EKeys::L))
		{
			SignIn(TEXT("Local Guest"));
		}
		return;
	}

	if (AppState == ECrooniesAppState::CharacterCreation)
	{
		if (WasInputKeyJustPressed(EKeys::LeftMouseButton))
		{
			HandleCharacterCreationClick();
		}
		else if (WasInputKeyJustPressed(EKeys::Up) || WasInputKeyJustPressed(EKeys::W))
		{
			CreationField = FMath::Max(0, CreationField - 1);
		}
		else if (WasInputKeyJustPressed(EKeys::Down) || WasInputKeyJustPressed(EKeys::S))
		{
			CreationField = FMath::Min(4, CreationField + 1);
		}
		else if (WasInputKeyJustPressed(EKeys::Left) || WasInputKeyJustPressed(EKeys::A))
		{
			ChangeCharacterOption(-1);
		}
		else if (WasInputKeyJustPressed(EKeys::Right) || WasInputKeyJustPressed(EKeys::D))
		{
			ChangeCharacterOption(1);
		}
		else if (WasInputKeyJustPressed(EKeys::Enter) || WasInputKeyJustPressed(EKeys::SpaceBar))
		{
			AdvanceCharacterCreation();
		}
		ApplyCustomizationPreview();
		return;
	}

	if (bChatOpen)
	{
		return;
	}

	if (WasInputKeyJustPressed(EKeys::P))
	{
		TogglePauseMenu();
		return;
	}

	if (WasInputKeyJustPressed(EKeys::Enter))
	{
		OpenChat(ActiveChatChannel);
		return;
	}
	if (WasInputKeyJustPressed(EKeys::Y))
	{
		OpenChat(ECrooniesChatChannel::World);
		return;
	}
	if (WasInputKeyJustPressed(EKeys::T))
	{
		OpenChat(ECrooniesChatChannel::Team);
		return;
	}
	if (WasInputKeyJustPressed(EKeys::U))
	{
		OpenChat(ECrooniesChatChannel::Private);
		return;
	}

	if (WasInputKeyJustPressed(EKeys::H))
	{
		TeleportToHub();
		return;
	}

	if (WasInputKeyJustPressed(EKeys::M))
	{
		ToggleHubMenu();
		return;
	}

	if (bHubMenuOpen)
	{
		static const FKey LevelKeys[] = {
			EKeys::One, EKeys::Two, EKeys::Three, EKeys::Four, EKeys::Five,
			EKeys::Six, EKeys::Seven, EKeys::Eight, EKeys::Nine, EKeys::Zero
		};
		for (int32 Index = 0; Index < UE_ARRAY_COUNT(LevelKeys); ++Index)
		{
			if (WasInputKeyJustPressed(LevelKeys[Index]))
			{
				if (HubMenuMode == 1)
				{
					const int32 TargetLevel = Index == 9 ? 11 : Index + 2;
					FString LockedReason;
					if (CanTeleportToAdventureLevel(TargetLevel, LockedReason))
					{
						TeleportToLevelNumber(TargetLevel);
					}
					else if (GEngine)
					{
						GEngine->AddOnScreenDebugMessage(-1, 3.5f, FColor(255, 205, 80), LockedReason);
					}
				}
				else if (HubMenuMode == 2 && Index < 4)
				{
					if (HasSpecialLevelsAccess())
					{
						TeleportToLevelNumber(101 + Index);
					}
					else
					{
						OpenSpecialLevelsMenu();
					}
				}
				else if (HubMenuMode == 3)
				{
					if (Index < 11)
					{
						BuySpecialItemForLevel(Index + 2);
					}
					else if (Index == 11)
					{
						BuySpecialLevelsAccessItem();
					}
				}
				else if (HubMenuMode == 4)
				{
					CompleteDailyTask(Index);
				}
				else if (HubMenuMode == 5)
				{
					if (Index < 6)
					{
						SetPremiumStoreTab(Index);
					}
					else if (PremiumStoreTab == 0 && Index == 6)
					{
						BuyLiquidCouragePotion();
					}
					else if (PremiumStoreTab == 0 && Index == 7)
					{
						UseLiquidCouragePotion();
					}
				}
				else if (HubMenuMode == 6)
				{
					if (Index < CrewChallengeRoster.Num())
					{
						SelectCrewChallengeMember(Index);
					}
					else if (Index == 8)
					{
						StartCrewChallengeMatch();
					}
				}
				else if (HubMenuMode == 0)
				{
					if (Index == 0)
					{
						TeleportToLevelNumber(1);
					}
					else if (Index == 1)
					{
						OpenCavernsMenu();
					}
					else if (Index == 2)
					{
						OpenDailyTasksMenu();
					}
					else if (Index == 3)
					{
						OpenLocalStoreMenu();
					}
					else if (Index == 4)
					{
						OpenSpecialLevelsMenu();
					}
					else if (Index == 5)
					{
						OpenPremiumStoreMenu();
					}
					else if (Index == 6)
					{
						OpenCrewChallengeMenu();
					}
				}
				return;
			}
		}
		if (WasInputKeyJustPressed(EKeys::B))
		{
			OpenSpecialLevelsMenu();
			return;
		}
		return;
	}

	if (WasInputKeyJustPressed(EKeys::SpaceBar))
	{
		PerformJump();
		return;
	}
	if (WasInputKeyJustReleased(EKeys::SpaceBar))
	{
		if (ACharacter* JumpCharacter = Cast<ACharacter>(ControlledPawn))
		{
			JumpCharacter->StopJumping();
		}
	}

	if (WasInputKeyJustPressed(EKeys::G))
	{
		PerformAction();
		return;
	}

	if (WasInputKeyJustPressed(EKeys::E) || WasInputKeyJustPressed(EKeys::F))
	{
		PerformAction();
		return;
	}

	if (bPauseMenuOpen)
	{
		if (WasInputKeyJustPressed(EKeys::Q))
		{
			QuitGame();
		}
		return;
	}

	FVector Direction = FVector::ZeroVector;
	if (IsInputKeyDown(EKeys::W) || IsInputKeyDown(EKeys::Up))
	{
		Direction.X += 1.f;
		if (ACrooniesTreasureTroopCharacter* TroopCharacter = Cast<ACrooniesTreasureTroopCharacter>(ControlledPawn))
		{
			TroopCharacter->NotifyMovedForward();
		}
	}
	if (IsInputKeyDown(EKeys::S) || IsInputKeyDown(EKeys::Down))
	{
		Direction.X -= 1.f;
	}
	if (IsInputKeyDown(EKeys::D) || IsInputKeyDown(EKeys::Right))
	{
		Direction.Y += 1.f;
	}
	if (IsInputKeyDown(EKeys::A) || IsInputKeyDown(EKeys::Left))
	{
		Direction.Y -= 1.f;
	}

	if (!Direction.IsNearlyZero())
	{
		StopMovement();
		ControlledPawn->AddMovementInput(Direction.GetSafeNormal(), 1.f);
	}

	if (ACrooniesTreasureTroopCharacter* CoveCharacter = Cast<ACrooniesTreasureTroopCharacter>(ControlledPawn))
	{
		static const FKey CrewKeys[] = {
			EKeys::One, EKeys::Two, EKeys::Three, EKeys::Four,
			EKeys::Five, EKeys::Six, EKeys::Seven, EKeys::Eight
		};

		for (int32 Index = 0; Index < UE_ARRAY_COUNT(CrewKeys); ++Index)
		{
			if (WasInputKeyJustPressed(CrewKeys[Index]))
			{
				CoveCharacter->SelectCrewmate(Index);
			}
		}
	}
}

bool ACrooniesTreasureTroopPlayerController::InputKey(const FInputKeyParams& Params)
{
	if (bChatOpen && Params.Event == IE_Pressed)
	{
		if (TryHandleChatKey(Params.Key))
		{
			return true;
		}
	}

	return Super::InputKey(Params);
}

void ACrooniesTreasureTroopPlayerController::OnInputStarted()
{
	StopMovement();
}

// Triggered every frame when the input is held down
void ACrooniesTreasureTroopPlayerController::OnSetDestinationTriggered()
{
	// We flag that the input is being pressed
	FollowTime += GetWorld()->GetDeltaSeconds();
	
	// We look for the location in the world where the player has pressed the input
	FHitResult Hit;
	bool bHitSuccessful = false;
	if (bIsTouch)
	{
		bHitSuccessful = GetHitResultUnderFinger(ETouchIndex::Touch1, ECollisionChannel::ECC_Visibility, true, Hit);
	}
	else
	{
		bHitSuccessful = GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);
	}

	// If we hit a surface, cache the location
	if (bHitSuccessful)
	{
		CachedDestination = Hit.Location;
	}
	
	// Move towards mouse pointer or touch
	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn != nullptr)
	{
		FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
		ControlledPawn->AddMovementInput(WorldDirection, 1.0, false);
	}
}

void ACrooniesTreasureTroopPlayerController::OnSetDestinationReleased()
{
	// If it was a short press
	if (FollowTime <= ShortPressThreshold)
	{
		// We move there and spawn some particles
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, CachedDestination);
		if (FXCursor)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, CachedDestination, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None, true);
		}
	}

	FollowTime = 0.f;
}

// Triggered every frame when the input is held down
void ACrooniesTreasureTroopPlayerController::OnTouchTriggered()
{
	bIsTouch = true;
	OnSetDestinationTriggered();
}

void ACrooniesTreasureTroopPlayerController::OnTouchReleased()
{
	bIsTouch = false;
	OnSetDestinationReleased();
}

void ACrooniesTreasureTroopPlayerController::QuitGame()
{
	ConsoleCommand(TEXT("quit"));
}

void ACrooniesTreasureTroopPlayerController::OpenChat(ECrooniesChatChannel Channel)
{
	if (AppState != ECrooniesAppState::Playing)
	{
		return;
	}

	ActiveChatChannel = Channel;
	bChatOpen = true;
	bPauseMenuOpen = false;
	bHubMenuOpen = false;
	HubMenuMode = 0;
	SetPause(false);
}

void ACrooniesTreasureTroopPlayerController::CloseChat()
{
	bChatOpen = false;
	ChatDraft.Empty();
}

void ACrooniesTreasureTroopPlayerController::CycleChatChannel()
{
	if (ActiveChatChannel == ECrooniesChatChannel::World)
	{
		ActiveChatChannel = ECrooniesChatChannel::Team;
	}
	else if (ActiveChatChannel == ECrooniesChatChannel::Team)
	{
		ActiveChatChannel = ECrooniesChatChannel::Private;
	}
	else
	{
		ActiveChatChannel = ECrooniesChatChannel::World;
	}
}

FString ACrooniesTreasureTroopPlayerController::GetChatChannelLabel() const
{
	if (ActiveChatChannel == ECrooniesChatChannel::Team)
	{
		return FString::Printf(TEXT("TEAM %d"), TeamId);
	}
	if (ActiveChatChannel == ECrooniesChatChannel::Private)
	{
		return FString::Printf(TEXT("PRIVATE -> %s"), *PrivateChatTarget);
	}
	return TEXT("WORLD");
}

FString ACrooniesTreasureTroopPlayerController::GetChatHelpText() const
{
	return TEXT("Enter sends | Esc cancels | Tab switches | Y world | T team | U private | /w PlayerName message | /report PlayerName reason");
}

FString ACrooniesTreasureTroopPlayerController::GetIdentityStatusLabel() const
{
	switch (IdentityStatus)
	{
	case ECrooniesIdentityStatus::Verified:
		return TEXT("Verified");
	case ECrooniesIdentityStatus::PendingPlatformValidation:
		return TEXT("Pending platform validation");
	case ECrooniesIdentityStatus::Failed:
		return TEXT("Validation failed");
	default:
		return TEXT("Local guest");
	}
}

FString ACrooniesTreasureTroopPlayerController::GetCrewDisplayName() const
{
	return FString::Printf(TEXT("%s [%s]"), *CrewName, *CrewTag.Left(4).ToUpper());
}

FString ACrooniesTreasureTroopPlayerController::GetChatDisplayName() const
{
	FString BaseName;
	if (PlayerState && !PlayerState->GetPlayerName().IsEmpty())
	{
		BaseName = PlayerState->GetPlayerName();
	}
	else if (LoginProvider == TEXT("Local Guest") || LoginProvider == TEXT("Not signed in") || LoginProvider == TEXT("Signed out"))
	{
		BaseName = TEXT("Local Croonie");
	}
	else
	{
		BaseName = LoginProvider;
	}

	const FString CleanTag = CrewTag.Left(4).ToUpper();
	return CleanTag.IsEmpty() ? BaseName : FString::Printf(TEXT("%s [%s]"), *BaseName, *CleanTag);
}

bool ACrooniesTreasureTroopPlayerController::IsChatRecipientMatch(const FString& RequestedName) const
{
	const FString OwnName = GetChatDisplayName();
	return OwnName.Equals(RequestedName, ESearchCase::IgnoreCase) || GetName().Equals(RequestedName, ESearchCase::IgnoreCase);
}

void ACrooniesTreasureTroopPlayerController::SubmitChatDraft()
{
	FString Body = ChatDraft;
	Body.TrimStartAndEndInline();
	if (Body.IsEmpty())
	{
		CloseChat();
		return;
	}

	ECrooniesChatChannel SendChannel = ActiveChatChannel;
	FString TargetName = PrivateChatTarget;

	if (Body.StartsWith(TEXT("/report "), ESearchCase::IgnoreCase))
	{
		TArray<FString> Parts;
		Body.ParseIntoArrayWS(Parts);
		if (Parts.Num() >= 3)
		{
			const FString ReportTarget = Parts[1];
			const int32 ReasonStart = Body.Find(Parts[2]);
			const FString Reason = ReasonStart != INDEX_NONE ? Body.Mid(ReasonStart) : TEXT("No reason supplied");
			ReportPlayerFromChat(ReportTarget, Reason);
		}
		else
		{
			AddLocalSystemChatMessage(TEXT("Report format: /report PlayerName reason"));
		}
		CloseChat();
		return;
	}

	if (Body.StartsWith(TEXT("/w "), ESearchCase::IgnoreCase) || Body.StartsWith(TEXT("/pm "), ESearchCase::IgnoreCase))
	{
		TArray<FString> Parts;
		Body.ParseIntoArrayWS(Parts);
		if (Parts.Num() >= 3)
		{
			SendChannel = ECrooniesChatChannel::Private;
			TargetName = Parts[1];
			const int32 MessageStart = Body.Find(Parts[2]);
			Body = MessageStart != INDEX_NONE ? Body.Mid(MessageStart) : TEXT("");
			PrivateChatTarget = TargetName;
		}
	}

	FString DeniedReason;
	if (!IsChatAllowed(SendChannel, DeniedReason))
	{
		AddLocalSystemChatMessage(DeniedReason);
		CloseChat();
		return;
	}

	ServerSendChatMessage(SendChannel, ApplyProfanityFilter(Body), TargetName);
	CloseChat();
}

void ACrooniesTreasureTroopPlayerController::AppendChatCharacter(const FString& Text)
{
	if (Text.IsEmpty() || ChatDraft.Len() >= 160)
	{
		return;
	}
	ChatDraft += Text.Left(1);
}

bool ACrooniesTreasureTroopPlayerController::TryHandleChatKey(const FKey& Key)
{
	if (Key == EKeys::Enter)
	{
		SubmitChatDraft();
		return true;
	}
	if (Key == EKeys::Escape)
	{
		CloseChat();
		return true;
	}
	if (Key == EKeys::Tab)
	{
		CycleChatChannel();
		return true;
	}
	if (Key == EKeys::BackSpace)
	{
		if (!ChatDraft.IsEmpty())
		{
			ChatDraft.LeftChopInline(1);
		}
		return true;
	}
	if (Key == EKeys::SpaceBar)
	{
		AppendChatCharacter(TEXT(" "));
		return true;
	}

	const bool bShift = IsInputKeyDown(EKeys::LeftShift) || IsInputKeyDown(EKeys::RightShift);
	const FString KeyName = Key.GetFName().ToString();
	if (KeyName.Len() == 1 && FChar::IsAlnum(KeyName[0]))
	{
		AppendChatCharacter(bShift ? KeyName.ToUpper() : KeyName.ToLower());
		return true;
	}
	if (Key == EKeys::Slash)
	{
		AppendChatCharacter(TEXT("/"));
		return true;
	}
	if (Key == EKeys::Period)
	{
		AppendChatCharacter(TEXT("."));
		return true;
	}
	if (Key == EKeys::Comma)
	{
		AppendChatCharacter(TEXT(","));
		return true;
	}
	if (Key == EKeys::Hyphen)
	{
		AppendChatCharacter(TEXT("-"));
		return true;
	}
	if (Key == EKeys::Apostrophe)
	{
		AppendChatCharacter(TEXT("'"));
		return true;
	}

	return true;
}

void ACrooniesTreasureTroopPlayerController::ServerSendChatMessage_Implementation(ECrooniesChatChannel Channel, const FString& Body, const FString& TargetName)
{
	FString CleanBody = Body.Left(160);
	CleanBody.TrimStartAndEndInline();
	if (CleanBody.IsEmpty() || !GetWorld())
	{
		return;
	}

	FString DeniedReason;
	if (!IsChatAllowed(Channel, DeniedReason))
	{
		AddLocalSystemChatMessage(DeniedReason);
		return;
	}

	FCrooniesChatMessage Message;
	Message.Channel = Channel;
	Message.SenderName = GetChatDisplayName();
	Message.TargetName = TargetName.Left(32);
	Message.Body = ApplyProfanityFilter(CleanBody);
	Message.TeamId = TeamId;

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ACrooniesTreasureTroopPlayerController* OtherPC = Cast<ACrooniesTreasureTroopPlayerController>(It->Get());
		if (!OtherPC)
		{
			continue;
		}

		const bool bDeliverWorld = Channel == ECrooniesChatChannel::World;
		const bool bDeliverTeam = Channel == ECrooniesChatChannel::Team && OtherPC->TeamId == TeamId;
		const bool bDeliverPrivate = Channel == ECrooniesChatChannel::Private && (OtherPC == this || OtherPC->IsChatRecipientMatch(Message.TargetName));
		if (bDeliverWorld || bDeliverTeam || bDeliverPrivate)
		{
			OtherPC->ClientReceiveChatMessage(Message);
		}
	}
}

bool ACrooniesTreasureTroopPlayerController::IsChatAllowed(ECrooniesChatChannel Channel, FString& DeniedReason) const
{
	if (!bParentalChatEnabled)
	{
		DeniedReason = TEXT("Chat is disabled by parental controls.");
		return false;
	}
	if (Channel == ECrooniesChatChannel::Private && !bPrivateChatEnabled)
	{
		DeniedReason = TEXT("Private chat is disabled by parental controls.");
		return false;
	}
	if (IdentityStatus == ECrooniesIdentityStatus::Failed)
	{
		DeniedReason = TEXT("Chat unavailable: account identity validation failed.");
		return false;
	}
	return true;
}

FString ACrooniesTreasureTroopPlayerController::ApplyProfanityFilter(const FString& RawMessage) const
{
	if (!bProfanityFilterEnabled)
	{
		return RawMessage.Left(160);
	}

	FString Filtered = RawMessage.Left(160);
	static const TCHAR* BlockedTerms[] = {
		TEXT("fuck"), TEXT("shit"), TEXT("bitch"), TEXT("asshole"), TEXT("dick"), TEXT("crap")
	};

	for (const TCHAR* Term : BlockedTerms)
	{
		const FString TermString(Term);
		FString Mask;
		for (int32 Index = 0; Index < TermString.Len(); ++Index)
		{
			Mask += TEXT("*");
		}
		Filtered = Filtered.Replace(*TermString, *Mask, ESearchCase::IgnoreCase);
	}

	return Filtered;
}

void ACrooniesTreasureTroopPlayerController::AddLocalSystemChatMessage(const FString& Body)
{
	FCrooniesChatMessage Message;
	Message.Channel = ECrooniesChatChannel::World;
	Message.SenderName = TEXT("Safety");
	Message.Body = Body;
	Message.TeamId = TeamId;
	Message.bSystemMessage = true;
	ClientReceiveChatMessage(Message);
}

void ACrooniesTreasureTroopPlayerController::ReportPlayerFromChat(const FString& TargetName, const FString& Reason)
{
	if (!bReportingEnabled)
	{
		AddLocalSystemChatMessage(TEXT("Reporting is disabled in parental controls."));
		return;
	}

	TArray<FCrooniesChatMessage> Context;
	const int32 FirstIndex = FMath::Max(0, ChatHistory.Num() - 12);
	for (int32 Index = FirstIndex; Index < ChatHistory.Num(); ++Index)
	{
		Context.Add(ChatHistory[Index]);
	}

	ServerReportPlayer(GetChatDisplayName(), TargetName.Left(32), Reason.Left(220), Context);
	AddLocalSystemChatMessage(FString::Printf(TEXT("Report submitted for %s. Recent chat context was attached."), *TargetName.Left(32)));
}

void ACrooniesTreasureTroopPlayerController::ServerReportPlayer_Implementation(const FString& ReporterName, const FString& TargetName, const FString& Reason, const TArray<FCrooniesChatMessage>& RecentContext)
{
	const FString ReportDir = FPaths::ProjectSavedDir() / TEXT("Reports");
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	PlatformFile.CreateDirectoryTree(*ReportDir);

	const FString Timestamp = FDateTime::UtcNow().ToString(TEXT("%Y%m%d-%H%M%S"));
	const FString SafeTarget = TargetName.Replace(TEXT(" "), TEXT("_")).Left(32);
	const FString ReportPath = ReportDir / FString::Printf(TEXT("chat-report-%s-%s.txt"), *Timestamp, *SafeTarget);

	FString Output;
	Output += FString::Printf(TEXT("Game: Croonies: Treasure Troop\nCreatedUtc: %s\nReporter: %s\nTarget: %s\nReason: %s\nIdentityStatus: %s\n\nRecent Chat Context:\n"),
		*FDateTime::UtcNow().ToIso8601(), *ReporterName.Left(64), *TargetName.Left(64), *Reason.Left(220), *GetIdentityStatusLabel());
	for (const FCrooniesChatMessage& Message : RecentContext)
	{
		Output += FString::Printf(TEXT("[%d] %s -> %s: %s\n"), static_cast<int32>(Message.Channel), *Message.SenderName, *Message.TargetName, *Message.Body);
	}

	FFileHelper::SaveStringToFile(Output, *ReportPath);
}

void ACrooniesTreasureTroopPlayerController::ValidatePlatformIdentity()
{
	if (LoginProvider == TEXT("Local Guest"))
	{
		IdentityStatus = ECrooniesIdentityStatus::LocalGuest;
		return;
	}

	// Real Google Play Games / Apple Game Center validation must happen against a backend
	// with platform app IDs, entitlements, token exchange, and replay protection configured.
	IdentityStatus = ECrooniesIdentityStatus::PendingPlatformValidation;
}

void ACrooniesTreasureTroopPlayerController::ClientReceiveChatMessage_Implementation(const FCrooniesChatMessage& Message)
{
	ChatHistory.Add(Message);
	while (ChatHistory.Num() > 60)
	{
		ChatHistory.RemoveAt(0);
	}
}

void ACrooniesTreasureTroopPlayerController::TogglePauseMenu()
{
	bPauseMenuOpen = !bPauseMenuOpen;
	SetPause(bPauseMenuOpen);
}

void ACrooniesTreasureTroopPlayerController::ToggleSettings()
{
	bSettingsOpen = !bSettingsOpen;
	if (bSettingsOpen)
	{
		SetPause(true);
	}
	else if (AppState == ECrooniesAppState::Playing && !bPauseMenuOpen)
	{
		SetPause(false);
	}
}

void ACrooniesTreasureTroopPlayerController::SignIn(const FString& Provider)
{
	bLoginOptionsOpen = false;
	LoginProvider = Provider;
	ValidatePlatformIdentity();
	TryGrantDailyLoginReward();
	AppState = ECrooniesAppState::CharacterCreation;
	SetPause(true);

	if (Provider != TEXT("Local Guest") && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 6.f, FColor(255, 210, 90),
			FString::Printf(TEXT("%s selected. Store SDK token validation will be wired during mobile platform setup; using local secure save slot now."), *Provider));
	}
}

void ACrooniesTreasureTroopPlayerController::AdvanceCharacterCreation()
{
	ApplyCustomizationPreview();
	SaveProfile();
	TeleportToOldAtticTutorial();
	AppState = ECrooniesAppState::Playing;
	SetPause(false);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 6.f, FColor::Green, TEXT("Profile ready. Level 1 tutorial begins in The Old Attic."));
	}
}

void ACrooniesTreasureTroopPlayerController::ChangeCharacterOption(int32 Delta)
{
	if (CreationField == 0)
	{
		bMaleChoice = !bMaleChoice;
	}
	else if (CreationField == 1)
	{
		SkinToneChoice = (SkinToneChoice + Delta + 5) % 5;
	}
	else if (CreationField == 2)
	{
		HairStyleChoice = (HairStyleChoice + Delta + 5) % 5;
	}
	else if (CreationField == 3)
	{
		HairColorChoice = (HairColorChoice + Delta + 6) % 6;
	}
	else if (CreationField == 4)
	{
		EyeColorChoice = (EyeColorChoice + Delta + 5) % 5;
	}
}

void ACrooniesTreasureTroopPlayerController::ApplyCustomizationPreview()
{
	if (ACrooniesTreasureTroopCharacter* TroopCharacter = Cast<ACrooniesTreasureTroopCharacter>(GetPawn()))
	{
		TroopCharacter->ApplyCharacterCustomization(bMaleChoice, SkinToneChoice, HairStyleChoice, HairColorChoice, EyeColorChoice);
	}
}

FString ACrooniesTreasureTroopPlayerController::GetCharacterSummary() const
{
	return FString::Printf(TEXT("%s  Skin:%d  Hair Style:%d  Hair Color:%d  Eyes:%d"),
		bMaleChoice ? TEXT("Male") : TEXT("Female"),
		SkinToneChoice + 1,
		HairStyleChoice + 1,
		HairColorChoice + 1,
		EyeColorChoice + 1);
}

FString ACrooniesTreasureTroopPlayerController::GetPlatformSignInLabel() const
{
#if PLATFORM_ANDROID
	return TEXT("Google Play Games");
#elif PLATFORM_IOS
	return TEXT("Apple Game Center");
#else
	return TEXT("Platform Sign In");
#endif
}

void ACrooniesTreasureTroopPlayerController::SaveProfile()
{
	UCrooniesTreasureTroopSaveGame* Save = Cast<UCrooniesTreasureTroopSaveGame>(
		UGameplayStatics::CreateSaveGameObject(UCrooniesTreasureTroopSaveGame::StaticClass()));
	if (!Save)
	{
		return;
	}

	Save->PlatformProvider = LoginProvider;
	Save->PlatformDisplayId = LoginProvider == TEXT("Local Guest") ? TEXT("Guest") : FString::Printf(TEXT("%s PendingSDK"), *LoginProvider);
	Save->PlatformIdentityStatus = GetIdentityStatusLabel();
	Save->bParentalChatEnabled = bParentalChatEnabled;
	Save->bProfanityFilterEnabled = bProfanityFilterEnabled;
	Save->bPrivateChatEnabled = bPrivateChatEnabled;
	Save->bReportingEnabled = bReportingEnabled;
	Save->LiquidCouragePotionCount = LiquidCouragePotionCount;
	Save->LastDailyLoginRewardDate = LastDailyLoginRewardDate;
	Save->DailyLoginStreak = DailyLoginStreak;
	Save->bCreatedCharacter = true;
	Save->bMale = bMaleChoice;
	Save->SkinTone = SkinToneChoice;
	Save->HairStyle = HairStyleChoice;
	Save->HairColor = HairColorChoice;
	Save->EyeColor = EyeColorChoice;
	UGameplayStatics::SaveGameToSlot(Save, TEXT("CrooniesProfile"), 0);
}

void ACrooniesTreasureTroopPlayerController::TryGrantDailyLoginReward()
{
	const FString Today = FDateTime::UtcNow().ToString(TEXT("%Y-%m-%d"));
	if (LastDailyLoginRewardDate == Today)
	{
		return;
	}

	const FString Yesterday = (FDateTime::UtcNow() - FTimespan::FromDays(1)).ToString(TEXT("%Y-%m-%d"));
	DailyLoginStreak = LastDailyLoginRewardDate == Yesterday ? DailyLoginStreak + 1 : 1;
	LastDailyLoginRewardDate = Today;

	const int32 CoinReward = 25 + FMath::Min(DailyLoginStreak, 7) * 5;
	const int32 PotionReward = DailyLoginStreak >= 3 ? 2 : 1;
	LiquidCouragePotionCount += PotionReward;

	if (ACrooniesTreasureTroopCharacter* TroopCharacter = Cast<ACrooniesTreasureTroopCharacter>(GetPawn()))
	{
		TroopCharacter->AddCoins(CoinReward);
	}

	DailyLoginRewardText = FString::Printf(TEXT("Daily Login Reward: +%d Coins, +%d Courage Potion%s. Streak: Day %d"),
		CoinReward, PotionReward, PotionReward == 1 ? TEXT("") : TEXT("s"), DailyLoginStreak);
	bDailyLoginRewardNotice = true;
	SaveProfile();

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor(255, 220, 80), DailyLoginRewardText);
	}
}

bool ACrooniesTreasureTroopPlayerController::HandleLoginClick()
{
	int32 SizeX = 0;
	int32 SizeY = 0;
	GetViewportSize(SizeX, SizeY);
	float MouseX = 0.f;
	float MouseY = 0.f;
	if (!GetMousePosition(MouseX, MouseY) || SizeX <= 0 || SizeY <= 0)
	{
		return false;
	}

	const float ButtonW = FMath::Clamp(SizeX * 0.30f, 420.f, 650.f);
	const float ButtonH = ButtonW * (209.f / 520.f);
	const float ButtonX = SizeX * 0.5f - ButtonW * 0.5f;
	const float SignInY = SizeY * 0.565f;
	const float PlatformY = SignInY + ButtonH + SizeY * 0.012f;
	const float LocalY = bLoginOptionsOpen ? PlatformY + SizeY * 0.055f : SignInY + ButtonH + SizeY * 0.006f;

	auto InRect = [MouseX, MouseY](float X, float Y, float W, float H)
	{
		return MouseX >= X && MouseX <= X + W && MouseY >= Y && MouseY <= Y + H;
	};

	if (InRect(ButtonX, SignInY, ButtonW, ButtonH))
	{
		bLoginOptionsOpen = true;
		return true;
	}
	if (bLoginOptionsOpen && InRect(ButtonX + ButtonW * 0.07f, PlatformY, ButtonW * 0.86f, SizeY * 0.044f))
	{
		SignIn(GetPlatformSignInLabel());
		return true;
	}
	if (InRect(SizeX * 0.5f - 170.f, LocalY - 6.f, 340.f, 42.f))
	{
		SignIn(TEXT("Local Guest"));
		return true;
	}
	return false;
}

bool ACrooniesTreasureTroopPlayerController::HandleCharacterCreationClick()
{
	int32 SizeX = 0;
	int32 SizeY = 0;
	GetViewportSize(SizeX, SizeY);
	float MouseX = 0.f;
	float MouseY = 0.f;
	if (!GetMousePosition(MouseX, MouseY) || SizeX <= 0 || SizeY <= 0)
	{
		return false;
	}

	const float PanelX = SizeX * 0.5f - 470.f;
	const float PanelY = SizeY * 0.5f - 300.f;
	for (int32 Index = 0; Index < 5; ++Index)
	{
		const float RowY = PanelY + 145.f + Index * 58.f;
		if (MouseX >= PanelX + 70.f && MouseX <= PanelX + 790.f && MouseY >= RowY && MouseY <= RowY + 46.f)
		{
			CreationField = Index;
			if (MouseX > PanelX + 520.f)
			{
				ChangeCharacterOption(1);
			}
			else if (MouseX < PanelX + 220.f)
			{
				ChangeCharacterOption(-1);
			}
			ApplyCustomizationPreview();
			return true;
		}
	}

	if (MouseX >= PanelX + 250.f && MouseX <= PanelX + 690.f && MouseY >= PanelY + 514.f && MouseY <= PanelY + 582.f)
	{
		AdvanceCharacterCreation();
		return true;
	}
	return false;
}

bool ACrooniesTreasureTroopPlayerController::IsInSettingsButton(float MouseX, float MouseY, int32 SizeX, int32 SizeY) const
{
	const float ButtonSize = 82.f;
	const float ButtonX = SizeX - 100.f;
	const float ButtonY = 20.f;
	return MouseX >= ButtonX && MouseX <= ButtonX + ButtonSize && MouseY >= ButtonY && MouseY <= ButtonY + ButtonSize;
}

void ACrooniesTreasureTroopPlayerController::ToggleHubMenu()
{
	bHubMenuOpen = !bHubMenuOpen;
	if (bHubMenuOpen)
	{
		HubMenuMode = 0;
		bPauseMenuOpen = false;
		SetPause(true);
	}
	else if (AppState == ECrooniesAppState::Playing && !bPauseMenuOpen && !bSettingsOpen)
	{
		HubMenuMode = 0;
		SetPause(false);
	}
}

void ACrooniesTreasureTroopPlayerController::TeleportToLevelNumber(int32 LevelNumber)
{
	ACrooniesTreasureTroopCharacter* TroopCharacter = Cast<ACrooniesTreasureTroopCharacter>(GetPawn());
	if (!TroopCharacter)
	{
		return;
	}

	struct FLevelInfo
	{
		int32 Number;
		float X;
		float Y;
		const TCHAR* Title;
		const TCHAR* Style;
		const TCHAR* Objective;
	};

	if (LevelNumber == 201)
	{
		OpenCrewChallengeMenu();
		return;
	}

	const FLevelInfo Levels[] = {
		{1, 650.f, 0.f, TEXT("The Old Attic"), TEXT("Tutorial switches"), TEXT("Follow prompts, recruit Mouth, switch Croonies, and collect the treasure map.")},
		{2, 60000.f, 0.f, TEXT("The Rusty Lighthouse"), TEXT("Vertical climb"), TEXT("Use Data's repairs, dodge debris, and find the hidden friend room.")},
		{3, 120000.f, 0.f, TEXT("The Flooded Tunnels"), TEXT("Water exploration"), TEXT("Drain water paths, push crates with Brand, and recruit Stef.")},
		{4, 180000.f, 0.f, TEXT("The Collapsing Mines"), TEXT("Isometric exploration"), TEXT("Cross timed platforms and survive the first Grumpelli encounter.")},
		{5, 240000.f, 0.f, TEXT("The Bat Caverns"), TEXT("Dark flashlight maze"), TEXT("Use flashlight mechanics to scatter bats and read echo clues.")},
		{6, 300000.f, 0.f, TEXT("Crystal Caverns"), TEXT("Light reflection puzzle"), TEXT("Bounce light through crystals and break fragile treasure walls.")},
		{7, 360000.f, 0.f, TEXT("The Mirror Maze"), TEXT("Grid puzzle dungeon"), TEXT("Solve moving mirror paths and unlock the Super Flashlight.")},
		{8, 420000.f, 0.f, TEXT("Ancient Pirate Outpost"), TEXT("Trap gauntlet"), TEXT("Clear spikes, boulders, rope hooks, and a full-team treasure trial.")},
		{9, 480000.f, 0.f, TEXT("The Great Chasm"), TEXT("Chase platforming"), TEXT("Swing over gaps while the Grumpelli crew closes in.")},
		{10, 540000.f, 0.f, TEXT("The Underground River"), TEXT("Raft and cave route"), TEXT("Push logs, read currents, and steer the raft path.")},
		{11, 600000.f, 0.f, TEXT("Neighborhood Getaway"), TEXT("Auto-runner chase"), TEXT("Dodge street obstacles, collect coins, and use teamwork boosts.")},
		{12, 660000.f, 0.f, TEXT("Cyclops Captain's Shipwreck"), TEXT("Final boss puzzle"), TEXT("Survive the tilting ship, open the treasure room, and beat the boss trial.")},
		{101, 6000.f, 9000.f, TEXT("Secret Treehouse"), TEXT("Bonus platforming"), TEXT("Optional bonus challenge: collect coins and practice jumps.")},
		{102, 14000.f, 9000.f, TEXT("Ghost Ship Trial"), TEXT("Bonus boss rematch"), TEXT("Harder treasure-room puzzles and a compact shipwreck rematch.")},
		{103, 22000.f, 9000.f, TEXT("Endless Caverns"), TEXT("Bonus endless run"), TEXT("Collect coins through a repeatable cave challenge for high-score practice.")},
		{104, 30000.f, 9000.f, TEXT("Neighborhood Mayhem"), TEXT("Bonus hub mischief"), TEXT("Practice open-area collection and quick obstacle routing.")}
	};

	for (const FLevelInfo& Level : Levels)
	{
		if (Level.Number == LevelNumber)
		{
			const float EntryOffsetX = (Level.Number >= 2 && Level.Number <= 12) ? 4300.f : 430.f;
			const FVector SafeLevelSpawn = Level.Number == 1
				? FVector(35.f, 620.f, 140.f)
				: FVector(Level.X - EntryOffsetX, Level.Y, 120.f);
			TroopCharacter->SetActorLocation(SafeLevelSpawn, false, nullptr, ETeleportType::TeleportPhysics);
			TroopCharacter->SetActorRotation(FRotator::ZeroRotator);
			TroopCharacter->EnterLevel(Level.Number, Level.Title, Level.Style, Level.Objective);
			bHubMenuOpen = false;
			HubMenuMode = 0;
			if (AppState == ECrooniesAppState::Playing && !bPauseMenuOpen && !bSettingsOpen)
			{
				SetPause(false);
			}
			return;
		}
	}
}

int32 ACrooniesTreasureTroopPlayerController::GetSpecialItemPriceForLevel(int32 LevelNumber) const
{
	return 10 + FMath::Max(0, LevelNumber - 2) * 10;
}

int32 ACrooniesTreasureTroopPlayerController::GetSpecialLevelsAccessPrice() const
{
	return 10;
}

bool ACrooniesTreasureTroopPlayerController::HasSpecialLevelsAccess() const
{
	const ACrooniesTreasureTroopCharacter* TroopCharacter = Cast<ACrooniesTreasureTroopCharacter>(GetPawn());
	return TroopCharacter && TroopCharacter->HasSpecialItem(TEXT("SpecialPortalKey"));
}

void ACrooniesTreasureTroopPlayerController::BuySpecialItemForLevel(int32 LevelNumber)
{
	ACrooniesTreasureTroopCharacter* TroopCharacter = Cast<ACrooniesTreasureTroopCharacter>(GetPawn());
	if (!TroopCharacter || LevelNumber < 2 || LevelNumber > 12)
	{
		return;
	}

	const FName ItemId = TroopCharacter->GetRequiredSpecialItemIdForLevel(LevelNumber);
	if (TroopCharacter->HasSpecialItem(ItemId))
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.5f, FColor(120, 230, 255), FString::Printf(TEXT("%s already owned."), *TroopCharacter->GetRequiredSpecialItemNameForLevel(LevelNumber)));
		}
		return;
	}

	const int32 Price = GetSpecialItemPriceForLevel(LevelNumber);
	if (TroopCharacter->SpendCoins(Price))
	{
		TroopCharacter->AddSpecialItem(ItemId);
	}
}

void ACrooniesTreasureTroopPlayerController::BuySpecialLevelsAccessItem()
{
	ACrooniesTreasureTroopCharacter* TroopCharacter = Cast<ACrooniesTreasureTroopCharacter>(GetPawn());
	if (!TroopCharacter)
	{
		return;
	}

	const FName ItemId(TEXT("SpecialPortalKey"));
	if (TroopCharacter->HasSpecialItem(ItemId))
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.5f, FColor(120, 230, 255), TEXT("Special Portal Key already owned."));
		}
		return;
	}

	const int32 Price = GetSpecialLevelsAccessPrice();
	if (TroopCharacter->SpendCoins(Price))
	{
		TroopCharacter->AddSpecialItem(ItemId);
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor(120, 255, 170), TEXT("Special Levels portal unlocked."));
		}
	}
}

void ACrooniesTreasureTroopPlayerController::BuyLiquidCouragePotion()
{
	// Windows demo grant. On mobile, this must only grant after app-store receipt validation.
	++LiquidCouragePotionCount;
	SaveProfile();
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor(255, 190, 75),
			FString::Printf(TEXT("Demo store purchase: Liquid Courage Potion x1 (%s placeholder, no real charge). Inventory: %d"),
				*GetLiquidCouragePotionPrice(), LiquidCouragePotionCount));
	}
}

void ACrooniesTreasureTroopPlayerController::UseLiquidCouragePotion()
{
	ACrooniesTreasureTroopCharacter* TroopCharacter = Cast<ACrooniesTreasureTroopCharacter>(GetPawn());
	if (!TroopCharacter)
	{
		return;
	}
	if (LiquidCouragePotionCount <= 0)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor(255, 205, 80), TEXT("No Liquid Courage Potions owned."));
		}
		return;
	}

	--LiquidCouragePotionCount;
	TroopCharacter->AddCourage(3);
	SaveProfile();
}

bool ACrooniesTreasureTroopPlayerController::CanTeleportToAdventureLevel(int32 LevelNumber, FString& LockedReason) const
{
	const ACrooniesTreasureTroopCharacter* TroopCharacter = Cast<ACrooniesTreasureTroopCharacter>(GetPawn());
	if (!TroopCharacter)
	{
		LockedReason = TEXT("No active Croonie.");
		return false;
	}

	const int32 MapCount = TroopCharacter->GetMapPieceCount();
	const int32 ArtifactCount = TroopCharacter->GetArtifactCount();
	const int32 TokenCount = TroopCharacter->GetFriendshipTokenCount();
	if (LevelNumber <= 1)
	{
		LockedReason = TEXT("Unlocked");
		return true;
	}

	if (LevelNumber >= 2 && LevelNumber <= 10)
	{
		const int32 Pieces = TroopCharacter->GetMapPiecesForLevel(LevelNumber);
		const FString ItemName = TroopCharacter->GetRequiredSpecialItemNameForLevel(LevelNumber);
		const bool bHasMap = TroopCharacter->HasCompletedMapForLevel(LevelNumber);
		const bool bHasItem = TroopCharacter->HasRequiredSpecialItemForLevel(LevelNumber);
		if (bHasMap && bHasItem)
		{
			LockedReason = TEXT("Unlocked");
			return true;
		}
		LockedReason = FString::Printf(TEXT("Level %d requires Map L%d 4/4 and %s. You have map %d/4, item: %s."),
			LevelNumber, LevelNumber, *ItemName, Pieces, bHasItem ? TEXT("yes") : TEXT("no"));
		return false;
	}

	if (LevelNumber == 11)
	{
		const int32 Pieces = TroopCharacter->GetMapPiecesForLevel(LevelNumber);
		const FString ItemName = TroopCharacter->GetRequiredSpecialItemNameForLevel(LevelNumber);
		const bool bHasMap = TroopCharacter->HasCompletedMapForLevel(LevelNumber);
		const bool bHasItem = TroopCharacter->HasRequiredSpecialItemForLevel(LevelNumber);
		if (bHasMap && bHasItem)
		{
			LockedReason = TEXT("Unlocked");
			return true;
		}
		LockedReason = FString::Printf(TEXT("Level 11 requires Map L11 4/4 and %s. You have map %d/4, item: %s."),
			*ItemName, Pieces, bHasItem ? TEXT("yes") : TEXT("no"));
		return false;
	}

	if (LevelNumber == 12)
	{
		const int32 Pieces = TroopCharacter->GetMapPiecesForLevel(LevelNumber);
		const FString ItemName = TroopCharacter->GetRequiredSpecialItemNameForLevel(LevelNumber);
		const bool bHasMap = TroopCharacter->HasCompletedMapForLevel(LevelNumber);
		const bool bHasItem = TroopCharacter->HasRequiredSpecialItemForLevel(LevelNumber);
		if (bHasMap && bHasItem)
		{
			LockedReason = TEXT("Unlocked");
			return true;
		}
		LockedReason = FString::Printf(TEXT("Level 12 requires Map L12 4/4 and %s. You have map %d/4, item: %s."),
			*ItemName, Pieces, bHasItem ? TEXT("yes") : TEXT("no"));
		return false;
	}

	LockedReason = TEXT("Unlocked");
	return true;
}

void ACrooniesTreasureTroopPlayerController::OpenCavernsMenu()
{
	bHubMenuOpen = true;
	HubMenuMode = 1;
	bPauseMenuOpen = false;
	SetPause(true);
}

void ACrooniesTreasureTroopPlayerController::OpenLocalStoreMenu()
{
	bHubMenuOpen = true;
	HubMenuMode = 3;
	bPauseMenuOpen = false;
	SetPause(true);
}

void ACrooniesTreasureTroopPlayerController::OpenPremiumStoreMenu()
{
	bHubMenuOpen = true;
	HubMenuMode = 5;
	PremiumStoreTab = 0;
	bPauseMenuOpen = false;
	SetPause(true);
}

void ACrooniesTreasureTroopPlayerController::SetPremiumStoreTab(int32 NewTab)
{
	PremiumStoreTab = FMath::Clamp(NewTab, 0, 5);
}

void ACrooniesTreasureTroopPlayerController::OpenDailyTasksMenu()
{
	bHubMenuOpen = true;
	HubMenuMode = 4;
	bPauseMenuOpen = false;
	SetPause(true);
}

void ACrooniesTreasureTroopPlayerController::OpenCrewChallengeMenu()
{
	bHubMenuOpen = true;
	HubMenuMode = 6;
	bPauseMenuOpen = false;
	SetPause(true);
}

void ACrooniesTreasureTroopPlayerController::OpenSpecialLevelsMenu()
{
	if (!HasSpecialLevelsAccess())
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.5f, FColor(255, 205, 80),
				FString::Printf(TEXT("Special Levels locked: buy the Special Portal Key at the Local Gear Store (%d coins)."), GetSpecialLevelsAccessPrice()));
		}
		return;
	}

	bHubMenuOpen = true;
	HubMenuMode = 2;
	bPauseMenuOpen = false;
	SetPause(true);
}

void ACrooniesTreasureTroopPlayerController::CompleteDailyTask(int32 TaskIndex)
{
	if (ACrooniesTreasureTroopCharacter* TroopCharacter = Cast<ACrooniesTreasureTroopCharacter>(GetPawn()))
	{
		TroopCharacter->CompleteDailyTask(TaskIndex);
	}
}

void ACrooniesTreasureTroopPlayerController::TeleportToHub()
{
	ACrooniesTreasureTroopCharacter* TroopCharacter = Cast<ACrooniesTreasureTroopCharacter>(GetPawn());
	if (!TroopCharacter || AppState != ECrooniesAppState::Playing)
	{
		return;
	}

		TroopCharacter->SetActorLocation(FVector(-9700.f, -150.f, 120.f), false, nullptr, ETeleportType::TeleportPhysics);
	TroopCharacter->SetActorRotation(FRotator::ZeroRotator);
	TroopCharacter->EnterLevel(0, TEXT("The Neighborhood"), TEXT("Walkable Hub"), TEXT("Walk into glowing portal pads to enter levels, or visit the collection showcase along the north wall."));
	bHubMenuOpen = false;
	HubMenuMode = 0;
	if (!bPauseMenuOpen && !bSettingsOpen)
	{
		SetPause(false);
	}
}

bool ACrooniesTreasureTroopPlayerController::IsInExitButton(float MouseX, float MouseY, int32 SizeX, int32 SizeY) const
{
	const float ButtonSize = 86.f;
	const float ButtonX = SizeX - 190.f;
	const float ButtonY = 20.f;
	return MouseX >= ButtonX && MouseX <= ButtonX + ButtonSize && MouseY >= ButtonY && MouseY <= ButtonY + ButtonSize;
}

bool ACrooniesTreasureTroopPlayerController::IsInActionButton(float MouseX, float MouseY, int32 SizeX, int32 SizeY) const
{
	const float ButtonSize = FMath::Clamp(SizeY * 0.095f, 78.f, 118.f);
	const float ButtonX = SizeX - ButtonSize - 38.f;
	const float ButtonY = SizeY - ButtonSize - 74.f;
	return MouseX >= ButtonX && MouseX <= ButtonX + ButtonSize && MouseY >= ButtonY && MouseY <= ButtonY + ButtonSize;
}

bool ACrooniesTreasureTroopPlayerController::IsInJumpButton(float MouseX, float MouseY, int32 SizeX, int32 SizeY) const
{
	const float ButtonSize = FMath::Clamp(SizeY * 0.085f, 70.f, 106.f);
	const float ButtonX = SizeX - (ButtonSize * 2.35f) - 52.f;
	const float ButtonY = SizeY - ButtonSize - 42.f;
	return MouseX >= ButtonX && MouseX <= ButtonX + ButtonSize && MouseY >= ButtonY && MouseY <= ButtonY + ButtonSize;
}

void ACrooniesTreasureTroopPlayerController::PerformJump()
{
	if (AppState != ECrooniesAppState::Playing)
	{
		return;
	}
	if (ACharacter* JumpCharacter = Cast<ACharacter>(GetPawn()))
	{
		JumpCharacter->Jump();
	}
}

void ACrooniesTreasureTroopPlayerController::PerformAction()
{
	ACrooniesTreasureTroopCharacter* TroopCharacter = Cast<ACrooniesTreasureTroopCharacter>(GetPawn());
	if (!TroopCharacter || AppState != ECrooniesAppState::Playing)
	{
		return;
	}

	TroopCharacter->NotifyInteractPressed();
	const FVector PlayerLocation = TroopCharacter->GetActorLocation();
	const float ActionRadiusSq = FMath::Square(260.f);

	for (TActorIterator<ACovePickup> It(GetWorld()); It; ++It)
	{
		ACovePickup* Pickup = *It;
		if (Pickup && FVector::DistSquared(Pickup->GetActorLocation(), PlayerLocation) <= ActionRadiusSq)
		{
			if (Pickup->TryCollect(TroopCharacter))
			{
				return;
			}
		}
	}

	for (TActorIterator<ACoveGate> It(GetWorld()); It; ++It)
	{
		ACoveGate* Gate = *It;
		if (Gate && FVector::DistSquared(Gate->GetActorLocation(), PlayerLocation) <= ActionRadiusSq)
		{
			Gate->TryUse(TroopCharacter);
			return;
		}
	}

	for (TActorIterator<ACoveAtticLightSwitch> It(GetWorld()); It; ++It)
	{
		ACoveAtticLightSwitch* LightSwitch = *It;
		if (LightSwitch && FVector::DistSquared(LightSwitch->GetActorLocation(), PlayerLocation) <= ActionRadiusSq)
		{
			LightSwitch->TryUse(TroopCharacter);
			return;
		}
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.8f, FColor(255, 220, 90), TEXT("Action: move closer to a pickup, switch, or obstacle."));
	}
}

bool ACrooniesTreasureTroopPlayerController::HandleSettingsClick()
{
	int32 SizeX = 0;
	int32 SizeY = 0;
	float MouseX = 0.f;
	float MouseY = 0.f;
	GetViewportSize(SizeX, SizeY);
	if (!GetMousePosition(MouseX, MouseY) || SizeX <= 0 || SizeY <= 0)
	{
		return false;
	}

	const float PanelX = SizeX * 0.5f - 470.f;
	const float PanelY = SizeY * 0.5f - 360.f;

	if (MouseX >= PanelX + 854.f && MouseX <= PanelX + 914.f && MouseY >= PanelY + 26.f && MouseY <= PanelY + 86.f)
	{
		ToggleSettings();
		return true;
	}

	const float ToggleX = PanelX + 700.f;
	const float ToggleW = 160.f;
	const float RowH = 44.f;
	const float MusicToggleY = PanelY + 178.f;
	const float SfxToggleY = PanelY + 274.f;
	const float SubtitleY = PanelY + 402.f;
	const float VibrationY = PanelY + 458.f;

	auto InRect = [MouseX, MouseY](float X, float Y, float W, float H)
	{
		return MouseX >= X && MouseX <= X + W && MouseY >= Y && MouseY <= Y + H;
	};

	if (InRect(ToggleX, MusicToggleY, ToggleW, RowH))
	{
		bMusicEnabled = !bMusicEnabled;
		return true;
	}
	if (InRect(ToggleX, SfxToggleY, ToggleW, RowH))
	{
		bSoundEffectsEnabled = !bSoundEffectsEnabled;
		return true;
	}
	if (InRect(ToggleX, SubtitleY, ToggleW, RowH))
	{
		bSubtitlesEnabled = !bSubtitlesEnabled;
		return true;
	}
	if (InRect(ToggleX, VibrationY, ToggleW, RowH))
	{
		bVibrationEnabled = !bVibrationEnabled;
		return true;
	}

	if (HandleSafetySettingsClick(PanelX, PanelY, MouseX, MouseY))
	{
		return true;
	}

	const float SliderX = PanelX + 310.f;
	const float SliderW = 340.f;
	const float MusicSliderY = PanelY + 225.f;
	const float SfxSliderY = PanelY + 321.f;
	if (InRect(SliderX, MusicSliderY - 12.f, SliderW, 34.f))
	{
		MusicVolume = FMath::Clamp((MouseX - SliderX) / SliderW, 0.f, 1.f);
		return true;
	}
	if (InRect(SliderX, SfxSliderY - 12.f, SliderW, 34.f))
	{
		SoundEffectsVolume = FMath::Clamp((MouseX - SliderX) / SliderW, 0.f, 1.f);
		return true;
	}

	if (InRect(PanelX + 80.f, PanelY + 552.f, 300.f, 56.f))
	{
		LoginProvider = TEXT("Signed out");
		bLoginOptionsOpen = false;
		AppState = ECrooniesAppState::Login;
		bSettingsOpen = false;
		bPauseMenuOpen = false;
		SetPause(true);
		return true;
	}

	return true;
}

bool ACrooniesTreasureTroopPlayerController::HandleSafetySettingsClick(float PanelX, float PanelY, float MouseX, float MouseY)
{
	const float ToggleX = PanelX + 700.f;
	const float ToggleW = 160.f;
	const float RowH = 44.f;
	const float SafetyRows[] = {
		PanelY + 512.f,
		PanelY + 554.f,
		PanelY + 596.f,
		PanelY + 638.f
	};

	auto InRect = [MouseX, MouseY](float X, float Y, float W, float H)
	{
		return MouseX >= X && MouseX <= X + W && MouseY >= Y && MouseY <= Y + H;
	};

	if (InRect(ToggleX, SafetyRows[0], ToggleW, RowH))
	{
		bParentalChatEnabled = !bParentalChatEnabled;
		if (!bParentalChatEnabled)
		{
			bPrivateChatEnabled = false;
		}
		SaveProfile();
		return true;
	}
	if (InRect(ToggleX, SafetyRows[1], ToggleW, RowH))
	{
		bProfanityFilterEnabled = !bProfanityFilterEnabled;
		SaveProfile();
		return true;
	}
	if (InRect(ToggleX, SafetyRows[2], ToggleW, RowH))
	{
		bPrivateChatEnabled = !bPrivateChatEnabled;
		if (bPrivateChatEnabled)
		{
			bParentalChatEnabled = true;
		}
		SaveProfile();
		return true;
	}
	if (InRect(ToggleX, SafetyRows[3], ToggleW, RowH))
	{
		bReportingEnabled = !bReportingEnabled;
		SaveProfile();
		return true;
	}

	return false;
}

bool ACrooniesTreasureTroopPlayerController::HandleHubMenuClick()
{
	int32 SizeX = 0;
	int32 SizeY = 0;
	float MouseX = 0.f;
	float MouseY = 0.f;
	GetViewportSize(SizeX, SizeY);
	if (!GetMousePosition(MouseX, MouseY) || SizeX <= 0 || SizeY <= 0)
	{
		return false;
	}

	const float PanelW = 980.f;
	const float PanelH = 690.f;
	const float PanelX = SizeX * 0.5f - PanelW * 0.5f;
	const float PanelY = SizeY * 0.5f - PanelH * 0.5f;

	auto InRect = [MouseX, MouseY](float X, float Y, float W, float H)
	{
		return MouseX >= X && MouseX <= X + W && MouseY >= Y && MouseY <= Y + H;
	};

	if (InRect(PanelX + PanelW - 82.f, PanelY + 22.f, 58.f, 58.f))
	{
		ToggleHubMenu();
		return true;
	}

	if (HubMenuMode == 0)
	{
		for (int32 Index = 0; Index < 7; ++Index)
		{
			const int32 Column = Index % 3;
			const int32 Row = Index / 3;
			const float X = PanelX + 72.f + Column * 305.f;
			const float Y = PanelY + 190.f + Row * 154.f;
			if (InRect(X, Y, 250.f, 122.f))
			{
				if (Index == 0)
				{
					TeleportToLevelNumber(1);
				}
				else if (Index == 1)
				{
					OpenCavernsMenu();
				}
				else if (Index == 2)
				{
					OpenDailyTasksMenu();
				}
				else if (Index == 3)
				{
					OpenLocalStoreMenu();
				}
				else if (Index == 4)
				{
					OpenSpecialLevelsMenu();
				}
				else if (Index == 5)
				{
					OpenPremiumStoreMenu();
				}
				else if (Index == 6)
				{
					OpenCrewChallengeMenu();
				}
				return true;
			}
		}
		return true;
	}

	if (HubMenuMode == 1)
	{
		for (int32 Index = 0; Index < 11; ++Index)
		{
			const int32 Column = Index % 3;
			const int32 Row = Index / 3;
			const float X = PanelX + 58.f + Column * 300.f;
			const float Y = PanelY + 154.f + Row * 82.f;
			if (InRect(X, Y, 260.f, 60.f))
			{
				const int32 TargetLevel = Index + 2;
				FString LockedReason;
				if (CanTeleportToAdventureLevel(TargetLevel, LockedReason))
				{
					TeleportToLevelNumber(TargetLevel);
				}
				else if (GEngine)
				{
					GEngine->AddOnScreenDebugMessage(-1, 3.5f, FColor(255, 205, 80), LockedReason);
				}
				return true;
			}
		}
		return true;
	}

	if (HubMenuMode == 3)
	{
		for (int32 Index = 0; Index < 12; ++Index)
		{
			const int32 Column = Index % 3;
			const int32 Row = Index / 3;
			const float X = PanelX + 58.f + Column * 300.f;
			const float Y = PanelY + 154.f + Row * 82.f;
			if (InRect(X, Y, 260.f, 60.f))
			{
				if (Index < 11)
				{
					BuySpecialItemForLevel(Index + 2);
				}
				else
				{
					BuySpecialLevelsAccessItem();
				}
				return true;
			}
		}
		return true;
	}

	if (HubMenuMode == 4)
	{
		for (int32 Index = 0; Index < 10; ++Index)
		{
			const int32 Column = Index % 2;
			const int32 Row = Index / 2;
			const float X = PanelX + 70.f + Column * 455.f;
			const float Y = PanelY + 198.f + Row * 80.f;
			if (InRect(X, Y, 390.f, 70.f))
			{
				CompleteDailyTask(Index);
				return true;
			}
		}
		return true;
	}

	if (HubMenuMode == 2)
	{
		if (!HasSpecialLevelsAccess())
		{
			OpenSpecialLevelsMenu();
			return true;
		}
		for (int32 Index = 0; Index < 4; ++Index)
		{
			const float X = PanelX + 58.f + Index * 225.f;
			const float Y = PanelY + 260.f;
			if (InRect(X, Y, 200.f, 82.f))
			{
				TeleportToLevelNumber(101 + Index);
				return true;
			}
		}
		return true;
	}

	if (HubMenuMode == 6)
	{
		for (int32 Index = 0; Index < CrewChallengeRoster.Num(); ++Index)
		{
			const int32 Column = Index % 4;
			const int32 Row = Index / 4;
			const float X = PanelX + 70.f + Column * 215.f;
			const float Y = PanelY + 260.f + Row * 82.f;
			if (InRect(X, Y, 185.f, 60.f))
			{
				SelectCrewChallengeMember(Index);
				return true;
			}
		}
		if (InRect(PanelX + 330.f, PanelY + 520.f, 320.f, 62.f))
		{
			StartCrewChallengeMatch();
			return true;
		}
		return true;
	}

	if (HubMenuMode == 5)
	{
		for (int32 Index = 0; Index < 6; ++Index)
		{
			const int32 Column = Index % 3;
			const int32 Row = Index / 3;
			const float X = PanelX + 54.f + Column * 302.f;
			const float Y = PanelY + 126.f + Row * 58.f;
			if (InRect(X, Y, 278.f, 44.f))
			{
				SetPremiumStoreTab(Index);
				return true;
			}
		}
		if (PremiumStoreTab == 0 && InRect(PanelX + 76.f, PanelY + 316.f, 400.f, 68.f))
		{
			BuyLiquidCouragePotion();
			return true;
		}
		if (PremiumStoreTab == 0 && InRect(PanelX + 762.f, PanelY + 246.f, 136.f, 34.f))
		{
			UseLiquidCouragePotion();
			return true;
		}
		return true;
	}

	return true;
}


void ACrooniesTreasureTroopPlayerController::SelectCrewChallengeMember(int32 Index)
{
	if (!CrewChallengeRoster.IsValidIndex(Index))
	{
		return;
	}
	SelectedCrewChallengeMemberIndex = Index;
	CrewChallengeState.SelectedMember = CrewChallengeRoster[Index];
}

ACovePickup* ACrooniesTreasureTroopPlayerController::SpawnCrewChallengePickup(const FVector& Location, ECovePickupKind Kind, FName PickupId)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return nullptr;
	}
	ACovePickup* Pickup = World->SpawnActor<ACovePickup>(Location, FRotator::ZeroRotator);
	if (Pickup)
	{
		Pickup->Kind = Kind;
		Pickup->FriendId = PickupId;
	}
	return Pickup;
}

void ACrooniesTreasureTroopPlayerController::SpawnCrewChallengePickups()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	for (TActorIterator<ACovePickup> It(World); It; ++It)
	{
		ACovePickup* Pickup = *It;
		if (Pickup && (Pickup->Kind == ECovePickupKind::CrewChallengeKey || Pickup->Kind == ECovePickupKind::CrewChallengeChest))
		{
			Pickup->Destroy();
		}
	}

	const FVector ArenaCenter(760000.f, 25000.f, 0.f);
	const FVector KeyOffsets[] = {
		FVector(-7600.f, -3900.f, 90.f), FVector(-4200.f, 4200.f, 90.f), FVector(900.f, -5200.f, 90.f), FVector(4700.f, 3600.f, 90.f), FVector(8200.f, -800.f, 90.f)
	};
	const FVector ChestOffsets[] = {
		FVector(-8500.f, 4600.f, 90.f), FVector(-1200.f, 5500.f, 90.f), FVector(2600.f, -5400.f, 90.f), FVector(6900.f, 4700.f, 90.f), FVector(9000.f, -3900.f, 90.f)
	};
	for (int32 Index = 0; Index < 5; ++Index)
	{
		SpawnCrewChallengePickup(ArenaCenter + KeyOffsets[Index], ECovePickupKind::CrewChallengeKey, FName(*FString::Printf(TEXT("CrewKey%d"), Index + 1)));
		SpawnCrewChallengePickup(ArenaCenter + ChestOffsets[Index], ECovePickupKind::CrewChallengeChest, FName(*FString::Printf(TEXT("CrewChest%d"), Index + 1)));
	}
}

void ACrooniesTreasureTroopPlayerController::StartCrewChallengeMatch()
{
	ACrooniesTreasureTroopCharacter* TroopCharacter = Cast<ACrooniesTreasureTroopCharacter>(GetPawn());
	if (!TroopCharacter || !TroopCharacter->SpendCourage(CrewChallengeCourageCost))
	{
		return;
	}

	SelectCrewChallengeMember(SelectedCrewChallengeMemberIndex);
	CrewChallengeState.bActive = true;
	CrewChallengeState.bFinished = false;
	CrewChallengeState.KeysFound = 0;
	CrewChallengeState.ChestsOpened = 0;
	CrewChallengeState.OpponentChestsOpened = 0;
	CrewChallengeState.SecondsRemaining = 300.f;
	CrewChallengeState.ResultText = TEXT("Match active: find keys, open 3 chests first.");
	CrewChallengeOpponentScoreTimer = 0.f;

	TroopCharacter->SetActorLocation(FVector(760000.f, 25000.f, 120.f), false, nullptr, ETeleportType::TeleportPhysics);
	TroopCharacter->SetActorRotation(FRotator::ZeroRotator);
	TroopCharacter->EnterLevel(201, TEXT("Crew Challenge Cove"), TEXT("4v4 Crew Match"), TEXT("Find keys, open chests, and claim 3 chests before the rival crew."));
	SpawnCrewChallengePickups();
	bHubMenuOpen = false;
	HubMenuMode = 0;
	if (!bPauseMenuOpen && !bSettingsOpen)
	{
		SetPause(false);
	}
}

void ACrooniesTreasureTroopPlayerController::CollectCrewChallengeKey()
{
	if (!CrewChallengeState.bActive || CrewChallengeState.bFinished)
	{
		return;
	}
	CrewChallengeState.KeysFound = FMath::Clamp(CrewChallengeState.KeysFound + 1, 0, 5);
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.5f, FColor(255, 225, 90), FString::Printf(TEXT("Crew key found: %d/5"), CrewChallengeState.KeysFound));
	}
}

bool ACrooniesTreasureTroopPlayerController::TryOpenCrewChallengeChest()
{
	if (!CrewChallengeState.bActive || CrewChallengeState.bFinished)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.5f, FColor(255, 205, 80), TEXT("Start a Crew Challenge before opening match chests."));
		}
		return false;
	}
	if (CrewChallengeState.KeysFound <= CrewChallengeState.ChestsOpened)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.5f, FColor(255, 205, 80), TEXT("Find another crew key before opening this chest."));
		}
		return false;
	}
	++CrewChallengeState.ChestsOpened;
	CrewChallengeState.ResultText = FString::Printf(TEXT("Chests: %d/3. Keys found: %d/5."), CrewChallengeState.ChestsOpened, CrewChallengeState.KeysFound);
	if (CrewChallengeState.ChestsOpened >= 3)
	{
		FinishCrewChallengeMatch(true, TEXT("Your crew opened 3 chests first."));
	}
	return true;
}

void ACrooniesTreasureTroopPlayerController::FinishCrewChallengeMatch(bool bWon, const FString& Reason)
{
	if (!CrewChallengeState.bActive && CrewChallengeState.bFinished)
	{
		return;
	}
	CrewChallengeState.bActive = false;
	CrewChallengeState.bFinished = true;
	CrewChallengeState.ResultText = bWon
		? FString::Printf(TEXT("WIN: %s You claimed 3 chests and the remaining chest contents."), *Reason)
		: FString::Printf(TEXT("LOSS: %s Rival crew score %d, your crew score %d."), *Reason, CrewChallengeState.OpponentChestsOpened, CrewChallengeState.ChestsOpened);
	if (bWon)
	{
		if (ACrooniesTreasureTroopCharacter* TroopCharacter = Cast<ACrooniesTreasureTroopCharacter>(GetPawn()))
		{
			TroopCharacter->AddCoins(75);
			TroopCharacter->AddRunLoot(120, 4.f);
		}
	}
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, bWon ? FColor(120, 255, 170) : FColor(255, 120, 90), CrewChallengeState.ResultText);
	}
}
void ACrooniesTreasureTroopPlayerController::TeleportToOldAtticTutorial()
{
	if (ACrooniesTreasureTroopCharacter* TroopCharacter = Cast<ACrooniesTreasureTroopCharacter>(GetPawn()))
	{
		TroopCharacter->SetActorLocation(FVector(35.f, 620.f, 140.f), false, nullptr, ETeleportType::TeleportPhysics);
		TroopCharacter->SetActorRotation(FRotator::ZeroRotator);
		TroopCharacter->EnterLevel(1, TEXT("The Old Attic"), TEXT("Tutorial"), TEXT("Follow the glowing attic path: recruit Mouth, switch Croonies, solve the switch path, and collect the treasure map."));
	}
}



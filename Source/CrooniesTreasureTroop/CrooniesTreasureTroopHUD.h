#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "CrooniesTreasureTroopHUD.generated.h"

UCLASS()
class ACrooniesTreasureTroopHUD : public AHUD
{
	GENERATED_BODY()

public:
	ACrooniesTreasureTroopHUD();

	virtual void DrawHUD() override;

private:
	UPROPERTY()
	class UTexture2D* CompassIcon;

	UPROPERTY()
	class UTexture2D* CoinIcon;

	UPROPERTY()
	class UTexture2D* MapIcon;

	UPROPERTY()
	class UTexture2D* FriendshipIcon;

	UPROPERTY()
	class UTexture2D* BackpackIcon;

	UPROPERTY()
	class UTexture2D* FlashlightIcon;

	UPROPERTY()
	class UTexture2D* SettingsIcon;

	UPROPERTY()
	class UTexture2D* ExitIcon;

	UPROPERTY()
	class UTexture2D* SignInButtonTexture;

	UPROPERTY()
	class UTexture2D* SignInButtonArtTexture;

	UPROPERTY()
	class UTexture2D* SignInBackgroundTexture;

	UPROPERTY()
	class UTexture2D* SignOutButtonTexture;

	UPROPERTY()
	class UTexture2D* MusicSoundIcon;

	UPROPERTY()
	class UTexture2D* MusicSliderTexture;

	UPROPERTY()
	class UTexture2D* SoundEffectsSliderTexture;

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

	class UTexture2D* GetLevelCard(int32 LevelNumber) const;
	void DrawTextureBox(class UTexture2D* Texture, const FVector2D& Position, const FVector2D& Size, const FLinearColor& Tint = FLinearColor::White);
	void DrawTextureFit(class UTexture2D* Texture, const FVector2D& Position, const FVector2D& Size, const FLinearColor& Tint = FLinearColor::White);
	void DrawSettingsButton(class ACrooniesTreasureTroopPlayerController* TroopPC);
	void DrawExitButton();
	void DrawSettingsOverlay(class ACrooniesTreasureTroopPlayerController* TroopPC);
	void DrawHubMenu(class ACrooniesTreasureTroopPlayerController* TroopPC);
	void DrawChatPanel(class ACrooniesTreasureTroopPlayerController* TroopPC);
	void DrawAtticDarkness(class ACrooniesTreasureTroopCharacter* Character);
};

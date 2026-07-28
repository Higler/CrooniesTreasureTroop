#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "CrooniesTreasureTroopSaveGame.generated.h"

UCLASS()
class UCrooniesTreasureTroopSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FString PlatformProvider = TEXT("Local Guest");

	UPROPERTY()
	FString PlatformDisplayId = TEXT("Guest");

	UPROPERTY()
	FString PlatformIdentityStatus = TEXT("LocalGuest");

	UPROPERTY()
	bool bParentalChatEnabled = true;

	UPROPERTY()
	bool bProfanityFilterEnabled = true;

	UPROPERTY()
	bool bPrivateChatEnabled = true;

	UPROPERTY()
	bool bReportingEnabled = true;

	UPROPERTY()
	int32 LiquidCouragePotionCount = 0;

	UPROPERTY()
	FString LastDailyLoginRewardDate;

	UPROPERTY()
	int32 DailyLoginStreak = 0;

	UPROPERTY()
	bool bCreatedCharacter = false;

	UPROPERTY()
	bool bMale = true;

	UPROPERTY()
	int32 SkinTone = 1;

	UPROPERTY()
	int32 HairStyle = 0;

	UPROPERTY()
	int32 HairColor = 1;

	UPROPERTY()
	int32 EyeColor = 1;
};

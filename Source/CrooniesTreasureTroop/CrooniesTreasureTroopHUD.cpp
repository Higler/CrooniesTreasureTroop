#include "CrooniesTreasureTroopHUD.h"
#include "CrooniesTreasureTroopCharacter.h"
#include "CrooniesTreasureTroopPlayerController.h"
#include "CanvasItem.h"
#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"
#include "GameFramework/PlayerController.h"

ACrooniesTreasureTroopHUD::ACrooniesTreasureTroopHUD()
{
	CompassIcon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Cove/UIIcons/compass.compass"));
	CoinIcon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Cove/UIIcons/goldcoin.goldcoin"));
	MapIcon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Cove/UIIcons/treasuremap.treasuremap"));
	FriendshipIcon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Cove/UIIcons/heartfriendship.heartfriendship"));
	BackpackIcon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Cove/UIIcons/backpack.backpack"));
	FlashlightIcon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Cove/UIIcons/flashlight.flashlight"));
	SettingsIcon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Cove/UIIcons/Settings_Icon_Overlay.Settings_Icon_Overlay"));
	ExitIcon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Cove/UIIcons/Exit_Icon_Overlay.Exit_Icon_Overlay"));
	SignInButtonTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Cove/UIIcons/Sign_In_Button.Sign_In_Button"));
	SignInButtonArtTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Cove/UIIcons/Sign_In_Button_Transparent.Sign_In_Button_Transparent"));
	SignInBackgroundTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Cove/UIIcons/Sign_In_Background_streched.Sign_In_Background_streched"));
	SignOutButtonTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Cove/UIIcons/Sign_Out_Button.Sign_Out_Button"));
	MusicSoundIcon = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Cove/UIIcons/Music_Sound_Icons.Music_Sound_Icons"));
	MusicSliderTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Cove/UIIcons/Music_Volume_Slider.Music_Volume_Slider"));
	SoundEffectsSliderTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Cove/UIIcons/Sound_Effects_Volume_Slider.Sound_Effects_Volume_Slider"));
	HubCaveEntranceTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Cove/UIIcons/Cave_Entrance_Adventure_Transparent.Cave_Entrance_Adventure_Transparent"));
	if (!HubCaveEntranceTexture)
	{
		HubCaveEntranceTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Cove/UIIcons/Cave_Entrance_Transparent.Cave_Entrance_Transparent"));
	}
	HubTaskBoardTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Cove/UIIcons/Task_Board_Transparent.Task_Board_Transparent"));
	HubGearStoreTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Cove/UIIcons/Hardware_Gadget_Store_Transparent.Hardware_Gadget_Store_Transparent"));
	HubPotionShopTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Cove/UIIcons/Potion_Time_Magic_Shop_Transparent.Potion_Time_Magic_Shop_Transparent"));
	HubSpecialAreaTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Cove/UIIcons/Magical_Specialty_Area_Fixed.Magical_Specialty_Area_Fixed"));
	if (!HubSpecialAreaTexture)
	{
		HubSpecialAreaTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Game/Cove/UIIcons/Magical_Specialty_Area.Magical_Specialty_Area"));
	}
}

UTexture2D* ACrooniesTreasureTroopHUD::GetLevelCard(int32 LevelNumber) const
{
	static const TCHAR* Paths[] = {
		TEXT(""),
		TEXT("/Game/Cove/LevelCards/Level_1_The_Old_AtticTutorial.Level_1_The_Old_AtticTutorial"),
		TEXT("/Game/Cove/LevelCards/Level_2_The_Rusty_Lighthouse.Level_2_The_Rusty_Lighthouse"),
		TEXT("/Game/Cove/LevelCards/Level_3_The_Flooded_Tunnels.Level_3_The_Flooded_Tunnels"),
		TEXT("/Game/Cove/LevelCards/Level_4_The_Collapsing_Mines.Level_4_The_Collapsing_Mines"),
		TEXT("/Game/Cove/LevelCards/Level_5_The_Bat_Caverns.Level_5_The_Bat_Caverns"),
		TEXT("/Game/Cove/LevelCards/Level_6_Crystal_Caverns.Level_6_Crystal_Caverns"),
		TEXT("/Game/Cove/LevelCards/Level_7_The_Mirror_Maze.Level_7_The_Mirror_Maze"),
		TEXT("/Game/Cove/LevelCards/Level_8_The_Ancient_Pirate_Outpost.Level_8_The_Ancient_Pirate_Outpost"),
		TEXT("/Game/Cove/LevelCards/Level_9_The_Great_Chasm.Level_9_The_Great_Chasm"),
		TEXT("/Game/Cove/LevelCards/Level_10_The_Underground_River.Level_10_The_Underground_River"),
	};

	if (LevelNumber > 0 && LevelNumber < UE_ARRAY_COUNT(Paths))
	{
		return LoadObject<UTexture2D>(nullptr, Paths[LevelNumber]);
	}
	return nullptr;
}

void ACrooniesTreasureTroopHUD::DrawTextureBox(UTexture2D* Texture, const FVector2D& Position, const FVector2D& Size, const FLinearColor& Tint)
{
	if (!Texture || !Texture->GetResource() || !Canvas)
	{
		return;
	}

	FCanvasTileItem Tile(Position, Texture->GetResource(), Size, Tint);
	Tile.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(Tile);
}

void ACrooniesTreasureTroopHUD::DrawTextureFit(UTexture2D* Texture, const FVector2D& Position, const FVector2D& Size, const FLinearColor& Tint)
{
	if (!Texture || !Texture->GetResource() || !Canvas || Texture->GetSizeX() <= 0 || Texture->GetSizeY() <= 0)
	{
		return;
	}

	const float TextureAspect = static_cast<float>(Texture->GetSizeX()) / static_cast<float>(Texture->GetSizeY());
	const float BoxAspect = Size.X / Size.Y;
	FVector2D DrawSize = Size;
	if (BoxAspect > TextureAspect)
	{
		DrawSize.X = Size.Y * TextureAspect;
	}
	else
	{
		DrawSize.Y = Size.X / TextureAspect;
	}

	const FVector2D DrawPos = Position + (Size - DrawSize) * 0.5f;
	FCanvasTileItem Tile(DrawPos, Texture->GetResource(), DrawSize, Tint);
	Tile.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(Tile);
}

namespace
{
	void DrawScaledText(UCanvas* Canvas, const FString& Text, const FVector2D& Position, UFont* Font, const FLinearColor& Color, float Scale = 1.f)
	{
		FCanvasTextItem Item(Position, FText::FromString(Text), Font, Color);
		Item.Scale = FVector2D(Scale, Scale);
		Item.EnableShadow(FLinearColor::Black);
		Canvas->DrawItem(Item);
	}

	void DrawPanel(UCanvas* Canvas, const FVector2D& Position, const FVector2D& Size, const FLinearColor& Color)
	{
		FCanvasTileItem Tile(Position, Size, Color);
		Tile.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(Tile);
	}

	void DrawTextureCover(UCanvas* Canvas, UTexture2D* Texture, const FVector2D& ViewSize, const FLinearColor& Tint, float ExtraScale = 1.f)
	{
		if (!Canvas || !Texture || !Texture->GetResource() || Texture->GetSizeX() <= 0 || Texture->GetSizeY() <= 0)
		{
			return;
		}

		const float TextureAspect = static_cast<float>(Texture->GetSizeX()) / static_cast<float>(Texture->GetSizeY());
		const float ViewAspect = ViewSize.X / ViewSize.Y;
		FVector2D DrawSize = ViewSize;
		if (ViewAspect > TextureAspect)
		{
			DrawSize.Y = ViewSize.X / TextureAspect;
		}
		else
		{
			DrawSize.X = ViewSize.Y * TextureAspect;
		}

		DrawSize *= FMath::Max(ExtraScale, 1.f);
		const FVector2D Pos((ViewSize.X - DrawSize.X) * 0.5f, (ViewSize.Y - DrawSize.Y) * 0.5f);
		FCanvasTileItem Tile(Pos, Texture->GetResource(), DrawSize, Tint);
		Tile.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(Tile);
	}

	void DrawTextureContain(UCanvas* Canvas, UTexture2D* Texture, const FVector2D& ViewSize, const FLinearColor& Tint)
	{
		if (!Canvas || !Texture || !Texture->GetResource() || Texture->GetSizeX() <= 0 || Texture->GetSizeY() <= 0)
		{
			return;
		}

		const float TextureAspect = static_cast<float>(Texture->GetSizeX()) / static_cast<float>(Texture->GetSizeY());
		const float ViewAspect = ViewSize.X / ViewSize.Y;
		FVector2D DrawSize = ViewSize;
		if (ViewAspect > TextureAspect)
		{
			DrawSize.X = ViewSize.Y * TextureAspect;
		}
		else
		{
			DrawSize.Y = ViewSize.X / TextureAspect;
		}

		const FVector2D Pos((ViewSize.X - DrawSize.X) * 0.5f, (ViewSize.Y - DrawSize.Y) * 0.5f);
		FCanvasTileItem Tile(Pos, Texture->GetResource(), DrawSize, Tint);
		Tile.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(Tile);
	}

	void DrawTexturePortraitCard(UCanvas* Canvas, UTexture2D* Texture, const FVector2D& ViewSize, const FLinearColor& Tint, float HeightScale)
	{
		if (!Canvas || !Texture || !Texture->GetResource() || Texture->GetSizeX() <= 0 || Texture->GetSizeY() <= 0)
		{
			return;
		}

		const float TextureAspect = static_cast<float>(Texture->GetSizeX()) / static_cast<float>(Texture->GetSizeY());
		const float DrawH = ViewSize.Y * HeightScale;
		const float DrawW = DrawH * TextureAspect;
		const FVector2D Pos((ViewSize.X - DrawW) * 0.5f, (ViewSize.Y - DrawH) * 0.5f);
		FCanvasTileItem Tile(Pos, Texture->GetResource(), FVector2D(DrawW, DrawH), Tint);
		Tile.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(Tile);
	}

	void DrawToggle(UCanvas* Canvas, const FVector2D& Position, bool bEnabled)
	{
		DrawPanel(Canvas, Position, FVector2D(160.f, 44.f), bEnabled ? FLinearColor(0.10f, 0.36f, 0.16f, 1.f) : FLinearColor(0.28f, 0.08f, 0.08f, 1.f));
		DrawScaledText(Canvas, bEnabled ? TEXT("ON") : TEXT("OFF"), Position + FVector2D(52.f, 11.f), GEngine->GetMediumFont(), FLinearColor::White, 0.9f);
	}

	void DrawSlider(UCanvas* Canvas, const FVector2D& Position, float Value, const FLinearColor& BarColor)
	{
		DrawPanel(Canvas, Position + FVector2D(0.f, 8.f), FVector2D(340.f, 12.f), FLinearColor(0.02f, 0.025f, 0.03f, 1.f));
		DrawPanel(Canvas, Position + FVector2D(0.f, 8.f), FVector2D(340.f * FMath::Clamp(Value, 0.f, 1.f), 12.f), BarColor);
		DrawPanel(Canvas, Position + FVector2D(340.f * FMath::Clamp(Value, 0.f, 1.f) - 9.f, 0.f), FVector2D(18.f, 28.f), FLinearColor(1.f, 0.86f, 0.35f, 1.f));
	}
}

void ACrooniesTreasureTroopHUD::DrawAtticDarkness(ACrooniesTreasureTroopCharacter* Character)
{
	if (!Canvas || !Character || Character->GetCurrentLevel() != 1 || Character->IsAtticLightOn())
	{
		return;
	}

	FVector2D PlayerScreen(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);
	if (APlayerController* PC = GetOwningPlayerController())
	{
		FVector2D Projected;
		if (PC->ProjectWorldLocationToScreen(Character->GetActorLocation(), Projected))
		{
			PlayerScreen = Projected;
		}
	}

	const float Radius = FMath::Clamp(Canvas->ClipY * 0.085f, 64.f, 118.f);
	const FLinearColor Blackout(0.f, 0.f, 0.f, 0.995f);
	const float TopHeight = FMath::Max(0.f, PlayerScreen.Y - Radius);
	const float BottomY = PlayerScreen.Y + Radius;
	const float BottomHeight = FMath::Max(0.f, Canvas->ClipY - BottomY);
	const float MiddleY = PlayerScreen.Y - Radius;
	const float MiddleHeight = Radius * 2.f;
	const float LeftWidth = FMath::Max(0.f, PlayerScreen.X - Radius);
	const float RightX = PlayerScreen.X + Radius;
	const float RightWidth = FMath::Max(0.f, Canvas->ClipX - RightX);

	DrawPanel(Canvas, FVector2D(0.f, 0.f), FVector2D(Canvas->ClipX, TopHeight), Blackout);
	DrawPanel(Canvas, FVector2D(0.f, BottomY), FVector2D(Canvas->ClipX, BottomHeight), Blackout);
	DrawPanel(Canvas, FVector2D(0.f, MiddleY), FVector2D(LeftWidth, MiddleHeight), Blackout);
	DrawPanel(Canvas, FVector2D(RightX, MiddleY), FVector2D(RightWidth, MiddleHeight), Blackout);
	DrawPanel(Canvas, FVector2D(PlayerScreen.X - Radius, PlayerScreen.Y - Radius), FVector2D(Radius * 2.f, 4.f), FLinearColor(1.f, 0.77f, 0.28f, 0.13f));
	DrawPanel(Canvas, FVector2D(PlayerScreen.X - Radius, PlayerScreen.Y + Radius - 4.f), FVector2D(Radius * 2.f, 4.f), FLinearColor(1.f, 0.77f, 0.28f, 0.13f));
}

void ACrooniesTreasureTroopHUD::DrawSettingsButton(ACrooniesTreasureTroopPlayerController* TroopPC)
{
	if (!Canvas || !TroopPC)
	{
		return;
	}

	const FVector2D Pos(Canvas->ClipX - 96.f, 22.f);
	const FVector2D Size(76.f, 76.f);
	if (SettingsIcon)
	{
		DrawTextureBox(SettingsIcon, Pos + FVector2D(3.f, 4.f), FVector2D(76.f, 76.f), TroopPC->IsSettingsOpen() ? FLinearColor(1.f, 0.92f, 0.55f, 1.f) : FLinearColor::White);
	}
	else
	{
		DrawScaledText(Canvas, TEXT("SET"), Pos + FVector2D(10.f, 22.f), GEngine->GetSmallFont(), FLinearColor(1.f, 0.86f, 0.42f), 1.1f);
	}
}

void ACrooniesTreasureTroopHUD::DrawExitButton()
{
	if (!Canvas)
	{
		return;
	}

	const FVector2D Pos(Canvas->ClipX - 188.f, 22.f);
	if (ExitIcon)
	{
		DrawTextureBox(ExitIcon, Pos, FVector2D(80.f, 80.f), FLinearColor::White);
	}
	else
	{
		DrawScaledText(Canvas, TEXT("X"), Pos + FVector2D(18.f, 4.f), GEngine->GetLargeFont(), FLinearColor(1.f, 0.88f, 0.38f), 2.0f);
	}

	const FString ExitText = TEXT("Esc : Exit");
	const FVector2D TextPos(Pos.X + 8.f, Pos.Y + 82.f);
	DrawScaledText(Canvas, ExitText, TextPos + FVector2D(2.f, 2.f), GEngine->GetSmallFont(), FLinearColor(0.17f, 0.08f, 0.02f, 0.90f), 1.12f);
	DrawScaledText(Canvas, ExitText, TextPos, GEngine->GetSmallFont(), FLinearColor(1.f, 0.82f, 0.28f), 1.12f);
}

void ACrooniesTreasureTroopHUD::DrawSettingsOverlay(ACrooniesTreasureTroopPlayerController* TroopPC)
{
	if (!Canvas || !TroopPC || !TroopPC->IsSettingsOpen())
	{
		return;
	}

	DrawPanel(Canvas, FVector2D(0.f, 0.f), FVector2D(Canvas->ClipX, Canvas->ClipY), FLinearColor(0.f, 0.f, 0.f, 0.62f));
	const FVector2D Panel(Canvas->ClipX * 0.5f - 470.f, Canvas->ClipY * 0.5f - 360.f);
	DrawPanel(Canvas, Panel, FVector2D(940.f, 720.f), FLinearColor(0.035f, 0.047f, 0.055f, 0.98f));
	DrawPanel(Canvas, Panel, FVector2D(940.f, 8.f), FLinearColor(1.f, 0.76f, 0.20f, 1.f));

	DrawScaledText(Canvas, TEXT("Game Settings"), Panel + FVector2D(58.f, 34.f), GEngine->GetLargeFont(), FLinearColor(1.f, 0.86f, 0.42f), 1.65f);
	DrawScaledText(Canvas, TEXT("X"), Panel + FVector2D(874.f, 38.f), GEngine->GetLargeFont(), FLinearColor(1.f, 0.62f, 0.52f), 1.2f);

	DrawScaledText(Canvas, TEXT("Audio"), Panel + FVector2D(70.f, 124.f), GEngine->GetMediumFont(), FLinearColor(0.86f, 0.95f, 1.f), 1.25f);
	DrawTextureBox(MusicSoundIcon, Panel + FVector2D(70.f, 170.f), FVector2D(82.f, 82.f), FLinearColor(1.f, 1.f, 1.f, 0.95f));
	DrawScaledText(Canvas, TEXT("Music"), Panel + FVector2D(180.f, 182.f), GEngine->GetMediumFont(), FLinearColor::White, 1.f);
	DrawToggle(Canvas, Panel + FVector2D(700.f, 178.f), TroopPC->IsMusicEnabled());
	DrawTextureBox(MusicSliderTexture, Panel + FVector2D(300.f, 215.f), FVector2D(370.f, 48.f), FLinearColor(1.f, 1.f, 1.f, 0.30f));
	DrawSlider(Canvas, Panel + FVector2D(310.f, 225.f), TroopPC->GetMusicVolume(), FLinearColor(0.22f, 0.70f, 0.26f, 1.f));
	DrawScaledText(Canvas, FString::Printf(TEXT("%d%%"), FMath::RoundToInt(TroopPC->GetMusicVolume() * 100.f)), Panel + FVector2D(700.f, 226.f), GEngine->GetSmallFont(), FLinearColor(0.90f, 1.f, 0.86f), 1.05f);

	DrawScaledText(Canvas, TEXT("Sound Effects"), Panel + FVector2D(180.f, 278.f), GEngine->GetMediumFont(), FLinearColor::White, 1.f);
	DrawToggle(Canvas, Panel + FVector2D(700.f, 274.f), TroopPC->IsSoundEffectsEnabled());
	DrawTextureBox(SoundEffectsSliderTexture, Panel + FVector2D(300.f, 311.f), FVector2D(370.f, 48.f), FLinearColor(1.f, 1.f, 1.f, 0.30f));
	DrawSlider(Canvas, Panel + FVector2D(310.f, 321.f), TroopPC->GetSoundEffectsVolume(), FLinearColor(0.20f, 0.58f, 0.88f, 1.f));
	DrawScaledText(Canvas, FString::Printf(TEXT("%d%%"), FMath::RoundToInt(TroopPC->GetSoundEffectsVolume() * 100.f)), Panel + FVector2D(700.f, 322.f), GEngine->GetSmallFont(), FLinearColor(0.86f, 0.94f, 1.f), 1.05f);

	DrawScaledText(Canvas, TEXT("Gameplay"), Panel + FVector2D(70.f, 372.f), GEngine->GetMediumFont(), FLinearColor(0.86f, 0.95f, 1.f), 1.25f);
	DrawScaledText(Canvas, TEXT("Subtitles / Tutorial Text"), Panel + FVector2D(90.f, 410.f), GEngine->GetMediumFont(), FLinearColor::White, 0.92f);
	DrawToggle(Canvas, Panel + FVector2D(700.f, 402.f), TroopPC->IsSubtitlesEnabled());
	DrawScaledText(Canvas, TEXT("Vibration / Haptics"), Panel + FVector2D(90.f, 466.f), GEngine->GetMediumFont(), FLinearColor::White, 0.92f);
	DrawToggle(Canvas, Panel + FVector2D(700.f, 458.f), TroopPC->IsVibrationEnabled());

	DrawScaledText(Canvas, TEXT("Chat Safety"), Panel + FVector2D(460.f, 476.f), GEngine->GetMediumFont(), FLinearColor(0.86f, 0.95f, 1.f), 1.05f);
	DrawScaledText(Canvas, TEXT("Parental Chat"), Panel + FVector2D(482.f, 524.f), GEngine->GetSmallFont(), FLinearColor::White, 0.82f);
	DrawToggle(Canvas, Panel + FVector2D(700.f, 512.f), TroopPC->IsParentalChatEnabled());
	DrawScaledText(Canvas, TEXT("Profanity Filter"), Panel + FVector2D(482.f, 566.f), GEngine->GetSmallFont(), FLinearColor::White, 0.82f);
	DrawToggle(Canvas, Panel + FVector2D(700.f, 554.f), TroopPC->IsProfanityFilterEnabled());
	DrawScaledText(Canvas, TEXT("Private Chat"), Panel + FVector2D(482.f, 608.f), GEngine->GetSmallFont(), FLinearColor::White, 0.82f);
	DrawToggle(Canvas, Panel + FVector2D(700.f, 596.f), TroopPC->IsPrivateChatEnabled());
	DrawScaledText(Canvas, TEXT("Player Reports"), Panel + FVector2D(482.f, 650.f), GEngine->GetSmallFont(), FLinearColor::White, 0.82f);
	DrawToggle(Canvas, Panel + FVector2D(700.f, 638.f), TroopPC->IsReportingEnabled());

	DrawScaledText(Canvas, TEXT("Account"), Panel + FVector2D(70.f, 526.f), GEngine->GetMediumFont(), FLinearColor(0.86f, 0.95f, 1.f), 1.15f);
	if (SignOutButtonTexture)
	{
		DrawTextureBox(SignOutButtonTexture, Panel + FVector2D(80.f, 552.f), FVector2D(300.f, 56.f), FLinearColor::White);
	}
	else
	{
		DrawPanel(Canvas, Panel + FVector2D(80.f, 552.f), FVector2D(300.f, 56.f), FLinearColor(0.36f, 0.12f, 0.10f, 1.f));
	}
	DrawScaledText(Canvas, TEXT("Sign Out"), Panel + FVector2D(172.f, 568.f), GEngine->GetMediumFont(), FLinearColor::White, 0.95f);
	DrawScaledText(Canvas, FString::Printf(TEXT("Identity: %s"), *TroopPC->GetIdentityStatusLabel()), Panel + FVector2D(80.f, 622.f), GEngine->GetSmallFont(), FLinearColor(0.74f, 0.88f, 1.f), 0.88f);
	DrawScaledText(Canvas, TEXT("Esc closes settings. Click X to close."), Panel + FVector2D(80.f, 674.f), GEngine->GetSmallFont(), FLinearColor(0.74f, 0.82f, 0.86f), 0.78f);
}

void ACrooniesTreasureTroopHUD::DrawHubMenu(ACrooniesTreasureTroopPlayerController* TroopPC)
{
	if (!Canvas || !TroopPC || !TroopPC->IsHubMenuOpen())
	{
		return;
	}

	DrawPanel(Canvas, FVector2D(0.f, 0.f), FVector2D(Canvas->ClipX, Canvas->ClipY), FLinearColor(0.02f, 0.03f, 0.035f, 0.82f));

	const FVector2D Panel(Canvas->ClipX * 0.5f - 490.f, Canvas->ClipY * 0.5f - 345.f);
	DrawPanel(Canvas, Panel, FVector2D(980.f, 690.f), FLinearColor(0.08f, 0.095f, 0.09f, 0.98f));
	DrawPanel(Canvas, Panel, FVector2D(980.f, 10.f), FLinearColor(1.f, 0.72f, 0.20f, 1.f));
	DrawPanel(Canvas, Panel + FVector2D(0.f, 96.f), FVector2D(980.f, 3.f), FLinearColor(0.42f, 0.25f, 0.10f, 1.f));

	const int32 Mode = TroopPC->GetHubMenuMode();
	const FString Title = Mode == 1 ? TEXT("Caverns Entrance") : (Mode == 2 ? TEXT("Special Levels") : (Mode == 3 ? TEXT("Local Gear Store") : (Mode == 4 ? TEXT("Daily Tasks") : (Mode == 5 ? TEXT("Adventure Shop") : (Mode == 6 ? TEXT("Crew Challenge") : TEXT("The Neighborhood"))))));
	const FString Subtitle = Mode == 1
		? TEXT("Choose a cavern. Locked routes require that level map 4/4 and store gear.")
		: (Mode == 2 ? TEXT("Choose one special mode. Requires the Special Portal Key from the Local Gear Store.") : (Mode == 3 ? TEXT("Spend daily-task coins on cavern gear and the Special Portal Key.") : (Mode == 4 ? TEXT("Complete tasks with Courage. Each finished task rewards coins for the store.") : (Mode == 5 ? TEXT("Premium consumables and cosmetics. Demo purchases are placeholders until app-store IAP is wired.") : (Mode == 6 ? TEXT("Pick a crew member, spend Courage, and start a timed 4v4 chest match.") : TEXT("Choose one entrance: tutorial, caverns, daily tasks, local store, special levels, adventure shop, or crew challenge."))))));
	DrawScaledText(Canvas, Title, Panel + FVector2D(46.f, 30.f), GEngine->GetLargeFont(), FLinearColor(1.f, 0.84f, 0.38f), 1.55f);
	DrawScaledText(Canvas, Subtitle, Panel + FVector2D(48.f, 70.f), GEngine->GetSmallFont(), FLinearColor(0.88f, 0.93f, 0.86f), 1.02f);
	DrawScaledText(Canvas, TEXT("X"), Panel + FVector2D(922.f, 26.f), GEngine->GetLargeFont(), FLinearColor(1.f, 0.78f, 0.28f), 1.28f);

	if (Mode == 0)
	{
		static const TCHAR* Entrances[] = {
			TEXT("Old Attic\nTutorial"),
			TEXT("Caverns\nLevels 2-12"),
			TEXT("Daily Tasks\nEarn Coins"),
			TEXT("Local Store\nGear"),
			TEXT("Special\nBonus Modes"),
			TEXT("Adventure\nShop"),
			TEXT("Crew\nChallenge")
		};
		static const TCHAR* EntranceHelp[] = {
			TEXT("Replay the first tutorial and map discovery."),
			TEXT("Open level select and check map locks."),
			TEXT("Spend Courage to earn store coins."),
			TEXT("Buy grappling hooks, lenses, keys, and more."),
			TEXT("Requires the Special Portal Key from the store."),
			TEXT("Buy Liquid Courage and future cosmetics."),
			TEXT("Start crew chest matches.")
		};
		for (int32 Index = 0; Index < 7; ++Index)
		{
			const int32 Column = Index % 3;
			const int32 Row = Index / 3;
			const FVector2D Button = Panel + FVector2D(72.f + Column * 305.f, 190.f + Row * 154.f);
			UTexture2D* EntranceTexture = nullptr;
			if (Index == 0)
			{
				EntranceTexture = GetLevelCard(1);
			}
			else if (Index == 1)
			{
				EntranceTexture = HubCaveEntranceTexture;
			}
			else if (Index == 2)
			{
				EntranceTexture = HubTaskBoardTexture;
			}
			else if (Index == 3)
			{
				EntranceTexture = HubGearStoreTexture;
			}
			else if (Index == 4)
			{
				EntranceTexture = HubSpecialAreaTexture;
			}
			else if (Index == 5)
			{
				EntranceTexture = HubPotionShopTexture;
			}
			else
			{
				EntranceTexture = HubTaskBoardTexture;
			}
			DrawPanel(Canvas, Button + FVector2D(6.f, 7.f), FVector2D(250.f, 122.f), FLinearColor(0.f, 0.f, 0.f, 0.36f));
			DrawPanel(Canvas, Button, FVector2D(250.f, 122.f), Index == 1 ? FLinearColor(0.09f, 0.20f, 0.24f, 1.f) : (Index == 2 ? FLinearColor(0.18f, 0.16f, 0.07f, 1.f) : (Index == 3 ? FLinearColor(0.13f, 0.18f, 0.09f, 1.f) : (Index == 5 ? FLinearColor(0.22f, 0.11f, 0.04f, 1.f) : FLinearColor(0.18f, 0.13f, 0.08f, 1.f)))));
			DrawPanel(Canvas, Button, FVector2D(250.f, 7.f), Index == 2 ? FLinearColor(1.f, 0.82f, 0.20f, 1.f) : (Index == 3 ? FLinearColor(0.44f, 0.84f, 0.28f, 1.f) : (Index == 4 ? FLinearColor(0.34f, 0.72f, 0.95f, 1.f) : (Index == 5 ? FLinearColor(1.f, 0.48f, 0.14f, 1.f) : FLinearColor(1.f, 0.72f, 0.20f, 1.f)))));
			DrawPanel(Canvas, Button + FVector2D(12.f, 16.f), FVector2D(88.f, 72.f), FLinearColor(0.f, 0.f, 0.f, 0.28f));
			DrawTextureFit(EntranceTexture, Button + FVector2D(14.f, 18.f), FVector2D(84.f, 68.f), FLinearColor(1.f, 1.f, 1.f, 0.96f));
			DrawScaledText(Canvas, Entrances[Index], Button + FVector2D(108.f, 22.f), GEngine->GetMediumFont(), FLinearColor::White, 0.78f);
			DrawScaledText(Canvas, EntranceHelp[Index], Button + FVector2D(18.f, 92.f), GEngine->GetSmallFont(), FLinearColor(0.86f, 0.93f, 0.88f), 0.66f);
		}
		DrawScaledText(Canvas, TEXT("Keyboard: 1 tutorial, 2 caverns, 3 tasks, 4 local store, 5 special, 6 adventure shop, 7 crew challenge. Esc or X closes."), Panel + FVector2D(58.f, 620.f), GEngine->GetSmallFont(), FLinearColor(0.80f, 0.88f, 0.86f), 0.86f);
		return;
	}

	if (Mode == 6)
	{
		ACrooniesTreasureTroopCharacter* Character = nullptr;
		if (APlayerController* PC = TroopPC)
		{
			Character = Cast<ACrooniesTreasureTroopCharacter>(PC->GetPawn());
		}

		const FCrooniesCrewChallengeState& State = TroopPC->GetCrewChallengeState();
		DrawPanel(Canvas, Panel + FVector2D(54.f, 120.f), FVector2D(872.f, 112.f), FLinearColor(0.06f, 0.10f, 0.11f, 1.f));
		DrawScaledText(Canvas, FString::Printf(TEXT("Crew: %s     Required Courage: %d     Your Courage: %d"),
			*TroopPC->GetCrewDisplayName(), TroopPC->GetCrewChallengeCourageCost(), Character ? Character->GetCourage() : 0),
			Panel + FVector2D(78.f, 146.f), GEngine->GetMediumFont(), FLinearColor(1.f, 0.86f, 0.38f), 0.86f);
		DrawScaledText(Canvas, FString::Printf(TEXT("Opponent: %s [%s]     Match: 5 keys, 5 chests, first crew to 3 chests wins."),
			*State.OpponentCrewName, *State.OpponentCrewTag),
			Panel + FVector2D(78.f, 180.f), GEngine->GetSmallFont(), FLinearColor(0.86f, 0.94f, 0.88f), 0.82f);

		const TArray<FName>& Roster = TroopPC->GetCrewChallengeRoster();
		const int32 SelectedIndex = TroopPC->GetSelectedCrewChallengeMemberIndex();
		for (int32 Index = 0; Index < Roster.Num(); ++Index)
		{
			const int32 Column = Index % 4;
			const int32 Row = Index / 4;
			const FVector2D Button = Panel + FVector2D(70.f + Column * 215.f, 260.f + Row * 82.f);
			const bool bSelected = Index == SelectedIndex;
			DrawPanel(Canvas, Button + FVector2D(4.f, 5.f), FVector2D(185.f, 60.f), FLinearColor(0.f, 0.f, 0.f, 0.30f));
			DrawPanel(Canvas, Button, FVector2D(185.f, 60.f), bSelected ? FLinearColor(0.13f, 0.24f, 0.16f, 1.f) : FLinearColor(0.09f, 0.10f, 0.11f, 1.f));
			DrawPanel(Canvas, Button, FVector2D(185.f, 5.f), bSelected ? FLinearColor(0.38f, 0.94f, 0.50f, 1.f) : FLinearColor(0.42f, 0.48f, 0.50f, 1.f));
			DrawScaledText(Canvas, FString::Printf(TEXT("%d  %s"), Index + 1, *Roster[Index].ToString()), Button + FVector2D(14.f, 17.f), GEngine->GetSmallFont(), FLinearColor::White, 0.82f);
			DrawScaledText(Canvas, bSelected ? TEXT("SELECTED") : TEXT("Crew member"), Button + FVector2D(14.f, 39.f), GEngine->GetSmallFont(), bSelected ? FLinearColor(0.70f, 1.f, 0.72f) : FLinearColor(0.80f, 0.88f, 0.86f), 0.62f);
		}

		DrawPanel(Canvas, Panel + FVector2D(330.f, 520.f), FVector2D(320.f, 62.f), FLinearColor(0.16f, 0.12f, 0.05f, 1.f));
		DrawPanel(Canvas, Panel + FVector2D(330.f, 520.f), FVector2D(320.f, 6.f), FLinearColor(1.f, 0.74f, 0.20f, 1.f));
		DrawScaledText(Canvas, TEXT("9  Start Crew Match"), Panel + FVector2D(398.f, 542.f), GEngine->GetMediumFont(), FLinearColor::White, 0.86f);
		DrawScaledText(Canvas, FString::Printf(TEXT("Status: %s     Keys %d/5     Chests %d/3     Rival %d/3     Time %.0fs"),
			State.ResultText.IsEmpty() ? TEXT("Ready") : *State.ResultText, State.KeysFound, State.ChestsOpened, State.OpponentChestsOpened, State.SecondsRemaining),
			Panel + FVector2D(58.f, 620.f), GEngine->GetSmallFont(), FLinearColor(0.80f, 0.88f, 0.86f), 0.74f);
		DrawScaledText(Canvas, TEXT("Keyboard: 1-8 selects member, 9 starts match. Esc or X closes."), Panel + FVector2D(58.f, 652.f), GEngine->GetSmallFont(), FLinearColor(0.80f, 0.88f, 0.86f), 0.78f);
		return;
	}

	if (Mode == 1)
	{
		static const TCHAR* LevelNames[] = {
			TEXT("2 Rusty Lighthouse"), TEXT("3 Flooded Tunnels"), TEXT("4 Collapsing Mines"),
			TEXT("5 Bat Caverns"), TEXT("6 Crystal Caverns"), TEXT("7 Mirror Maze"),
			TEXT("8 Pirate Outpost"), TEXT("9 Great Chasm"), TEXT("10 Underground River"),
			TEXT("11 Getaway Chase"), TEXT("12 Shipwreck Boss")
		};

		for (int32 Index = 0; Index < UE_ARRAY_COUNT(LevelNames); ++Index)
		{
			const int32 TargetLevel = Index + 2;
			FString LockedReason;
			const bool bUnlocked = TroopPC->CanTeleportToAdventureLevel(TargetLevel, LockedReason);
			const int32 Column = Index % 3;
			const int32 Row = Index / 3;
			const FVector2D Button = Panel + FVector2D(58.f + Column * 300.f, 154.f + Row * 82.f);
			DrawPanel(Canvas, Button + FVector2D(4.f, 5.f), FVector2D(260.f, 60.f), FLinearColor(0.f, 0.f, 0.f, 0.32f));
			DrawPanel(Canvas, Button, FVector2D(260.f, 60.f), bUnlocked ? FLinearColor(0.10f, 0.22f, 0.18f, 1.f) : FLinearColor(0.16f, 0.11f, 0.10f, 1.f));
			DrawPanel(Canvas, Button, FVector2D(260.f, 5.f), bUnlocked ? FLinearColor(0.35f, 0.92f, 0.62f, 1.f) : FLinearColor(0.90f, 0.32f, 0.20f, 1.f));
			DrawScaledText(Canvas, LevelNames[Index], Button + FVector2D(16.f, 13.f), GEngine->GetSmallFont(), FLinearColor::White, 0.98f);
			ACrooniesTreasureTroopCharacter* Character = nullptr;
			if (APlayerController* PC = TroopPC)
			{
				Character = Cast<ACrooniesTreasureTroopCharacter>(PC->GetPawn());
			}
			const int32 Pieces = Character ? Character->GetMapPiecesForLevel(TargetLevel) : 0;
			const bool bHasItem = Character ? Character->HasRequiredSpecialItemForLevel(TargetLevel) : false;
			const FString Requirement = FString::Printf(TEXT("Map %d/4  Item %s"), Pieces, bHasItem ? TEXT("YES") : TEXT("NO"));
			DrawScaledText(Canvas, bUnlocked ? TEXT("UNLOCKED") : TEXT("LOCKED"), Button + FVector2D(16.f, 34.f), GEngine->GetSmallFont(), bUnlocked ? FLinearColor(0.66f, 1.f, 0.72f) : FLinearColor(1.f, 0.66f, 0.42f), 0.66f);
			DrawScaledText(Canvas, Requirement, Button + FVector2D(108.f, 34.f), GEngine->GetSmallFont(), FLinearColor(0.82f, 0.90f, 0.86f), 0.58f);
		}
		DrawScaledText(Canvas, TEXT("Locked button clicks explain the missing treasure map/items. Keyboard 1-0 selects visible routes."), Panel + FVector2D(58.f, 620.f), GEngine->GetSmallFont(), FLinearColor(0.80f, 0.88f, 0.86f), 0.88f);
		return;
	}

	if (Mode == 3)
	{
		ACrooniesTreasureTroopCharacter* Character = nullptr;
		if (APlayerController* PC = TroopPC)
		{
			Character = Cast<ACrooniesTreasureTroopCharacter>(PC->GetPawn());
		}
		static const int32 StoreLevels[] = {2,3,4,5,6,7,8,9,10,11,12};
		for (int32 Index = 0; Index < UE_ARRAY_COUNT(StoreLevels) + 1; ++Index)
		{
			const int32 Column = Index % 3;
			const int32 Row = Index / 3;
			const FVector2D Button = Panel + FVector2D(58.f + Column * 300.f, 154.f + Row * 82.f);
			const bool bSpecialPortalCard = Index == UE_ARRAY_COUNT(StoreLevels);
			const int32 TargetLevel = bSpecialPortalCard ? 0 : StoreLevels[Index];
			const bool bOwned = bSpecialPortalCard ? TroopPC->HasSpecialLevelsAccess() : (Character ? Character->HasRequiredSpecialItemForLevel(TargetLevel) : false);
			const int32 Price = bSpecialPortalCard ? TroopPC->GetSpecialLevelsAccessPrice() : TroopPC->GetSpecialItemPriceForLevel(TargetLevel);
			DrawPanel(Canvas, Button + FVector2D(4.f, 5.f), FVector2D(260.f, 60.f), FLinearColor(0.f, 0.f, 0.f, 0.32f));
			DrawPanel(Canvas, Button, FVector2D(260.f, 60.f), bOwned ? FLinearColor(0.08f, 0.22f, 0.14f, 1.f) : (bSpecialPortalCard ? FLinearColor(0.13f, 0.14f, 0.23f, 1.f) : FLinearColor(0.16f, 0.13f, 0.08f, 1.f)));
			DrawPanel(Canvas, Button, FVector2D(260.f, 5.f), bOwned ? FLinearColor(0.42f, 0.92f, 0.44f, 1.f) : (bSpecialPortalCard ? FLinearColor(0.50f, 0.74f, 1.f, 1.f) : FLinearColor(1.f, 0.72f, 0.20f, 1.f)));
			const FString ItemLine = bSpecialPortalCard ? TEXT("Special Portal Key") : (Character ? Character->GetRequiredSpecialItemNameForLevel(TargetLevel) : TEXT("Gear"));
			const FString TitleLine = bSpecialPortalCard ? ItemLine : FString::Printf(TEXT("L%d  %s"), TargetLevel, *ItemLine);
			DrawScaledText(Canvas, TitleLine, Button + FVector2D(14.f, 8.f), GEngine->GetSmallFont(), FLinearColor::White, 0.82f);
			DrawScaledText(Canvas, bOwned ? TEXT("OWNED") : FString::Printf(TEXT("Requires: %d Coins"), Price), Button + FVector2D(16.f, 30.f), GEngine->GetSmallFont(), bOwned ? FLinearColor(0.66f, 1.f, 0.72f) : FLinearColor(1.f, 0.84f, 0.38f), 0.68f);
			DrawScaledText(Canvas, bSpecialPortalCard ? TEXT("Unlocks: Special Levels") : FString::Printf(TEXT("Unlocks: Level %d"), TargetLevel), Button + FVector2D(16.f, 47.f), GEngine->GetSmallFont(), FLinearColor(0.86f, 0.94f, 0.88f), 0.62f);
		}
		DrawScaledText(Canvas, TEXT("Coins are earned from Daily Tasks. Buy cavern gear here, plus the Special Portal Key for bonus modes."), Panel + FVector2D(58.f, 620.f), GEngine->GetSmallFont(), FLinearColor(0.80f, 0.88f, 0.86f), 0.96f);
		return;
	}

	if (Mode == 4)
	{
		ACrooniesTreasureTroopCharacter* Character = nullptr;
		if (APlayerController* PC = TroopPC)
		{
			Character = Cast<ACrooniesTreasureTroopCharacter>(PC->GetPawn());
		}

		const int32 Coins = Character ? Character->GetCoinCount() : 0;
		const int32 Courage = Character ? Character->GetCourage() : 0;
		const int32 Done = Character ? Character->GetCompletedDailyTaskCount() : 0;
		DrawPanel(Canvas, Panel + FVector2D(50.f, 118.f), FVector2D(880.f, 62.f), FLinearColor(0.11f, 0.09f, 0.04f, 1.f));
		DrawScaledText(Canvas, FString::Printf(TEXT("Progress: %d/10 complete     Courage: %d     Coins: %d     Goal: Buy Grappling Hook for 10 coins"), Done, Courage, Coins),
			Panel + FVector2D(70.f, 138.f), GEngine->GetMediumFont(), FLinearColor(1.f, 0.86f, 0.38f), 0.92f);

		static const TCHAR* TaskNames[] = {
			TEXT("Sweep the attic"),
			TEXT("Sort map scraps"),
			TEXT("Pack lanterns"),
			TEXT("Fix a shoelace"),
			TEXT("Check walkie"),
			TEXT("Feed neighborhood dog"),
			TEXT("Polish compass"),
			TEXT("Stack supply crates"),
			TEXT("Practice courage pose"),
			TEXT("Mark safe route")
		};

		for (int32 Index = 0; Index < UE_ARRAY_COUNT(TaskNames); ++Index)
		{
			const int32 Column = Index % 2;
			const int32 Row = Index / 2;
			const FVector2D Button = Panel + FVector2D(70.f + Column * 455.f, 198.f + Row * 80.f);
			const bool bComplete = Character ? Character->IsDailyTaskComplete(Index) : false;
			const bool bCanDo = Character && !bComplete && Character->GetCourage() > 0;
			DrawPanel(Canvas, Button + FVector2D(5.f, 6.f), FVector2D(390.f, 70.f), FLinearColor(0.f, 0.f, 0.f, 0.34f));
			DrawPanel(Canvas, Button, FVector2D(390.f, 70.f), bComplete ? FLinearColor(0.08f, 0.22f, 0.14f, 1.f) : (bCanDo ? FLinearColor(0.20f, 0.16f, 0.08f, 1.f) : FLinearColor(0.12f, 0.12f, 0.12f, 1.f)));
			DrawPanel(Canvas, Button, FVector2D(390.f, 7.f), bComplete ? FLinearColor(0.42f, 0.92f, 0.44f, 1.f) : FLinearColor(1.f, 0.72f, 0.20f, 1.f));
			DrawScaledText(Canvas, FString::Printf(TEXT("%d"), Index + 1), Button + FVector2D(18.f, 14.f), GEngine->GetMediumFont(), FLinearColor(1.f, 0.86f, 0.38f), 0.92f);
			DrawScaledText(Canvas, TaskNames[Index], Button + FVector2D(58.f, 14.f), GEngine->GetMediumFont(), FLinearColor::White, 0.82f);
			DrawScaledText(Canvas, bComplete ? TEXT("DONE") : TEXT("Requires: 1 Courage"), Button + FVector2D(18.f, 42.f), GEngine->GetSmallFont(), bComplete ? FLinearColor(0.66f, 1.f, 0.72f) : FLinearColor(1.f, 0.86f, 0.44f), 0.88f);
			DrawScaledText(Canvas, bComplete ? TEXT("Reward Claimed") : TEXT("Reward: 1 Coin"), Button + FVector2D(205.f, 42.f), GEngine->GetSmallFont(), bComplete ? FLinearColor(0.66f, 1.f, 0.72f) : FLinearColor(0.86f, 1.f, 0.72f), 0.88f);
		}

		DrawPanel(Canvas, Panel + FVector2D(50.f, 602.f), FVector2D(880.f, 48.f), FLinearColor(0.05f, 0.08f, 0.07f, 1.f));
		DrawScaledText(Canvas, TEXT("Loop: Complete tasks -> earn coins -> buy Local Gear Store items -> enter caverns with completed maps."), Panel + FVector2D(70.f, 615.f), GEngine->GetSmallFont(), FLinearColor(0.86f, 0.94f, 0.88f), 0.90f);
		DrawScaledText(Canvas, TEXT("Keyboard: 1-0 completes tasks. Esc or X closes."), Panel + FVector2D(58.f, 662.f), GEngine->GetSmallFont(), FLinearColor(0.80f, 0.88f, 0.86f), 0.82f);
		return;
	}

	if (Mode == 5)
	{
		ACrooniesTreasureTroopCharacter* Character = nullptr;
		if (APlayerController* PC = TroopPC)
		{
			Character = Cast<ACrooniesTreasureTroopCharacter>(PC->GetPawn());
		}

		static const TCHAR* TabNames[] = {
			TEXT("Energy"),
			TEXT("Currency"),
			TEXT("Progression & Boost"),
			TEXT("Gadgets & Tools"),
			TEXT("Convenience"),
			TEXT("Daily Deals")
		};
		const int32 ActiveTab = FMath::Clamp(TroopPC->GetPremiumStoreTab(), 0, 5);
		for (int32 Index = 0; Index < UE_ARRAY_COUNT(TabNames); ++Index)
		{
			const int32 Column = Index % 3;
			const int32 Row = Index / 3;
			const FVector2D Tab = Panel + FVector2D(54.f + Column * 302.f, 126.f + Row * 58.f);
			const bool bActive = Index == ActiveTab;
			DrawPanel(Canvas, Tab + FVector2D(4.f, 5.f), FVector2D(278.f, 44.f), FLinearColor(0.f, 0.f, 0.f, 0.30f));
			DrawPanel(Canvas, Tab, FVector2D(278.f, 44.f), bActive ? FLinearColor(0.26f, 0.14f, 0.04f, 1.f) : FLinearColor(0.075f, 0.085f, 0.09f, 1.f));
			DrawPanel(Canvas, Tab, FVector2D(278.f, 5.f), bActive ? FLinearColor(1.f, 0.68f, 0.16f, 1.f) : FLinearColor(0.36f, 0.42f, 0.46f, 1.f));
			DrawScaledText(Canvas, FString::Printf(TEXT("%d  %s"), Index + 1, TabNames[Index]), Tab + FVector2D(14.f, 13.f), GEngine->GetSmallFont(), bActive ? FLinearColor(1.f, 0.88f, 0.38f) : FLinearColor(0.86f, 0.92f, 0.92f), 0.82f);
		}

		DrawPanel(Canvas, Panel + FVector2D(62.f, 238.f), FVector2D(856.f, 50.f), FLinearColor(0.12f, 0.07f, 0.025f, 1.f));
		DrawScaledText(Canvas, FString::Printf(TEXT("Inventory: Liquid Courage x%d     Courage: %d/1000     Coins: %d     Tab: %s"),
			TroopPC->GetLiquidCouragePotionCount(), Character ? Character->GetCourage() : 0, Character ? Character->GetCoinCount() : 0, TabNames[ActiveTab]),
			Panel + FVector2D(84.f, 253.f), GEngine->GetSmallFont(), FLinearColor(1.f, 0.86f, 0.38f), 0.96f);
		if (ActiveTab == 0)
		{
			DrawPanel(Canvas, Panel + FVector2D(762.f, 246.f), FVector2D(136.f, 34.f), FLinearColor(0.08f, 0.22f, 0.11f, 1.f));
			DrawScaledText(Canvas, TEXT("8  Use Potion"), Panel + FVector2D(780.f, 255.f), GEngine->GetSmallFont(), FLinearColor(0.72f, 1.f, 0.70f), 0.72f);
		}

		struct FShopDisplayItem
		{
			const TCHAR* Name;
			const TCHAR* Detail;
			const TCHAR* Price;
		};

		auto DrawStoreCard = [this](const FVector2D& Card, const FString& Name, const FString& Detail, const FString& Price, const FLinearColor& Accent, bool bFeatured)
		{
			const FVector2D Size(400.f, 68.f);
			DrawPanel(Canvas, Card + FVector2D(5.f, 6.f), Size, FLinearColor(0.f, 0.f, 0.f, 0.34f));
			DrawPanel(Canvas, Card, Size, bFeatured ? FLinearColor(0.22f, 0.11f, 0.035f, 1.f) : FLinearColor(0.085f, 0.072f, 0.052f, 1.f));
			DrawPanel(Canvas, Card, FVector2D(Size.X, 6.f), Accent);
			DrawScaledText(Canvas, Name, Card + FVector2D(16.f, 13.f), GEngine->GetSmallFont(), FLinearColor::White, bFeatured ? 0.92f : 0.82f);
			DrawScaledText(Canvas, Detail, Card + FVector2D(16.f, 34.f), GEngine->GetSmallFont(), FLinearColor(0.90f, 0.94f, 0.90f), 0.62f);
			DrawScaledText(Canvas, Price, Card + FVector2D(292.f, 39.f), GEngine->GetSmallFont(), FLinearColor(1.f, 0.86f, 0.38f), 0.70f);
		};

		static const FShopDisplayItem EnergyItems[] = {
			{TEXT("Courage Potion"), TEXT("Main energy item: restores 3 Courage."), TEXT("$0.99")},
			{TEXT("Super Courage Elixir"), TEXT("5-10 extra attempts or 30 min unlimited courage."), TEXT("$2.99")},
			{TEXT("Team Courage"), TEXT("Restores courage for the whole active group."), TEXT("$3.99")},
			{TEXT("Courage Refill Pack"), TEXT("Bundles of 10/25/50 Courage Potions. Best value."), TEXT("$4.99+")}
		};
		static const FShopDisplayItem CurrencyItems[] = {
			{TEXT("Small Coin Bundle"), TEXT("Starter pack of in-game coins."), TEXT("$0.99")},
			{TEXT("Medium Coin Bundle"), TEXT("Better value coin pack for gear and outfits."), TEXT("$2.99")},
			{TEXT("Large / Mega Coin Bundle"), TEXT("Big coin packs for heavy testing and unlocks."), TEXT("$7.99+")},
			{TEXT("Treasure Key Pack"), TEXT("Keys for locked treasure chests and bonus areas."), TEXT("$1.99")}
		};
		static const FShopDisplayItem ProgressionItems[] = {
			{TEXT("Flashlight Battery Pack"), TEXT("Longer or brighter light in dark cave puzzles."), TEXT("$0.99")},
			{TEXT("Map Fragments Booster"), TEXT("Reveals or grants extra map pieces."), TEXT("$2.99")},
			{TEXT("Lucky Charm"), TEXT("Better loot chance for a limited time."), TEXT("$1.99")},
			{TEXT("Speed Potion"), TEXT("Faster movement for a level/session."), TEXT("$0.99")},
			{TEXT("Hint Token Pack"), TEXT("Extra puzzle hints for tough rooms."), TEXT("$1.99")},
			{TEXT("Gold Doubloons"), TEXT("Premium currency usable across the shop."), TEXT("$4.99+")}
		};
		static const FShopDisplayItem GadgetItems[] = {
			{TEXT("Grappling Hook Upgrade"), TEXT("Better rope tool for reaching high areas."), TEXT("$2.99")},
			{TEXT("Skeleton Key"), TEXT("Opens any locked door once."), TEXT("$1.99")},
			{TEXT("Treasure Detector"), TEXT("Highlights collectibles/secrets for one level."), TEXT("$2.99")},
			{TEXT("Magic Compass"), TEXT("Points toward the exit or main objective."), TEXT("$1.99")},
			{TEXT("Lantern of Truth"), TEXT("Reveals hidden platforms or invisible paths."), TEXT("$2.99")}
		};
		static const FShopDisplayItem ConvenienceItems[] = {
			{TEXT("Instant Level Restart"), TEXT("Skips fail animation/loading."), TEXT("$0.99")},
			{TEXT("Daily Adventure Pass"), TEXT("Daily courage + coins + one booster bundle."), TEXT("$2.99")},
			{TEXT("Extra Inventory Slots"), TEXT("Carry more items during a run."), TEXT("$1.99")},
			{TEXT("Auto-Save Checkpoint"), TEXT("Adds extra mid-level save points."), TEXT("$1.99")}
		};
		static const FShopDisplayItem EventItems[] = {
			{TEXT("Event Treasure Rush Pack"), TEXT("Weekend event bonus pack."), TEXT("$4.99")},
			{TEXT("Season Pass Tier Unlock"), TEXT("Instant tier skip in battle-pass style events."), TEXT("$1.99")},
			{TEXT("Time Warp Token"), TEXT("Resets daily challenges or cooldowns."), TEXT("$1.99")}
		};

		const FShopDisplayItem* Items = EnergyItems;
		int32 ItemCount = UE_ARRAY_COUNT(EnergyItems);
		FLinearColor Accent(1.f, 0.52f, 0.14f, 1.f);
		if (ActiveTab == 1)
		{
			Items = CurrencyItems;
			ItemCount = UE_ARRAY_COUNT(CurrencyItems);
			Accent = FLinearColor(1.f, 0.78f, 0.18f, 1.f);
		}
		else if (ActiveTab == 2)
		{
			Items = ProgressionItems;
			ItemCount = UE_ARRAY_COUNT(ProgressionItems);
			Accent = FLinearColor(0.30f, 0.72f, 1.f, 1.f);
		}
		else if (ActiveTab == 3)
		{
			Items = GadgetItems;
			ItemCount = UE_ARRAY_COUNT(GadgetItems);
			Accent = FLinearColor(0.48f, 0.90f, 1.f, 1.f);
		}
		else if (ActiveTab == 4)
		{
			Items = ConvenienceItems;
			ItemCount = UE_ARRAY_COUNT(ConvenienceItems);
			Accent = FLinearColor(0.46f, 0.78f, 0.38f, 1.f);
		}
		else if (ActiveTab == 5)
		{
			Items = EventItems;
			ItemCount = UE_ARRAY_COUNT(EventItems);
			Accent = FLinearColor(0.84f, 0.42f, 1.f, 1.f);
		}

		for (int32 Index = 0; Index < ItemCount; ++Index)
		{
			const int32 Column = Index % 2;
			const int32 Row = Index / 2;
			const FVector2D Card = Panel + FVector2D(76.f + Column * 438.f, 316.f + Row * 76.f);
			DrawStoreCard(Card, Items[Index].Name, Items[Index].Detail, Items[Index].Price, Accent, ActiveTab == 0 && Index == 0);
		}

		DrawPanel(Canvas, Panel + FVector2D(62.f, 492.f), FVector2D(856.f, 86.f), FLinearColor(0.04f, 0.055f, 0.06f, 1.f));
		DrawScaledText(Canvas, TEXT("Monetization safety note"), Panel + FVector2D(88.f, 510.f), GEngine->GetMediumFont(), FLinearColor(1.f, 0.84f, 0.38f), 0.82f);
		DrawScaledText(Canvas, TEXT("Windows demo purchases do not charge real money. Mobile builds must validate Google Play / Apple receipts on a backend before granting items."), Panel + FVector2D(88.f, 542.f), GEngine->GetSmallFont(), FLinearColor(0.86f, 0.94f, 0.88f), 0.78f);
		DrawScaledText(Canvas, TEXT("Keyboard: 1-6 switches tabs. Energy tab: 7 buys demo potion, 8 uses potion. Esc or X closes."), Panel + FVector2D(58.f, 620.f), GEngine->GetSmallFont(), FLinearColor(0.80f, 0.88f, 0.86f), 0.90f);
		return;
	}

	static const TCHAR* BonusNames[] = {
		TEXT("Secret Treehouse"),
		TEXT("Ghost Ship"),
		TEXT("Endless Caverns"),
		TEXT("Neighborhood Mayhem")
	};
	for (int32 Index = 0; Index < UE_ARRAY_COUNT(BonusNames); ++Index)
	{
		const FVector2D Button = Panel + FVector2D(58.f + Index * 225.f, 260.f);
		DrawPanel(Canvas, Button + FVector2D(4.f, 5.f), FVector2D(200.f, 82.f), FLinearColor(0.f, 0.f, 0.f, 0.32f));
		DrawPanel(Canvas, Button, FVector2D(200.f, 82.f), FLinearColor(0.12f, 0.22f, 0.15f, 1.f));
		DrawPanel(Canvas, Button, FVector2D(200.f, 6.f), FLinearColor(0.44f, 0.84f, 0.28f, 1.f));
		DrawScaledText(Canvas, BonusNames[Index], Button + FVector2D(18.f, 30.f), GEngine->GetSmallFont(), FLinearColor::White, 0.92f);
	}
	DrawScaledText(Canvas, TEXT("Special levels require the Special Portal Key. Keyboard: 1-4 selects. Esc or X closes."), Panel + FVector2D(58.f, 620.f), GEngine->GetSmallFont(), FLinearColor(0.80f, 0.88f, 0.86f), 0.95f);
}

void ACrooniesTreasureTroopHUD::DrawChatPanel(ACrooniesTreasureTroopPlayerController* TroopPC)
{
	if (!Canvas || !TroopPC)
	{
		return;
	}

	const TArray<FCrooniesChatMessage>& Messages = TroopPC->GetChatHistory();
	const bool bOpen = TroopPC->IsChatOpen();
	if (!bOpen && Messages.Num() == 0)
	{
		const FVector2D HintPos(24.f, Canvas->ClipY - 184.f);
		DrawPanel(Canvas, HintPos, FVector2D(530.f, 34.f), FLinearColor(0.02f, 0.025f, 0.03f, 0.42f));
		DrawScaledText(Canvas, TEXT("Chat: Enter opens | Y world | T team | U private"), HintPos + FVector2D(12.f, 9.f), GEngine->GetSmallFont(), FLinearColor(0.82f, 0.92f, 1.f), 0.78f);
		return;
	}

	const FVector2D PanelPos(24.f, Canvas->ClipY - 330.f);
	const FVector2D PanelSize(670.f, bOpen ? 286.f : 206.f);
	DrawPanel(Canvas, PanelPos, PanelSize, FLinearColor(0.015f, 0.020f, 0.024f, bOpen ? 0.82f : 0.56f));
	DrawPanel(Canvas, PanelPos, FVector2D(PanelSize.X, 6.f), FLinearColor(1.f, 0.76f, 0.20f, bOpen ? 1.f : 0.78f));

	auto ChannelColor = [](ECrooniesChatChannel Channel)
	{
		if (Channel == ECrooniesChatChannel::Team)
		{
			return FLinearColor(0.48f, 1.f, 0.62f, 1.f);
		}
		if (Channel == ECrooniesChatChannel::Private)
		{
			return FLinearColor(0.46f, 0.78f, 1.f, 1.f);
		}
		return FLinearColor(1.f, 0.84f, 0.38f, 1.f);
	};

	auto ChannelName = [](ECrooniesChatChannel Channel)
	{
		if (Channel == ECrooniesChatChannel::Team)
		{
			return FString(TEXT("TEAM"));
		}
		if (Channel == ECrooniesChatChannel::Private)
		{
			return FString(TEXT("PRIVATE"));
		}
		return FString(TEXT("WORLD"));
	};

	const int32 MaxVisible = bOpen ? 7 : 5;
	const int32 FirstIndex = FMath::Max(0, Messages.Num() - MaxVisible);
	float LineY = PanelPos.Y + 22.f;
	for (int32 Index = FirstIndex; Index < Messages.Num(); ++Index)
	{
		const FCrooniesChatMessage& Message = Messages[Index];
		const FString Prefix = Message.Channel == ECrooniesChatChannel::Private
			? FString::Printf(TEXT("[%s] %s -> %s:"), *ChannelName(Message.Channel), *Message.SenderName, *Message.TargetName)
			: FString::Printf(TEXT("[%s] %s:"), *ChannelName(Message.Channel), *Message.SenderName);
		DrawScaledText(Canvas, Prefix, FVector2D(PanelPos.X + 14.f, LineY), GEngine->GetSmallFont(), ChannelColor(Message.Channel), 0.86f);
		DrawScaledText(Canvas, Message.Body.Left(96), FVector2D(PanelPos.X + 220.f, LineY), GEngine->GetSmallFont(), FLinearColor::White, 0.86f);
		LineY += 28.f;
	}

	if (bOpen)
	{
		const FVector2D InputPos(PanelPos.X + 14.f, PanelPos.Y + PanelSize.Y - 82.f);
		DrawPanel(Canvas, InputPos, FVector2D(PanelSize.X - 28.f, 46.f), FLinearColor(0.06f, 0.075f, 0.085f, 0.96f));
		DrawPanel(Canvas, InputPos, FVector2D(112.f, 46.f), ChannelColor(TroopPC->GetActiveChatChannel()) * FLinearColor(0.55f, 0.55f, 0.55f, 1.f));
		DrawScaledText(Canvas, TroopPC->GetChatChannelLabel(), InputPos + FVector2D(10.f, 13.f), GEngine->GetSmallFont(), FLinearColor::White, 0.78f);
		DrawScaledText(Canvas, TroopPC->GetChatDraft() + TEXT("_"), InputPos + FVector2D(126.f, 12.f), GEngine->GetMediumFont(), FLinearColor::White, 0.88f);
		DrawScaledText(Canvas, TroopPC->GetChatHelpText(), PanelPos + FVector2D(14.f, PanelSize.Y - 28.f), GEngine->GetSmallFont(), FLinearColor(0.80f, 0.90f, 0.96f), 0.68f);
	}
	else
	{
		DrawScaledText(Canvas, TEXT("Enter opens chat"), PanelPos + FVector2D(14.f, PanelSize.Y - 30.f), GEngine->GetSmallFont(), FLinearColor(0.80f, 0.90f, 0.96f), 0.72f);
	}
}

void ACrooniesTreasureTroopHUD::DrawHUD()
{
	Super::DrawHUD();

	ACrooniesTreasureTroopCharacter* Character = nullptr;
	ACrooniesTreasureTroopPlayerController* TroopPC = nullptr;
	if (APlayerController* PC = GetOwningPlayerController())
	{
		TroopPC = Cast<ACrooniesTreasureTroopPlayerController>(PC);
		Character = Cast<ACrooniesTreasureTroopCharacter>(PC->GetPawn());
	}

	if (!Character || !Canvas)
	{
		return;
	}

	const float Pad = 24.f;
	const bool bPauseOpen = TroopPC && TroopPC->IsPauseMenuOpen();
	const ECrooniesAppState AppState = TroopPC ? TroopPC->GetAppState() : ECrooniesAppState::Playing;
	auto DrawDailyLoginRewardNotice = [&]()
	{
		if (!TroopPC || !TroopPC->HasDailyLoginRewardNotice())
		{
			return;
		}

		const FVector2D RewardSize(FMath::Min(Canvas->ClipX - 64.f, 760.f), 124.f);
		const FVector2D RewardPos(Canvas->ClipX * 0.5f - RewardSize.X * 0.5f, 224.f);
		DrawPanel(Canvas, RewardPos + FVector2D(10.f, 12.f), RewardSize, FLinearColor(0.f, 0.f, 0.f, 0.28f));
		DrawPanel(Canvas, RewardPos, RewardSize, FLinearColor(0.07f, 0.045f, 0.018f, 0.94f));
		DrawPanel(Canvas, RewardPos, FVector2D(RewardSize.X, 9.f), FLinearColor(1.f, 0.76f, 0.20f, 1.f));
		DrawScaledText(Canvas, TEXT("FREE DAILY LOGIN REWARD"), RewardPos + FVector2D(34.f, 26.f), GEngine->GetMediumFont(), FLinearColor(1.f, 0.86f, 0.38f), 1.18f);
		DrawScaledText(Canvas, TroopPC->GetDailyLoginRewardText(), RewardPos + FVector2D(34.f, 66.f), GEngine->GetSmallFont(), FLinearColor::White, 1.05f);
		DrawScaledText(Canvas, TEXT("Come back tomorrow for another free reward."), RewardPos + FVector2D(34.f, 94.f), GEngine->GetSmallFont(), FLinearColor(0.82f, 0.95f, 0.88f), 0.86f);
	};

	if (AppState == ECrooniesAppState::Login)
	{
		if (SignInBackgroundTexture)
		{
			DrawTextureBox(SignInBackgroundTexture, FVector2D(0.f, 0.f), FVector2D(Canvas->ClipX, Canvas->ClipY), FLinearColor::White);
		}
		else
		{
			DrawPanel(Canvas, FVector2D(0.f, 0.f), FVector2D(Canvas->ClipX, Canvas->ClipY), FLinearColor(0.92f, 0.88f, 0.76f, 1.f));
		}

		const bool bOptionsOpen = TroopPC && TroopPC->AreLoginOptionsOpen();
		const float ButtonW = FMath::Clamp(Canvas->ClipX * 0.30f, 420.f, 650.f);
		const float ButtonH = ButtonW * (209.f / 520.f);
		const float ButtonX = Canvas->ClipX * 0.5f - ButtonW * 0.5f;
		const float SignInY = Canvas->ClipY * 0.565f;
		const float PlatformY = SignInY + ButtonH + Canvas->ClipY * 0.012f;
		const float LocalY = bOptionsOpen ? PlatformY + Canvas->ClipY * 0.055f : SignInY + ButtonH + Canvas->ClipY * 0.006f;

		if (SignInButtonArtTexture)
		{
			DrawTextureBox(SignInButtonArtTexture, FVector2D(ButtonX, SignInY), FVector2D(ButtonW, ButtonH), FLinearColor::White);
		}
		else if (!SignInBackgroundTexture)
		{
			DrawPanel(Canvas, FVector2D(ButtonX, SignInY), FVector2D(ButtonW, ButtonH), FLinearColor(0.96f, 0.56f, 0.17f, 0.96f));
			DrawScaledText(Canvas, TEXT("Sign In"), FVector2D(ButtonX + ButtonW * 0.5f - 72.f, SignInY + ButtonH * 0.35f), GEngine->GetLargeFont(), FLinearColor::White, 1.25f);
		}

		if (bOptionsOpen)
		{
			DrawPanel(Canvas, FVector2D(ButtonX + ButtonW * 0.07f, PlatformY), FVector2D(ButtonW * 0.86f, Canvas->ClipY * 0.044f), FLinearColor(0.10f, 0.26f, 0.18f, 0.95f));
			DrawScaledText(Canvas, TroopPC ? TroopPC->GetPlatformSignInLabel() : TEXT("Platform Sign In"), FVector2D(ButtonX + ButtonW * 0.15f, PlatformY + Canvas->ClipY * 0.012f), GEngine->GetMediumFont(), FLinearColor::White, 0.95f);
		}

		if (bOptionsOpen || !SignInBackgroundTexture)
		{
			DrawScaledText(Canvas, TEXT("Continue without signing in"), FVector2D(Canvas->ClipX * 0.5f - 126.f, LocalY), GEngine->GetSmallFont(), FLinearColor(0.17f, 0.09f, 0.045f), 0.95f);
			DrawScaledText(Canvas, bOptionsOpen ? TEXT("Choose the available platform sign-in, or keep playing locally.") : TEXT("Tap Sign In to choose the platform account for this device."), FVector2D(Canvas->ClipX * 0.5f - 248.f, LocalY + Canvas->ClipY * 0.032f), GEngine->GetSmallFont(), FLinearColor(0.20f, 0.12f, 0.065f), 0.82f);
		}

		DrawExitButton();
		DrawSettingsButton(TroopPC);
		DrawSettingsOverlay(TroopPC);
		return;
	}

	if (AppState == ECrooniesAppState::CharacterCreation)
	{
		FCanvasTileItem Backdrop(FVector2D(0.f, 0.f), FVector2D(Canvas->ClipX, Canvas->ClipY), FLinearColor(0.018f, 0.024f, 0.030f, 1.f));
		Backdrop.BlendMode = SE_BLEND_Opaque;
		Canvas->DrawItem(Backdrop);

		const FVector2D Panel(Canvas->ClipX * 0.5f - 470.f, Canvas->ClipY * 0.5f - 300.f);
		DrawPanel(Canvas, Panel, FVector2D(940.f, 620.f), FLinearColor(0.055f, 0.065f, 0.075f, 0.98f));
		DrawPanel(Canvas, Panel, FVector2D(940.f, 8.f), FLinearColor(1.f, 0.76f, 0.20f, 1.f));

		DrawScaledText(Canvas, TEXT("Create Your Croonie"), Panel + FVector2D(54.f, 38.f), GEngine->GetLargeFont(), FLinearColor(1.f, 0.86f, 0.45f), 1.65f);

		const FString Provider = FString::Printf(TEXT("Signed in as: %s"), *TroopPC->GetLoginProvider());
		DrawScaledText(Canvas, Provider, Panel + FVector2D(58.f, 102.f), GEngine->GetSmallFont(), FLinearColor(0.72f, 0.88f, 1.f), 1.1f);

		const FString Lines[] = {
			FString::Printf(TEXT("Body Type: %s"), TroopPC->IsMaleChoice() ? TEXT("Male") : TEXT("Female")),
			FString::Printf(TEXT("Skin Tone: %d"), TroopPC->GetSkinToneChoice() + 1),
			FString::Printf(TEXT("Hair Style: %d"), TroopPC->GetHairStyleChoice() + 1),
			FString::Printf(TEXT("Hair Color: %d"), TroopPC->GetHairColorChoice() + 1),
			FString::Printf(TEXT("Eye Color: %d"), TroopPC->GetEyeColorChoice() + 1)
		};

		for (int32 Index = 0; Index < UE_ARRAY_COUNT(Lines); ++Index)
		{
			const float RowY = 145.f + Index * 58.f;
			DrawPanel(Canvas, Panel + FVector2D(70.f, RowY), FVector2D(720.f, 46.f), Index == 0 ? FLinearColor(0.12f, 0.18f, 0.22f, 1.f) : FLinearColor(0.08f, 0.105f, 0.13f, 1.f));
			DrawScaledText(Canvas, TEXT("<"), Panel + FVector2D(92.f, RowY + 9.f), GEngine->GetMediumFont(), FLinearColor(1.f, 0.82f, 0.42f), 1.05f);
			DrawScaledText(Canvas, Lines[Index], Panel + FVector2D(148.f, RowY + 9.f), GEngine->GetMediumFont(), FLinearColor(0.92f, 0.96f, 1.f), 1.05f);
			DrawScaledText(Canvas, TEXT(">"), Panel + FVector2D(720.f, RowY + 9.f), GEngine->GetMediumFont(), FLinearColor(1.f, 0.82f, 0.42f), 1.05f);
		}

		DrawTextureBox(BackpackIcon, Panel + FVector2D(795.f, 154.f), FVector2D(92.f, 92.f), FLinearColor(1.f, 1.f, 1.f, 0.92f));
		DrawTextureBox(FriendshipIcon, Panel + FVector2D(815.f, 268.f), FVector2D(74.f, 74.f), FLinearColor(1.f, 1.f, 1.f, 0.92f));

		DrawScaledText(Canvas, TEXT("Click/tap a row or use Up/Down. Left/Right changes values."), Panel + FVector2D(58.f, 452.f), GEngine->GetSmallFont(), FLinearColor(1.f, 0.84f, 0.55f), 1.05f);

		DrawScaledText(Canvas, TroopPC->GetCharacterSummary(), Panel + FVector2D(58.f, 490.f), GEngine->GetSmallFont(), FLinearColor(0.78f, 1.f, 0.74f), 1.f);
		DrawPanel(Canvas, Panel + FVector2D(250.f, 514.f), FVector2D(440.f, 68.f), FLinearColor(0.18f, 0.36f, 0.18f, 1.f));
		DrawScaledText(Canvas, TEXT("START LEVEL 1: THE OLD ATTIC"), Panel + FVector2D(300.f, 536.f), GEngine->GetMediumFont(), FLinearColor::White, 1.05f);
		DrawDailyLoginRewardNotice();
		DrawSettingsButton(TroopPC);
		DrawSettingsOverlay(TroopPC);
		return;
	}

	DrawAtticDarkness(Character);

	const FString Title = FString::Printf(TEXT("Croonies: Treasure Troop    L%d %s"),
		Character->GetCurrentLevel(), *Character->GetLevelTitle());
	const FString Objective = Character->GetObjectiveText();
	const FString Stats = FString::Printf(TEXT("Active: %s   Party: %s   Style: %s"),
		*Character->GetSelectedCrewmate(), *Character->GetPartySummary(), *Character->GetLevelStyle());
	const FString Collection = FString::Printf(TEXT("Coins: %d   Artifacts: %d/12   Map Pieces: %d/44   Tokens: %d/8   Upgrades: %d   Courage: %d   Stamina: %.0f/%.0f   Loot: %d value / %.1f wt"),
		Character->GetCoinCount(), Character->GetArtifactCount(), Character->GetMapPieceCount(), Character->GetFriendshipTokenCount(), Character->GetUpgradeCount(), Character->GetCourage(), Character->GetRunStamina(), Character->GetMaxRunStamina(), Character->GetRunLootValue(), Character->GetCarryWeight());
	const FString Controls = TEXT("WASD/Arrows or click/touch to move    Space jump    G action/use/pickup    1-6 switch friends    H hub    M map menu    P pause    Esc quit");
	const FString TutorialPrompt = Character->GetTutorialPromptText();

	const FVector2D HudPanelSize(1120.f, 184.f);
	const FVector2D HudOrigin(FMath::Max(16.f, Canvas->ClipX * 0.5f - HudPanelSize.X * 0.5f), 16.f);

	FCanvasTileItem Backdrop(HudOrigin, HudPanelSize, FLinearColor(0.02f, 0.025f, 0.03f, 0.78f));
	Backdrop.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(Backdrop);

	DrawTextureBox(CompassIcon, HudOrigin + FVector2D(12.f, 14.f), FVector2D(44.f, 44.f));

	FCanvasTextItem TitleItem(HudOrigin + FVector2D(66.f, 6.f), FText::FromString(Title), GEngine->GetLargeFont(), FLinearColor(0.95f, 0.9f, 0.65f));
	TitleItem.EnableShadow(FLinearColor::Black);
	Canvas->DrawItem(TitleItem);

	FCanvasTextItem ObjItem(HudOrigin + FVector2D(10.f, 46.f), FText::FromString(Objective), GEngine->GetMediumFont(), FLinearColor::White);
	ObjItem.EnableShadow(FLinearColor::Black);
	Canvas->DrawItem(ObjItem);

	FCanvasTextItem StatItem(HudOrigin + FVector2D(10.f, 78.f), FText::FromString(Stats), GEngine->GetSmallFont(), FLinearColor(0.7f, 0.9f, 1.f));
	StatItem.EnableShadow(FLinearColor::Black);
	Canvas->DrawItem(StatItem);

	FCanvasTextItem CollectionItem(HudOrigin + FVector2D(10.f, 108.f), FText::FromString(Collection), GEngine->GetSmallFont(), FLinearColor(1.f, 0.82f, 0.42f));
	CollectionItem.EnableShadow(FLinearColor::Black);
	Canvas->DrawItem(CollectionItem);

	FCanvasTextItem ControlItem(HudOrigin + FVector2D(10.f, 138.f), FText::FromString(Controls), GEngine->GetSmallFont(), FLinearColor(0.88f, 0.95f, 1.f));
	ControlItem.EnableShadow(FLinearColor::Black);
	Canvas->DrawItem(ControlItem);

	DrawTextureBox(CoinIcon, HudOrigin + FVector2D(10.f, 188.f), FVector2D(34.f, 34.f));
	DrawTextureBox(MapIcon, HudOrigin + FVector2D(54.f, 188.f), FVector2D(34.f, 34.f));
	DrawTextureBox(FriendshipIcon, HudOrigin + FVector2D(98.f, 188.f), FVector2D(34.f, 34.f));
	DrawTextureBox(BackpackIcon, HudOrigin + FVector2D(142.f, 188.f), FVector2D(34.f, 34.f));
	DrawTextureBox(FlashlightIcon, HudOrigin + FVector2D(186.f, 188.f), FVector2D(34.f, 34.f));
	if (!TutorialPrompt.IsEmpty())
	{
		const FVector2D PromptPos(16.f, Canvas->ClipY - 132.f);
		DrawPanel(Canvas, PromptPos, FVector2D(FMath::Min(Canvas->ClipX - 32.f, 1260.f), 92.f), FLinearColor(0.07f, 0.045f, 0.02f, 0.88f));
		DrawPanel(Canvas, PromptPos, FVector2D(FMath::Min(Canvas->ClipX - 32.f, 1260.f), 8.f), FLinearColor(1.f, 0.76f, 0.20f, 1.f));
		DrawScaledText(Canvas, TEXT("OLD ATTIC TUTORIAL"), PromptPos + FVector2D(24.f, 20.f), GEngine->GetSmallFont(), FLinearColor(1.f, 0.84f, 0.38f), 1.08f);
		DrawScaledText(Canvas, TutorialPrompt, PromptPos + FVector2D(24.f, 50.f), GEngine->GetMediumFont(), FLinearColor::White, 1.05f);
	}

	const float ActionSize = FMath::Clamp(Canvas->ClipY * 0.095f, 78.f, 118.f);
	const FVector2D ActionPos(Canvas->ClipX - ActionSize - 38.f, Canvas->ClipY - ActionSize - 74.f);
	DrawPanel(Canvas, ActionPos, FVector2D(ActionSize, ActionSize), FLinearColor(0.95f, 0.62f, 0.12f, 0.72f));
	DrawPanel(Canvas, ActionPos + FVector2D(6.f, 6.f), FVector2D(ActionSize - 12.f, ActionSize - 12.f), FLinearColor(0.22f, 0.10f, 0.02f, 0.38f));
	DrawScaledText(Canvas, TEXT("ACTION"), ActionPos + FVector2D(12.f, ActionSize * 0.32f), GEngine->GetSmallFont(), FLinearColor::White, 1.05f);
	DrawScaledText(Canvas, TEXT("G"), ActionPos + FVector2D(ActionSize * 0.43f, ActionSize * 0.58f), GEngine->GetSmallFont(), FLinearColor(0.25f, 0.12f, 0.02f), 1.f);

	const float JumpSize = FMath::Clamp(Canvas->ClipY * 0.085f, 70.f, 106.f);
	const FVector2D JumpPos(Canvas->ClipX - (JumpSize * 2.35f) - 52.f, Canvas->ClipY - JumpSize - 42.f);
	DrawPanel(Canvas, JumpPos, FVector2D(JumpSize, JumpSize), FLinearColor(0.12f, 0.48f, 0.78f, 0.70f));
	DrawPanel(Canvas, JumpPos + FVector2D(6.f, 6.f), FVector2D(JumpSize - 12.f, JumpSize - 12.f), FLinearColor(0.02f, 0.10f, 0.18f, 0.38f));
	DrawScaledText(Canvas, TEXT("JUMP"), JumpPos + FVector2D(12.f, JumpSize * 0.36f), GEngine->GetSmallFont(), FLinearColor::White, 1.02f);
	DrawScaledText(Canvas, TEXT("Space"), JumpPos + FVector2D(12.f, JumpSize * 0.62f), GEngine->GetSmallFont(), FLinearColor(0.82f, 0.94f, 1.f), 0.72f);

	if (Character->GetCurrentLevel() == 0)
	{
		const FString Welcome = TEXT("The Neighborhood Hub: complete Daily Tasks for coins, buy Local Store gear for Caverns, and unlock Special Levels with the Special Portal Key.");
		FCanvasTileItem WelcomeBox(FVector2D(16.f, Canvas->ClipY - 116.f), FVector2D(1160.f, 92.f), FLinearColor(0.02f, 0.04f, 0.03f, 0.72f));
		WelcomeBox.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(WelcomeBox);

		FCanvasTextItem WelcomeItem(FVector2D(32.f, Canvas->ClipY - 100.f), FText::FromString(Welcome), GEngine->GetMediumFont(), FLinearColor(0.96f, 0.88f, 0.56f));
		WelcomeItem.EnableShadow(FLinearColor::Black);
		Canvas->DrawItem(WelcomeItem);

		FCanvasTextItem HubHelp(FVector2D(32.f, Canvas->ClipY - 62.f), FText::FromString(TEXT("Main gates: Old Attic, Caverns Entrance, Daily Tasks, Local Gear Store, Special Levels. Caverns require maps plus gear. Special Levels require the Special Portal Key.")), GEngine->GetSmallFont(), FLinearColor(0.82f, 0.95f, 0.88f));
		HubHelp.EnableShadow(FLinearColor::Black);
		Canvas->DrawItem(HubHelp);
	}

	if (UTexture2D* LevelCard = GetLevelCard(Character->GetCurrentLevel()))
	{
		const FVector2D CardSize(260.f, 146.f);
		DrawTextureBox(LevelCard, FVector2D(Canvas->ClipX - CardSize.X - 24.f, 24.f), CardSize, FLinearColor(1.f, 1.f, 1.f, 0.92f));
	}

	if (bPauseOpen)
	{
		FCanvasTileItem PauseBackdrop(FVector2D(0.f, 0.f), FVector2D(Canvas->ClipX, Canvas->ClipY), FLinearColor(0.f, 0.f, 0.f, 0.55f));
		PauseBackdrop.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(PauseBackdrop);

		const FVector2D Center(Canvas->ClipX * 0.5f - 250.f, Canvas->ClipY * 0.5f - 90.f);
		FCanvasTileItem PausePanel(Center, FVector2D(500.f, 180.f), FLinearColor(0.04f, 0.06f, 0.08f, 0.94f));
		PausePanel.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(PausePanel);

		FCanvasTextItem PauseTitle(Center + FVector2D(78.f, 32.f), FText::FromString(TEXT("Paused")), GEngine->GetLargeFont(), FLinearColor(1.f, 0.88f, 0.52f));
		PauseTitle.EnableShadow(FLinearColor::Black);
		Canvas->DrawItem(PauseTitle);

		FCanvasTextItem PauseHelp(Center + FVector2D(78.f, 92.f), FText::FromString(TEXT("Press P to resume, Q or Esc to quit.")), GEngine->GetMediumFont(), FLinearColor::White);
		PauseHelp.EnableShadow(FLinearColor::Black);
		Canvas->DrawItem(PauseHelp);
	}

	DrawDailyLoginRewardNotice();
	DrawHubMenu(TroopPC);
	DrawChatPanel(TroopPC);
	DrawSettingsButton(TroopPC);
	DrawSettingsOverlay(TroopPC);
}


// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Enum.h"
#include "HUD_Main.generated.h"

/**
 * 
 */
UCLASS()
class TIC_TAC_TOE_API AHUD_Main : public AHUD
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	class UUserWidget_HUD* pWidget_HUD;
	class UUserWidget_Login* pWidget_Login;
	class UUserWidget_Register* pWidget_Register;
	class UUserWidget_Hall* pWidget_Hall;
	class UUserWidget_Room* pWidget_Room;

public:
	FORCEINLINE class UUserWidget_HUD* GetHUDWidget() const { return pWidget_HUD; }
	FORCEINLINE class UUserWidget_Hall* GetHallWidget() const { return pWidget_Hall; }
	FORCEINLINE class UUserWidget_Room* GetRoomWidget() const { return pWidget_Room; }
	void ShowMessage(const FString Message, const EMessageType MessageType);

	void ToRegister();
	void ToLogin();
	void ToHall(const FString username, const FString email, TSharedPtr<class FJsonObject> rooms);
	void ToRoom();
	void GameStarted(const bool current, const FString team);
	void GameDone(const bool win);
};

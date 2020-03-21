// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UserWidget_Room.generated.h"

/**
 * 
 */
UCLASS()
class TIC_TAC_TOE_API UUserWidget_Room : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UUserWidget_Room(const FObjectInitializer& objectInitializer);

protected:
	virtual void NativeConstruct() override;

	TSubclassOf<class UUserWidget_RoomSlot> class_RoomSlot;
	class UGameInstance_Main* pGameInstance;
	class AGameModeBase_Game* pGameMode;

	UFUNCTION(BlueprintCallable)
		void SendMessage(const FString message);
	UFUNCTION(BlueprintCallable)
		void Ready();
	UFUNCTION(BlueprintCallable)
		void Start();

	UPROPERTY(BlueprintReadOnly)
		FText RoomTitle;
	UPROPERTY(BlueprintReadOnly)
		bool bMaster;
	UPROPERTY(BlueprintReadOnly)
		bool bReady;

	int32 nRoomID;

public:
	void SetInfo(const TSharedPtr<class FJsonObject> data);
	void AddNewPlayer(const TSharedPtr<class FJsonObject> data);
	void ReceiveMessage(const TSharedPtr<class FJsonObject> data);
	void UpdateReady(const TSharedPtr<class FJsonObject> data);
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UserWidget_Hall.generated.h"

/**
 * 
 */
UCLASS()
class TIC_TAC_TOE_API UUserWidget_Hall : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UUserWidget_Hall(const FObjectInitializer& objectInitializer);

protected:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
		void CreateRoom();
	UFUNCTION(BlueprintCallable)
		void JoinRoom();

	class UGameInstance_Main* pGameInstance;

	UPROPERTY(BlueprintReadOnly)
		FText Username;
	UPROPERTY(BlueprintReadOnly)
		FText EMail;

	int32 nSelectedRoomID;
	TSubclassOf<class UUserWidget_HallSlot> class_HallSlot;

public:
	void SetInfo(const FString username, const FString email, TSharedPtr<class FJsonObject> rooms);
	void ClearRooms();
	void InitializeRooms(const TMap<FString, TSharedPtr<class FJsonValue>> data);
	void AddNewRoom(const TSharedPtr<class FJsonObject> data);
	void UpdateRoomPlayerCount(const int32 nRoomID);
	void UnhoverLastHallSlot();

	FORCEINLINE void SetSelectedRoomID(const int32 roomID) { nSelectedRoomID = roomID; }
	FORCEINLINE int32 GetSelectedRoomID() const { return nSelectedRoomID; }
};

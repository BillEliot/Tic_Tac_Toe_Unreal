// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UserWidget_HallSlot.generated.h"

/**
 * 
 */
UCLASS()
class TIC_TAC_TOE_API UUserWidget_HallSlot : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	UPROPERTY(BlueprintReadOnly)
		FText ID;
	UPROPERTY(BlueprintReadOnly)
		FText RoomTitle;
	UPROPERTY(BlueprintReadOnly)
		FText PlayerCount;
	UPROPERTY(BlueprintReadOnly)
		FText MasterUsername;

	int32 nRoomID;
	int32 nPlayerCount;

	UFUNCTION(BlueprintCallable)
		bool CanUnhover();

public:
	void SetInfo(const int32 id, const FString roomTitle, const int32 playerCount, const FString masterUsername);
	int32 GetRoomID() const { return nRoomID; }
	void UpdatePlayerCount();

	UFUNCTION(BlueprintImplementableEvent)
		void Unhover();
};

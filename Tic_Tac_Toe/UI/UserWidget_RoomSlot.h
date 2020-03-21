// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UserWidget_RoomSlot.generated.h"

/**
 * 
 */
UCLASS()
class TIC_TAC_TOE_API UUserWidget_RoomSlot : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(BlueprintReadOnly)
		bool bMaster;
	UPROPERTY(BlueprintReadOnly)
		bool bReady;
	UPROPERTY(BlueprintReadOnly)
		FText Username;

	int32 nUID;

public:
	FORCEINLINE int32 GetUID() const { return nUID; }
	void SetInfo(const int32 uid, const bool master, const bool ready, const FString username);
	void UpdateReady(const bool ready);
};

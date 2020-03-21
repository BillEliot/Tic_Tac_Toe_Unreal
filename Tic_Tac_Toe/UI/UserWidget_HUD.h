// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Enum.h"
#include "UserWidget_HUD.generated.h"

/**
 * 
 */
UCLASS()
class TIC_TAC_TOE_API UUserWidget_HUD : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(BlueprintReadOnly)
		FText Message;
	UPROPERTY(BlueprintReadOnly)
		EMessageType MessageType;
	UPROPERTY(BlueprintReadOnly)
		bool bCurrent;
	UPROPERTY(BlueprintReadOnly)
		FString Team;

	UFUNCTION(BlueprintCallable)
		void SendMessage(const FString msg);
	UFUNCTION(BlueprintCallable)
		void Back();

	FTimerHandle TimerHandle_Delay;
	UFUNCTION(BlueprintImplementableEvent)
		void Anim_ShowMessage();
	UFUNCTION(BlueprintImplementableEvent)
		void Anim_HideMessage();

public:
	void ShowMessage(const FString message, const EMessageType messageType);
	void ReceiveMessage(const TSharedPtr<class FJsonObject> data);

	FORCEINLINE void SetIsCurrent(const bool current) { bCurrent = current; }
	FORCEINLINE void SetTeam(const FString team) { Team = team; }
};

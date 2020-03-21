// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/TimelineComponent.h"
#include "Pawn_Game.generated.h"

UCLASS()
class TIC_TAC_TOE_API APawn_Game : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APawn_Game();
	~APawn_Game() {
		delete pTimeline;
	}

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	class UCameraComponent* pCameraCom;

	class AGameModeBase_Game* pGameMode;
	class UGameInstance_Main* pGameInstance;

	class UCurveFloat* pCurve;
	FTimeline* pTimeline;
	UFUNCTION()
		void TimelineFun(const float value);
	FVector OriginalLocation;
	FVector DesiredLocation;
	FRotator OriginalRotation;
	FRotator DesiredRotation;

	void Click();

public:
	void GameStarted();
	void GameBack();
};

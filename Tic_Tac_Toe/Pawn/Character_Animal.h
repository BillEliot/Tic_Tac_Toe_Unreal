// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Character_Animal.generated.h"

UCLASS()
class TIC_TAC_TOE_API ACharacter_Animal : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACharacter_Animal();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UAnimMontage* pAnimMontage_Wolf;
	UAnimMontage* pAnimMontage_Fox;
	UAnimMontage* pAnimMontage_DeerDoe;
	UAnimMontage* pAnimMontage_Dragon;

public:
	void Action();
};

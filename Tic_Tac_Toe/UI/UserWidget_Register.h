// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UserWidget_Register.generated.h"

/**
 * 
 */
UCLASS()
class TIC_TAC_TOE_API UUserWidget_Register : public UUserWidget
{
	GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
        void Register();
    UFUNCTION(BlueprintCallable)
        void ToLogin();
};

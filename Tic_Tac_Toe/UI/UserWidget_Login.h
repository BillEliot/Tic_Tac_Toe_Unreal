// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UserWidget_Login.generated.h"

/**
 * 
 */
UCLASS()
class TIC_TAC_TOE_API UUserWidget_Login : public UUserWidget
{
	GENERATED_BODY()


protected:
    UFUNCTION(BlueprintCallable)
        void Login();
    UFUNCTION(BlueprintCallable)
        void ToRegister();
};

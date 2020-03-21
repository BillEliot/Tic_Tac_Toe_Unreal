// Fill out your copyright notice in the Description page of Project Settings.


#include "UserWidget_Login.h"
#include "GameInstance_Main.h"
#include "Components/EditableTextBox.h"
#include "HUD_Main.h"
#include "UI/UserWidget_HUD.h"


void UUserWidget_Login::Login() {
	FString Username = Cast<UEditableTextBox>(GetWidgetFromName("EditableTextBox_Username"))->GetText().ToString();
	FString Password = Cast<UEditableTextBox>(GetWidgetFromName("EditableTextBox_Password"))->GetText().ToString();

	Username.RemoveSpacesInline();
	Password.RemoveSpacesInline();

	if (!Username.IsEmpty()) {
		if (!Password.IsEmpty()) {
			Cast<UGameInstance_Main>(GetGameInstance())->Login(Username, Password);
		}
		else Cast<UGameInstance_Main>(GetGameInstance())->ShowMessage(UTF8_TO_TCHAR("密码不能为空"), EMessageType::E_ERROR);
	}
	else Cast<UGameInstance_Main>(GetGameInstance())->ShowMessage(UTF8_TO_TCHAR("用户名不能为空"), EMessageType::E_ERROR);
}

void UUserWidget_Login::ToRegister() {
	Cast<AHUD_Main>(GetWorld()->GetFirstPlayerController()->GetHUD())->ToRegister();
}

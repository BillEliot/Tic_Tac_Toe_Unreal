// Fill out your copyright notice in the Description page of Project Settings.


#include "UserWidget_Register.h"
#include "GameInstance_Main.h"
#include "Components/EditableTextBox.h"
#include "HUD_Main.h"
#include "UI/UserWidget_HUD.h"

void UUserWidget_Register::Register() {
	FString Username = Cast<UEditableTextBox>(GetWidgetFromName("EditableTextBox_Username"))->GetText().ToString();
	FString Password = Cast<UEditableTextBox>(GetWidgetFromName("EditableTextBox_Password"))->GetText().ToString();
	FString Password2 = Cast<UEditableTextBox>(GetWidgetFromName("EditableTextBox_Password2"))->GetText().ToString();
	FString EMail = Cast<UEditableTextBox>(GetWidgetFromName("EditableTextBox_EMail"))->GetText().ToString();

	Username.RemoveSpacesInline();
	Password.RemoveSpacesInline();
	Password2.RemoveSpacesInline();
	EMail.RemoveSpacesInline();

	if (!Username.IsEmpty()) {
		if (!EMail.IsEmpty()) {
			if (!Password.IsEmpty() && !Password2.IsEmpty()) {
				if (Password == Password2) Cast<UGameInstance_Main>(GetGameInstance())->Register(Username, Password, EMail);
				else Cast<UGameInstance_Main>(GetGameInstance())->ShowMessage(UTF8_TO_TCHAR("输入密码不同"), EMessageType::E_ERROR);
			}
			else Cast<UGameInstance_Main>(GetGameInstance())->ShowMessage(UTF8_TO_TCHAR("密码不能为空"), EMessageType::E_ERROR);
		}
		else Cast<UGameInstance_Main>(GetGameInstance())->ShowMessage(UTF8_TO_TCHAR("EMail不能为空"), EMessageType::E_ERROR);
	}
	else Cast<UGameInstance_Main>(GetGameInstance())->ShowMessage(UTF8_TO_TCHAR("用户名不能为空"), EMessageType::E_ERROR);
}

void UUserWidget_Register::ToLogin() {
	Cast<AHUD_Main>(GetWorld()->GetFirstPlayerController()->GetHUD())->ToLogin();
}

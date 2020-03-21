// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD_Main.h"
#include "UI/UserWidget_HUD.h"
#include "UI/UserWidget_Login.h"
#include "UI/UserWidget_Register.h"
#include "UI/UserWidget_Hall.h"
#include "UI/UserWidget_Room.h"
#include "Components/MultiLineEditableTextBox.h"


void AHUD_Main::BeginPlay() {
	Super::BeginPlay();

	TSubclassOf<UUserWidget_HUD> class_HUD = LoadClass<UUserWidget_HUD>(this, TEXT("WidgetBlueprint'/Game/UI/UW_HUD.UW_HUD_C'"));
	if (class_HUD) {
		pWidget_HUD = CreateWidget<UUserWidget_HUD>(GetWorld(), class_HUD);
		pWidget_HUD->AddToViewport(99);
	}
	TSubclassOf<UUserWidget_Login> class_Login = LoadClass<UUserWidget_Login>(this, TEXT("WidgetBlueprint'/Game/UI/UW_Login.UW_Login_C'"));
	if (class_Login) {
		pWidget_Login = CreateWidget<UUserWidget_Login>(GetWorld(), class_Login);
		pWidget_Login->AddToViewport();
	}
	TSubclassOf<UUserWidget_Register> class_Register = LoadClass<UUserWidget_Register>(this, TEXT("WidgetBlueprint'/Game/UI/UW_Register.UW_Register_C'"));
	if (class_Register) {
		pWidget_Register = CreateWidget<UUserWidget_Register>(GetWorld(), class_Register);
		pWidget_Register->AddToViewport();
		pWidget_Register->SetVisibility(ESlateVisibility::Hidden);
	}
	TSubclassOf<UUserWidget_Hall> class_Hall = LoadClass<UUserWidget_Hall>(this, TEXT("WidgetBlueprint'/Game/UI/UW_Hall.UW_Hall_C'"));
	if (class_Hall) {
		pWidget_Hall = CreateWidget<UUserWidget_Hall>(GetWorld(), class_Hall);
		pWidget_Hall->AddToViewport();
		pWidget_Hall->SetVisibility(ESlateVisibility::Hidden);
	}
	TSubclassOf<UUserWidget_Room> class_Room = LoadClass<UUserWidget_Room>(this, TEXT("WidgetBlueprint'/Game/UI/UW_Room.UW_Room_C'"));
	if (class_Room) {
		pWidget_Room = CreateWidget<UUserWidget_Room>(GetWorld(), class_Room);
		pWidget_Room->AddToViewport();
		pWidget_Room->SetVisibility(ESlateVisibility::Hidden);
	}
}

void AHUD_Main::ShowMessage(const FString Message, const EMessageType MessageType) {
	pWidget_HUD->ShowMessage(Message, MessageType);
}

void AHUD_Main::ToRegister() {
	pWidget_Login->SetVisibility(ESlateVisibility::Hidden);
	pWidget_Register->SetVisibility(ESlateVisibility::Visible);
	pWidget_Hall->SetVisibility(ESlateVisibility::Hidden);
	pWidget_Room->SetVisibility(ESlateVisibility::Hidden);
}
void AHUD_Main::ToLogin() {
	pWidget_Login->SetVisibility(ESlateVisibility::Visible);
	pWidget_Register->SetVisibility(ESlateVisibility::Hidden);
	pWidget_Hall->SetVisibility(ESlateVisibility::Hidden);
	pWidget_Room->SetVisibility(ESlateVisibility::Hidden);
}
void AHUD_Main::ToHall(const FString username, const FString email, TSharedPtr<FJsonObject> rooms) {
	pWidget_Login->SetVisibility(ESlateVisibility::Hidden);
	pWidget_Register->SetVisibility(ESlateVisibility::Hidden);
	pWidget_Hall->SetVisibility(ESlateVisibility::Visible);
	pWidget_Room->SetVisibility(ESlateVisibility::Hidden);

	pWidget_Hall->SetInfo(username, email, rooms);
}
void AHUD_Main::ToRoom() {
	pWidget_Login->SetVisibility(ESlateVisibility::Hidden);
	pWidget_Register->SetVisibility(ESlateVisibility::Hidden);
	pWidget_Hall->SetVisibility(ESlateVisibility::Hidden);
	pWidget_Room->SetVisibility(ESlateVisibility::Visible);

	pWidget_HUD->GetWidgetFromName("TextBlock_Current")->SetVisibility(ESlateVisibility::Hidden);
	pWidget_HUD->GetWidgetFromName("TextBlock_Team")->SetVisibility(ESlateVisibility::Hidden);
	pWidget_HUD->GetWidgetFromName("VerticalBox_Message")->SetVisibility(ESlateVisibility::Hidden);
	pWidget_HUD->GetWidgetFromName("Button_Back")->SetVisibility(ESlateVisibility::Hidden);

	Cast<UMultiLineEditableTextBox>(pWidget_HUD->GetWidgetFromName("MultiLineEditableTextBox_Message"))->SetText(
		Cast<UMultiLineEditableTextBox>(pWidget_Room->GetWidgetFromName("MultiLineEditableTextBox_Message"))->GetText()
	);
}

void AHUD_Main::GameStarted(const bool current, const FString team) {
	pWidget_Login->SetVisibility(ESlateVisibility::Hidden);
	pWidget_Register->SetVisibility(ESlateVisibility::Hidden);
	pWidget_Hall->SetVisibility(ESlateVisibility::Hidden);
	pWidget_Room->SetVisibility(ESlateVisibility::Hidden);

	pWidget_HUD->GetWidgetFromName("TextBlock_Current")->SetVisibility(ESlateVisibility::Visible);
	pWidget_HUD->GetWidgetFromName("TextBlock_Team")->SetVisibility(ESlateVisibility::Visible);
	pWidget_HUD->GetWidgetFromName("VerticalBox_Message")->SetVisibility(ESlateVisibility::Visible);
	pWidget_HUD->SetIsCurrent(current);
	pWidget_HUD->SetTeam(team);
}

void AHUD_Main::GameDone(const bool win) {
	if (win) pWidget_HUD->ShowMessage(UTF8_TO_TCHAR("恭喜获胜"), EMessageType::E_SUCCESS);
	else pWidget_HUD->ShowMessage(UTF8_TO_TCHAR("胜败乃兵家常事，大侠请重新来过"), EMessageType::E_SUCCESS);

	pWidget_HUD->GetWidgetFromName("Button_Back")->SetVisibility(ESlateVisibility::Visible);
}

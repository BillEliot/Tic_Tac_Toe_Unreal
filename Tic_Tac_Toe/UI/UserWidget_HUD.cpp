// Fill out your copyright notice in the Description page of Project Settings.


#include "UserWidget_HUD.h"
#include "Components/WidgetSwitcher.h"
#include "Components/MultiLineEditableTextBox.h"
#include "TimerManager.h"
#include "HUD_Main.h"
#include "GameInstance_Main.h"
#include "GameMode/GameModeBase_Game.h"
#include "Pawn/Pawn_Game.h"
#include "Dom/JsonObject.h"


void UUserWidget_HUD::ShowMessage(const FString message, const EMessageType messageType) {
	Message = FText::FromString(message);
	MessageType = messageType;
	Anim_ShowMessage();

	GetWorld()->GetTimerManager().SetTimer(TimerHandle_Delay, [this]() {
		Anim_HideMessage();
	}, 1.f, false, 2.f);
}

void UUserWidget_HUD::SendMessage(const FString msg) {
	if (!msg.IsEmpty()) Cast<UGameInstance_Main>(GetGameInstance())->SendMessage(Cast<AGameModeBase_Game>(GetWorld()->GetAuthGameMode())->GetRoomID(), msg);
}
void UUserWidget_HUD::ReceiveMessage(const TSharedPtr<class FJsonObject> data) {
	FString username = data->GetStringField("username");
	FString message = data->GetStringField("message");

	UMultiLineEditableTextBox* pMessageBox = Cast<UMultiLineEditableTextBox>(GetWidgetFromName("MultiLineEditableTextBox_Message"));
	if (pMessageBox->GetText().ToString().IsEmpty()) pMessageBox->SetText(FText::FromString(username + ":" + message));
	else pMessageBox->SetText(FText::FromString(pMessageBox->GetText().ToString() + "\n" + username + ":" + message));
}

void UUserWidget_HUD::Back() {
	Cast<AHUD_Main>(GetWorld()->GetFirstPlayerController()->GetHUD())->ToRoom();
	Cast<APawn_Game>(GetWorld()->GetFirstPlayerController()->GetPawn())->GameBack();
}

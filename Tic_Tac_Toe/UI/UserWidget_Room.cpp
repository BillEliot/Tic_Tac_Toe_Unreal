// Fill out your copyright notice in the Description page of Project Settings.


#include "UserWidget_Room.h"
#include "HUD_Main.h"
#include "UI/UserWidget_Hall.h"
#include "UI/UserWidget_RoomSlot.h"
#include "Dom/JsonObject.h"
#include "Components/VerticalBox.h"
#include "Components/MultiLineEditableTextBox.h"
#include "GameMode/GameModeBase_Game.h"
#include "GameInstance_Main.h"
#include "UObject/ConstructorHelpers.h"

UUserWidget_Room::UUserWidget_Room(const FObjectInitializer& objectInitializer) : 
	Super(objectInitializer),
	pGameInstance(nullptr),
	bMaster(false),
	bReady(false),
	nRoomID(-1)
{
	static ConstructorHelpers::FClassFinder<UUserWidget_RoomSlot> class_Widget_RoomSlot(TEXT("WidgetBlueprint'/Game/UI/UW_RoomSlot.UW_RoomSlot_C'"));
	if (class_Widget_RoomSlot.Succeeded()) class_RoomSlot = class_Widget_RoomSlot.Class;
}

void UUserWidget_Room::NativeConstruct() {
	pGameInstance = Cast<UGameInstance_Main>(GetGameInstance());
	pGameMode = Cast<AGameModeBase_Game>(GetWorld()->GetAuthGameMode());
}

void UUserWidget_Room::SetInfo(const TSharedPtr<FJsonObject> data) {
	RoomTitle = FText::FromString(data->GetObjectField("data")->GetStringField("title"));
	nRoomID = data->GetIntegerField("room_id");
	
	pGameMode->SetRoomID(nRoomID);
	pGameMode->SetTeam(data->GetStringField("team"));
	// Team A
	TArray<TSharedPtr<FJsonValue>> Array_TeamA = data->GetObjectField("data")->GetArrayField("teamA");
	for (auto player : Array_TeamA) {
		UUserWidget_RoomSlot* pWidget_RoomSlot = CreateWidget<UUserWidget_RoomSlot>(this, class_RoomSlot);
		if (pWidget_RoomSlot) {
			pWidget_RoomSlot->SetInfo(player->AsObject()->GetIntegerField("uid") ,player->AsObject()->GetBoolField("bMaster"), player->AsObject()->GetBoolField("bReady"), player->AsObject()->GetStringField("username"));
			Cast<UVerticalBox>(GetWidgetFromName("VerticalBox_TeamA"))->AddChild(pWidget_RoomSlot);
		}
		if (player->AsObject()->GetBoolField("bMaster") && player->AsObject()->GetIntegerField("uid") == pGameInstance->GetUID()) {
			bMaster = true;
			pGameMode->SetIsMaster(true);
		}
	}
	// Team B
	TArray<TSharedPtr<FJsonValue>> Array_TeamB = data->GetObjectField("data")->GetArrayField("teamB");
	for (auto player : Array_TeamB) {
		UUserWidget_RoomSlot* pWidget_RoomSlot = CreateWidget<UUserWidget_RoomSlot>(this, class_RoomSlot);
		if (pWidget_RoomSlot) {
			pWidget_RoomSlot->SetInfo(player->AsObject()->GetIntegerField("uid"), player->AsObject()->GetBoolField("bMaster"), player->AsObject()->GetBoolField("bReady"), player->AsObject()->GetStringField("username"));
			Cast<UVerticalBox>(GetWidgetFromName("VerticalBox_TeamB"))->AddChild(pWidget_RoomSlot);
		}
		if (player->AsObject()->GetBoolField("bMaster") && player->AsObject()->GetIntegerField("uid") == pGameInstance->GetUID()) {
			bMaster = true;
			pGameMode->SetIsMaster(true);
		}
	}
	// Message
	UMultiLineEditableTextBox* pMessageBox = Cast<UMultiLineEditableTextBox>(GetWidgetFromName("MultiLineEditableTextBox_Message"));
	pMessageBox->SetText(FText::FromString(UTF8_TO_TCHAR("系统消息：欢迎来到") + RoomTitle.ToString()));
	// Navigate
	AHUD_Main* pHUD = Cast<AHUD_Main>(GetWorld()->GetFirstPlayerController()->GetHUD());
	if (pHUD) {
		pHUD->GetHallWidget()->GetWidgetFromName("Throbber_Loading")->SetVisibility(ESlateVisibility::Hidden);
		pHUD->ToRoom();
	}
}

void UUserWidget_Room::AddNewPlayer(const TSharedPtr<class FJsonObject> data) {
	UUserWidget_RoomSlot* pWidget_RoomSlot = CreateWidget<UUserWidget_RoomSlot>(this, class_RoomSlot);
	if (pWidget_RoomSlot) {
		pWidget_RoomSlot->SetInfo(data->GetIntegerField("uid"), false, false, data->GetStringField("username"));
		if (data->GetStringField("team") == "A") Cast<UVerticalBox>(GetWidgetFromName("VerticalBox_TeamA"))->AddChild(pWidget_RoomSlot);
		else if (data->GetStringField("team") == "B") Cast<UVerticalBox>(GetWidgetFromName("VerticalBox_TeamB"))->AddChild(pWidget_RoomSlot);
	}
}

void UUserWidget_Room::SendMessage(const FString message) {
	if (!message.IsEmpty()) pGameInstance->SendMessage(nRoomID, message);
}
void UUserWidget_Room::ReceiveMessage(const TSharedPtr<class FJsonObject> data) {
	FString username = data->GetStringField("username");
	FString message = data->GetStringField("message");

	UMultiLineEditableTextBox* pMessageBox = Cast<UMultiLineEditableTextBox>(GetWidgetFromName("MultiLineEditableTextBox_Message"));
	if (pMessageBox->GetText().ToString().IsEmpty()) pMessageBox->SetText(FText::FromString(username + ":" + message));
	else pMessageBox->SetText(FText::FromString(pMessageBox->GetText().ToString() + "\n" + username + ":" + message));
}

void UUserWidget_Room::Ready() {
	if (pGameInstance) pGameInstance->Ready(nRoomID);
}

void UUserWidget_Room::Start() {
	if (pGameInstance) pGameInstance->Start(nRoomID);
}

void UUserWidget_Room::UpdateReady(const TSharedPtr<class FJsonObject> data) {
	UVerticalBox* pUVerticalBox_A = Cast<UVerticalBox>(GetWidgetFromName("VerticalBox_TeamA"));
	UVerticalBox* pUVerticalBox_B = Cast<UVerticalBox>(GetWidgetFromName("VerticalBox_TeamB"));

	bool bFound = false;
	for (auto roomSlot : pUVerticalBox_A->GetAllChildren()) {
		UUserWidget_RoomSlot* pRoomSlot = Cast<UUserWidget_RoomSlot>(roomSlot);
		if (pRoomSlot && pRoomSlot->GetUID() == data->GetIntegerField("uid")) {
			pRoomSlot->UpdateReady(data->GetBoolField("ready"));
			bFound = true;
			break;
		}
	}
	if (!bFound) {
		for (auto roomSlot : pUVerticalBox_B->GetAllChildren()) {
			UUserWidget_RoomSlot* pRoomSlot = Cast<UUserWidget_RoomSlot>(roomSlot);
			if (pRoomSlot && pRoomSlot->GetUID() == data->GetIntegerField("uid")) {
				pRoomSlot->UpdateReady(data->GetBoolField("ready"));
				break;
			}
		}
	}

	if (data->GetIntegerField("uid") == pGameInstance->GetUID()) bReady = data->GetBoolField("ready");
}

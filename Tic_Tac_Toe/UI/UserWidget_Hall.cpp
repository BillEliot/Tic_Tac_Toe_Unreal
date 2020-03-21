// Fill out your copyright notice in the Description page of Project Settings.


#include "UserWidget_Hall.h"
#include "UI/UserWidget_HallSlot.h"
#include "Components/EditableTextBox.h"
#include "Components/VerticalBox.h"
#include "GameInstance_Main.h"
#include "UObject/ConstructorHelpers.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"

UUserWidget_Hall::UUserWidget_Hall(const FObjectInitializer& objectInitializer) :
	Super(objectInitializer),
	nSelectedRoomID(-1)
{
	static ConstructorHelpers::FClassFinder<UUserWidget_HallSlot> class_Widget_HallSlot(TEXT("WidgetBlueprint'/Game/UI/UW_HallSlot.UW_HallSlot_C'"));
	if (class_Widget_HallSlot.Succeeded()) class_HallSlot = class_Widget_HallSlot.Class;
}

void UUserWidget_Hall::NativeConstruct() {
	Super::NativeConstruct();

	pGameInstance = Cast<UGameInstance_Main>(GetGameInstance());
}

void UUserWidget_Hall::CreateRoom() {
	if (pGameInstance) {
		FString RoomTitle = Cast<UEditableTextBox>(GetWidgetFromName("EditableTextBox_RoomTitle"))->GetText().ToString();
		if (!RoomTitle.IsEmpty()) pGameInstance->CreateRoom(RoomTitle);
		else pGameInstance->ShowMessage(UTF8_TO_TCHAR("房间名不能为空"), EMessageType::E_ERROR);
	}
}

void UUserWidget_Hall::JoinRoom() {
	if (pGameInstance) {
		if (nSelectedRoomID != -1) pGameInstance->JoinRoom(nSelectedRoomID);
		else pGameInstance->ShowMessage(UTF8_TO_TCHAR("请先选择一个房间"), EMessageType::E_ERROR);
	}
}

void UUserWidget_Hall::SetInfo(const FString username, const FString email, TSharedPtr<FJsonObject> rooms) {
	Username = FText::FromString(username);
	EMail = FText::FromString(email);
	// Rooms
	InitializeRooms(rooms->Values);
}

void UUserWidget_Hall::UnhoverLastHallSlot() {
	UVerticalBox* pUVerticalBox = Cast<UVerticalBox>(GetWidgetFromName("VerticalBox_Rooms"));
	if (nSelectedRoomID != -1) {
		for (auto widget : pUVerticalBox->GetAllChildren()) {
			UUserWidget_HallSlot* pHallSlot = Cast<UUserWidget_HallSlot>(widget);
			if (pHallSlot && pHallSlot->GetRoomID() == nSelectedRoomID) {
				pHallSlot->Unhover();
				break;
			}
		}
	}
}

void UUserWidget_Hall::ClearRooms() {
	Cast<UVerticalBox>(GetWidgetFromName("VerticalBox_Rooms"))->ClearChildren();
}

void UUserWidget_Hall::AddNewRoom(const TSharedPtr<FJsonObject> roomJson) {
	UUserWidget_HallSlot* pWidget_HallSlot = CreateWidget<UUserWidget_HallSlot>(this, class_HallSlot);
	if (pWidget_HallSlot) {
		int32 nPlayerCount = roomJson->GetObjectField("data")->GetArrayField("teamA").Num() + roomJson->GetObjectField("data")->GetArrayField("teamB").Num();
		pWidget_HallSlot->SetInfo(roomJson->GetIntegerField("room_id"), roomJson->GetObjectField("data")->GetStringField("title"), nPlayerCount, roomJson->GetObjectField("data")->GetStringField("master_username"));
		UVerticalBox* pVerticalBox = Cast<UVerticalBox>(GetWidgetFromName("VerticalBox_Rooms"));
		if (pVerticalBox) pVerticalBox->AddChild(pWidget_HallSlot);
	}
}
void UUserWidget_Hall::InitializeRooms(const TMap<FString, TSharedPtr<class FJsonValue>> data) {
	for (auto room : data) {
		UUserWidget_HallSlot* pWidget_HallSlot = CreateWidget<UUserWidget_HallSlot>(this, class_HallSlot);
		if (pWidget_HallSlot) {
			int32 nPlayerCount = room.Value->AsObject()->GetArrayField("teamA").Num() + room.Value->AsObject()->GetArrayField("teamB").Num();
			pWidget_HallSlot->SetInfo(FCString::Atoi(*room.Key), room.Value->AsObject()->GetStringField("title"), nPlayerCount, room.Value->AsObject()->GetStringField("master_username"));
			UVerticalBox* pVerticalBox = Cast<UVerticalBox>(GetWidgetFromName("VerticalBox_Rooms"));
			if (pVerticalBox) pVerticalBox->AddChild(pWidget_HallSlot);
		}
	}
}
void UUserWidget_Hall::UpdateRoomPlayerCount(const int32 nRoomID) {
	UVerticalBox* pVerticalBox = Cast<UVerticalBox>(GetWidgetFromName("VerticalBox_Rooms"));
	if (pVerticalBox) {
		for (auto hallSlot : pVerticalBox->GetAllChildren()) {
			UUserWidget_HallSlot* pHallSlot = Cast<UUserWidget_HallSlot>(hallSlot);
			if (pHallSlot && pHallSlot->GetRoomID() == nRoomID) {
				pHallSlot->UpdatePlayerCount();
				return;
			}
		}
	}
}

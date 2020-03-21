// Fill out your copyright notice in the Description page of Project Settings.


#include "UserWidget_HallSlot.h"
#include "HUD_Main.h"
#include "UI/UserWidget_Hall.h"


void UUserWidget_HallSlot::SetInfo(const int32 id, const FString roomTitle, const int32 playerCount, const FString masterUsername) {
	nRoomID = id;
	nPlayerCount = playerCount;

	ID = FText::FromString(FString::FromInt(id));
	RoomTitle = FText::FromString(roomTitle);
	PlayerCount = FText::FromString(FString::FromInt(playerCount) + "/2");
	MasterUsername = FText::FromString(masterUsername);
}

void UUserWidget_HallSlot::UpdatePlayerCount() {
	nPlayerCount++;
	PlayerCount = FText::FromString(FString::FromInt(nPlayerCount) + "/2");
}

FReply UUserWidget_HallSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) {
	Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	Cast<AHUD_Main>(GetWorld()->GetFirstPlayerController()->GetHUD())->GetHallWidget()->UnhoverLastHallSlot();
	Cast<AHUD_Main>(GetWorld()->GetFirstPlayerController()->GetHUD())->GetHallWidget()->SetSelectedRoomID(nRoomID);
	return FReply::Unhandled();
}

bool UUserWidget_HallSlot::CanUnhover() {
	if (nRoomID == Cast<AHUD_Main>(GetWorld()->GetFirstPlayerController()->GetHUD())->GetHallWidget()->GetSelectedRoomID()) return false;
	else return true;
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "UserWidget_RoomSlot.h"


void UUserWidget_RoomSlot::SetInfo(const int32 uid, const bool master, const bool ready, const FString username) {
	nUID = uid;

	bMaster = master;
	bReady = ready;
	Username = FText::FromString(username);
}

void UUserWidget_RoomSlot::UpdateReady(const bool ready) {
	bReady = ready;
}

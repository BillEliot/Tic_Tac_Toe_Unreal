// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModeBase_Game.h"
#include "HUD_Main.h"
#include "UI/UserWidget_HUD.h"
#include "Actor/Actor_Piece.h"
#include "Pawn/Character_Animal.h"
#include "Engine/World.h"
#include "Pawn/Pawn_Game.h"
#include "GameInstance_Main.h"
#include "GameFramework/PlayerController.h"
#include "Dom/JsonObject.h"
#include "EngineUtils.h"

AGameModeBase_Game::AGameModeBase_Game() {
	PrimaryActorTick.bCanEverTick = true;

	HUDClass = AHUD_Main::StaticClass();
	DefaultPawnClass = APawn_Game::StaticClass();
}

void AGameModeBase_Game::BeginPlay() {
	Super::BeginPlay();

	pGameInstance = Cast<UGameInstance_Main>(GetGameInstance());

	pPlayerController = GetWorld()->GetFirstPlayerController();
	FInputModeGameAndUI InputUIMode;
	InputUIMode.SetHideCursorDuringCapture(false);
	pPlayerController->SetInputMode(InputUIMode);
	pPlayerController->bShowMouseCursor = true;
	pPlayerController->SetIgnoreLookInput(true);
	pPlayerController->SetIgnoreMoveInput(true);

	// Get Character Dragon
	for (TActorIterator<ACharacter_Animal> animal(GetWorld()); animal; ++animal) {
		if (animal->ActorHasTag("Dragon")) {
			pDragon = *animal;
			break;
		}
	}
}

void AGameModeBase_Game::Tick(const float DeltaTime) {
	Super::Tick(DeltaTime);

	// Trace
	FVector WorldLocation;
	FVector WorldDirection;
	pPlayerController->DeprojectMousePositionToWorld(WorldLocation, WorldDirection);

	FVector Location = pPlayerController->PlayerCameraManager->GetCameraLocation();
	FVector Normal = WorldLocation - pPlayerController->PlayerCameraManager->GetCameraLocation();

	FHitResult HitResult;
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Location, Location + Normal * 1000.f, ECollisionChannel::ECC_Visibility);
	if (bHit) {
		pPiece = Cast<AActor_Piece>(HitResult.GetActor());
		pAnimal = Cast<ACharacter_Animal>(HitResult.GetActor());
		if (pPiece) {
			if (pLastPiece && pLastPiece != pPiece) pLastPiece->Hover(false);
			pPiece->Hover();
			pLastPiece = pPiece;
			pAnimal = nullptr;
		}
		else if (pAnimal) {
			if (pLastPiece) pLastPiece->Hover(false);
			pPiece = nullptr;
			pLastPiece = nullptr;
		}
		else {
			if (pLastPiece) pLastPiece->Hover(false);
			pAnimal = nullptr;
			pPiece = nullptr;
			pLastPiece = nullptr;
		}
	}
	else {
		if (pLastPiece) pLastPiece->Hover(false);
		pAnimal = nullptr;
		pPiece = nullptr;
		pLastPiece = nullptr;
	}
}

void AGameModeBase_Game::GameStarted(const bool current) {
	bCurrent = current;

	Cast<APawn_Game>(pPlayerController->GetPawn())->GameStarted();
	Cast<AHUD_Main>(GetWorld()->GetFirstPlayerController()->GetHUD())->GameStarted(bCurrent, Team);
	// Set Pieces
	int32 nX = -320.f;
	int32 nY = 300.f;
	for (int32 i = 0; i < 3; ++i) {
		for (int32 j = 0; j < 3; ++j) {
			if (Board[i][j]) Board[i][j]->Restore();
			else {
				AActor_Piece* pSpawnedPiece = GetWorld()->SpawnActor<AActor_Piece>(AActor_Piece::StaticClass(), FVector(nX, nY, 30.f), FRotator(0, 0, 0));
				pSpawnedPiece->SetCoordinate(i, j);
				Board[i][j] = pSpawnedPiece;
			}
			nY -= 310.f;
		}
		nY = 300.f;
		nX += 310.f;
	}
}

void AGameModeBase_Game::GameDone(const TSharedPtr<class FJsonObject> data) {
	if (data->GetIntegerField("uid") == pGameInstance->GetUID()) Cast<AHUD_Main>(pPlayerController->GetHUD())->GameDone(true);
	else Cast<AHUD_Main>(pPlayerController->GetHUD())->GameDone(false);
	bGameDone = true;
}

void AGameModeBase_Game::Play(const TSharedPtr<FJsonObject> data) {
	Board[data->GetIntegerField("x")][data->GetIntegerField("y")]->Click(data->GetStringField("type"));
	if (data->GetIntegerField("current") == pGameInstance->GetUID()) {
		bCurrent = true;
		Cast<AHUD_Main>(pPlayerController->GetHUD())->GetHUDWidget()->SetIsCurrent(true);
	}
	else {
		bCurrent = false;
		Cast<AHUD_Main>(pPlayerController->GetHUD())->GetHUDWidget()->SetIsCurrent(false);
	}
	if (pDragon) pDragon->Action();
}

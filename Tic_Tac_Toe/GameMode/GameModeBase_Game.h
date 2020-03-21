// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameModeBase_Game.generated.h"

/**
 * 
 */
UCLASS()
class TIC_TAC_TOE_API AGameModeBase_Game : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AGameModeBase_Game();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(const float DeltaTime) override;

	class UGameInstance_Main* pGameInstance;
	class APlayerController* pPlayerController;
	class AActor_Piece* pPiece;
	class AActor_Piece* pLastPiece;
	class ACharacter_Animal* pAnimal;

	class AActor_Piece* Board[3][3];
	bool bCurrent;
	bool bGameDone;
	int32 nRoomID;
	FString Team;
	bool bMaster;

	class ACharacter_Animal* pDragon;

public:
	class AActor_Piece* GetCurrentPiece() const { return pPiece; }
	class ACharacter_Animal* GetCurrentAnimal() const { return pAnimal; }

	FORCEINLINE bool GetIsCurrent() const { return bCurrent; }
	FORCEINLINE int32 GetRoomID() const { return nRoomID; }
	FORCEINLINE void SetRoomID(const int32 roomID) { nRoomID = roomID; }
	FORCEINLINE FString GetTeam() const { return Team; }
	FORCEINLINE void SetTeam(const FString team) { Team = team; }
	FORCEINLINE bool GetIsMaster() const { return bMaster; }
	FORCEINLINE void SetIsMaster(const bool master) { bMaster = master; }
	FORCEINLINE bool GetIsGameDone() const { return bGameDone; }
	FORCEINLINE void SetIsGameDone(const bool done) { bGameDone = done; }

	void GameStarted(const bool current);
	void GameDone(const TSharedPtr<class FJsonObject> data);
	void Play(const TSharedPtr<class FJsonObject> data);
};

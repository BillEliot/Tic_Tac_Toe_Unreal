// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawn_Game.h"
#include "Engine/World.h"
#include "HUD_Main.h"
#include "GameInstance_Main.h"
#include "GameMode/GameModeBase_Game.h"
#include "Actor/Actor_Piece.h"
#include "Pawn/Character_Animal.h"
#include "Camera/CameraComponent.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
APawn_Game::APawn_Game()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	pCameraCom = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	pCameraCom->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UCurveFloat> object_Curve(TEXT("CurveFloat'/Game/Blueprint/CurveFloat.CurveFloat'"));
	if (object_Curve.Succeeded()) pCurve = object_Curve.Object;
}

// Called when the game starts or when spawned
void APawn_Game::BeginPlay()
{
	Super::BeginPlay();
	
	pGameInstance = Cast<UGameInstance_Main>(GetGameInstance());
	pGameMode = Cast<AGameModeBase_Game>(GetWorld()->GetAuthGameMode());

	pTimeline = new FTimeline();
	FOnTimelineFloat OnTimelineFloat;
	OnTimelineFloat.BindUFunction(this, "TimelineFun");
	pTimeline->AddInterpFloat(pCurve, OnTimelineFloat);

	SetActorLocation(FVector(-1777.f, -80.f, 1300.f));
	SetActorRotation(FRotator(0, 0, 0));
}

// Called every frame
void APawn_Game::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (pTimeline && pTimeline->IsPlaying()) pTimeline->TickTimeline(DeltaTime);
}

// Called to bind functionality to input
void APawn_Game::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Click", IE_Pressed, this, &APawn_Game::Click);
}

void APawn_Game::Click() {
	AActor_Piece* pPiece = Cast<AGameModeBase_Game>(GetWorld()->GetAuthGameMode())->GetCurrentPiece();
	ACharacter_Animal* pAnimal = Cast<AGameModeBase_Game>(GetWorld()->GetAuthGameMode())->GetCurrentAnimal();

	if (pPiece && !pPiece->GetIsClicked() && pGameMode->GetIsCurrent() && !pGameMode->GetIsGameDone()) {
		int32 x = 0, y = 0;
		pPiece->GetCoordinate(x, y);
		pGameInstance->Play(pGameMode->GetRoomID(), pGameMode->GetTeam(), x, y);
	}
	else if (pAnimal) pAnimal->Action();
}

void APawn_Game::GameStarted() {
	OriginalLocation = GetActorLocation();
	DesiredLocation = FVector(-710.f, 11.f, 771.f);
	OriginalRotation = GetActorRotation();
	DesiredRotation = FRotator(-45.f, 0, 0);
	pTimeline->PlayFromStart();
}
void APawn_Game::GameBack() {
	pTimeline->ReverseFromEnd();
	pGameMode->SetIsGameDone(false);
}

void APawn_Game::TimelineFun(const float value) {
	FVector Location = FMath::Lerp<FVector>(OriginalLocation, DesiredLocation, value);
	FRotator Rotation = FMath::Lerp<FRotator>(OriginalRotation, DesiredRotation, value);

	SetActorLocation(Location);
	SetActorRotation(Rotation);
}

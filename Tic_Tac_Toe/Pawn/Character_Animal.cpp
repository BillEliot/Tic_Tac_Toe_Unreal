// Fill out your copyright notice in the Description page of Project Settings.


#include "Character_Animal.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
ACharacter_Animal::ACharacter_Animal()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	GetMesh()->SetRelativeLocation(FVector(0, 0, -90.f));
	GetMesh()->SetRelativeRotation(FRotator(0, -90.f, 0));

	static ConstructorHelpers::FObjectFinder<UAnimMontage> object_AM_Wolf(TEXT("AnimMontage'/Game/AnimalVarietyPack/Wolf/Animations/AM_Wolf.AM_Wolf'"));
	static ConstructorHelpers::FObjectFinder<UAnimMontage> object_AM_Fox(TEXT("AnimMontage'/Game/AnimalVarietyPack/Fox/Animations/AM_Fox.AM_Fox'"));
	static ConstructorHelpers::FObjectFinder<UAnimMontage> object_AM_DeerDoe(TEXT("AnimMontage'/Game/AnimalVarietyPack/DeerDoe/Animations/AM_DeerDoe.AM_DeerDoe'"));
	static ConstructorHelpers::FObjectFinder<UAnimMontage> object_AM_Dragon(TEXT("AnimMontage'/Game/DesertDragon/Animations/AM_Dragon.AM_Dragon'"));
	if (object_AM_Wolf.Succeeded()) pAnimMontage_Wolf = object_AM_Wolf.Object;
	if (object_AM_Fox.Succeeded()) pAnimMontage_Fox = object_AM_Fox.Object;
	if (object_AM_DeerDoe.Succeeded()) pAnimMontage_DeerDoe = object_AM_DeerDoe.Object;
	if (object_AM_Dragon.Succeeded()) pAnimMontage_Dragon = object_AM_Dragon.Object;
}

// Called when the game starts or when spawned
void ACharacter_Animal::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACharacter_Animal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACharacter_Animal::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ACharacter_Animal::Action() {
	if (!GetCurrentMontage()) {
		if (ActorHasTag("Wolf")) PlayAnimMontage(pAnimMontage_Wolf);
		else if (ActorHasTag("Fox")) PlayAnimMontage(pAnimMontage_Fox);
		else if (ActorHasTag("DeerDoe")) PlayAnimMontage(pAnimMontage_DeerDoe);
		else if (ActorHasTag("Dragon")) PlayAnimMontage(pAnimMontage_Dragon);
	}
}

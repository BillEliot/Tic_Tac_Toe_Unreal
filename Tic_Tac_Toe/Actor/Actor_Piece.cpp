// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor_Piece.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
AActor_Piece::AActor_Piece()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> object_Material_Enabled(TEXT("MaterialInstanceConstant'/Game/Material/IceCool/ExampleMaterials/Environment/MI_IceBall.MI_IceBall'"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> object_Material_Disabled(TEXT("MaterialInstanceConstant'/Game/Material/IceCool/ExampleMaterials/Environment/MI_IceBlockInside.MI_IceBlockInside'"));
	if (object_Material_Enabled.Succeeded()) pMaterial_Enable = object_Material_Enabled.Object;
	if (object_Material_Disabled.Succeeded()) pMaterial_Disable = object_Material_Disabled.Object;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> object_Piece(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> object_Sphere(TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> object_Cone(TEXT("StaticMesh'/Engine/BasicShapes/Cone.Cone'"));
	if (object_Piece.Succeeded()) pStaticMesh_Default = object_Piece.Object;
	if (object_Sphere.Succeeded()) pStaticMesh_Sphere = object_Sphere.Object;
	if (object_Cone.Succeeded()) pStaticMesh_Cone = object_Cone.Object;

	pStaticMeshCom_Piece = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Piece"));
	RootComponent = pStaticMeshCom_Piece;
	pStaticMeshCom_Piece->SetStaticMesh(pStaticMesh_Default);
	pStaticMeshCom_Piece->SetMaterial(0, pMaterial_Disable);
	pStaticMeshCom_Piece->SetWorldScale3D(FVector(2.5f, 2.5f, 0.2f));

	static ConstructorHelpers::FObjectFinder<UCurveFloat> object_Curve(TEXT("CurveFloat'/Game/Blueprint/CurveFloat_Dissolve.CurveFloat_Dissolve'"));
	if (object_Curve.Succeeded()) pCurve = object_Curve.Object;
}

// Called when the game starts or when spawned
void AActor_Piece::BeginPlay()
{
	Super::BeginPlay();
	
	pTimeline_Dissolve = new FTimeline();
	FOnTimelineFloat OnTimelineFloat;
	OnTimelineFloat.BindUFunction(this, "TimelineFun_Dissolve");
	FOnTimelineEvent OnTimelineEvent;
	OnTimelineEvent.BindUFunction(this, "TimelineFinished_Dissolve");
	FOnTimelineEvent OnTimelineEvent_Reverse;
	OnTimelineEvent_Reverse.BindUFunction(this, "TimelineFinished_Dissolve_Reverse");

	pTimeline_Dissolve->AddInterpFloat(pCurve, OnTimelineFloat);
	pTimeline_Dissolve->AddEvent(0.5f, OnTimelineEvent);
	pTimeline_Dissolve->AddEvent(0, OnTimelineEvent_Reverse);

	UMaterialInterface* object_Material = LoadObject<UMaterialInterface>(this, TEXT("Material'/Game/Material/M_DissolveEffect.M_DissolveEffect'"));
	pMaterial_Dissolve = UMaterialInstanceDynamic::Create(object_Material, this);
}

// Called every frame
void AActor_Piece::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (pTimeline_Dissolve && pTimeline_Dissolve->IsPlaying()) pTimeline_Dissolve->TickTimeline(DeltaTime);
}

void AActor_Piece::Hover(bool activate) {
	if (activate) pStaticMeshCom_Piece->SetMaterial(0, pMaterial_Enable);
	else pStaticMeshCom_Piece->SetMaterial(0, pMaterial_Disable);
}

void AActor_Piece::Restore() {
	pStaticMeshCom_Piece->SetStaticMesh(pStaticMesh_Default);
	pStaticMeshCom_Piece->SetWorldScale3D(FVector(2.5f, 2.5f, 0.2f));
}

void AActor_Piece::SetCoordinate(const int32 x, const int32 y) {
	nX = x;
	nY = y;
}
void AActor_Piece::GetCoordinate(int32& x, int32& y) {
	x = nX;
	y = nY;
}

void AActor_Piece::Click(const FString type) {
	Type = type;
	bClicked = true;
	pTimeline_Dissolve->PlayFromStart();
}

void AActor_Piece::TimelineFun_Dissolve(const float value) {
	if (pMaterial_Dissolve) {
		pMaterial_Dissolve->SetScalarParameterValue("amount", value);
		pStaticMeshCom_Piece->SetMaterial(0, pMaterial_Dissolve);
	}
}
void AActor_Piece::TimelineFinished_Dissolve() {
	if (!pTimeline_Dissolve->IsReversing()) {
		if (Type == "correct") pStaticMeshCom_Piece->SetStaticMesh(pStaticMesh_Sphere);
		else pStaticMeshCom_Piece->SetStaticMesh(pStaticMesh_Cone);
		pStaticMeshCom_Piece->SetWorldScale3D(FVector(2.5f, 2.5f, 2.5f));
		pTimeline_Dissolve->ReverseFromEnd();
	}
}
void AActor_Piece::TimelineFinished_Dissolve_Reverse() {
	pStaticMeshCom_Piece->SetMaterial(0, pMaterial_Disable);
}

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"
#include "Actor_Piece.generated.h"

UCLASS()
class TIC_TAC_TOE_API AActor_Piece : public AActor
{
	GENERATED_BODY()

public:
	AActor_Piece();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	class UStaticMeshComponent* pStaticMeshCom_Piece;

	class UStaticMesh* pStaticMesh_Default;
	class UStaticMesh* pStaticMesh_Sphere;
	class UStaticMesh* pStaticMesh_Cone;

	class UMaterialInterface* pMaterial_Enable;
	class UMaterialInterface* pMaterial_Disable;

	UPROPERTY()
		class UMaterialInstanceDynamic* pMaterial_Dissolve;
	class UCurveFloat* pCurve;
	FTimeline* pTimeline_Dissolve;
	UFUNCTION()
		void TimelineFun_Dissolve(const float value);
	UFUNCTION()
		void TimelineFinished_Dissolve();
	UFUNCTION()
		void TimelineFinished_Dissolve_Reverse();

	FString Type;
	int32 nX;
	int32 nY;
	bool bClicked;

public:
	void Hover(bool activate = true);
	void Restore();
	void SetCoordinate(const int32 x, const int32 y);
	void GetCoordinate(int32& x, int32& y);
	void Click(const FString type);

	bool GetIsClicked() const { return bClicked; }
};

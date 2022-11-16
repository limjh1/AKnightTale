// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FloatingPlatform.generated.h"

UCLASS()
class FIRSTPROJECT_API AFloatingPlatform : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFloatingPlatform();

	// Mesh for the platform
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Platform")
	class UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere)
	FVector StartPoint;

	// 끝 지점은 화면에서 편집 가능
	UPROPERTY(EditAnywhere, meta = (MakeEditWidget = "ture"))
	FVector EndPoint;

	// 보간 속도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Platform")
	float InterpSpeed;

	// 시간
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Platform")
	float InterpTime;

	FTimerHandle InterpTImer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Platform")
	bool bInterping;

	float Distance;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void ToggleInterping();

	void SwapVectors(FVector& VecOne, FVector& VecTwo);

};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "FollowEnemyController.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPROJECT_API AFollowEnemyController : public AAIController
{
	GENERATED_BODY()
	
public:
	AFollowEnemyController();

	virtual void OnPossess(APawn* InPawn) override; //빙의
	virtual void OnUnPossess() override; //빙의풀림

private:
	void RandomMove();


private:
	FTimerHandle TimerHandle; // 예약한 일감 취소할 때
};

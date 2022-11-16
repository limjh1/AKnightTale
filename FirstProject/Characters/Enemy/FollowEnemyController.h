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

	virtual void OnPossess(APawn* InPawn) override; //����
	virtual void OnUnPossess() override; //����Ǯ��

private:
	void RandomMove();


private:
	FTimerHandle TimerHandle; // ������ �ϰ� ����� ��
};

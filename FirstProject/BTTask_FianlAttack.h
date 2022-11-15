// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FianlAttack.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPROJECT_API UBTTask_FianlAttack : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_FianlAttack();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	class AEnemy* Enemy;

};

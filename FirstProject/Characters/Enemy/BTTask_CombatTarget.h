// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_CombatTarget.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPROJECT_API UBTTask_CombatTarget : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_CombatTarget();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	class AEnemy* Enemy;
};

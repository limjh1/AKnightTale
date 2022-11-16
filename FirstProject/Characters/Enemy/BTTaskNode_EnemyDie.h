// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskNode_EnemyDie.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPROJECT_API UBTTaskNode_EnemyDie : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTaskNode_EnemyDie();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	class AEnemy* Enemy;
	
};

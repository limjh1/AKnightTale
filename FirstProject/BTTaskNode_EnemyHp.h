// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskNode_EnemyHp.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPROJECT_API UBTTaskNode_EnemyHp : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTaskNode_EnemyHp();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	class AEnemy* Enemy;

};
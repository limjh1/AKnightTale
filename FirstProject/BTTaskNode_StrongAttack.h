// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskNode_StrongAttack.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPROJECT_API UBTTaskNode_StrongAttack : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTaskNode_StrongAttack();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	class AEnemy* Enemy;

};
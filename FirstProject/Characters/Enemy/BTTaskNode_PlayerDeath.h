// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskNode_PlayerDeath.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPROJECT_API UBTTaskNode_PlayerDeath : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTaskNode_PlayerDeath();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	class AEnemy* Enemy;

	class AMain* Main;

};
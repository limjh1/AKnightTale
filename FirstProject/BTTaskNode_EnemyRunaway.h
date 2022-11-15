// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskNode_EnemyRunaway.generated.h"

/**
 * 
 */
UCLASS()
class FIRSTPROJECT_API UBTTaskNode_EnemyRunaway : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTaskNode_EnemyRunaway();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	class AEnemy* Enemy;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Controller")
	class AMainPlayerController* MainPlayerController;
};

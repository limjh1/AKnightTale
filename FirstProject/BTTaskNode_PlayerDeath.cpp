// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_PlayerDeath.h"
#include "FollowEnemyController.h"
#include "NavigationSystem.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemy.h"


UBTTaskNode_PlayerDeath::UBTTaskNode_PlayerDeath()
{
	NodeName = TEXT("PlayerHpIsZero"); // 행동트리에서 볼 수 있는 이름
}

EBTNodeResult::Type UBTTaskNode_PlayerDeath::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);


	auto CurrentPawn = OwnerComp.GetAIOwner()->GetPawn();	

	if (CurrentPawn == nullptr)
		return EBTNodeResult::Failed;

	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());

	if (NavSystem == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	FNavLocation RandomLocation;

	return EBTNodeResult::Failed;
}
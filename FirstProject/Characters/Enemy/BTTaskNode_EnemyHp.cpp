// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_EnemyHp.h"
#include "FollowEnemyController.h"
#include "NavigationSystem.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemy.h"


UBTTaskNode_EnemyHp::UBTTaskNode_EnemyHp()
{
	NodeName = TEXT("EnemyHpIsNotZero"); // 행동트리에서 볼 수 있는 이름
}

EBTNodeResult::Type UBTTaskNode_EnemyHp::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

	AEnemy* Enemy1 = Cast<AEnemy>(CurrentPawn);

	if (Enemy1->Health > 0.f)
	{
		Enemy1->bEnemyIsDie = true;
		//UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, RandomLocation);		
		//OwnerComp.GetBlackboardComponent()->SetValueAsVector(FName(TEXT("PatrolLocation")), RandomLocation.Location);
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}

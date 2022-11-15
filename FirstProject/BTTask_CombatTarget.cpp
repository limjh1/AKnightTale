
// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_CombatTarget.h"
#include "FollowEnemyController.h"
#include "NavigationSystem.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemy.h"


UBTTask_CombatTarget::UBTTask_CombatTarget()
{
	NodeName = TEXT("AttackThePlayerWeakly"); // �ൿƮ������ �� �� �ִ� �̸�
}

EBTNodeResult::Type UBTTask_CombatTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

	if (Enemy1->bOverlappingCombatSphere && Enemy1->Health > 75)
	{
		Enemy1->bStrongAttack = false;

		//UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, RandomLocation);		
		//OwnerComp.GetBlackboardComponent()->SetValueAsVector(FName(TEXT("PatrolLocation")), RandomLocation.Location);
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_FindPatrolPos.h"
#include "FollowEnemyController.h"
#include "NavigationSystem.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemy.h"

UBTTask_FindPatrolPos::UBTTask_FindPatrolPos()
{
	NodeName = TEXT("FindPlayer"); // 행동트리에서 볼 수 있는 이름
}

EBTNodeResult::Type UBTTask_FindPatrolPos::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

	if (Enemy1->Health > 25 && Enemy1->bAgroSphereOn && NavSystem->GetRandomPointInNavigableRadius(FVector::ZeroVector, 1000.f, RandomLocation)) // && bAgroSphereOn
	{
		//Enemy1->bAgroSphereOn = true;

		//UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, RandomLocation);		
		//OwnerComp.GetBlackboardComponent()->SetValueAsVector(FName(TEXT("PatrolLocation")), RandomLocation.Location);
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
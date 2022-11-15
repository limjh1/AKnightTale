// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_FianlAttack.h"
#include "FollowEnemyController.h"
#include "NavigationSystem.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemy.h"


UBTTask_FianlAttack::UBTTask_FianlAttack()
{
	NodeName = TEXT("FinalAttack"); // �ൿƮ������ �� �� �ִ� �̸�
}

EBTNodeResult::Type UBTTask_FianlAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

	if (Enemy1->bOverlappingCombatSphere && Enemy1->Health <= 50)
	{
		Enemy1->bFinalAttack = true;

		//UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, RandomLocation);		
		//OwnerComp.GetBlackboardComponent()->SetValueAsVector(FName(TEXT("PatrolLocation")), RandomLocation.Location);
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}


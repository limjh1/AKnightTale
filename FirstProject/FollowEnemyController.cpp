// Fill out your copyright notice in the Description page of Project Settings.


#include "FollowEnemyController.h"
#include "NavigationSystem.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"

AFollowEnemyController::AFollowEnemyController()
{
	// �ε��ϱ�
	/*static ConstructorHelpers::FObjectFinder<UBlackboardData> BD(TEXT("BlackboardData'/Game/BT/FollowEnemy_BB.FollowEnemy_BB'"));
	if (BD.Succeeded())
	{
		BlackboardData = BD.Object;
	}

	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BT(TEXT("BehaviorTree'/Game/BT/FollowEnemy_BT.FollowEnemy_BT'"));
	if (BT.Succeeded())
	{
		BehaviorTree = BT.Object;
	}*/
}

void AFollowEnemyController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AFollowEnemyController::RandomMove, 3.f, true);

	//if (UseBlackboard(BlackboardData, Blackboard)) // ������ ����ؼ�
	//{
	//	if (RunBehaviorTree(BehaviorTree)) // ������ �����ϸ�
	//	{
	//		// TODO ���� �Ѵ�
	//	}
	//}
}

void AFollowEnemyController::OnUnPossess()
{
	Super::OnUnPossess();

	GetWorld()->GetTimerManager().ClearTimer(TimerHandle); // ���
}

void AFollowEnemyController::RandomMove()
{	
	auto CurrentPawn = GetPawn();

	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	if (NavSystem == nullptr)
	{		
		return;
	}

	FNavLocation RandomLocation;

	if (NavSystem->GetRandomPointInNavigableRadius(FVector::ZeroVector, 500.f, RandomLocation))
	{		
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, RandomLocation);
	}
}
// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Components/SphereComponent.h"
#include "AIController.h"
#include "Main.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Sound/SoundCue.h"
#include "Animation/AnimInstance.h"
#include "TimerManager.h"
#include "Components/CapsuleComponent.h"
#include "MainPlayerController.h"
#include "FollowEnemyController.h"
#include "NavigationSystem.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AgroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AgroSphere"));
	AgroSphere->SetupAttachment(GetRootComponent());
	AgroSphere->InitSphereRadius(600.f);
	AgroSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);

	CombatSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatSphere"));
	CombatSphere->SetupAttachment(GetRootComponent());
	CombatSphere->InitSphereRadius(75.f);

	LeftCombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftCombatCollision"));
	LeftCombatCollision->SetupAttachment(GetMesh(), FName("LeftEnemySocket"));	

	RightCombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("RightCombatCollision"));	
	RightCombatCollision->SetupAttachment(GetMesh(), FName("RightEnemySocket"));

	bOverlappingCombatSphere = false;

	Health = 75.f;
	MaxHealth = 100.f;
	Damage = 10.f;

	Section = 0;

	AttackMinTime = 0.0f;		
	AttackMaxTime = 1.5f;

	EnemyMovementStatus = EEnemyMovementStatus::EMS_Idle;

	DeathDelay = 1.5f;

	bHasValidTarget = false;

	// AI에 의해서 조작될때
	AIControllerClass = AFollowEnemyController::StaticClass(); // 이게 기본 클래스
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned; // 어떤 상황에서 포세스, 빙의가 될 것인지

	bAgroSphereOn = false;
	bEnemyIsDie = false;
	
	bStrongAttack = false;
	bFinalAttack = false;
	realDeath = false;

}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	AIController = Cast<AAIController>(GetController());
	MainPlayerController = Cast<AMainPlayerController>(GetController());

	AgroSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::AgroSphereOnOverlapBegin);
	AgroSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::AgroSphereOnOverLapEnd);

	CombatSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatSphereOnOverlapBegin);
	CombatSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatSphereOnOverLapEnd);
	
	LeftCombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::LeftCombatOnOverlapBegin);
	LeftCombatCollision->OnComponentEndOverlap.AddDynamic(this, &AEnemy::LeftCombatOnOverLapEnd);

	RightCombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::RightCombatOnOverlapBegin);
	RightCombatCollision->OnComponentEndOverlap.AddDynamic(this, &AEnemy::RightCombatOnOverLapEnd);

	LeftCombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftCombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	LeftCombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	LeftCombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	RightCombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightCombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	RightCombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	RightCombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	// 적 메쉬, 콜라이더가 카메라와 겹치는 것을 무시함
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);


	isOneTime = false;
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Health <= 0.f && !isOneTime)
	{
		SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Dead);
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->Montage_Play(CombatMontage, 1.35f);
			AnimInstance->Montage_JumpToSection(FName("Death"), CombatMontage);
		}

		LeftCombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		RightCombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		AgroSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		CombatSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		GetWorldTimerManager().SetTimer(DeathTimer, this, &AEnemy::Disappear, DeathDelay);

		isOneTime = true;
	}

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::AgroSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && Alive())
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main)
		{
			bAgroSphereOn = true;
			if (!bOverlappingCombatSphere)// && Health > 25) // 적 25이하면 도망침
				MoveToTarget(Main);
		}
	}
}

void AEnemy::AgroSphereOnOverLapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		{
			if (Main)// && Health) // 적 25이하면 도망침
			{			
				bAgroSphereOn = false;
				bHasValidTarget = false;
				if (Main->CombatTarget == this)
				{
					Main->SetCombarTarget(nullptr);
				}
				Main->SetHasCombatTarget(false);

				Main->UpdateCombatTarget();

				SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Idle);
				if (AIController)
				{
					AIController->StopMovement();			
				}
			}
		}
	}
}

void AEnemy::CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && Alive())
	{
		AMain* Main = Cast<AMain>(OtherActor);
		{
			if (Main)// && Health > 25)
			{
				bHasValidTarget = true;

				Main->SetCombarTarget(this);
				Main->SetHasCombatTarget(true);

				Main->UpdateCombatTarget();

				CombatTarget = Main;
				bOverlappingCombatSphere = true;				
				//SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Attacking);
				
				float AttackTime = FMath::FRandRange(AttackMinTime, AttackMaxTime);
				GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackTime);
			}
		}			
	}

}

void AEnemy::CombatSphereOnOverLapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && OtherComp)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		{
			if (Main)// && Health > 25)
			{							
				bOverlappingCombatSphere = false;
				MoveToTarget(Main);
				CombatTarget = nullptr;		

				if (Main->CombatTarget == this)
				{
					Main->SetCombarTarget(nullptr);
					Main->bHasCombatTarget = false;
					Main->UpdateCombatTarget();
				}
				GetWorldTimerManager().ClearTimer(AttackTimer);
			}
		}
	}
}


void AEnemy::MoveToTarget(class AMain* Target)
{
	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_MoveToTarget);

	if (AIController)
	{
		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalActor(Target);
		MoveRequest.SetAcceptanceRadius(25.0f);

		FNavPathSharedPtr NavPath;

		AIController->MoveTo(MoveRequest, &NavPath);

		UE_LOG(LogTemp, Warning, TEXT("Moving"));
		
		/* --> Debug Sphere Move to Location
		//TArray<FNavPathPoint> PathPoints = NavPath->GetPathPoints();
		auto PathPoints = NavPath->GetPathPoints();

		for (auto Point : PathPoints)
		{
			FVector Location = Point.Location;

			UKismetSystemLibrary::DrawDebugSphere(this, Location, 25.f, 8, FLinearColor::Red, 10.f, 0.5f);
		}
		*/
	}
}

void AEnemy::LeftCombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main)
		{
			if (Main->HitParticles)
			{
				const USkeletalMeshSocket* LeftTipSocket = GetMesh()->GetSocketByName("LeftTipSocket");
				if (LeftTipSocket)
				{
					FVector SocketLocation = LeftTipSocket->GetSocketLocation(GetMesh());
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Main->HitParticles, SocketLocation, FRotator(0.f), false);
				}
			}
			if (Main->HitSound)
			{
				UGameplayStatics::PlaySound2D(this, Main->HitSound);
			}
			if (DamageTypeClass)
			{
				UGameplayStatics::ApplyDamage(Main, Damage, AIController, this, DamageTypeClass);
			}
		}
	}
}

void AEnemy::LeftCombatOnOverLapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void AEnemy::RightCombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main)
		{
			if (Main->HitParticles)
			{
				const USkeletalMeshSocket* RightTipSocket = GetMesh()->GetSocketByName("RightTipSocket");
				if (RightTipSocket)
				{
					FVector SocketLocation = RightTipSocket->GetSocketLocation(GetMesh());
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Main->HitParticles, SocketLocation, FRotator(0.f), false);
				}
			}
			if (Main->HitSound)
			{
				UGameplayStatics::PlaySound2D(this, Main->HitSound);
			}
			if (DamageTypeClass)
			{
				UGameplayStatics::ApplyDamage(Main, Damage, AIController, this, DamageTypeClass);
			}
		}
	}
}

void AEnemy::RightCombatOnOverLapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void AEnemy::ActivateCollision()
{
	LeftCombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	RightCombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	if (SwingSound)
	{
		UGameplayStatics::PlaySound2D(this, SwingSound);
	}
}

void AEnemy::DeactivateCollision()
{
	LeftCombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightCombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy::Attack()
{
	if (Alive() && bHasValidTarget)
	{
		if (AIController)
		{
			AIController->StopMovement();
			UE_LOG(LogTemp, Warning, TEXT("Attacking"));
			SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Attacking);
		}
		if (!bAttacking)// && Health > 25)
		{
			bAttacking = true;

			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

			if (AnimInstance && Section == 0) // && !bStrongAttack && !bFinalAttack)
			{
				AnimInstance->Montage_Play(CombatMontage, 1.35f);
				AnimInstance->Montage_JumpToSection(FName("Attack1"), CombatMontage);
				UE_LOG(LogTemp, Warning, TEXT("Weak Attack"));		
			}
			if (AnimInstance && Section == 1)// && bStrongAttack && !bFinalAttack)
			{
				AnimInstance->Montage_Play(CombatMontage, 1.35f);
				AnimInstance->Montage_JumpToSection(FName("Attack2"), CombatMontage);
				UE_LOG(LogTemp, Warning, TEXT("Mid Attack"));		
			}
			if (AnimInstance && Section == 2)// && bFinalAttack)
			{
				AnimInstance->Montage_Play(CombatMontage, 1.35f);
				AnimInstance->Montage_JumpToSection(FName("Attack3"), CombatMontage);
				UE_LOG(LogTemp, Warning, TEXT("Strong Attack"));
			}
		}
	}
}

void AEnemy::AttackEnd()
{
	bAttacking = false;

	Section++;
	if (Section > 2)
		Section = 0;

	if (bOverlappingCombatSphere) // 아직 겹쳐있다면.
	{		
		float AttackTime = FMath::FRandRange(AttackMinTime, AttackMaxTime);
		GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackTime);		
	}
}

void AEnemy::FianlAttack()
{
	if (FinalSwingSound)
		UGameplayStatics::PlaySound2D(this, FinalSwingSound);
}

float AEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	if (Health - DamageAmount <= 0.f)
	{
		Health = 0.f;
		MaxHealth = 100.f;
		realDeath = true;
		Die(DamageCauser);
		if (bEnemyIsDie == true)
		{
			
		}			
	}
	else
	{
		Health -= DamageAmount;
	}
	return DamageAmount;
}

void AEnemy::Die(AActor* Causer)
{
	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Dead);


	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Play(CombatMontage, 1.35f);
		AnimInstance->Montage_JumpToSection(FName("Death"), CombatMontage);	
	}	

	LeftCombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightCombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AgroSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		

	//AMain* Main = Cast<AMain>(Causer);
	////AMain* Main = Cast<AMain>(Causer);
	//if (Main)
	//{
	//	// cast가 안되고 있음. Causer가 Weapon을 가리키고 있음


	//	// 경험치 상승
	//	Main->exp += 100;

	//	// 새로운 타겟으로 업데이트
	//	Main->UpdateCombatTarget();

	//	UE_LOG(LogTemp, Warning, TEXT("After Cast Main"));
	//}

	/*AMain* Main;
	Main->exp += 100;
	Main->UpdateCombatTarget();*/


	//AWeapon* MainWeapon = Cast<AWeapon>(Causer);
	////AMain* Main = Cast<AMain>(Causer);
	//if (MainWeapon)
	//{
	//	// cast가 안되고 있음. Causer가 Weapon을 가리키고 있음


	//	// 경험치 상승
	//	//Main->exp += 100;

	//	// 새로운 타겟으로 업데이트
	//	//Main->UpdateCombatTarget();

	//	UE_LOG(LogTemp, Warning, TEXT("After Cast Main"));
	//}
	
}

void AEnemy::DeathEnd()
{
	GetMesh()->bPauseAnims = true;
	GetMesh()->bNoSkeletonUpdate = true;

	UE_LOG(LogTemp, Warning, TEXT("Before Cast Main"));

	//Destroy();
	GetWorldTimerManager().SetTimer(DeathTimer, this, &AEnemy::Disappear, DeathDelay);
}

bool AEnemy::Alive()
{
	return GetEnemyMovementStatus() != EEnemyMovementStatus::EMS_Dead;
}

void AEnemy::Disappear()
{
	// 파괴처리는 소모품, 장비 수집하는 것까지 하고 난 뒤에 파괴처리 하기
	Destroy();
}

void AEnemy::PlayerDeath()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Play(CombatMontage, 1.35f);
		AnimInstance->Montage_JumpToSection(FName("PlayerDeath"), CombatMontage);
	}
}

void AEnemy::DisplayHealthBar()
{
	if (MainPlayerController)
	{
		MainPlayerController->DisplayEnemyHealthBar();
	}
}

void AEnemy::RemoveHealthBar()
{
	if (MainPlayerController)
	{
		MainPlayerController->RemoveEnemyHealthBar();
	}
}
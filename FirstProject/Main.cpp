// Fill out your copyright notice in the Description page of Project Settings.


#include "Main.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Engine/World.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Enemy.h"
#include "MainPlayerController.h"
#include "HongikSaveGame.h"
#include "ItemStorage.h"

// Sets default values
AMain::AMain()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create Camera Boom (pulls towards the player if there's a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 600.f; // Camera follows at this distance
	CameraBoom->bUsePawnControlRotation = true; // Rotate arm based on controller

	//Set size for collision capsule
	GetCapsuleComponent()->SetCapsuleSize(29.f, 92.f); // character size

	// Create Follow Camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// Attach the camera to the end of the boom and let the boom adjust to match
	// the controller orientation
	FollowCamera->bUsePawnControlRotation = false;

	// Set our turn rates for input
	BaseTurnRate = 65.f;
	BaseLookUpRate = 65.f;

	// Don't rotate when the controller rotates
	// Let that just affect the camera
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...
	GetCharacterMovement()->RotationRate = FRotator(0.0f,700.f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 650.f;
	GetCharacterMovement()->AirControl = 0.2f; // 공중제어

	MaxHealth = 100.f;
	Health = 65.f;
	MaxStaminaa = 150.f;
	Stamina = 120.f;
	MaxMana = 100.f;
	Mana = 100.f;
	Experience = 0;
	MaxExperience = 5;
	Level = 1;
	realCurrency = 0;


	/*Coins = 0;*/

	CrouchSpeed = 350.f;
	RunningSpeed = 650.f;
	SprintingSpeed = 950.f;
	
	bShiftKeyDown = false;
	bLMBDown = false;
	bESCDown = false;

	bCrouch = false;

	// Initialize ENUM
	MovementStatus = EMovementStatus::EMS_Normal;
	StaminaStatus = EStaminaStatus::ESS_Normal;

	StaminaDrainRate = 25.f;
	MinSprintStamina = 40.f;

	Section = 0;

	InterpSpeed = 15.f;
	bInterpToEnemy = false;

	bHasCombatTarget = false;
	bMovingForward = false;
	bMovingRight = false;

	isTutorial = false;
	isEquipped = false;
}

// Called when the game starts or when spawned
void AMain::BeginPlay()
{
	Super::BeginPlay();

	MainPlayerController = Cast<AMainPlayerController>(GetController());

	FString Map = GetWorld()->GetMapName();
	Map.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);

	if (Map != "MyTutoriallLevel")
	{
		LoadGameNoSwitch();
		if (MainPlayerController)
		{
			MainPlayerController->GameModeOnly();
		}
	}
}

// Called every frame
void AMain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MovementStatus == EMovementStatus::EMS_Dead) return;

	if (MovementStatus == EMovementStatus::EMS_Sprinting)
	{
		CameraBoom->TargetArmLength = 850.f;
	}
	else if (MovementStatus == EMovementStatus::EMS_Crouch)
	{
		CameraBoom->TargetArmLength = 550.f;
	}
	else
	{
		CameraBoom->TargetArmLength = 700.f;
	}

	float DeltaStamina = StaminaDrainRate * DeltaTime;

	if (!bCrouch) // 앉을때 불가능
	{

		// 달리기 스테미나
		switch (StaminaStatus)
		{
		case EStaminaStatus::ESS_Normal:
			if (bShiftKeyDown)
			{
				if (Stamina - DeltaStamina <= MinSprintStamina)
				{
					if (SprintSoundEnd)
						UGameplayStatics::PlaySound2D(this, SprintSoundEnd);
					SetStaminaStatus(EStaminaStatus::ESS_BelowMinimum);
					Stamina -= DeltaStamina;
				}
				else
				{

				}
				if (bMovingForward || bMovingRight)
				{
					Stamina -= DeltaStamina;
					SetMovementStatus(EMovementStatus::EMS_Sprinting);
				}
				else
				{
					Stamina += DeltaStamina;
					SetMovementStatus(EMovementStatus::EMS_Normal);
				}

			}
			else // Shift Key Up
			{
				if (Stamina + DeltaStamina >= MaxStaminaa)
				{
					Stamina = MaxStaminaa;
				}
				else
				{
					Stamina += DeltaStamina;
				}
				SetMovementStatus(EMovementStatus::EMS_Normal);
			}
			break;
		case EStaminaStatus::ESS_BelowMinimum:
			if (bShiftKeyDown)
			{
				if (Stamina - DeltaStamina <= 0.f)
				{
					if (TiredSound)
						UGameplayStatics::PlaySound2D(this, TiredSound);
					SetStaminaStatus(EStaminaStatus::ESS_Exhausted);
					Stamina = 0;
					SetMovementStatus(EMovementStatus::EMS_Normal);
				}
				else
				{
					Stamina -= DeltaStamina;
					if (bMovingForward || bMovingRight)
					{
						SetMovementStatus(EMovementStatus::EMS_Sprinting);
					}
					else
					{
						SetMovementStatus(EMovementStatus::EMS_Normal);
					}
				}
			}
			else // Shift key up
			{
				if (Stamina + DeltaStamina >= MinSprintStamina)
				{
					SetStaminaStatus(EStaminaStatus::ESS_Normal);
					Stamina += DeltaStamina;
				}
				else
				{
					Stamina += DeltaStamina;
				}
				SetMovementStatus(EMovementStatus::EMS_Normal);
			}
			break;
		case EStaminaStatus::ESS_Exhausted:
			if (bShiftKeyDown)
			{
				Stamina = 0.f;
			}
			else // Shift key up
			{
				SetStaminaStatus(EStaminaStatus::ESS_ExhaustedRecovering);
				Stamina += DeltaStamina;
			}
			SetMovementStatus(EMovementStatus::EMS_Normal);
			break;
		case EStaminaStatus::ESS_ExhaustedRecovering:
			if (Stamina + DeltaStamina >= MinSprintStamina)
			{
				if (RecoveryBreathSound)
					UGameplayStatics::PlaySound2D(this, RecoveryBreathSound);
				SetStaminaStatus(EStaminaStatus::ESS_Normal);
				Stamina += DeltaStamina;
			}
			else
			{
				Stamina += DeltaStamina;
			}
			SetMovementStatus(EMovementStatus::EMS_Normal);
			break;
		default:
			break;
		}
	}

	if (bInterpToEnemy && CombatTarget)
	{
		FRotator LookAtYaw = GetLookAtRotationYaw(CombatTarget->GetActorLocation());
		FRotator InterpRotation = FMath::RInterpTo(GetActorRotation(), LookAtYaw, DeltaTime, InterpSpeed);

		SetActorRotation(InterpRotation);
	}

	if (CombatTarget)
	{
		CombatTargetLocation = CombatTarget->GetActorLocation();
		if (MainPlayerController)
		{
			MainPlayerController->EnemyLocation = CombatTargetLocation;
		}
	}

	/*if (isTutorial)
	{
		CrouchStart();
	}
	else if (!isTutorial)
	{
		CrouchEnd();
	}*/
}

FRotator AMain::GetLookAtRotationYaw(FVector Target)
{
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target);
	FRotator LookAtRotationYaw(0.f, LookAtRotation.Yaw, 0.f);
	return LookAtRotationYaw;
}

// Called to bind functionality to input
void AMain::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	// Jump는 이미 있기때문에 선언 안하고 사용가능
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMain::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AMain::ShiftKeyDown);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AMain::ShiftKeyUp);

	PlayerInputComponent->BindAction("ESC", IE_Pressed, this, &AMain::ESCDown);
	PlayerInputComponent->BindAction("ESC", IE_Released, this, &AMain::ESCUp);

	PlayerInputComponent->BindAction("LMB", IE_Pressed, this, &AMain::LMBDown);
	PlayerInputComponent->BindAction("LMB", IE_Released, this, &AMain::LMBUp);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AMain::CrouchStart);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AMain::CrouchEnd);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMain::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMain::MoveRight);

	// Yaw,Pitch는 이미 있기때문에 선언 안하고 사용가능
	PlayerInputComponent->BindAxis("Turn", this, &AMain::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &AMain::LookUp);
	PlayerInputComponent->BindAxis("TurnRate", this, &AMain::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMain::LookUpAtRate);


}


void AMain::CrouchStart()
{
	bCrouch = true;
	SetMovementStatus(EMovementStatus::EMS_Crouch);
	//UE_LOG(LogTemp, Warning, TEXT("Press"));
}

void AMain::CrouchEnd()
{
	bCrouch = false;
	SetMovementStatus(EMovementStatus::EMS_Normal);
	//UE_LOG(LogTemp, Warning, TEXT("UnPress"));
}

void AMain::Jump()
{
	if (MainPlayerController) if (MainPlayerController->bPauseMenuVisible) return;

	if (MovementStatus != EMovementStatus::EMS_Dead && MovementStatus != EMovementStatus::EMS_Crouch)
	{
		Super::Jump();

		bPressedJump = true;
		JumpKeyHoldTime = 0.0f;

		if (!bWasJumping && JumpSound)
			UGameplayStatics::PlaySound2D(this, JumpSound);
	}	
}

bool AMain::CanMove(float Value)
{
	if (MainPlayerController)
	{
		return			
			Value != 0.0f
			&& (!bAttacking)
			&& (MovementStatus != EMovementStatus::EMS_Dead)
			&& !MainPlayerController->bPauseMenuVisible;
	}
	return false;	
}

void AMain::Turn(float value)
{
	if (CanMove(value))
	{
		AddControllerYawInput(value);
	}
}


void AMain::LookUp(float value)
{
	if (CanMove(value))
	{
		AddControllerPitchInput(value);
	}
}

void AMain::MoveForward(float value)
{
	bMovingForward = false;

	if (CanMove(value))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, value);

		bMovingForward = true;
	}	
}

void AMain::MoveRight(float value)
{
	bMovingRight = false;

	if (CanMove(value))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, value);

		bMovingRight = true;
	}
}

void AMain::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}


void AMain::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMain::LMBDown()
{
	bLMBDown = true;

	if (MovementStatus == EMovementStatus::EMS_Dead) return;

	if (MainPlayerController) if (MainPlayerController->bPauseMenuVisible) return;

	


	if (ActiveOverlappingItem) // 무기 없을 때
	{
		AWeapon* Weapon = Cast<AWeapon>(ActiveOverlappingItem);
		if (Weapon)
		{
			// 이거를 인벤토리에서 클릭했을 때 호출되게끔 수정하기
			Weapon->Equip(this);
			isEquipped = true;
			SetActiveOverlappingItem(nullptr);
		}
	}
	else if (EquippedWeapon) // 무기 장착 후
	{		
		Attack();
	}
}

void AMain::LMBUp()
{
	bLMBDown = false;
}

void AMain::ESCDown()
{
	bESCDown = true;

	if (MainPlayerController)
	{
		MainPlayerController->TogglePauseMenu();
	}
}

void AMain::ESCUp()
{
	bESCDown = false;
}

void AMain::DecrementHealth(float Amount)
{

}

void AMain::Die()
{
	if (MovementStatus == EMovementStatus::EMS_Dead) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && CombatMontage)
	{
		AnimInstance->Montage_Play(CombatMontage, 1.0f);
		AnimInstance->Montage_JumpToSection(FName("Death"));
	}

	SetMovementStatus(EMovementStatus::EMS_Dead);

	if (MainPlayerController)
	{
		MainPlayerController->ShowGameOverUI();
		MainPlayerController->bShowMouseCursor = true;
	}
}

void AMain::DeathEnd()
{
	GetMesh()->bPauseAnims = true;
	GetMesh()->bNoSkeletonUpdate = true;
}

void AMain::IncrementHealth(float Amount)
{
	if (Health + Amount >= MaxHealth)
	{
		Health = MaxHealth;
	}
	else
	{
		Health += Amount;
	}
}

void AMain::SetMovementStatus(EMovementStatus Status)
{
	MovementStatus = Status;
	if (MovementStatus == EMovementStatus::EMS_Sprinting)
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintingSpeed;
	}
	else if (MovementStatus == EMovementStatus::EMS_Crouch)
	{
		GetCharacterMovement()->MaxWalkSpeed = CrouchSpeed;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;
	}
}

void AMain::ShiftKeyDown()
{
	bShiftKeyDown = true;
}

void AMain::ShiftKeyUp()
{
	bShiftKeyDown = false;
}

void AMain::ShowPickupLocations()
{

	//for (int32 i = 0; i < PickupLocations.Num(); i++)
	//{
	//	UKismetSystemLibrary::DrawDebugSphere(this, PickupLocations[i], 25.f, 8, FLinearColor::Green, 10.f, 0.5f);
	//}

	for (FVector Location : PickupLocations)
	{
		UKismetSystemLibrary::DrawDebugSphere(this, Location, 25.f, 8, FLinearColor::Green, 10.f, 0.5f);
	}
}

void AMain::SetEquippedWeapon(AWeapon* WeaponToSet)
{
	if (EquippedWeapon) // nullptr이 아닐 경우에만 파괴해야 충돌 안남
	{
		//EquippedWeapon->Destroy();
	}

	EquippedWeapon = WeaponToSet;

}

void AMain::Attack()
{
	if (!bAttacking && MovementStatus != EMovementStatus::EMS_Dead)
	{		
		bAttacking = true;
		SetInterpToEnemy(true);

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && CombatMontage)
		{			
			switch (Section)
			{
			case 0:
				AnimInstance->Montage_Play(CombatMontage, 1.1f);
				AnimInstance->Montage_JumpToSection(FName("Attack_1"), CombatMontage);				
				break;
			case 1:
				AnimInstance->Montage_Play(CombatMontage, 1.1f);
				AnimInstance->Montage_JumpToSection(FName("Attack_2"), CombatMontage);					
				break;
			case 2:
				AnimInstance->Montage_Play(CombatMontage, 1.1f);
				AnimInstance->Montage_JumpToSection(FName("Attack_3"), CombatMontage);					
				break;
			default:
				break;
			}			
		}
	}	
}

void AMain::AttackEnd()
{
	bAttacking = false;
	SetInterpToEnemy(false);

	Section++;
	if (Section >= 3)
		Section = 0;

	if (bLMBDown)
	{	
		//UE_LOG(LogTemp, Warning, TEXT("dfs"));
		Attack();
	}
}



void AMain::PlaySwingSound()
{
	if (EquippedWeapon->SwingSound)
	{
		UGameplayStatics::PlaySound2D(this, EquippedWeapon->SwingSound);
	}
}

void AMain::PlayAttackSound()
{
	if (Section == 0)
		UGameplayStatics::PlaySound2D(this, AttackSound1);
	else if (Section == 1)
		UGameplayStatics::PlaySound2D(this, AttackSound2);
	else if (Section == 2)
		UGameplayStatics::PlaySound2D(this, AttackSound3);
}

void AMain::SetInterpToEnemy(bool Interp)
{
	bInterpToEnemy = Interp;
}

float AMain::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	if (Health - DamageAmount <= 0.f)
	{
		Health -= DamageAmount;
		Die();
		if (DamageCauser)
		{
			AEnemy* Enemy = Cast<AEnemy>(DamageCauser);
			if (Enemy)
			{
				Enemy->bHasValidTarget = false;
			}
		}
	}
	else
	{
		Health -= DamageAmount;
	}

	return DamageAmount;
}

void AMain::UpdateCombatTarget()
{
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors, EnemyFilter);

	if (OverlappingActors.Num() == 0) 
	{
		if (MainPlayerController)
		{
			//MainPlayerController->RemoveEnemyHealthBar();
			
		}
		return;
	}

	AEnemy* ClosestEnemy = Cast<AEnemy>(OverlappingActors[0]);
	if (ClosestEnemy)
	{
		FVector Location = GetActorLocation();

		float MinDistance = (ClosestEnemy->GetActorLocation() - Location).Size();

		for (auto Actor : OverlappingActors)
		{
			AEnemy* Enemy = Cast<AEnemy>(Actor);
			if (Enemy)
			{
				float DistanceToActor = (Enemy->GetActorLocation() - Location).Size();

				if (DistanceToActor < MinDistance)
				{
					MinDistance = DistanceToActor;
					ClosestEnemy = Enemy;
				}
			}			
		}
		if (MainPlayerController)
		{
			//MainPlayerController->DisplayEnemyHealthBar();
		}
		if (ClosestEnemy->Health > 25)
		{
			SetCombarTarget(ClosestEnemy);
			bHasCombatTarget = true;
		}
		else
		{
			bHasCombatTarget = false;
		}
	}

}

void AMain::SwitchLevel(FName LevelName)
{
	UWorld* World = GetWorld();
	if (World)
	{
		FString CurrentLevel = World->GetMapName();

		FName CurrentLevelName(*CurrentLevel);

		if (CurrentLevelName != LevelName)
		{
			UGameplayStatics::OpenLevel(World, LevelName);
		}
	}
}

void AMain::SaveGame()
{
	UHongikSaveGame* SaveGameInstance = Cast<UHongikSaveGame>(UGameplayStatics::CreateSaveGameObject(UHongikSaveGame::StaticClass()));

	SaveGameInstance->CharacterStats.Health = Health;
	SaveGameInstance->CharacterStats.MaxHealth = MaxHealth;
	SaveGameInstance->CharacterStats.Stamina = Stamina;
	SaveGameInstance->CharacterStats.MaxStamina = MaxStaminaa;
	//SaveGameInstance->CharacterStats.Experience = Experience;
	//SaveGameInstance->CharacterStats.MaxExperience = MaxExperience;
	//SaveGameInstance->CharacterStats.Level = Level;
	//SaveGameInstance->CharacterStats.Currency = realCurrency;


	// 현재 레벨 이름 가져오기
	FString MapName = GetWorld()->GetMapName();
	// 현재 레벨 이름을 제외한 것들을 삭제해서 가져옴. UE_머시기
	MapName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);

	SaveGameInstance->CharacterStats.LevelName = MapName;

	if (EquippedWeapon)
	{
		SaveGameInstance->CharacterStats.WeaponaName = EquippedWeapon->Name;
	}

	SaveGameInstance->CharacterStats.Location = GetActorLocation();
	SaveGameInstance->CharacterStats.Rotation = GetActorRotation();

	UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveGameInstance->PlayerName, SaveGameInstance->UserIndex);
}

void AMain::LoadGame(bool SetPosition)
{
	UHongikSaveGame* LoadGameInstance = Cast<UHongikSaveGame>(UGameplayStatics::CreateSaveGameObject(UHongikSaveGame::StaticClass()));

	LoadGameInstance = Cast<UHongikSaveGame>(UGameplayStatics::LoadGameFromSlot(LoadGameInstance->PlayerName, LoadGameInstance->UserIndex));

	Health = LoadGameInstance->CharacterStats.Health;
	MaxHealth = LoadGameInstance->CharacterStats.MaxHealth;
	Stamina = LoadGameInstance->CharacterStats.Stamina;
	MaxStaminaa = LoadGameInstance->CharacterStats.MaxStamina;
	/*Experience = LoadGameInstance->CharacterStats.Experience;
	MaxExperience = LoadGameInstance->CharacterStats.MaxExperience;
	Level = LoadGameInstance->CharacterStats.Level;
	realCurrency = LoadGameInstance->CharacterStats.Currency;*/

	if (WeaponStorage)
	{
		AItemStorage* Weapons = GetWorld()->SpawnActor<AItemStorage>(WeaponStorage);
		if (Weapons)
		{
			FString WeaponName = LoadGameInstance->CharacterStats.WeaponaName;			

			if (Weapons->WeaponMap.Contains(WeaponName))
			{
				AWeapon* WeaponToEquip = GetWorld()->SpawnActor<AWeapon>(Weapons->WeaponMap[WeaponName]);
				WeaponToEquip->Equip(this);
			}
		}
	}


	if (SetPosition)
	{
		SetActorLocation(LoadGameInstance->CharacterStats.Location);
		SetActorRotation(LoadGameInstance->CharacterStats.Rotation);
	}

	SetMovementStatus(EMovementStatus::EMS_Normal);
	GetMesh()->bPauseAnims = false;
	GetMesh()->bNoSkeletonUpdate = false;

	// 공백이 아닐 경우
	if (LoadGameInstance->CharacterStats.LevelName != TEXT(""))
	{
		FName LevelName(*LoadGameInstance->CharacterStats.LevelName);

		SwitchLevel(LevelName);
	}
}

void AMain::LoadGameNoSwitch()
{
	UHongikSaveGame* LoadGameInstance = Cast<UHongikSaveGame>(UGameplayStatics::CreateSaveGameObject(UHongikSaveGame::StaticClass()));

	LoadGameInstance = Cast<UHongikSaveGame>(UGameplayStatics::LoadGameFromSlot(LoadGameInstance->PlayerName, LoadGameInstance->UserIndex));

	Health = LoadGameInstance->CharacterStats.Health;
	MaxHealth = LoadGameInstance->CharacterStats.MaxHealth;
	Stamina = LoadGameInstance->CharacterStats.Stamina;
	MaxStaminaa = LoadGameInstance->CharacterStats.MaxStamina;
	/*Experience = LoadGameInstance->CharacterStats.Experience;
	MaxExperience = LoadGameInstance->CharacterStats.MaxExperience;
	Level = LoadGameInstance->CharacterStats.Level;
	realCurrency = LoadGameInstance->CharacterStats.Currency;*/

	if (WeaponStorage)
	{
		AItemStorage* Weapons = GetWorld()->SpawnActor<AItemStorage>(WeaponStorage);
		if (Weapons)
		{
			FString WeaponName = LoadGameInstance->CharacterStats.WeaponaName;

			if (Weapons->WeaponMap.Contains(WeaponName))
			{
				AWeapon* WeaponToEquip = GetWorld()->SpawnActor<AWeapon>(Weapons->WeaponMap[WeaponName]);
				WeaponToEquip->Equip(this);
			}
		}
	}

	

	SetMovementStatus(EMovementStatus::EMS_Normal);
	GetMesh()->bPauseAnims = false;
	GetMesh()->bNoSkeletonUpdate = false;
}
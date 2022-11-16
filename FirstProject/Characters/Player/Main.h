// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Main.generated.h"

UENUM(BlueprintType)
enum class EMovementStatus : uint8
{
	EMS_Normal UMETA(DisplayName = "Normal"),
	EMS_Sprinting UMETA(DisplayName = "Sprinting"),
	EMS_Dead UMETA(DisplayName = "Dead"),
	EMS_Crouch UMETA(DisplayName = "Crouch"),		

	EMS_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EStaminaStatus : uint8
{
	ESS_Normal UMETA(DisplyName= "Normal"),
	ESS_BelowMinimum UMETA(DisplyName= "BelowMinimum"),
	ESS_Exhausted UMETA(DisplyName= "Exhausted"),
	ESS_ExhaustedRecovering UMETA(DisplyName= "ExhaustedRecovering"),

	ESS_Max UMETA(DisplyName = "DefaultMax")
};


UCLASS()
class FIRSTPROJECT_API AMain : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMain();

	UPROPERTY(EditDefaultsOnly, Category = "SaveData")
	TSubclassOf<class AItemStorage> WeaponStorage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bHasCombatTarget;

	FORCEINLINE void SetHasCombatTarget(bool HasTarget) { bHasCombatTarget = HasTarget; }

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Combat")
	FVector CombatTargetLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Controller")
	class AMainPlayerController* MainPlayerController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class UParticleSystem* HitParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class USoundCue* HitSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	USoundCue* AttackSound1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	USoundCue* AttackSound2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	USoundCue* AttackSound3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	USoundCue* SprintSoundEnd;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	USoundCue* TiredSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	USoundCue* RecoveryBreathSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	USoundCue* JumpSound;

	TArray<FVector> PickupLocations;
	
	UFUNCTION(BlueprintCallable)
	void ShowPickupLocations();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	EMovementStatus MovementStatus;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	EStaminaStatus StaminaStatus;

	FORCEINLINE void SetStaminaStatus(EStaminaStatus Status) { StaminaStatus = Status; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float StaminaDrainRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MinSprintStamina;

	float InterpSpeed;
	bool bInterpToEnemy;
	void SetInterpToEnemy(bool Interp);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	class AEnemy* CombatTarget;

	FORCEINLINE void SetCombarTarget(AEnemy* Target) { CombatTarget = Target; }

	FRotator GetLookAtRotationYaw(FVector Target);

	// Set movement Status and running speed;
	void SetMovementStatus(EMovementStatus Status);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Running")
	float RunningSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Running")
	float SprintingSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Running")
	float CrouchSpeed;

	bool bShiftKeyDown;

	// Pressed down to enable sprinting
	void ShiftKeyDown();

	// Released down to stop sprinting
	void ShiftKeyUp();

	// Camera boom positioning the camera behind the player
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	// Follow Camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	// Base turn rates to scale turning functions for the camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

	/*
	/*
	/* Player Stats 
	/* 
	*/

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Stats")
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Stats")
	float MaxStaminaa;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float Stamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float MaxMana;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float Mana;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	int Experience;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	int MaxExperience;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	int Level;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	int realCurrency;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	//int32 Coins;

	void DecrementHealth(float Amount);

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
		
	UFUNCTION(BlueprintCallable)
	void IncrementHealth(float Amount);

	//FORCEINLINE void HealItem() { Health += 20.f; }

	UFUNCTION(BlueprintCallable)
	void Die();

	virtual void Jump() override;
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Called for Forwards/Backwards input
	void MoveForward(float value);
	
	// Called for side to side input
	void MoveRight(float value);

	// Called for Yaw Roation
	void Turn(float value);

	// Called for Pitch Rotation
	void LookUp(float value);

	bool bMovingForward;
	bool bMovingRight;

	bool CanMove(float Value);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bCrouch;

	// Called via input to turn at a given rate
	// @param Rate This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	void TurnAtRate(float Rate);

	// Called via input to look up/down at a given rate
	// @param Rate This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	void LookUpAtRate(float Rate);

	bool bLMBDown;
	void LMBDown();
	void LMBUp();

	bool bESCDown;
	void ESCDown();
	void ESCUp();

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Items")
	class AWeapon* EquippedWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Items")
	bool isEquipped;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Items")
	class AItem* ActiveOverlappingItem;

	void SetEquippedWeapon(AWeapon* WeaponToSet);
	FORCEINLINE AWeapon* GetEquippedWeapon() { return EquippedWeapon; }
	FORCEINLINE void SetActiveOverlappingItem(AItem* Item) { ActiveOverlappingItem = Item; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anims")
	bool bAttacking;

	void Attack();

	UFUNCTION(BlueprintCallable)
	void AttackEnd();

	UFUNCTION(BlueprintCallable)
	void CrouchStart();

	UFUNCTION(BlueprintCallable)	
	void CrouchEnd();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims")
	class UAnimMontage* CombatMontage;

	int32 Section;

	UFUNCTION(BlueprintCallable)
	void PlaySwingSound();

	UFUNCTION(BlueprintCallable)
	void PlayAttackSound();

	UFUNCTION(BlueprintCallable)
	void DeathEnd();

	void UpdateCombatTarget();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TSubclassOf<AEnemy> EnemyFilter;

	void SwitchLevel(FName LevelName);

	bool isTutorial;

	UFUNCTION(BlueprintCallable)
	void SaveGame();

	UFUNCTION(BlueprintCallable)
	void LoadGame(bool SetPosition);

	void LoadGameNoSwitch();
};

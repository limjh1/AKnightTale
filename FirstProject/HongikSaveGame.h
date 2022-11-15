// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "HongikSaveGame.generated.h"

USTRUCT(BlueprintType)
struct FCharacterStats 
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	float Health;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	float MaxHealth;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	float Stamina;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	float MaxStamina;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	int Experience;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	int MaxExperience;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	int Level;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	int Currency;

	//UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	//int32 Coins;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	FVector Location;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	FRotator Rotation;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	FString WeaponaName;

	UPROPERTY(VisibleAnywhere, Category = "SaveGameData")
	FString LevelName;
};

/**
 * 
 */
UCLASS()
class FIRSTPROJECT_API UHongikSaveGame : public USaveGame
{
	GENERATED_BODY()

public:

	UHongikSaveGame();

	UPROPERTY(VisibleAnywhere, Category = Basic)
	FString PlayerName;

	UPROPERTY(VisibleAnywhere, Category = Basic)
	uint32 UserIndex;
	
	UPROPERTY(VisibleAnywhere, Category = Basic)
	FCharacterStats CharacterStats;

};

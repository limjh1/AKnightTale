// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelChangeSetUpCollision.h"
#include "Main.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Sound/SoundCue.h"

ALevelChangeSetUpCollision::ALevelChangeSetUpCollision()
{

}

void ALevelChangeSetUpCollision::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	
	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main)
		{
			if (!Main->isTutorial)
			{
				Main->isTutorial = true;
				Main->CrouchStart();
			}				
			else if (Main->isTutorial)
			{
				Main->isTutorial = false;
				Main->CrouchEnd();
			}				

			Destroy();
		}
	}
}
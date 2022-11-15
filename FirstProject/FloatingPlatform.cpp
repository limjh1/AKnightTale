/*
이동하는 플랫폼
*/

#include "FloatingPlatform.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"

// 생성자
AFloatingPlatform::AFloatingPlatform()
{
 	// 액터의 Tick함수를 사용할 건지
	PrimaryActorTick.bCanEverTick = true;

	// Mesh를 RootComponent에 붙여줌
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	StartPoint = FVector(0.f);
	EndPoint = FVector(0.f);

	bInterping = false;

	InterpSpeed = 4.0f;	
	InterpTime = 1.f;	
}

// Called when the game starts or when spawned
void AFloatingPlatform::BeginPlay()
{
	Super::BeginPlay();
	
	// 시작 지점과 끝 지점을 할당함
	StartPoint = GetActorLocation();
	EndPoint += StartPoint;

	// interp인지 아닌지
	bInterping = false;

	// interpTime 만큼 딜레이를 주고 함수를 실행함
	GetWorldTimerManager().SetTimer(InterpTImer, this, &AFloatingPlatform::ToggleInterping, InterpTime);

	// 거리를 계산함
	Distance = (EndPoint - StartPoint).Size();
}

// Called every frame
void AFloatingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bInterping) 
	{
		// 현재 위치를 업데이트 해주면서 계속 보간하여 위치를 업데이트해줌
		FVector CurrentLocation = GetActorLocation();
		FVector Interp = FMath::VInterpTo(CurrentLocation, EndPoint, DeltaTime, InterpSpeed);
		SetActorLocation(Interp);

		// 이동한 거리를 계산함
		float DistanceTraveled = (GetActorLocation() - StartPoint).Size();
		// 1.0보다 작으면 스왑해줌
		if (Distance - DistanceTraveled <= 1.f)
		{
			ToggleInterping();

			GetWorldTimerManager().SetTimer(InterpTImer, this, &AFloatingPlatform::ToggleInterping, InterpTime);
			SwapVectors(StartPoint, EndPoint);
		}
	}	
}

// Bool값을 토글해주는 함수
void AFloatingPlatform::ToggleInterping()
{
	bInterping = !bInterping;
}

// Swap 알고리즘
void AFloatingPlatform::SwapVectors(FVector& VecOne, FVector& VecTwo)
{
	FVector Temp = VecOne;
	VecOne = VecTwo;
	VecTwo = Temp;
}


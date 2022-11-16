/*
�̵��ϴ� �÷���
*/

#include "FloatingPlatform.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"

// ������
AFloatingPlatform::AFloatingPlatform()
{
 	// ������ Tick�Լ��� ����� ����
	PrimaryActorTick.bCanEverTick = true;

	// Mesh�� RootComponent�� �ٿ���
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
	
	// ���� ������ �� ������ �Ҵ���
	StartPoint = GetActorLocation();
	EndPoint += StartPoint;

	// interp���� �ƴ���
	bInterping = false;

	// interpTime ��ŭ �����̸� �ְ� �Լ��� ������
	GetWorldTimerManager().SetTimer(InterpTImer, this, &AFloatingPlatform::ToggleInterping, InterpTime);

	// �Ÿ��� �����
	Distance = (EndPoint - StartPoint).Size();
}

// Called every frame
void AFloatingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bInterping) 
	{
		// ���� ��ġ�� ������Ʈ ���ָ鼭 ��� �����Ͽ� ��ġ�� ������Ʈ����
		FVector CurrentLocation = GetActorLocation();
		FVector Interp = FMath::VInterpTo(CurrentLocation, EndPoint, DeltaTime, InterpSpeed);
		SetActorLocation(Interp);

		// �̵��� �Ÿ��� �����
		float DistanceTraveled = (GetActorLocation() - StartPoint).Size();
		// 1.0���� ������ ��������
		if (Distance - DistanceTraveled <= 1.f)
		{
			ToggleInterping();

			GetWorldTimerManager().SetTimer(InterpTImer, this, &AFloatingPlatform::ToggleInterping, InterpTime);
			SwapVectors(StartPoint, EndPoint);
		}
	}	
}

// Bool���� ������ִ� �Լ�
void AFloatingPlatform::ToggleInterping()
{
	bInterping = !bInterping;
}

// Swap �˰���
void AFloatingPlatform::SwapVectors(FVector& VecOne, FVector& VecTwo)
{
	FVector Temp = VecOne;
	VecOne = VecTwo;
	VecTwo = Temp;
}


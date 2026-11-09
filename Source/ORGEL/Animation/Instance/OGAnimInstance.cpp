#include "OGAnimInstance.h"

#include "Character/OGCharacter.h"

void UOGAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	
	if (AActor* Owner = GetOwningActor())
	{
		if (AOGCharacter* OGCharacter = Cast<AOGCharacter>(Owner))
		{
			InitializeWithAbilitySystem(OGCharacter->GetAbilitySystemComponent());
		}
	}
}

void UOGAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// 에디터 미리보기 등에서 소유 Pawn이 없으면 이동 값을 초기화합니다.
	const APawn* Pawn = TryGetPawnOwner();
	if (!Pawn)
	{
		Speed = 0.0f;
		MovementDirection = FVector2D::ZeroVector;
		return;
	}

	// 실제 수평 속도를 저장하고 이동 방향을 캐릭터의 Yaw 기준으로 변환합니다.
	const FVector Velocity = Pawn->GetVelocity();
	Speed = Velocity.Size2D();

	const FRotator FacingRotation(0.0, Pawn->GetActorRotation().Yaw, 0.0);
	const FVector LocalDirection = FacingRotation.UnrotateVector(Velocity.GetSafeNormal2D());

	// 로컬 Y(좌우)와 X(전후)를 Blend Space의 X축과 Y축에 각각 배치합니다.
	MovementDirection.X = FMath::Clamp(LocalDirection.Y, -1.0, 1.0);
	MovementDirection.Y = FMath::Clamp(LocalDirection.X, -1.0, 1.0);
}

void UOGAnimInstance::InitializeWithAbilitySystem(UAbilitySystemComponent* ASC)
{
	GameplayTagPropertyMap.Initialize(this, ASC);
}

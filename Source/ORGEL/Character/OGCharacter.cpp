#include "OGCharacter.h"

#include "AbilitySystem/ORAbilitySystemComponent.h"
#include "OGCharacterMovementComponent.h"
#include "ORGameplayTags.h"

AOGCharacter::AOGCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UOGCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// 이동 상태를 매 프레임 태그에 반영하도록 설정합니다.
	PrimaryActorTick.bCanEverTick = true;
}

void AOGCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 최초 이동 전에는 생성 시 정면을 사용하고 이후 커서 회전으로 갱신하지 않도록 합니다.
	LastMovementDirection = GetActorForwardVector().GetSafeNormal2D();
}

void AOGCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// 게임플레이 어빌리티에서 참조할 이동 상태를 갱신합니다.
	UpdateMovingTag();
}

UAbilitySystemComponent* AOGCharacter::GetAbilitySystemComponent() const
{
	return ASC;
}

void AOGCharacter::UpdateMovingTag()
{
	// 빙의 전이나 해제 후에는 플레이어 상태의 태그를 수정하지 않도록 처리합니다.
	if (!ASC || ASC->GetAvatarActor() != this)
	{
		return;
	}

	// 이동 입력 또는 실제 수평 속도가 있는 동안 이동 태그를 유지합니다.
	const UCharacterMovementComponent* Movement = GetCharacterMovement();
	const int32 MovingCount = !Movement->GetCurrentAcceleration().IsNearlyZero() || Movement->Velocity.Size2D() > 3.0f ? 1 : 0;
	if (ASC->GetTagCount(ORGameplayTags::Status_Moving) != MovingCount)
	{
		ASC->SetLooseGameplayTagCount(ORGameplayTags::Status_Moving, MovingCount);
	}
}

FVector AOGCharacter::GetDodgeDirection() const
{
	return LastMovementDirection;
}

void AOGCharacter::UpdateLastMovementDirection(const FVector& WorldDirection)
{
	// 입력이 0인 프레임에는 이전 방향을 보존하고 대각선도 단위 방향으로 기록합니다.
	const FVector Direction = WorldDirection.GetSafeNormal2D();
	if (!Direction.IsNearlyZero())
	{
		LastMovementDirection = Direction;
	}
}

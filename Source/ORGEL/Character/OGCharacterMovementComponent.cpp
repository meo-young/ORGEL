#include "OGCharacterMovementComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "ORGameplayTags.h"

float UOGCharacterMovementComponent::GetMaxSpeed() const
{
	// 빙의 전에는 ASC가 없을 수 있으므로 준비된 경우에만 태그를 검사합니다.
	if (const UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner()))
	{
		if (ASC->HasMatchingGameplayTag(ORGameplayTags::Gameplay_MovementStopped))
		{
			return 0.0f;
		}
	}

	// 이동 제한이 없으면 기존 이동 모드의 속도를 사용합니다.
	return Super::GetMaxSpeed();
}

FRotator UOGCharacterMovementComponent::GetDeltaRotation(float DeltaTime) const
{
	// 빙의 전에는 ASC가 없을 수 있으므로 준비된 경우에만 태그를 검사합니다.
	if (const UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner()))
	{
		if (ASC->HasMatchingGameplayTag(ORGameplayTags::Gameplay_MovementStopped))
		{
			return FRotator::ZeroRotator;
		}
	}

	// 회전 제한이 없으면 기존 이동 모드의 회전량을 사용합니다.
	return Super::GetDeltaRotation(DeltaTime);
}

#include "ORGameplayAbility.h"

#include "AbilitySystem/ORAbilitySystemComponent.h"
#include "Character/OGCharacter.h"
#include "Character/Player/OGPlayerController.h"

UORGameplayAbility::UORGameplayAbility()
{
	// 실행 상태를 소유자별로 보관하고 싱글플레이에서 로컬로 실행합니다.
	{
		InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
		NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalOnly;
	}
}

void UORGameplayAbility::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	// 아바타보다 먼저 부여되었거나 아바타가 교체된 패시브를 실행합니다.
	TryActivateAbilityOnSpawn(ActorInfo, Spec);
}

bool UORGameplayAbility::CanActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	// 아바타 초기화 전에는 발동하지 않고 엔진의 비용, 쿨타임, 태그 조건을 먼저 검사합니다.
	if (!ActorInfo || !ActorInfo->AvatarActor.IsValid() || !ActorInfo->AbilitySystemComponent.IsValid() || !Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	// 같은 시점의 다른 배타적 어빌리티와 충돌하는지 검사합니다.
	return !CastChecked<UORAbilitySystemComponent>(ActorInfo->AbilitySystemComponent.Get())->IsActivationGroupBlocked(ActivationGroup);
}

void UORGameplayAbility::SetCanBeCanceled(bool bCanBeCanceled)
{
	// 교체 가능한 어빌리티는 다음 배타적 어빌리티가 취소할 수 있도록 유지합니다.
	if (!bCanBeCanceled && ActivationGroup == EORAbilityActivationGroup::Exclusive_Replaceable)
	{
		UE_LOG(LogTemp, Warning, TEXT("교체 가능한 어빌리티는 취소 불가 상태로 변경할 수 없습니다: %s"), *GetName());
		return;
	}

	Super::SetCanBeCanceled(bCanBeCanceled);
}

void UORGameplayAbility::TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) const
{
	// 아바타가 없거나 종료 중인 경우와 이미 활성화된 경우에는 실행하지 않도록 합니다.
	if (ActivationPolicy != EORAbilityActivationPolicy::OnSpawn || Spec.IsActive() || !ActorInfo || !ActorInfo->AvatarActor.IsValid() || ActorInfo->AvatarActor->IsActorBeingDestroyed())
	{
		return;
	}

	// 유효한 아바타에서 자동 발동을 시도합니다.
	ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle);
}

AOGPlayerController* UORGameplayAbility::GetOGPlayerControllerFromActorInfo() const
{
	// ActorInfo 초기화 전이거나 컨트롤러 타입이 다르면 nullptr를 반환합니다.
	return CurrentActorInfo ? Cast<AOGPlayerController>(CurrentActorInfo->PlayerController.Get()) : nullptr;
}

UORAbilitySystemComponent* UORGameplayAbility::GetORAbilitySystemComponentFromActorInfo() const
{
	// ActorInfo 초기화 전이거나 ASC 타입이 다르면 nullptr를 반환합니다.
	return CurrentActorInfo ? Cast<UORAbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent.Get()) : nullptr;
}

AOGCharacter* UORGameplayAbility::GetOGCharacterFromActorInfo() const
{
	// 캐릭터가 해제되었거나 다른 종류의 아바타를 사용하면 nullptr를 반환합니다.
	return CurrentActorInfo ? Cast<AOGCharacter>(CurrentActorInfo->AvatarActor.Get()) : nullptr;
}

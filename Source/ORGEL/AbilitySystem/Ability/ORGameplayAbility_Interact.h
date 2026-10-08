#pragma once

#include "CoreMinimal.h"
#include "ORGameplayAbility.h"
#include "ORGameplayAbility_Interact.generated.h"

UCLASS(Abstract)
class ORGEL_API UORGameplayAbility_Interact : public UORGameplayAbility
{
	GENERATED_BODY()

// ─────────────────────────────────────────────────────────────
// Gameplay Ability
// ─────────────────────────────────────────────────────────────
public:
	/** 단발 입력과 독립 발동 그룹 및 상호작용 차단 조건을 설정합니다. */
	UORGameplayAbility_Interact();

	/** 공통 발동 조건과 일시정지 상태를 확인합니다. */
	virtual bool CanActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	/** 현재 대상을 재탐색하고 실행 조건을 확인한 뒤 Interface로 요청합니다. */
	virtual void ActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

};
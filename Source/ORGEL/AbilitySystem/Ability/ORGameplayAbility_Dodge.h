#pragma once

#include "CoreMinimal.h"
#include "ORGameplayAbility.h"
#include "ORGameplayAbility_Dodge.generated.h"

class UAbilityTask_ApplyRootMotionConstantForce;

UCLASS(Abstract)
class ORGEL_API UORGameplayAbility_Dodge : public UORGameplayAbility
{
	GENERATED_BODY()

// ─────────────────────────────────────────────────────────────
// Gameplay Ability
// ─────────────────────────────────────────────────────────────
public:
	/** 유지 입력과 고정 쿨타임 및 구르기 상태 태그를 설정합니다. */
	UORGameplayAbility_Dodge();
	
	/** 이동 입력 방향을 확정하고 무적 효과와 엔진 이동 태스크를 시작합니다. */
	virtual void ActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	/** 이동을 정리하고 종료 시점에 쿨타임을 시작하며 취소 시 자신의 무적을 제거합니다. */
	virtual void EndAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	

// ─────────────────────────────────────────────────────────────
// Dodge Variables
// ─────────────────────────────────────────────────────────────
private:
	/** 종료와 취소 시 구르기 이동 소스를 해제할 엔진 태스크를 보관합니다. */
	UPROPERTY(Transient)
	TObjectPtr<UAbilityTask_ApplyRootMotionConstantForce> MovementTask;

};

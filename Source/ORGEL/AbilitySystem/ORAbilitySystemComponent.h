#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Ability/ORGameplayAbility.h"
#include "ORAbilitySystemComponent.generated.h"

UCLASS()
class ORGEL_API UORAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

// ─────────────────────────────────────────────────────────────
// Ability Lifecycle
// ─────────────────────────────────────────────────────────────
public:
	/** 발동 그룹을 등록하고 교체 가능한 기존 어빌리티를 취소합니다. */
	virtual void NotifyAbilityActivated(FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability) override;

	/** 종료한 어빌리티의 발동 그룹을 해제합니다. */
	virtual void NotifyAbilityEnded(FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, bool bWasCancelled) override;

private:
	/** 회수된 어빌리티의 대기 입력을 함께 제거합니다. */
	virtual void OnRemoveAbility(FGameplayAbilitySpec& AbilitySpec) override;

	/** 활성 어빌리티와 입력 대기 태스크에 누름 이벤트를 전달합니다. */
	virtual void AbilitySpecInputPressed(FGameplayAbilitySpec& Spec) override;

	/** 활성 어빌리티와 입력 대기 태스크에 해제 이벤트를 전달합니다. */
	virtual void AbilitySpecInputReleased(FGameplayAbilitySpec& Spec) override;


// ─────────────────────────────────────────────────────────────
// Ability Input
// ─────────────────────────────────────────────────────────────
public:
	/** 입력 태그에 연결된 어빌리티의 누름과 유지 요청을 수집합니다. */
	void AbilityInputTagPressed(const FGameplayTag& InputTag);

	/** 입력 태그에 연결된 어빌리티의 유지 요청을 해제합니다. */
	void AbilityInputTagReleased(const FGameplayTag& InputTag);

	/** 수집한 입력을 정책과 우선순위에 따라 처리합니다. */
	void ProcessAbilityInput();

	/** 일시정지나 조종 해제 시 저장된 입력과 눌림 상태를 정리합니다. */
	void ClearAbilityInput();


// ─────────────────────────────────────────────────────────────
// Activation Groups
// ─────────────────────────────────────────────────────────────
public:
	/** 현재 배타적 어빌리티가 지정한 발동 그룹을 차단하는지 검사합니다. */
	bool IsActivationGroupBlocked(EORAbilityActivationGroup Group) const;
	

// ─────────────────────────────────────────────────────────────
// Ability Input Variables
// ─────────────────────────────────────────────────────────────
private:
	/** 이번 입력 처리에서 누름 이벤트를 전달할 어빌리티 핸들을 보관합니다. */
	TArray<FGameplayAbilitySpecHandle> InputPressedSpecHandles;

	/** 이번 입력 처리에서 해제 이벤트를 전달할 어빌리티 핸들을 보관합니다. */
	TArray<FGameplayAbilitySpecHandle> InputReleasedSpecHandles;

	/** 키가 눌린 동안 반복 발동을 시도할 어빌리티 핸들을 보관합니다. */
	TArray<FGameplayAbilitySpecHandle> InputHeldSpecHandles;


// ─────────────────────────────────────────────────────────────
// Activation Group Variables
// ─────────────────────────────────────────────────────────────
private:
	/** 발동 그룹별 활성 어빌리티 수를 보관하여 배타적 발동 가능 여부를 판단합니다. */
	int32 ActivationGroupCounts[static_cast<uint8>(EORAbilityActivationGroup::MAX)] = {};
};

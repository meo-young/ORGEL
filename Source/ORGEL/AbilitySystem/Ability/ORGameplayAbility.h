#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "ORGameplayAbility.generated.h"

class AOGPlayerController;
class UORAbilitySystemComponent;
class AOGCharacter;

UENUM()
enum class EORAbilityActivationPolicy : uint8
{
	OnInputTriggered,
	WhileInputActive,
	OnSpawn
};

UENUM()
enum class EORAbilityActivationGroup : uint8
{
	Independent,
	Exclusive_Replaceable,
	Exclusive_Blocking,
	MAX UMETA(Hidden)
};

UCLASS(Abstract)
class ORGEL_API UORGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

// ─────────────────────────────────────────────────────────────
// Gameplay Ability
// ─────────────────────────────────────────────────────────────
public:
	/** 소유자별 인스턴스와 싱글플레이 실행 정책을 설정합니다. */
	UORGameplayAbility();

	/** 아바타가 준비되거나 교체되면 자동 발동 정책을 처리합니다. */
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	/** 기본 조건과 배타적 발동 그룹을 함께 검사합니다. */
	virtual bool CanActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	/** 교체 가능한 그룹이 취소 불가 상태가 되지 않도록 처리합니다. */
	virtual void SetCanBeCanceled(bool bCanBeCanceled) override;


// ─────────────────────────────────────────────────────────────
// Activation
// ─────────────────────────────────────────────────────────────
private:
	/** 유효한 아바타가 준비되면 패시브 발동을 시도합니다. */
	void TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) const;


// ─────────────────────────────────────────────────────────────
// Utilization
// ─────────────────────────────────────────────────────────────
public:
	/** ActorInfo의 플레이어 컨트롤러를 반환하며, 정보가 없거나 타입이 다르면 nullptr를 반환합니다. */
	AOGPlayerController* GetOGPlayerControllerFromActorInfo() const;

	/** ActorInfo의 ORGEL ASC를 반환하며, 정보가 없거나 타입이 다르면 nullptr를 반환합니다. */
	UORAbilitySystemComponent* GetORAbilitySystemComponentFromActorInfo() const;

	/** ActorInfo의 아바타 캐릭터를 반환하며, 정보가 없거나 타입이 다르면 nullptr를 반환합니다. */
	AOGCharacter* GetOGCharacterFromActorInfo() const;


// ─────────────────────────────────────────────────────────────
// Activation Variables
// ─────────────────────────────────────────────────────────────
public:
	/** 입력 시작, 입력 유지 또는 아바타 연결 중 자동 발동을 시도할 시점을 지정합니다. */
	UPROPERTY(EditDefaultsOnly, Category = "변수|발동")
	EORAbilityActivationPolicy ActivationPolicy = EORAbilityActivationPolicy::OnInputTriggered;

	/** 다른 어빌리티와 함께 실행하거나 교체 또는 차단할 발동 그룹을 지정합니다. */
	UPROPERTY(EditDefaultsOnly, Category = "변수|발동")
	EORAbilityActivationGroup ActivationGroup = EORAbilityActivationGroup::Independent;

	/** 동시에 요청된 어빌리티 중 큰 값부터 발동을 시도합니다. */
	UPROPERTY(EditDefaultsOnly, Category = "변수|발동")
	int32 InputPriority = 0;

	/** 입력 해제 시 취소가 필요한 지속형 어빌리티에만 적용합니다. */
	UPROPERTY(EditDefaultsOnly, Category = "변수|발동")
	uint8 bCancelOnInputReleased : 1 = false;

};

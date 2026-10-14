#pragma once

#include "CoreMinimal.h"
#include "ORAttributeSet.h"
#include "ORHealthSet.generated.h"

UCLASS()
class ORGEL_API UORHealthSet : public UORAttributeSet
{
	GENERATED_BODY()

// ─────────────────────────────────────────────────────────────
// Initialization
// ─────────────────────────────────────────────────────────────
public:
	/** 캐릭터별 초기값 적용 전 사용할 기본 체력 상태를 구성합니다. */
	UORHealthSet();

	/** 최초 생성 시 최대 체력과 현재 체력을 설정하고 임시 계산 상태를 초기화합니다. */
	void InitializeHealth(float InMaxHealth);


// ─────────────────────────────────────────────────────────────
// AttributeSet Interface
// ─────────────────────────────────────────────────────────────
public:
	/** 효과 실행 전 체력 값을 저장합니다. */
	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;

	/** 실행된 피해와 회복을 체력에 반영하고 변경 이벤트를 전달합니다. */
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	/** 속성 변경 후 체력 상한과 체력 고갈 상태를 보정합니다. */
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;


// ─────────────────────────────────────────────────────────────
// Attribute Accessors
// ─────────────────────────────────────────────────────────────
public:
	/** 현재 체력의 GAS 속성 접근 함수를 생성합니다. */
	OR_ATTRIBUTE_ACCESSORS(UORHealthSet, Health);

	/** 최대 체력의 GAS 속성 접근 함수를 생성합니다. */
	OR_ATTRIBUTE_ACCESSORS(UORHealthSet, MaxHealth);

	/** 임시 회복량의 GAS 속성 접근 함수를 생성합니다. */
	OR_ATTRIBUTE_ACCESSORS(UORHealthSet, Healing);

	/** 임시 피해량의 GAS 속성 접근 함수를 생성합니다. */
	OR_ATTRIBUTE_ACCESSORS(UORHealthSet, Damage);


// ─────────────────────────────────────────────────────────────
// Attribute Clamp
// ─────────────────────────────────────────────────────────────
protected:
	/** 체력 속성별 허용 범위를 적용합니다. */
	virtual void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const override;


// ─────────────────────────────────────────────────────────────
// Attribute Events
// ─────────────────────────────────────────────────────────────
public:
	/** 초기화와 피해, 회복 및 상한 보정에 따른 실제 체력 변경 정보를 전달합니다. */
	mutable FORAttributeEvent OnHealthChanged;

	/** 최대 체력 변경 정보를 전달합니다. */
	mutable FORAttributeEvent OnMaxHealthChanged;

	/** 체력이 고갈되는 시점을 전달합니다. */
	mutable FORAttributeEvent OnOutOfHealth;


// ─────────────────────────────────────────────────────────────
// Health Variables
// ─────────────────────────────────────────────────────────────
private:
	/** 피해와 회복이 반영된 현재 체력을 관리합니다. */
	UPROPERTY(BlueprintReadOnly, Category = "변수|체력", meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData Health;

	/** 현재 체력의 상한으로 사용할 최대 체력을 관리합니다. */
	UPROPERTY(BlueprintReadOnly, Category = "변수|체력", meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxHealth;

	/** 효과 실행 시 현재 체력에 반영한 뒤 비울 임시 회복량을 관리합니다. */
	UPROPERTY(BlueprintReadOnly, Category = "변수|체력", meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Healing;

	/** 효과 실행 시 현재 체력에서 차감한 뒤 비울 임시 피해량을 관리합니다. */
	UPROPERTY(BlueprintReadOnly, Category = "변수|체력", meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Damage;


// ─────────────────────────────────────────────────────────────
// Health State Variables
// ─────────────────────────────────────────────────────────────
private:
	/** 최초 사망을 확정하고 이후 피해와 회복 및 체력 증가를 차단합니다. */
	uint8 bOutOfHealth : 1;

	/** 효과 실행 전 최대 체력을 저장하여 변경 이벤트에 사용합니다. */
	float MaxHealthBeforeAttributeChange;

	/** 효과 실행 전 현재 체력을 저장하여 변경 이벤트에 사용합니다. */
	float HealthBeforeAttributeChange;
};
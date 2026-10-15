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

	/** 초기 데이터로 체력을 설정하고 사망 상태와 임시 계산값을 초기화합니다. */
	virtual void InitializeSet(const UORCharacterStatData& StatData) override;


// ─────────────────────────────────────────────────────────────
// AttributeSet Interface
// ─────────────────────────────────────────────────────────────
public:
	/** 피해와 회복 효과의 적용 조건과 수치를 검사합니다. */
	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;

	/** 체력 변경 시 사망을 확정하고, 최대 체력을 보정하며 변경 이벤트를 전달합니다. */
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	/** 실행된 피해와 회복을 현재 체력에 반영합니다. */
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;


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
	/** 현재 체력의 실제 변화량과 변경 전후 값을 전달합니다. */
	mutable FORAttributeEvent OnHealthChanged;

	/** 최대 체력의 실제 변화량과 변경 전후 값을 전달합니다. */
	mutable FORAttributeEvent OnMaxHealthChanged;

	/** 최초 체력 고갈 시 실제 변화량과 변경 전후 값을 전달합니다. */
	mutable FORAttributeEvent OnOutOfHealth;

	/** 유효한 피해로 감소한 체력 변화량과 변경 전후 체력을 전달합니다. */
	mutable FORAttributeEvent OnDamageReceived;

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
	
};
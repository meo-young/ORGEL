#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "ORAttributeSet.generated.h"

struct FGameplayEffectSpec;
class UORAbilitySystemComponent;

#define OR_ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/** 능력치의 실제 변화량과 변경 전후 값을 전달합니다. */
DECLARE_MULTICAST_DELEGATE_ThreeParams(FORAttributeEvent, float /*ChangeAmount*/, float /*OldValue*/, float /*NewValue*/);

UCLASS()
class ORGEL_API UORAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

// ─────────────────────────────────────────────────────────────
// Attribute Changes
// ─────────────────────────────────────────────────────────────
public:
	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	virtual void PostAttributeBaseChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) const override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

protected:
	/** 파생 능력치 세트에서 속성별 허용 범위를 적용합니다. */
	virtual void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;


// ─────────────────────────────────────────────────────────────
// Ability Getter
// ─────────────────────────────────────────────────────────────
public:
	/** 소유 액터의 ORGEL ASC를 반환하며, ASC가 없거나 타입이 다르면 nullptr를 반환합니다. */
	UORAbilitySystemComponent* GetORAbilitySystemComponent() const;
};

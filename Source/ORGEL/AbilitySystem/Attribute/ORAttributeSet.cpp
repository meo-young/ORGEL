#include "ORAttributeSet.h"

#include "AbilitySystem/ORAbilitySystemComponent.h"

void UORAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	// 기본값과 현재값 변경에 같은 능력치 보정 규칙을 적용합니다.
	ClampAttribute(Attribute, NewValue);
}

void UORAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	// 효과가 반영된 값에도 파생 세트의 보정 규칙을 적용합니다.
	ClampAttribute(Attribute, NewValue);
}

void UORAttributeSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	// 공통 세트에는 개별 능력치가 없으므로 파생 세트에서 범위를 정의합니다.
}

UORAbilitySystemComponent* UORAttributeSet::GetORAbilitySystemComponent() const
{
	// 소유 액터에서 조회한 ASC를 프로젝트 타입으로 변환합니다.
	return Cast<UORAbilitySystemComponent>(GetOwningAbilitySystemComponent());
}

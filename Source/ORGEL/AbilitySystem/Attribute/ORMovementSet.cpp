#include "ORMovementSet.h"

#include "AbilitySystem/ORAbilitySystemComponent.h"
#include "Data/ORCharacterStatData.h"

UORMovementSet::UORMovementSet()
	: MovementSpeed(0.0f)
{
}

void UORMovementSet::InitializeSet(const UORCharacterStatData& StatData)
{
	// 초기 이동속도를 적용하며 기존 속성 보정 함수에서 허용 범위를 제한합니다.
	GetORAbilitySystemComponent()->SetNumericAttributeBase(GetMovementSpeedAttribute(), StatData.GetMovementSpeed());
}

void UORMovementSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	// 음수 속도와 유효하지 않은 수치가 이동 계산에 전달되지 않도록 처리합니다.
	if (Attribute == GetMovementSpeedAttribute())
	{
		NewValue = FMath::IsFinite(NewValue) ? FMath::Max(NewValue, 0.0f) : 0.0f;
	}
}
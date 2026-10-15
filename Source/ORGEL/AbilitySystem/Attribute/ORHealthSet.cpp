#include "ORHealthSet.h"
#include "AbilitySystem/Attribute/ORAttributeSet.h"
#include "AbilitySystem/ORAbilitySystemComponent.h"
#include "GameplayEffectExtension.h"
#include "ORGameplayTags.h"
#include "Data/ORCharacterStatData.h"

UORHealthSet::UORHealthSet()
	: Health(1.0f)
	, MaxHealth(1.0f)
	, Healing(0.0f)
	, Damage(0.0f)
	, bOutOfHealth(false)
{
}

void UORHealthSet::InitializeSet(const UORCharacterStatData& StatData)
{
	// 명시적인 초기화에서 사망 상태를 해제합니다.
	bOutOfHealth = false;

	// 최대 체력을 최소 1 HP로 보정하고 현재 체력도 같은 값으로 초기화합니다.
	UORAbilitySystemComponent* ASC = GetORAbilitySystemComponent();
	const float InitialMaxHealth = FMath::Max(StatData.GetMaxHealth(), 1.0f);
	ASC->SetNumericAttributeBase(GetMaxHealthAttribute(), InitialMaxHealth);
	ASC->SetNumericAttributeBase(GetHealthAttribute(), InitialMaxHealth);

	// 이전 효과의 임시 계산값이 남지 않도록 초기화합니다.
	ASC->SetNumericAttributeBase(GetHealingAttribute(), 0.0f);
	ASC->SetNumericAttributeBase(GetDamageAttribute(), 0.0f);

	// 기존 초기화 확인 로그를 유지합니다.
	UE_LOG(LogTemp, Warning, TEXT("%f health"), InitialMaxHealth);
}

bool UORHealthSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	if (!Super::PreGameplayEffectExecute(Data))
	{
		return false;
	}
	
	// 사망이 확정된 대상에게는 이번 속성 수정을 실행하지 않습니다.
	if (bOutOfHealth)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] 사망이 확정된 대상의 속성 수정을 거부합니다. 속성: %s"), *GetNameSafe(GetOwningActor()), *Data.EvaluatedData.Attribute.GetName());
		return false;
	}
	
	// 실행되는 효과가 Health를 직접 수정하지 않도록 Damage 또는 Healing 사용을 강제합니다.
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] Health 직접 수정을 거부합니다. Damage 또는 Healing 속성을 사용해야 합니다."), *GetNameSafe(GetOwningActor()));
		return false;
	}
	else if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		// 무적 상태의 대상에게는 피해를 적용하지 않습니다.
		if (GetORAbilitySystemComponent()->HasMatchingGameplayTag(ORGameplayTags::Status_Invulnerable))
		{
			UE_LOG(LogTemp, Warning, TEXT("[%s] 무적 상태이므로 피해 적용을 거부합니다. 피해량: %g"), *GetNameSafe(GetOwningActor()), Data.EvaluatedData.Magnitude);
			return false;
		}
	}
	else if (Data.EvaluatedData.Attribute == GetHealingAttribute())
	{
		// 
	}
	
	return true;
}

void UORHealthSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	
	// 피해를 체력에 반영한 뒤 실제 감소가 발생한 경우에만 피격을 알립니다.
	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		const float OldHealth = GetHealth();
		const float DamageAmount = Data.EvaluatedData.Magnitude;

		SetDamage(0.0f);
		SetHealth(FMath::Clamp(OldHealth - DamageAmount, 0.0f, GetMaxHealth()));

		// 실제 체력 변화량과 변경 전후 체력을 전달합니다.
		const float NewHealth = GetHealth();
		if (DamageAmount > 0.0f && NewHealth < OldHealth)
		{
			OnDamageReceived.Broadcast(NewHealth - OldHealth, OldHealth, NewHealth);
		}
	}
	else if (Data.EvaluatedData.Attribute == GetHealingAttribute())
	{
		// 회복은 피격 알림 없이 체력에 반영합니다.
		SetHealing(0.0f);
		SetHealth(FMath::Clamp(GetHealth() + Data.EvaluatedData.Magnitude, 0.0f, GetMaxHealth()));
	}
}

void UORHealthSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
	
	const float ChangeAmount = NewValue - OldValue;
	
	if (ChangeAmount == 0.0f) return;
	
	if (Attribute == GetHealthAttribute())
	{
		// 알림에서 일반 회복이 실행되어도 부활하지 않도록 최초 사망을 먼저 확정합니다.
		if (NewValue <= 0.0f && !bOutOfHealth)
		{
			bOutOfHealth = true;
			OnOutOfHealth.Broadcast(ChangeAmount, OldValue, NewValue);
		}

		// 콜백 인자로 받은 값을 사용하여 중첩 효과와 이번 변경을 구분합니다.
		OnHealthChanged.Broadcast(ChangeAmount, OldValue, NewValue);
	}
	else if (Attribute == GetMaxHealthAttribute())
	{
		const float OldHealth = GetHealth();
		const float HealthIncrease = FMath::Max(ChangeAmount, 0.0f);
		const float TargetHealth = bOutOfHealth ? 0.0f : FMath::Clamp(OldHealth + HealthIncrease, 0.0f, NewValue);
		
		// 최대 체력 값이 수정되었는데, 현재 체력이 해당 값보다 크다면 최대 체력 값에 맞게 보정합니다.
		if (TargetHealth != OldHealth)
		{
			UORAbilitySystemComponent* ASC = GetORAbilitySystemComponent();
			check(ASC);

			ASC->ApplyModToAttribute(GetHealthAttribute(), EGameplayModOp::Override, TargetHealth);
		}
		
		OnMaxHealthChanged.Broadcast(ChangeAmount, OldValue, NewValue);
	}
}

void UORHealthSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	// 사망 확정 이후에는 일반 체력 변경으로 다시 양수가 되지 않도록 제한합니다.
	if (Attribute == GetHealthAttribute())
	{
		NewValue = bOutOfHealth ? 0.0f : FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
	else if (Attribute == GetMaxHealthAttribute())
	{
		// 최대 체력은 최소 1 HP를 유지합니다.
		NewValue = FMath::Max(NewValue, 1.0f);
	}
}
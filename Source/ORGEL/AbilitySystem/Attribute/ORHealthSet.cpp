#include "ORHealthSet.h"
#include "AbilitySystem/Attribute/ORAttributeSet.h"
#include "AbilitySystem/ORAbilitySystemComponent.h"
#include "GameplayEffectExtension.h"
#include "ORGameplayTags.h"

UORHealthSet::UORHealthSet()
	: Health(1.0f)
	, MaxHealth(1.0f)
	, Healing(0.0f)
	, Damage(0.0f)
	, bOutOfHealth(false)
{
}

void UORHealthSet::InitializeHealth(float InMaxHealth)
{
	// 명시적인 초기화에서만 사망 상태를 해제합니다.
	bOutOfHealth = false;

	// 등록된 ASC를 통해 최대 체력과 현재 체력을 같은 값으로 설정합니다.
	UORAbilitySystemComponent* ASC = GetORAbilitySystemComponent();
	const float InitialMaxHealth = FMath::Max(InMaxHealth, 1.0f);
	ASC->SetNumericAttributeBase(GetMaxHealthAttribute(), InitialMaxHealth);
	ASC->SetNumericAttributeBase(GetHealthAttribute(), InitialMaxHealth);

	// 초기화 시 피해와 회복의 임시 계산값을 비웁니다.
	ASC->SetNumericAttributeBase(GetHealingAttribute(), 0.0f);
	ASC->SetNumericAttributeBase(GetDamageAttribute(), 0.0f);

	// 기존 초기화 확인 로그를 유지합니다.
	UE_LOG(LogTemp, Warning, TEXT("%f health"), InitialMaxHealth);
}

bool UORHealthSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	// --- 1. 공통 적용 조건 검사 ---------------------------------
	if (!Super::PreGameplayEffectExecute(Data))
	{
		return false;
	}
	
	// 사망이 확정된 대상에게는 추가 속성 수정을 적용하지 않습니다.
	if (bOutOfHealth)
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] 사망이 확정된 대상의 속성 수정을 거부합니다. 속성: %s"), *GetNameSafe(GetOwningActor()), *Data.EvaluatedData.Attribute.GetName());
		return false;
	}
	
	// 현재 체력의 직접 수정을 차단하고 피해 또는 회복 속성을 사용하도록 제한합니다.
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] Health 직접 수정을 거부합니다. Damage 또는 Healing 속성을 사용해야 합니다."), *GetNameSafe(GetOwningActor()));
		return false;
	}
	// -----------------------------------------------------------
	
	
	// --- 2. 속성별 유효성 검사 -----------------------------------
	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		// 피해량은 유한한 양수이며 기본값에 더하는 방식이어야 합니다.
		if (!FMath::IsFinite(Data.EvaluatedData.Magnitude) || Data.EvaluatedData.Magnitude <= 0.0f || Data.EvaluatedData.ModifierOp != EGameplayModOp::AddBase)
		{
			UE_LOG(LogTemp, Error, TEXT("[%s] 유효하지 않은 피해 효과를 거부합니다. 피해량: %g, 연산: %d"), *GetNameSafe(GetOwningActor()), Data.EvaluatedData.Magnitude, static_cast<int32>(Data.EvaluatedData.ModifierOp.GetValue()));
			return false;
		}
		
		// 무적 상태의 대상에게는 피해를 적용하지 않습니다.
		if (GetORAbilitySystemComponent()->HasMatchingGameplayTag(ORGameplayTags::Status_Invulnerable))
		{
			UE_LOG(LogTemp, Warning, TEXT("[%s] 무적 상태이므로 피해 적용을 거부합니다. 피해량: %g"), *GetNameSafe(GetOwningActor()), Data.EvaluatedData.Magnitude);
			return false;
		}
	}
	else if (Data.EvaluatedData.Attribute == GetHealingAttribute())
	{
		// 회복량은 유한한 양수이며 기본값에 더하는 방식이어야 합니다.
		if (!FMath::IsFinite(Data.EvaluatedData.Magnitude) || Data.EvaluatedData.Magnitude <= 0.0f || Data.EvaluatedData.ModifierOp != EGameplayModOp::AddBase)
		{
			UE_LOG(LogTemp, Error, TEXT("[%s] 유효하지 않은 회복 효과를 거부합니다. 회복량: %g, 연산: %d"), *GetNameSafe(GetOwningActor()), Data.EvaluatedData.Magnitude, static_cast<int32>(Data.EvaluatedData.ModifierOp.GetValue()));
			return false;
		}
	}
	// -----------------------------------------------------------
	
	
	// --- 3. 변경 전 상태 저장 -----------------------------------
	HealthBeforeAttributeChange = GetHealth();
	MaxHealthBeforeAttributeChange = GetMaxHealth();
	// -----------------------------------------------------------
	
	return true;
}

void UORHealthSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	
	// --- 1. 효과 정보 및 변경 전 상태 보관 ----------------------------------
	// 중첩 효과가 멤버 값을 덮어써도 이번 효과의 이전 값을 유지합니다.
	const float OldHealth = HealthBeforeAttributeChange;
	const float OldMaxHealth = MaxHealthBeforeAttributeChange;
	
	// 예: 플레이어가 발사한 총알이 적중 → Instigator : 플레이어 / Causer : 총알
	const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetEffectContext();
	AActor* Instigator = EffectContext.GetOriginalInstigator();
	AActor* Causer = EffectContext.GetEffectCauser();
	// -----------------------------------------------------------
	
	
	// --- 2. 속성별 효과 반영 ------------------------------------
	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth() - GetDamage(), 0.0f, GetMaxHealth()));
		SetDamage(0.0f);
	}
	else if (Data.EvaluatedData.Attribute == GetHealingAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth() + GetHealing(), 0.0f, GetMaxHealth()));
		SetHealing(0.0f);
	}
	
	// 첫 이벤트를 전달하기 전에 이번 효과의 처리 결과를 고정합니다.
	const float NewHealth = GetHealth();
	const float NewMaxHealth = GetMaxHealth();
	// -----------------------------------------------------------
	
	
	// --- 3. 체력 변경 및 소진 알림 ------------------------------
	if (Data.EvaluatedData.Attribute == GetMaxHealthAttribute())
	{
		OnMaxHealthChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, OldMaxHealth, NewMaxHealth);
	}
	
	if (NewHealth != OldHealth)
	{
		if ((NewHealth <= 0.0f) && !bOutOfHealth)
		{
			bOutOfHealth = true;
			OnOutOfHealth.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, OldHealth, NewHealth);
		}
		
		OnHealthChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, OldHealth, NewHealth);
	}
	// -----------------------------------------------------------
}

void UORHealthSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
	
	if (Attribute == GetMaxHealthAttribute())
	{
		// 최대 체력 값이 수정되었는데, 현재 체력이 해당 값보다 크다면 최대 체력 값에 맞게 보정합니다.
		if (GetHealth() > NewValue)
		{
			UORAbilitySystemComponent* ASC = GetORAbilitySystemComponent();
			check(ASC);

			ASC->ApplyModToAttribute(GetHealthAttribute(), EGameplayModOp::Override, NewValue);
		}
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
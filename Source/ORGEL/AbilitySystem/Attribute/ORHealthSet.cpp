#include "ORHealthSet.h"
#include "AbilitySystem/Attribute/ORAttributeSet.h"
#include "AbilitySystem/ORAbilitySystemComponent.h"
#include "GameplayEffectExtension.h"

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
	if (!Super::PreGameplayEffectExecute(Data))
	{
		return false;
	}

	// 수치 변화가 적용되기 전 값을 저장합니다.
	HealthBeforeAttributeChange = GetHealth();
	MaxHealthBeforeAttributeChange = GetMaxHealth();

	return true;
}

void UORHealthSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	// 치트 모드를 적용한 경우 해당 수치를 1.0으로 설정하면 캐릭터는 체력이 1 이하로 떨어지지 않습니다.
	float MinimumHealth = 0.0f;
	
	// 효과에 대한 책임을 가지는 객체와 적용을 담당하는 객체를 추출합니다.
	// ex) 플레이어가 발사한 총알이 적중 → Instigator : 플레이어 / Causer : 총알
	const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetEffectContext();
	AActor* Instigator = EffectContext.GetOriginalInstigator();
	AActor* Causer = EffectContext.GetEffectCauser();

	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		// Damage 값만큼 현재 체력을 감소시킨 후 보정합니다.
		SetHealth(FMath::Clamp(GetHealth() - GetDamage(), MinimumHealth, GetMaxHealth()));
		SetDamage(0.0f);
	}
	else if (Data.EvaluatedData.Attribute == GetHealingAttribute())
	{
		// Healing 값만큼 현재 체력을 증가시킨 후 보정합니다.
		SetHealth(FMath::Clamp(GetHealth() + GetHealing(), MinimumHealth, GetMaxHealth()));
		SetHealing(0.0f);
	}
	else if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), MinimumHealth, GetMaxHealth()));
	}
	else if (Data.EvaluatedData.Attribute == GetMaxHealthAttribute())
	{
		OnMaxHealthChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, MaxHealthBeforeAttributeChange, GetMaxHealth());
	}

	// 수치 변화 적용 전/후의 값이 동일하지 않은 경우에만 브로드캐스팅합니다.
	if (GetHealth() != HealthBeforeAttributeChange)
	{
		OnHealthChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, HealthBeforeAttributeChange, GetHealth());
	}

	// 사망 브로드캐스팅은 단 한 번만 진행합니다.
	if ((GetHealth() <= 0.0f) && !bOutOfHealth)
	{
		OnOutOfHealth.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, HealthBeforeAttributeChange, GetHealth());
	}

	// 사망 상태를 갱신합니다.
	bOutOfHealth = (GetHealth() <= 0.0f);
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

	if (bOutOfHealth && (GetHealth() > 0.0f))
	{
		bOutOfHealth = false;
	}
}

void UORHealthSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
	else if (Attribute == GetMaxHealthAttribute())
	{
		NewValue = FMath::Max(NewValue, 1.0f);
	}
}
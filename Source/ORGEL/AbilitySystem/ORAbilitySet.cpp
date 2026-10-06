#include "ORAbilitySet.h"

#include "ORAbilitySystemComponent.h"

void FORAbilitySet_GrantedHandles::AddAbilitySpecHandle(FGameplayAbilitySpecHandle Handle)
{
	// 실제 부여된 어빌리티만 회수 목록에 등록합니다.
	if (Handle.IsValid())
	{
		AbilitySpecHandles.AddUnique(Handle);
	}
}

void FORAbilitySet_GrantedHandles::TakeFromAbilitySystem(UORAbilitySystemComponent* ASC)
{
	// 회수 콜백이 재진입해도 같은 핸들을 다시 처리하지 않도록 먼저 이동합니다.
	const TArray<FGameplayAbilitySpecHandle> Handles = MoveTemp(AbilitySpecHandles);
	for (const FGameplayAbilitySpecHandle& Handle : Handles)
	{
		ASC->ClearAbility(Handle);
	}
}

void UORAbilitySet::GiveToAbilitySystem(UORAbilitySystemComponent* ASC, FORAbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject) const
{
	// 설정된 클래스를 레벨과 입력 태그로 묶어 부여합니다.
	for (const FORAbilitySet_GameplayAbility& Entry : GrantedGameplayAbilities)
	{
		if (!Entry.Ability)
		{
			UE_LOG(LogTemp, Warning, TEXT("AbilitySet의 비어 있는 어빌리티 항목을 건너뜁니다: %s"), *GetName());
			continue;
		}

		// 패시브는 입력 태그 없이도 부여할 수 있도록 처리합니다.
		FGameplayAbilitySpec Spec(Entry.Ability, Entry.AbilityLevel);
		Spec.SourceObject = SourceObject;
		if (Entry.InputTag.IsValid())
		{
			Spec.GetDynamicSpecSourceTags().AddTag(Entry.InputTag);
		}

		// 회수를 요청한 호출자에게만 부여 핸들을 보관합니다.
		const FGameplayAbilitySpecHandle Handle = ASC->GiveAbility(Spec);
		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddAbilitySpecHandle(Handle);
		}
	}
}

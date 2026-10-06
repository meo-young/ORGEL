#include "ORAbilitySystemComponent.h"

#include "GameplayEffect.h"

void UORAbilitySystemComponent::NotifyAbilityActivated(FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability)
{
	// 발동 알림에서 즉시 종료되더라도 종료 쪽 카운트와 맞도록 먼저 등록합니다.
	if (const UORGameplayAbility* ORAbility = Cast<UORGameplayAbility>(Ability))
	{
		++ActivationGroupCounts[static_cast<uint8>(ORAbility->ActivationGroup)];
		if (ORAbility->ActivationGroup != EORAbilityActivationGroup::Independent)
		{
			// 새 어빌리티 자신을 제외하고 교체 가능한 활성 인스턴스만 취소합니다.
			ABILITYLIST_SCOPE_LOCK();
			for (const FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
			{
				const TArray<UGameplayAbility*> Instances = Spec.GetAbilityInstances();
				for (UGameplayAbility* Instance : Instances)
				{
					const UORGameplayAbility* Other = Cast<UORGameplayAbility>(Instance);
					if (Other && Other != Ability && Other->IsActive() && Other->ActivationGroup == EORAbilityActivationGroup::Exclusive_Replaceable && Other->CanBeCanceled())
					{
						Instance->CancelAbility(Spec.Handle, AbilityActorInfo.Get(), Instance->GetCurrentActivationInfo(), false);
					}
				}
			}
		}
	}

	Super::NotifyAbilityActivated(Handle, Ability);
}

void UORAbilitySystemComponent::NotifyAbilityEnded(FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, bool bWasCancelled)
{
	// 종료 콜백에서 새 어빌리티를 실행할 수 있도록 먼저 그룹을 해제합니다.
	if (const UORGameplayAbility* ORAbility = Cast<UORGameplayAbility>(Ability))
	{
		int32& Count = ActivationGroupCounts[static_cast<uint8>(ORAbility->ActivationGroup)];
		check(Count > 0);
		--Count;
	}

	Super::NotifyAbilityEnded(Handle, Ability, bWasCancelled);
}

void UORAbilitySystemComponent::OnRemoveAbility(FGameplayAbilitySpec& AbilitySpec)
{
	// 회수된 어빌리티를 다음 프레임의 발동 후보에서 제외합니다.
	InputPressedSpecHandles.Remove(AbilitySpec.Handle);
	InputReleasedSpecHandles.Remove(AbilitySpec.Handle);
	InputHeldSpecHandles.Remove(AbilitySpec.Handle);

	Super::OnRemoveAbility(AbilitySpec);
}

void UORAbilitySystemComponent::AbilitySpecInputPressed(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputPressed(Spec);

	// 싱글플레이에서도 입력 대기 태스크가 사용하는 엔진 이벤트를 전달합니다.
	for (UGameplayAbility* Instance : Spec.GetAbilityInstances())
	{
		if (Instance->IsActive())
		{
			InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, Spec.Handle, Instance->GetCurrentActivationInfo().GetActivationPredictionKey());
		}
	}
}

void UORAbilitySystemComponent::AbilitySpecInputReleased(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputReleased(Spec);

	// 입력 해제를 기다리는 태스크가 활성 인스턴스를 식별하도록 이벤트를 전달합니다.
	for (UGameplayAbility* Instance : Spec.GetAbilityInstances())
	{
		if (Instance->IsActive())
		{
			InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, Spec.Handle, Instance->GetCurrentActivationInfo().GetActivationPredictionKey());
		}
	}
}

void UORAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	// 입력으로 활성화할 어빌리티를 정확한 태그로 찾아 중복 없이 보관합니다.
	ABILITYLIST_SCOPE_LOCK();
	for (const FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{
		if (Spec.Ability && Spec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			InputPressedSpecHandles.AddUnique(Spec.Handle);
			InputHeldSpecHandles.AddUnique(Spec.Handle);
		}
	}
}

void UORAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	// 해제 이벤트는 한 번 전달하고 유지 목록에서는 즉시 제거합니다.
	ABILITYLIST_SCOPE_LOCK();
	for (const FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{
		if (Spec.Ability && Spec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			InputReleasedSpecHandles.AddUnique(Spec.Handle);
			InputHeldSpecHandles.Remove(Spec.Handle);
		}
	}
}

void UORAbilitySystemComponent::ProcessAbilityInput()
{
	// 초기화 전이나 아바타 해제 후에는 저장된 입력을 실행하지 않도록 처리합니다.
	if (!AbilityActorInfo.IsValid() || !AbilityActorInfo->AvatarActor.IsValid())
	{
		ClearAbilityInput();
		return;
	}

	// 콜백이 어빌리티나 입력을 변경해도 현재 순회가 무효화되지 않도록 보관합니다.
	ABILITYLIST_SCOPE_LOCK();
	const TArray<FGameplayAbilitySpecHandle> PressedHandles = MoveTemp(InputPressedSpecHandles);
	const TArray<FGameplayAbilitySpecHandle> ReleasedHandles = MoveTemp(InputReleasedSpecHandles);
	const TArray<FGameplayAbilitySpecHandle> HeldHandles = InputHeldSpecHandles;
	TArray<FGameplayAbilitySpecHandle> AbilitiesToActivate;

	// 유지 정책은 쿨타임이나 행동 제한이 끝나는 프레임에 다시 발동을 시도합니다.
	for (const FGameplayAbilitySpecHandle& Handle : HeldHandles)
	{
		if (FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(Handle))
		{
			const UORGameplayAbility* Ability = Cast<UORGameplayAbility>(Spec->Ability);
			if (Ability && !Spec->IsActive() && Ability->ActivationPolicy == EORAbilityActivationPolicy::WhileInputActive)
			{
				Spec->InputPressed = true;
				AbilitiesToActivate.AddUnique(Handle);
			}
		}
	}

	// 최초 누름은 활성 어빌리티에 전달하거나 입력 발동 후보에 추가합니다.
	for (const FGameplayAbilitySpecHandle& Handle : PressedHandles)
	{
		if (FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(Handle))
		{
			Spec->InputPressed = true;
			if (Spec->IsActive())
			{
				AbilitySpecInputPressed(*Spec);
			}
			else if (const UORGameplayAbility* Ability = Cast<UORGameplayAbility>(Spec->Ability))
			{
				// 같은 프레임에 눌렀다 놓은 짧은 입력도 한 번 발동하도록 처리합니다.
				if (Ability->ActivationPolicy != EORAbilityActivationPolicy::OnSpawn)
				{
					AbilitiesToActivate.AddUnique(Handle);
				}
			}
		}
	}

	// 바인딩 순서 대신 어빌리티에 지정한 우선순위로 동시 입력을 정렬합니다.
	AbilitiesToActivate.StableSort([this](const FGameplayAbilitySpecHandle& Left, const FGameplayAbilitySpecHandle& Right)
	{
		const FGameplayAbilitySpec* LeftSpec = FindAbilitySpecFromHandle(Left);
		const FGameplayAbilitySpec* RightSpec = FindAbilitySpecFromHandle(Right);
		const UORGameplayAbility* LeftAbility = LeftSpec ? Cast<UORGameplayAbility>(LeftSpec->Ability) : nullptr;
		const UORGameplayAbility* RightAbility = RightSpec ? Cast<UORGameplayAbility>(RightSpec->Ability) : nullptr;
		return (LeftAbility ? LeftAbility->InputPriority : 0) > (RightAbility ? RightAbility->InputPriority : 0);
	});

	// 앞선 어빌리티가 설정한 태그와 배타적 그룹을 후속 발동에서 다시 검사합니다.
	for (const FGameplayAbilitySpecHandle& Handle : AbilitiesToActivate)
	{
		TryActivateAbility(Handle);
	}

	// 입력 해제와 어빌리티 취소는 분리하여 구르기 같은 완결 동작을 유지합니다.
	for (const FGameplayAbilitySpecHandle& Handle : ReleasedHandles)
	{
		if (FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(Handle))
		{
			Spec->InputPressed = false;
			if (Spec->IsActive())
			{
				AbilitySpecInputReleased(*Spec);
				const UORGameplayAbility* Ability = Cast<UORGameplayAbility>(Spec->Ability);
				if (Ability && Ability->bCancelOnInputReleased)
				{
					CancelAbilityHandle(Handle);
				}
			}
		}
	}
}

void UORAbilitySystemComponent::ClearAbilityInput()
{
	// 다음 프레임에 유지 입력이 재실행되지 않도록 대기 목록을 먼저 비웁니다.
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
	InputHeldSpecHandles.Reset();

	// 입력을 기다리는 활성 태스크에도 해제를 전달하여 눌림 상태가 남지 않도록 처리합니다.
	ABILITYLIST_SCOPE_LOCK();
	for (FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{
		if (Spec.InputPressed)
		{
			AbilitySpecInputReleased(Spec);
			const UORGameplayAbility* Ability = Cast<UORGameplayAbility>(Spec.Ability);
			if (Ability && Ability->bCancelOnInputReleased && Spec.IsActive())
			{
				CancelAbilityHandle(Spec.Handle);
			}
		}
	}
}

bool UORAbilitySystemComponent::IsActivationGroupBlocked(EORAbilityActivationGroup Group) const
{
	// 독립 그룹은 허용하고 배타적 그룹은 차단 그룹의 활성 개수를 확인합니다.
	return Group != EORAbilityActivationGroup::Independent && ActivationGroupCounts[static_cast<uint8>(EORAbilityActivationGroup::Exclusive_Blocking)] > 0;
}
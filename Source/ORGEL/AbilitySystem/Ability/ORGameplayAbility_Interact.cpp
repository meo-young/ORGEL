#include "ORGameplayAbility_Interact.h"

#include "Character/Player/OGPlayer.h"
#include "Interact/Interactable.h"
#include "Interact/InteractionComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ORGameplayTags.h"

UORGameplayAbility_Interact::UORGameplayAbility_Interact()
{
	// 누르는 순간에만 실행하며 이동과 공격을 불필요하게 취소하지 않도록 설정합니다.
	{
		ActivationPolicy = EORAbilityActivationPolicy::OnInputTriggered;
		ActivationGroup = EORAbilityActivationGroup::Independent;
		InputPriority = 0;
		bCancelOnInputReleased = false;
	}

	// 행동 제한을 유발한 시스템이 부여한 전용 태그로 발동을 차단합니다.
	ActivationBlockedTags.AddTag(ORGameplayTags::Gameplay_InteractionBlocked);
}

bool UORGameplayAbility_Interact::CanActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	// 기존 GAS 조건과 프로젝트의 발동 정책을 먼저 확인합니다.
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	// 입력 경로 외부에서 발동을 요청해도 일시정지 중에는 실행하지 않도록 처리합니다.
	return !UGameplayStatics::IsGamePaused(ActorInfo->AvatarActor.Get());
}

void UORGameplayAbility_Interact::ActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	UE_LOG(LogTemp, Warning, TEXT("상호작용 시도"))
	
	// 입력 시점의 위치를 기준으로 상호작용 대상을 다시 선정합니다.
	AOGPlayer* Player = CastChecked<AOGPlayer>(ActorInfo->AvatarActor.Get());
	UInteractionComponent* InteractionComponent = Player->GetInteractionComponent();
	InteractionComponent->RefreshInteractionTarget();

	// 호출 가능한 대상이 있을 때만 요청하며 실행 조건 검사는 대상에 맡깁니다.
	IInteractable* Interactable = Cast<IInteractable>(InteractionComponent->GetCurrentTarget());
	const bool bSucceeded = Interactable && Interactable->TryInteract(Player);

	// 대상의 실행 중 이미 취소되었다면 중복 종료하지 않도록 처리합니다.
	if (IsActive())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, false, !bSucceeded);
	}
}
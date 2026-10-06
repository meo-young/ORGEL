#include "ORGameplayAbility_Dodge.h"

#include "Abilities/Tasks/AbilityTask_ApplyRootMotionConstantForce.h"
#include "AbilitySystem/ORAbilitySystemComponent.h"
#include "Character/OGCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/RootMotionSource.h"
#include "ORGameplayTags.h"

UORGameplayAbility_Dodge::UORGameplayAbility_Dodge()
{
	// 기존 ASC의 유지 입력 재시도와 배타적 발동 관리를 재사용합니다.
	{
		ActivationPolicy = EORAbilityActivationPolicy::WhileInputActive;
		ActivationGroup = EORAbilityActivationGroup::Exclusive_Blocking;
		InputPriority = 100;
		bCancelOnInputReleased = false;
	}

	// 구르기 동안 일반 이동과 조준을 차단하고 종료 후 구르기만 제한합니다.
	{
		ActivationOwnedTags.AddTag(ORGameplayTags::Status_Dodging);
		ActivationOwnedTags.AddTag(ORGameplayTags::Gameplay_MovementStopped);
		ActivationBlockedTags.AddTag(ORGameplayTags::Gameplay_MovementStopped);
	}
}

void UORGameplayAbility_Dodge::ActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	// 쿨타임은 종료 시 적용하므로 발동 시에는 비용 검사만 확정합니다.
	if (!CommitAbilityCost(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, false, true);
		return;
	}
	
	// 현재 또는 마지막 이동 입력을 사용하고 구르기 도중 방향과 속도를 변경하지 않도록 처리합니다.
	AOGCharacter* Character = GetOGCharacterFromActorInfo();
	const FVector Direction = Character->GetDodgeDirection();
	const float DodgeSpeed = Character->GetCharacterMovement()->MaxWalkSpeed * 2.0f;

	// 일반 이동 모드는 유지하고 엔진의 캡슐 충돌과 Root Motion 이동을 사용합니다.
	Character->ConsumeMovementInputVector();
	MovementTask = UAbilityTask_ApplyRootMotionConstantForce::ApplyRootMotionConstantForce(this, TEXT("DodgeMovement"), Direction, DodgeSpeed, 0.25f, false, nullptr, ERootMotionFinishVelocityMode::ClampVelocity, FVector::ZeroVector, 0.0f, true);
	
	// 엔진 기본값의 마지막 프레임 전체 이동을 해제하여 지속 시간을 넘긴 거리 증가를 방지합니다.
	// 지금까지 이동한 시간이 0.39초이고 프레임 길이가 0.04초이어도 0.01초만 이동합니다.
	Character->GetCharacterMovement()->GetRootMotionSource(TEXT("DodgeMovement"))->Settings.UnSetFlag(ERootMotionSourceSettingsFlags::DisablePartialEndTick);
	MovementTask->OnFinish.AddDynamic(this, &ThisClass::K2_EndAbility);
	MovementTask->ReadyForActivation();
}

void UORGameplayAbility_Dodge::EndAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	// 중복 종료로 쿨타임이 갱신되지 않도록 엔진의 활성 상태를 검사합니다.
	if (!IsEndAbilityValid(Handle, ActorInfo))
	{
		return;
	}

	// 실제 이동을 시작한 경우에만 태스크를 정리하고 종료 쿨타임을 적용합니다.
	if (MovementTask)
	{
		MovementTask->OnFinish.RemoveDynamic(this, &ThisClass::K2_EndAbility);
		MovementTask->EndTask();
		MovementTask = nullptr;
		ApplyCooldown(Handle, ActorInfo, ActivationInfo);
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, false, bWasCancelled);
}

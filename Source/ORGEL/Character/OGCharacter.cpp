#include "OGCharacter.h"

#include "AIController.h"
#include "BrainComponent.h"
#include "AbilitySystem/ORAbilitySystemComponent.h"
#include "OGCharacterMovementComponent.h"
#include "ORGameplayTags.h"
#include "AbilitySystem/Attribute/ORHealthSet.h"

AOGCharacter::AOGCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UOGCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// 이동 상태를 매 프레임 태그에 반영하도록 설정합니다.
	PrimaryActorTick.bCanEverTick = true;
}

void AOGCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 최초 이동 전에는 생성 시 정면을 사용하고 이후 커서 회전으로 갱신하지 않도록 합니다.
	LastMovementDirection = GetActorForwardVector().GetSafeNormal2D();
}

void AOGCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// 게임플레이 어빌리티에서 참조할 이동 상태를 갱신합니다.
	UpdateMovingTag();
}

void AOGCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// 캐릭터 제거 후 체력 이벤트가 전달되지 않도록 연결을 해제합니다.
	UnbindHealthEvents();
	
	Super::EndPlay(EndPlayReason);
}

UAbilitySystemComponent* AOGCharacter::GetAbilitySystemComponent() const
{
	return ASC;
}

void AOGCharacter::UpdateMovingTag()
{
	// 빙의 전이나 해제 후에는 플레이어 상태의 태그를 수정하지 않도록 처리합니다.
	if (!ASC || ASC->GetAvatarActor() != this)
	{
		return;
	}

	// 이동 입력 또는 실제 수평 속도가 있는 동안 이동 태그를 유지합니다.
	const UCharacterMovementComponent* Movement = GetCharacterMovement();
	const int32 MovingCount = !Movement->GetCurrentAcceleration().IsNearlyZero() || Movement->Velocity.Size2D() > 3.0f ? 1 : 0;
	if (ASC->GetTagCount(ORGameplayTags::Status_Moving) != MovingCount)
	{
		ASC->SetLooseGameplayTagCount(ORGameplayTags::Status_Moving, MovingCount);
	}
}

void AOGCharacter::FinishDeath()
{
	// 빙의가 해제된 캐릭터의 이전 애니메이션 알림을 무시합니다.
	if (!ASC || ASC->GetAvatarActor() != this)
	{
		return;
	}

	// 사망 시작 전의 호출과 중복 완료 알림을 무시합니다.
	if (!ASC->HasMatchingGameplayTag(ORGameplayTags::Status_Dead) || ASC->HasMatchingGameplayTag(ORGameplayTags::Status_DeathFinished))
	{
		return;
	}

	// 완료 알림보다 먼저 상태를 확정하고 마지막 사망 포즈를 유지합니다.
	ASC->AddLooseGameplayTag(ORGameplayTags::Status_DeathFinished);
	GetMesh()->bPauseAnims = true;

	// 실제 사망 애니메이션의 완료 시점을 전달합니다.
	OnDeathFinished.Broadcast(this);
}

void AOGCharacter::BindHealthEvents()
{
	// 같은 체력 세트에 반복 연결되어도 구독은 하나만 유지합니다.
	UnbindHealthEvents();

	// 체력 고갈 알림을 캐릭터의 사망 처리에 연결합니다.
	const UORHealthSet* HealthSet = ASC->GetSetChecked<UORHealthSet>();
	HealthSet->OnOutOfHealth.AddWeakLambda(this, [this](float, float, float)
	{
		StartDeath();
	});
}

void AOGCharacter::UnbindHealthEvents()
{
	// 최초 연결 전이나 플레이어의 ASC 연결 해제 후에는 구독 해제를 생략합니다.
	if (!ASC)
	{
		return;
	}

	// 다른 캐릭터가 ASC의 아바타가 되었더라도 이 캐릭터의 구독은 제거합니다.
	ASC->GetSetChecked<UORHealthSet>()->OnOutOfHealth.RemoveAll(this);
}

void AOGCharacter::StartDeath()
{
	// 현재 아바타가 아닌 캐릭터와 이미 사망을 시작한 캐릭터의 요청을 무시합니다.
	if (!ASC || ASC->GetAvatarActor() != this || ASC->HasMatchingGameplayTag(ORGameplayTags::Status_Dead))
	{
		return;
	}

	// 어빌리티 종료 콜백보다 먼저 사망 상태와 행동 제한을 적용합니다.
	ASC->SetLooseGameplayTagCount(ORGameplayTags::Status_Dead, 1);
	ASC->AddLooseGameplayTag(ORGameplayTags::Gameplay_MovementStopped);
	ASC->AddLooseGameplayTag(ORGameplayTags::Gameplay_InteractionBlocked);

	// 구르기 등의 이동 태스크를 종료한 다음 실제 이동을 중지합니다.
	ASC->CancelAllAbilities();
	ConsumeMovementInputVector();
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();

	// 캐릭터의 이동 상태 갱신과 조준 갱신을 중지합니다.
	SetActorTickEnabled(false);
	
	// 접촉 공격과 피격 충돌을 차단하며 별도로 생성된 투사체는 유지합니다.
	SetActorEnableCollision(false);

	// AI의 경로 이동과 행동 로직을 중지합니다.
	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		AIController->StopMovement();

		if (UBrainComponent* BrainComponent = AIController->GetBrainComponent())
		{
			BrainComponent->StopLogic(TEXT("Death"));
		}
	}

	// Pawn의 조작만 중지하여 PlayerController의 ESC 입력은 유지합니다.
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		DisableInput(PlayerController);
	}
	
	// 방 클리어와 음악 처리에 사용할 사망 시작 시점을 전달합니다.
	OnDeathStarted.Broadcast(this);

	// 기존 행동 몽타주와 이동용 Root Motion이 사망 연출에 섞이지 않도록 처리합니다.
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DeathMontage)
	{
		AnimInstance->Montage_Play(DeathMontage);
		return;
	}

	// 에셋이 준비되지 않은 경우에도 사망 상태가 미완료로 남지 않도록 처리합니다.
	UE_LOG(LogTemp, Warning, TEXT("사망 몽타주를 재생하지 못해 사망을 즉시 완료합니다: %s"), *GetName());
	FinishDeath();
}

FVector AOGCharacter::GetDodgeDirection() const
{
	return LastMovementDirection;
}

void AOGCharacter::UpdateLastMovementDirection(const FVector& WorldDirection)
{
	// 입력이 0인 프레임에는 이전 방향을 보존하고 대각선도 단위 방향으로 기록합니다.
	const FVector Direction = WorldDirection.GetSafeNormal2D();
	if (!Direction.IsNearlyZero())
	{
		LastMovementDirection = Direction;
	}
}

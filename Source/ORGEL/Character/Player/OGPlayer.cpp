#include "OGPlayer.h"

#include "Camera/OGCameraComponent.h"
#include "Camera/OGSpringArmComponent.h"
#include "AbilitySystem/ORAbilitySystemComponent.h"
#include "OGPlayerState.h"
#include "ORGameplayTags.h"
#include "Input/ORInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "Interact/InteractionComponent.h"

AOGPlayer::AOGPlayer(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// 이동 방향과 조준 방향을 분리하고 매 프레임 커서 조준을 갱신하도록 설정합니다.
	{
		PrimaryActorTick.bCanEverTick = true;
		bUseControllerRotationPitch = false;
		bUseControllerRotationYaw = false;
		bUseControllerRotationRoll = false;
		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		GetCharacterMovement()->MaxWalkSpeed = 500.0f;
	}

	// 플레이어의 회전과 무관한 고정 탑뷰 카메라 암을 생성합니다.
	{
		SpringArmComponent = CreateDefaultSubobject<UOGSpringArmComponent>(TEXT("SpringArmComponent"));
		SpringArmComponent->SetupAttachment(RootComponent);
		SpringArmComponent->SetUsingAbsoluteRotation(true);
		SpringArmComponent->SetRelativeRotation(FRotator(-60.0f, 0.0f, 0.0f));
		SpringArmComponent->TargetArmLength = 1000.0f;
		SpringArmComponent->bUsePawnControlRotation = false;
		SpringArmComponent->bDoCollisionTest = false;
	}

	// 카메라는 플레이어 중심을 추적하며 커서 방향의 위치 보정을 적용하지 않도록 설정합니다.
	{
		CameraComponent = CreateDefaultSubobject<UOGCameraComponent>(TEXT("CameraComponent"));
		CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
		CameraComponent->bUsePawnControlRotation = false;
	}

	// 캐릭터의 위치와 수명에 맞춰 상호작용 탐색 컴포넌트를 생성합니다.
	{
		InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
	}

	// 입력 액션과 태그의 연결은 공통 입력 설정 에셋에서 관리합니다.
	{
		InputConfig = TSoftObjectPtr<UORInputConfig>(FSoftObjectPath(TEXT("/Game/_ORGEL/Blueprint/Character/Player/DA_PlayerInputConfig.DA_PlayerInputConfig")));
	}
}

void AOGPlayer::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// 빙의가 완료된 뒤 현재 캐릭터를 ASC 아바타로 연결합니다.
	InitializeAbilitySystem();
}

void AOGPlayer::UnPossessed()
{
	// 컨트롤러 연결이 끊어지기 전에 어빌리티 입력과 아바타를 정리합니다.
	UninitializeAbilitySystem();

	Super::UnPossessed();
}

void AOGPlayer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// 빙의 해제를 거치지 않고 제거되는 캐릭터도 정리합니다.
	UninitializeAbilitySystem();

	Super::EndPlay(EndPlayReason);
}

void AOGPlayer::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// 매 프레임 커서 위치에 맞춰 조준 방향을 갱신합니다.
	UpdateCursorAim();
}

void AOGPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// 기본 조작은 직접 처리하고 어빌리티 입력은 태그로 ASC에 전달합니다.
	const UORInputConfig* LoadedInputConfig = InputConfig.LoadSynchronous();
	check(LoadedInputConfig);

	UORInputComponent* ORInputComponent = CastChecked<UORInputComponent>(PlayerInputComponent);
	ORInputComponent->BindNativeAction(LoadedInputConfig, ORGameplayTags::InputTag_Move, ETriggerEvent::Triggered, this, &AOGPlayer::Input_Move);
	ORInputComponent->BindAbilityActions(LoadedInputConfig, this, &AOGPlayer::AbilityInputTagPressed, &AOGPlayer::AbilityInputTagReleased);
}

void AOGPlayer::Input_Move(const FInputActionValue& Value)
{
	// 태그로 이동이 차단된 동안에는 대기 이동 입력도 쌓이지 않도록 처리합니다.
	if (ASC && ASC->HasMatchingGameplayTag(ORGameplayTags::Gameplay_MovementStopped))
	{
		return;
	}

	// 대각선 입력의 크기를 제한하고 카메라의 기울기를 제외한 수평 방향으로 변환합니다.
	const FVector2D MoveInput = Value.Get<FVector2D>().GetClampedToMaxSize(1.0f);
	const FRotationMatrix CameraRotation(FRotator(0.0f, CameraComponent->GetComponentRotation().Yaw, 0.0f));

	// 이동과 구르기가 같은 월드 방향을 사용하도록 입력 전달 전에 방향을 기록합니다.
	const FVector WorldDirection = CameraRotation.GetUnitAxis(EAxis::X) * MoveInput.Y + CameraRotation.GetUnitAxis(EAxis::Y) * MoveInput.X;
	UpdateLastMovementDirection(WorldDirection);
	AddMovementInput(WorldDirection);
}

void AOGPlayer::AbilityInputTagPressed(FGameplayTag InputTag)
{
	// 빙의로 ASC가 연결된 뒤에만 어빌리티 입력을 전달합니다.
	if (ASC)
	{
		ASC->AbilityInputTagPressed(InputTag);
	}
}

void AOGPlayer::AbilityInputTagReleased(FGameplayTag InputTag)
{
	// 조종 해제로 ASC 연결이 끊어진 뒤에는 입력을 전달하지 않도록 처리합니다.
	if (ASC)
	{
		ASC->AbilityInputTagReleased(InputTag);
	}
}

void AOGPlayer::InitializeAbilitySystem()
{
	// 플레이어 상태가 없는 캐릭터는 이전 연결만 정리합니다.
	AOGPlayerState* ORPlayerState = GetPlayerState<AOGPlayerState>();
	if (!ORPlayerState)
	{
		UninitializeAbilitySystem();
		return;
	}

	// 소유자는 플레이어 상태로 유지하고 현재 캐릭터를 아바타로 지정합니다.
	ASC = CastChecked<UORAbilitySystemComponent>(ORPlayerState->GetAbilitySystemComponent());
	ASC->InitAbilityActorInfo(ORPlayerState, this);
}

void AOGPlayer::UninitializeAbilitySystem()
{
	// 이미 다른 캐릭터로 옮겨진 ASC의 아바타는 변경하지 않도록 처리합니다.
	if (ASC && ASC->GetAvatarActor() == this)
	{
		ASC->ClearAbilityInput();
		ASC->CancelAllAbilities();
		ASC->SetLooseGameplayTagCount(ORGameplayTags::Status_Moving, 0);
		ASC->InitAbilityActorInfo(ASC->GetOwnerActor(), nullptr);
	}

	// 캐릭터의 캐시만 해제하고 플레이어 상태의 ASC는 유지합니다.
	ASC = nullptr;
}

void AOGPlayer::UpdateCursorAim()
{
	// 플레이어가 조종하지 않는 캐릭터에는 커서 조준을 적용하지 않도록 처리합니다.
	if (!IsPlayerControlled())
	{
		return;
	}

	// 이동 컴포넌트 밖에서 수행하는 커서 회전에도 같은 차단 태그를 적용합니다.
	if (ASC && ASC->HasMatchingGameplayTag(ORGameplayTags::Gameplay_MovementStopped))
	{
		return;
	}

	// 커서를 얻을 수 없는 프레임에는 직전 조준 방향을 유지합니다.
	FVector RayOrigin;
	FVector RayDirection;
	if (!CastChecked<APlayerController>(Controller)->DeprojectMousePositionToWorld(RayOrigin, RayDirection))
	{
		return;
	}

	// 캐릭터 중심 높이의 평면과 평행한 레이는 유효한 조준점을 만들지 않으므로 제외합니다.
	if (FMath::IsNearlyZero(RayDirection.Z))
	{
		return;
	}

	// 화면 뒤쪽의 교차점은 사용하지 않고 기존 방향을 유지합니다.
	const float RayDistance = (GetActorLocation().Z - RayOrigin.Z) / RayDirection.Z;
	if (RayDistance <= 0.0f)
	{
		return;
	}

	// 커서가 캐릭터 중심에 겹치면 회전을 유지하고 그 외에는 수평 조준 방향만 반영합니다.
	const FVector AimDirection = (RayOrigin + RayDirection * RayDistance - GetActorLocation()).GetSafeNormal2D();
	if (!AimDirection.IsNearlyZero())
	{
		SetActorRotation(FRotator(0.0f, AimDirection.Rotation().Yaw, 0.0f));
	}
}


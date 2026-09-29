#include "OGPlayer.h"

#include "Camera/OGCameraComponent.h"
#include "Camera/OGSpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "UObject/ConstructorHelpers.h"

AOGPlayer::AOGPlayer()
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

	// 컨트롤러의 기본 입력 매핑과 동일한 이동 액션을 연결합니다.
	{
		static ConstructorHelpers::FObjectFinder<UInputAction> MoveAction(TEXT("/Game/_ORGEL/Input/IA_Move.IA_Move"));
		MoveInputAction = MoveAction.Object;
	}
}

void AOGPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// 프로젝트의 Enhanced Input 컴포넌트에서 이동 입력을 처리합니다.
	CastChecked<UEnhancedInputComponent>(PlayerInputComponent)->BindAction(MoveInputAction, ETriggerEvent::Triggered, this, &AOGPlayer::DoMove);
}

void AOGPlayer::DoMove(const FInputActionValue& Value)
{
	// 대각선 입력의 크기를 제한하고 카메라의 기울기를 제외한 수평 방향으로 변환합니다.
	const FVector2D MoveInput = Value.Get<FVector2D>().GetClampedToMaxSize(1.0f);
	const FRotationMatrix CameraRotation(FRotator(0.0f, CameraComponent->GetComponentRotation().Yaw, 0.0f));
	AddMovementInput(CameraRotation.GetUnitAxis(EAxis::X), MoveInput.Y);
	AddMovementInput(CameraRotation.GetUnitAxis(EAxis::Y), MoveInput.X);
}

void AOGPlayer::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// 미조종 캐릭터와 다른 플레이어의 캐릭터에는 로컬 커서 조준을 적용하지 않도록 처리합니다.
	if (!IsPlayerControlled() || !IsLocallyControlled())
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


#include "OGPlayerController.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/Pawn.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "UI/Widget/Setting/OGSettingWidget.h"
#include "UObject/ConstructorHelpers.h"

AOGPlayerController::AOGPlayerController()
{
	// 플레이어의 수명과 분리하여 컨트롤러가 기본 입력 매핑을 관리하도록 설정합니다.
	{
		static ConstructorHelpers::FObjectFinder<UInputMappingContext> InputMapping(TEXT("/Game/_ORGEL/Input/IMC_Default.IMC_Default"));
		DefaultInputMappingContext = InputMapping.Object;
	}

	// 일시정지 중에도 동작할 공통 매핑과 액션을 연결합니다.
	{
		static ConstructorHelpers::FObjectFinder<UInputMappingContext> CommonInputMapping(TEXT("/Game/_ORGEL/Input/IMC_Common.IMC_Common"));
		CommonInputMappingContext = CommonInputMapping.Object;
		static ConstructorHelpers::FObjectFinder<UInputAction> PauseAction(TEXT("/Game/_ORGEL/Input/IA_Pause.IA_Pause"));
		PauseInputAction = PauseAction.Object;
	}

	// 기존 설정 위젯을 일시정지 화면으로 사용합니다.
	{
		static ConstructorHelpers::FClassFinder<UOGSettingWidget> SettingClass(TEXT("/Game/_ORGEL/Blueprint/UI/Widget/WBP_Setting"));
		SettingWidgetClass = SettingClass.Class;
	}
}

void AOGPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// 입력 서브시스템과 화면 커서는 로컬 컨트롤러에서만 설정합니다.
	if (IsLocalController())
	{
		GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>()->AddMappingContext(DefaultInputMappingContext, 0);
		GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>()->AddMappingContext(CommonInputMappingContext, 1);
		bShowMouseCursor = true;
		FInputModeGameAndUI InputMode;
		InputMode.SetHideCursorDuringCapture(false);
		SetInputMode(InputMode);
	}
}

void AOGPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// 종료 시 이미 분리된 로컬 플레이어는 건너뛰고 이 컨트롤러의 매핑만 제거합니다.
	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>()->RemoveMappingContext(DefaultInputMappingContext);
		LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>()->RemoveMappingContext(CommonInputMappingContext);
	}

	// 생성된 설정 화면이 다음 월드에 남지 않도록 제거합니다.
	if (SettingWidget)
	{
		SettingWidget->RemoveFromParent();
	}

	Super::EndPlay(EndPlayReason);
}

void AOGPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// ESC를 누르는 순간에만 전환하여 키를 누른 채 유지해도 반복 실행되지 않도록 처리합니다.
	CastChecked<UEnhancedInputComponent>(InputComponent)->BindAction(PauseInputAction, ETriggerEvent::Started, this, &AOGPlayerController::ToggleSettings);
}

void AOGPlayerController::ToggleSettings()
{
	// 게임 모드가 일시정지 전환을 허용한 경우에만 화면 상태를 변경합니다.
	const bool bPause = !IsPaused();
	if (!SetPause(bPause))
	{
		UE_LOG(LogTemp, Warning, TEXT("설정 화면의 일시정지 상태를 전환하지 못했습니다."));
		return;
	}

	// 전환 전에 누른 키와 대기 중인 이동 입력이 재개 직후 남지 않도록 정리합니다.
	FlushPressedKeys();
	if (APawn* ControlledPawn = GetPawn())
	{
		ControlledPawn->ConsumeMovementInputVector();
	}

	// 게임 입력의 ESC 처리도 유지하여 설정창 밖을 클릭해도 다시 닫을 수 있도록 설정합니다.
	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	if (bPause)
	{
		// 최초 진입에만 위젯을 생성하고 이후에는 같은 설정 화면을 재사용합니다.
		if (!SettingWidget)
		{
			SettingWidget = CreateWidget<UOGSettingWidget>(this, SettingWidgetClass);
			SettingWidget->SetIsFocusable(true);
		}

		// 설정 화면을 표시하고 키보드 입력을 위젯으로 전달합니다.
		SettingWidget->AddToViewport(100);
		InputMode.SetWidgetToFocus(SettingWidget->TakeWidget());
	}
	// 이 컨트롤러에서 설정 화면을 열었던 경우에만 화면에서 제거합니다.
	else if (SettingWidget)
	{
		SettingWidget->RemoveFromParent();
	}

	// 이동 및 커서 조준을 위한 입력 모드를 양쪽 전환에 동일하게 적용합니다.
	SetInputMode(InputMode);

	// 제거된 설정 위젯에 포커스가 남지 않도록 게임 화면으로 복귀합니다.
	if (!bPause)
	{
		UWidgetBlueprintLibrary::SetFocusToGameViewport();
	}
}

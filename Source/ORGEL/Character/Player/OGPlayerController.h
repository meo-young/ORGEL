#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "OGPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class UOGSettingWidget;

UCLASS()
class ORGEL_API AOGPlayerController : public APlayerController
{
	GENERATED_BODY()

// ─────────────────────────────────────────────────────────────
// Lifecycle
// ─────────────────────────────────────────────────────────────
public:
	/** 기본 입력 매핑과 설정 위젯 클래스를 연결합니다. */
	AOGPlayerController();

private:
	/** 로컬 플레이어에 입력 매핑과 커서 표시를 적용합니다. */
	virtual void BeginPlay() override;

	/** 컨트롤러가 등록한 입력 매핑과 설정 화면을 정리합니다. */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** 일시정지 액션을 컨트롤러 입력에 바인딩합니다. */
	virtual void SetupInputComponent() override;


// ─────────────────────────────────────────────────────────────
// Settings
// ─────────────────────────────────────────────────────────────
public:
	/** 설정 화면 표시와 게임 일시정지 상태를 함께 전환합니다. */
	void ToggleSettings();

private:
	/** 일시정지 시 표시할 설정 위젯 클래스를 참조합니다. */
	UPROPERTY(EditDefaultsOnly, Category = "변수|설정")
	TSubclassOf<UOGSettingWidget> SettingWidgetClass;

	/** 처음 열 때 생성하여 반복 사용하고 화면에서 제거해도 유지합니다. */
	UPROPERTY(Transient)
	TObjectPtr<UOGSettingWidget> SettingWidget;


// ─────────────────────────────────────────────────────────────
// Input
// ─────────────────────────────────────────────────────────────
private:
	/** 로컬 플레이어에 등록할 기본 키와 입력 액션의 매핑을 참조합니다. */
	UPROPERTY(EditDefaultsOnly, Category = "변수|입력")
	TObjectPtr<UInputMappingContext> DefaultInputMappingContext;

	/** 캐릭터의 행동과 독립적으로 ESC 입력을 유지할 공통 매핑을 참조합니다. */
	UPROPERTY(EditDefaultsOnly, Category = "변수|입력")
	TObjectPtr<UInputMappingContext> CommonInputMappingContext;

	/** 일시정지 중에도 설정 화면을 닫을 수 있는 입력 액션을 참조합니다. */
	UPROPERTY(EditDefaultsOnly, Category = "변수|입력")
	TObjectPtr<UInputAction> PauseInputAction;
};

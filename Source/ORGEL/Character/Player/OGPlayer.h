#pragma once

#include "CoreMinimal.h"
#include "ORGEL/Character/OGCharacter.h"
#include "OGPlayer.generated.h"

class UOGSpringArmComponent;
class UOGCameraComponent;
class UInputAction;
struct FInputActionValue;

UCLASS()
class ORGEL_API AOGPlayer : public AOGCharacter
{
	GENERATED_BODY()

// ─────────────────────────────────────────────────────────────
// Lifecycle
// ─────────────────────────────────────────────────────────────
public:
	/** 고정 카메라와 이동 기본값을 설정합니다. */
	AOGPlayer();

private:
	/** 커서 위치에 맞춰 캐릭터의 조준 방향을 갱신합니다. */
	virtual void Tick(float DeltaSeconds) override;

	/** 이동 액션을 플레이어 입력에 바인딩합니다. */
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;


// ─────────────────────────────────────────────────────────────
// Input
// ─────────────────────────────────────────────────────────────
private:
	/** 카메라의 수평 방향을 기준으로 이동합니다. */
	void DoMove(const FInputActionValue& Value);

	/** 카메라 기준 이동에 사용할 2차원 입력 액션을 참조합니다. */
	UPROPERTY(EditDefaultsOnly, Category = "변수|입력")
	TObjectPtr<UInputAction> MoveInputAction;


// ─────────────────────────────────────────────────────────────
// Camera
// ─────────────────────────────────────────────────────────────
private:
	/** 캐릭터 회전과 독립적으로 카메라의 거리와 각도를 유지합니다. */
	UPROPERTY(VisibleAnywhere, Category = "변수|카메라")
	TObjectPtr<UOGSpringArmComponent> SpringArmComponent;

	/** 플레이어 중심을 추적하는 고정 시점 카메라를 참조합니다. */
	UPROPERTY(VisibleAnywhere, Category = "변수|카메라")
	TObjectPtr<UOGCameraComponent> CameraComponent;
};

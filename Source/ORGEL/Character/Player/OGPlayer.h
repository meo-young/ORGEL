#pragma once

#include "CoreMinimal.h"
#include "ORGEL/Character/OGCharacter.h"
#include "OGPlayer.generated.h"

class UInteractionComponent;
class UOGSpringArmComponent;
class UOGCameraComponent;
class UORInputConfig;
struct FGameplayTag;
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
	AOGPlayer(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

private:
	/** 빙의 시 플레이어 상태의 ASC를 캐릭터에 연결합니다. */
	virtual void PossessedBy(AController* NewController) override;

	/** 조종권을 해제하기 전에 아바타와 입력 상태를 정리합니다. */
	virtual void UnPossessed() override;

	/** 캐릭터가 제거될 때 연결된 아바타 정보를 정리합니다. */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** 커서 위치에 맞춰 캐릭터의 조준 방향을 갱신합니다. */
	virtual void Tick(float DeltaSeconds) override;

	/** 이동, 구르기, 공격 액션을 플레이어 입력에 바인딩합니다. */
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;


// ─────────────────────────────────────────────────────────────
// Input
// ─────────────────────────────────────────────────────────────
private:
	/** 카메라의 수평 방향을 기준으로 이동합니다. */
	void Input_Move(const FInputActionValue& Value);

	/** 어빌리티 입력의 누름을 ASC에 전달합니다. */
	void AbilityInputTagPressed(FGameplayTag InputTag);

	/** 어빌리티 입력의 해제를 ASC에 전달합니다. */
	void AbilityInputTagReleased(FGameplayTag InputTag);


// ─────────────────────────────────────────────────────────────
// Ability System Initialization
// ─────────────────────────────────────────────────────────────
private:
	/** 소유자는 플레이어 상태로, 아바타는 현재 캐릭터로 설정합니다. */
	void InitializeAbilitySystem();

	/** 현재 캐릭터가 소유한 아바타 연결과 활성 어빌리티를 정리합니다. */
	void UninitializeAbilitySystem();


// ─────────────────────────────────────────────────────────────
// Camera
// ─────────────────────────────────────────────────────────────
private:
	/** 로컬 마우스 커서를 기준으로 캐릭터의 수평 조준 방향을 갱신합니다. */
	void UpdateCursorAim();


// ─────────────────────────────────────────────────────────────
// Input Variables
// ─────────────────────────────────────────────────────────────
private:
	/** 기본 조작과 어빌리티 입력 태그를 연결하는 설정을 참조합니다. */
	UPROPERTY(EditDefaultsOnly, Category = "변수|입력")
	TSoftObjectPtr<UORInputConfig> InputConfig;


// ─────────────────────────────────────────────────────────────
// Components
// ─────────────────────────────────────────────────────────────
private:
	/** 캐릭터 회전과 독립적으로 카메라의 거리와 각도를 유지합니다. */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UOGSpringArmComponent> SpringArmComponent;

	/** 플레이어 중심을 추적하는 고정 시점 카메라를 참조합니다. */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UOGCameraComponent> CameraComponent;
	
	/** 주변 상호작용 후보를 탐색하고 현재 대상을 관리하는 컴포넌트를 참조합니다. */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UInteractionComponent> InteractionComponent;
	
	
// ─────────────────────────────────────────────────────────────
// Getter
// ─────────────────────────────────────────────────────────────
public:
	/** 플레이어의 상호작용 대상 탐색 컴포넌트를 반환합니다. */
	FORCEINLINE UInteractionComponent* GetInteractionComponent() const { return InteractionComponent; };
	
};

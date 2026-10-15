#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "TestSubsystem.generated.h"

class APlayerController;
class UGameplayEffect;

UCLASS()
class ORGEL_API UTestSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

// ─────────────────────────────────────────────────────────────
// Lifecycle
// ─────────────────────────────────────────────────────────────
public:
	/** Shipping 빌드에서는 테스트 서브시스템 생성을 차단합니다. */
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	/** 테스트 피해 효과를 준비하고 입력 확인 콜백을 등록합니다. */
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** 입력 확인 콜백을 해제하고 참조를 정리합니다. */
	virtual void Deinitialize() override;


// ─────────────────────────────────────────────────────────────
// Test Actions
// ─────────────────────────────────────────────────────────────
private:
	/** 전달받은 컨트롤러의 현재 플레이어에게 피해 10을 적용합니다. */
	void ApplyTestDamage(APlayerController* PlayerController);


// ─────────────────────────────────────────────────────────────
// Test Variables
// ─────────────────────────────────────────────────────────────
private:
	/** 테스트 피해량을 SetByCaller로 전달할 기존 피해 효과 클래스를 유지합니다. */
	UPROPERTY(Transient)
	TSubclassOf<UGameplayEffect> DamageEffectClass;
};
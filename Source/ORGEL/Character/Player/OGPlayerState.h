#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "OGPlayerState.generated.h"

class UORMovementSet;
class UORAbilitySet;
class UORAbilitySystemComponent;
class UORCharacterStatData;
class UORHealthSet;

UCLASS()
class ORGEL_API AOGPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

// ─────────────────────────────────────────────────────────────
// Lifecycle
// ─────────────────────────────────────────────────────────────
public:
	/** 플레이어가 소유할 ASC와 체력 속성 세트를 생성합니다. */
	AOGPlayerState();

private:
	/** 최초 체력을 초기화하고 기본 어빌리티 세트를 부여합니다. */
	virtual void BeginPlay() override;


// ─────────────────────────────────────────────────────────────
// Ability System
// ─────────────────────────────────────────────────────────────
public:
	/** 플레이어 상태가 소유한 ASC를 반환합니다. */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;


// ─────────────────────────────────────────────────────────────
// Components
// ─────────────────────────────────────────────────────────────
private:
	/** 캐릭터 교체 후에도 능력치와 어빌리티를 유지할 ASC를 소유합니다. */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UORAbilitySystemComponent> ASC;

	/** 현재 체력과 최대 체력을 유지할 체력 속성 세트를 소유합니다. */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UORHealthSet> HealthSet;
	
	/** 같은 런에서 기본 이동속도와 이동속도 효과를 유지할 속성 세트를 소유합니다. */
	UPROPERTY(VisibleAnywhere, Category = "변수|능력치")
	TObjectPtr<UORMovementSet> MovementSet;
	
	
// ─────────────────────────────────────────────────────────────
// Ability System Variables
// ─────────────────────────────────────────────────────────────
private:
	/** 플레이어 최초 생성 시 적용할 필수 초기 스탯 데이터를 참조합니다. */
	UPROPERTY(EditDefaultsOnly, Category = "변수|초기능력치")
	TObjectPtr<UORCharacterStatData> InitialStatData;

	/** 플레이어 상태가 시작될 때 한 번 부여할 기본 어빌리티 세트를 참조합니다. */
	UPROPERTY(EditDefaultsOnly, Category = "변수|어빌리티")
	TSoftObjectPtr<UORAbilitySet> DefaultAbilitySet;
	
};
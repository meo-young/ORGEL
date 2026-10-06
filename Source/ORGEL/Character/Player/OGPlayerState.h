#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "OGPlayerState.generated.h"

class UORAbilitySet;
class UORAbilitySystemComponent;
class UORAttributeSet;

UCLASS()
class ORGEL_API AOGPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

// ─────────────────────────────────────────────────────────────
// Lifecycle
// ─────────────────────────────────────────────────────────────
public:
	/** 플레이어가 소유할 ASC와 기본 능력치 세트를 생성합니다. */
	AOGPlayerState();

private:
	/** 기본 어빌리티 세트를 한 번 부여합니다. */
	virtual void BeginPlay() override;


// ─────────────────────────────────────────────────────────────
// Ability System
// ─────────────────────────────────────────────────────────────
public:
	/** 플레이어 상태가 소유한 ASC를 반환합니다. */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;


// ─────────────────────────────────────────────────────────────
// Ability System Variables
// ─────────────────────────────────────────────────────────────
private:
	/** 캐릭터 교체 후에도 어빌리티를 유지할 ASC를 소유합니다. */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UORAbilitySystemComponent> ASC;

	/** 플레이어의 GAS 능력치를 관리할 기본 속성 세트를 소유합니다. */
	UPROPERTY(VisibleAnywhere, Category = "변수|능력치")
	TObjectPtr<UORAttributeSet> AttributeSet;

	/** 플레이어 상태가 시작될 때 한 번 부여할 기본 어빌리티 세트를 참조합니다. */
	UPROPERTY(EditDefaultsOnly, Category = "변수|어빌리티")
	TArray<TSoftObjectPtr<UORAbilitySet>> DefaultAbilitySets;
};

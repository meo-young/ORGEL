#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "OGCharacter.generated.h"

class UORAbilitySystemComponent;

UCLASS()
class ORGEL_API AOGCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

// ─────────────────────────────────────────────────────────────
// Lifecycle
// ─────────────────────────────────────────────────────────────
public:
	/** GAS 이동 제한을 반영할 이동 컴포넌트를 설정합니다. */
	AOGCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	/** 이동한 적이 없는 캐릭터의 구르기 방향을 생성 시 정면으로 초기화합니다. */
	virtual void BeginPlay() override;

	/** 실제 이동 상태를 게임플레이 태그에 반영합니다. */
	virtual void Tick(float DeltaSeconds) override;


// ─────────────────────────────────────────────────────────────
// Ability System
// ─────────────────────────────────────────────────────────────
public:
	/** 캐릭터의 아바타에 연결된 ASC를 반환합니다. */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

private:
	/** 이동 입력과 속도로 이동 상태 태그를 갱신합니다. */
	void UpdateMovingTag();


// ─────────────────────────────────────────────────────────────
// Movement Direction
// ─────────────────────────────────────────────────────────────
public:
	/** 현재 입력이 있으면 해당 방향을, 정지 중에는 마지막 유효 이동 방향을 반환합니다. */
	FVector GetDodgeDirection() const;

protected:
	/** 조준 회전과 독립적으로 마지막 유효 수평 이동 입력을 기록합니다. */
	void UpdateLastMovementDirection(const FVector& WorldDirection);


// ─────────────────────────────────────────────────────────────
// Ability System Variables
// ─────────────────────────────────────────────────────────────
protected:
	/** 현재 캐릭터를 아바타로 사용하는 ASC를 참조하며, 연결 해제 시 초기화합니다. */
	UPROPERTY(Transient)
	TObjectPtr<UORAbilitySystemComponent> ASC;


// ─────────────────────────────────────────────────────────────
// Movement Direction Variables
// ─────────────────────────────────────────────────────────────
private:
	/** 이동 입력이 없는 동안에도 구르기에 사용할 마지막 수평 이동 방향을 유지합니다. */
	FVector LastMovementDirection = FVector::ForwardVector;
};

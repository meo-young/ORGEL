#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "OGCharacter.generated.h"

class UORHealthSet;
class AOGCharacter;
class UORAbilitySystemComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FORCharacterDeathEvent, AOGCharacter* /*Character*/);

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
	
	/** 제거되는 캐릭터의 체력 이벤트 연결을 해제합니다. */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	
// ─────────────────────────────────────────────────────────────
// Delegate
// ─────────────────────────────────────────────────────────────
public:
	/** 체력이 고갈되어 사망 처리를 시작한 캐릭터를 전달합니다. */
	FORCharacterDeathEvent OnDeathStarted;

	/** 사망 애니메이션을 마치고 마지막 포즈를 유지하는 캐릭터를 전달합니다. */
	FORCharacterDeathEvent OnDeathFinished;
	

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
// Attribute Events
// ─────────────────────────────────────────────────────────────
protected:
	/** 공통 속성 이벤트를 등록하며 파생 클래스에서 개별 연결을 추가합니다. */
	virtual void BindAttributeEvents();

	/** 공통 속성 이벤트를 해제하며 파생 클래스에서 개별 연결을 정리합니다. */
	virtual void UnbindAttributeEvents();
	
	
// ─────────────────────────────────────────────────────────────
// Death
// ─────────────────────────────────────────────────────────────
public:
	/** 사망 애니메이션 종료 시 마지막 포즈를 유지하고 완료 이벤트를 전달합니다. */
	virtual void FinishDeath();

private:
	/** 행동을 중지하고 사망 애니메이션을 시작합니다. */
	void StartDeath();


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
// Death Variables
// ─────────────────────────────────────────────────────────────
protected:
	/** 사망 시작 시 재생하고 마지막 포즈를 유지할 몽타주를 참조합니다. */
	UPROPERTY(EditDefaultsOnly, Category = "변수|사망")
	TObjectPtr<UAnimMontage> DeathMontage;
	

// ─────────────────────────────────────────────────────────────
// Movement Direction Variables
// ─────────────────────────────────────────────────────────────
private:
	/** 이동 입력이 없는 동안에도 구르기에 사용할 마지막 수평 이동 방향을 유지합니다. */
	FVector LastMovementDirection = FVector::ForwardVector;
};

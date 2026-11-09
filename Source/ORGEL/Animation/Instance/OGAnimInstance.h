#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "Animation/AnimInstance.h"
#include "OGAnimInstance.generated.h"

UCLASS()
class ORGEL_API UOGAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
// ─────────────────────────────────────────────────────────────
// AnimInstance Interface
// ─────────────────────────────────────────────────────────────
public:
	/** 소유 캐릭터의 ASC를 사용해 게임플레이 태그 바인딩을 초기화합니다. */
	virtual void NativeInitializeAnimation() override;
	
	/** 걷기 Blend Space에 사용할 수평 속도와 이동 각도를 갱신합니다. */
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
	
// ─────────────────────────────────────────────────────────────
// Initializer
// ─────────────────────────────────────────────────────────────
public:
	/** 지정한 ASC에 게임플레이 태그 프로퍼티를 바인딩합니다. */
	void InitializeWithAbilitySystem(UAbilitySystemComponent* ASC);

	
// ─────────────────────────────────────────────────────────────
// Tag
// ─────────────────────────────────────────────────────────────
protected:
	/** Tag ↔ 프로퍼티 바인딩 맵 — Tag 상태 변화가 BlueprintReadOnly 변수에 자동 반영됩니다. */
	UPROPERTY(EditDefaultsOnly, Category = "변수|태그")
	FGameplayTagBlueprintPropertyMap GameplayTagPropertyMap;
	
	
// ─────────────────────────────────────────────────────────────
// Movement Variables
// ─────────────────────────────────────────────────────────────
protected:
	/** 실제 수평 이동 속도를 초당 센티미터 단위로 저장합니다. */
	UPROPERTY(BlueprintReadOnly, Category = "변수|이동")
	float Speed = 0.0f;

	/** 캐릭터 기준 좌우(X)와 전후(Y) 이동 방향을 -1부터 1 사이의 값으로 저장합니다. */
	UPROPERTY(BlueprintReadOnly, Category = "변수|이동")
	FVector2D MovementDirection = FVector2D::ZeroVector;
};

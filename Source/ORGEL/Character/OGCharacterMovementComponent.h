#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "OGCharacterMovementComponent.generated.h"

UCLASS()
class ORGEL_API UOGCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

// ─────────────────────────────────────────────────────────────
// Movement Restrictions
// ─────────────────────────────────────────────────────────────
public:
	/** 이동 차단 태그가 활성화되면 최대 속도를 제한합니다. */
	virtual float GetMaxSpeed() const override;

	/** 이동 차단 태그가 활성화되면 이동에 따른 회전을 제한합니다. */
	virtual FRotator GetDeltaRotation(float DeltaTime) const override;
};

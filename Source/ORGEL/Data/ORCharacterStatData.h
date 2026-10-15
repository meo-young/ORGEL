#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ORCharacterStatData.generated.h"

UCLASS()
class ORGEL_API UORCharacterStatData : public UDataAsset
{
	GENERATED_BODY()

// ─────────────────────────────────────────────────────────────
// Initial Stats
// ─────────────────────────────────────────────────────────────
public:
	/** 최초 생성 시 적용할 최대 체력을 반환합니다. */
	float GetMaxHealth() const { return MaxHealth; }

	/** 최초 생성 시 적용할 기본 이동속도를 반환합니다. */
	float GetMovementSpeed() const { return MovementSpeed; }


// ─────────────────────────────────────────────────────────────
// Initial Stat Variables
// ─────────────────────────────────────────────────────────────
private:
	/** 최초 최대 체력을 지정하며 현재 체력도 이 값으로 시작하도록 사용합니다. */
	UPROPERTY(EditDefaultsOnly, Category = "변수|초기능력치", meta = (ClampMin = "1.0", UIMin = "1.0"))
	float MaxHealth = 3.0f;

	/** 최초 기본 이동속도를 cm/s 단위로 지정합니다. */
	UPROPERTY(EditDefaultsOnly, Category = "변수|초기능력치", meta = (ClampMin = "0.0", UIMin = "0.0", Units = "cm/s"))
	float MovementSpeed = 500.0f;
};
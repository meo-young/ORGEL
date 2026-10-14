#pragma once

#include "CoreMinimal.h"
#include "Character/OGCharacter.h"
#include "OGEnemy.generated.h"

class UORCharacterStatData;
class UORHealthSet;

UCLASS()
class ORGEL_API AOGEnemy : public AOGCharacter
{
	GENERATED_BODY()

// ─────────────────────────────────────────────────────────────
// Lifecycle
// ─────────────────────────────────────────────────────────────
public:
	/** 몬스터가 소유할 ASC와 체력 속성 세트를 생성합니다. */
	AOGEnemy(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	/** 몬스터 최초 생성 시 ASC를 연결하고 체력을 초기화합니다. */
	virtual void BeginPlay() override;


// ─────────────────────────────────────────────────────────────
// Health Variables
// ─────────────────────────────────────────────────────────────
private:
	/** 몬스터의 현재 체력과 최대 체력을 관리할 속성 세트를 소유합니다. */
	UPROPERTY(VisibleAnywhere, Category = "변수|능력치")
	TObjectPtr<UORHealthSet> HealthSet;

	/** 몬스터 최초 생성 시 적용할 필수 초기 스탯 데이터를 참조합니다. */
	UPROPERTY(EditDefaultsOnly, Category = "변수|초기능력치")
	TObjectPtr<UORCharacterStatData> InitialStatData;
};
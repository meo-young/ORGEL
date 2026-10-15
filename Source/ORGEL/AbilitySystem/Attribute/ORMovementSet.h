#pragma once

#include "CoreMinimal.h"
#include "ORAttributeSet.h"
#include "ORMovementSet.generated.h"

UCLASS()
class ORGEL_API UORMovementSet : public UORAttributeSet
{
	GENERATED_BODY()

// ─────────────────────────────────────────────────────────────
// Initialization
// ─────────────────────────────────────────────────────────────
public:
	/** 초기 스탯 적용 전 이동속도를 0으로 설정합니다. */
	UORMovementSet();

	/** 초기 데이터에서 기본 이동속도를 읽어 초기화합니다. */
	virtual void InitializeSet(const UORCharacterStatData& StatData) override;


// ─────────────────────────────────────────────────────────────
// Attribute Accessors
// ─────────────────────────────────────────────────────────────
public:
	/** 효과가 반영된 이동속도의 GAS 접근 함수를 생성합니다. */
	OR_ATTRIBUTE_ACCESSORS(UORMovementSet, MovementSpeed);


// ─────────────────────────────────────────────────────────────
// Attribute Clamp
// ─────────────────────────────────────────────────────────────
protected:
	/** 이동속도가 유효한 0 이상의 값이 되도록 보정합니다. */
	virtual void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const override;


// ─────────────────────────────────────────────────────────────
// Movement Variables
// ─────────────────────────────────────────────────────────────
private:
	/** 버프와 디버프가 반영된 최대 이동속도를 cm/s 단위로 관리합니다. */
	UPROPERTY(BlueprintReadOnly, Category = "변수|이동", meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MovementSpeed;
	
};
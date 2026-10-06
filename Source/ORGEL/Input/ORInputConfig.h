#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "ORInputConfig.generated.h"

class UInputAction;

USTRUCT(BlueprintType)
struct FORInputAction
{
	GENERATED_BODY()

	/** 입력 이벤트를 수신할 Enhanced Input 액션을 참조합니다. */
	UPROPERTY(EditAnywhere, Category = "변수|입력")
	TObjectPtr<UInputAction> InputAction;

	/** 액션을 기본 조작 또는 어빌리티에 연결할 입력 태그를 지정합니다. */
	UPROPERTY(EditAnywhere, Category = "변수|입력", meta = (Categories = "InputTag"))
	FGameplayTag InputTag;
};

UCLASS(BlueprintType)
class ORGEL_API UORInputConfig : public UDataAsset
{
	GENERATED_BODY()

// ─────────────────────────────────────────────────────────────
// Input Actions
// ─────────────────────────────────────────────────────────────
public:
	/** 기본 조작에 사용할 입력 액션을 태그로 검색합니다. */
	const UInputAction* FindNativeInputActionForTag(const FGameplayTag& InputTag) const;


// ─────────────────────────────────────────────────────────────
// Input Action Variables
// ─────────────────────────────────────────────────────────────
public:
	/** 이동처럼 캐릭터 함수에 직접 연결할 입력 액션과 태그 목록을 보관합니다. */
	UPROPERTY(EditAnywhere, Category = "변수|입력", meta = (TitleProperty = "InputTag"))
	TArray<FORInputAction> NativeInputActions;

	/** ASC에 입력 태그를 전달하여 어빌리티를 실행할 액션 목록을 보관합니다. */
	UPROPERTY(EditAnywhere, Category = "변수|입력", meta = (TitleProperty = "InputTag"))
	TArray<FORInputAction> AbilityInputActions;
};

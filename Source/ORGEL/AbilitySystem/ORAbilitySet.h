#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayAbilitySpecHandle.h"
#include "GameplayTagContainer.h"
#include "ORAbilitySet.generated.h"

class UORAbilitySystemComponent;
class UORGameplayAbility;

USTRUCT()
struct FORAbilitySet_GameplayAbility
{
	GENERATED_BODY()

	/** 세트에서 부여할 어빌리티 클래스를 지정합니다. */
	UPROPERTY(EditAnywhere, Category = "변수|어빌리티")
	TSubclassOf<UORGameplayAbility> Ability;

	/** 어빌리티를 부여할 때 적용할 레벨을 지정합니다. */
	UPROPERTY(EditAnywhere, Category = "변수|어빌리티", meta = (ClampMin = "1"))
	int32 AbilityLevel = 1;

	/** 어빌리티와 연결할 입력 태그를 지정하며, 입력이 없는 패시브는 비워 둡니다. */
	UPROPERTY(EditAnywhere, Category = "변수|입력", meta = (Categories = "InputTag"))
	FGameplayTag InputTag;
};

USTRUCT()
struct FORAbilitySet_GrantedHandles
{
	GENERATED_BODY()

// ─────────────────────────────────────────────────────────────
// Ability Handles
// ─────────────────────────────────────────────────────────────
public:
	/** 부여한 어빌리티를 기록하여 묶음 회수를 준비합니다. */
	void AddAbilitySpecHandle(FGameplayAbilitySpecHandle Handle);

	/** 기록한 어빌리티를 ASC에서 회수하고 핸들을 비웁니다. */
	void TakeFromAbilitySystem(UORAbilitySystemComponent* ASC);


// ─────────────────────────────────────────────────────────────
// Ability Handle Variables
// ─────────────────────────────────────────────────────────────
private:
	/** 이 세트에서 부여한 어빌리티를 나중에 함께 회수할 수 있도록 핸들을 보관합니다. */
	UPROPERTY(Transient)
	TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;
};

UCLASS()
class ORGEL_API UORAbilitySet : public UPrimaryDataAsset
{
	GENERATED_BODY()

// ─────────────────────────────────────────────────────────────
// Ability Grant
// ─────────────────────────────────────────────────────────────
public:
	/** 어빌리티와 입력 태그를 부여하고 회수용 핸들을 기록합니다. */
	void GiveToAbilitySystem(UORAbilitySystemComponent* ASC, FORAbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject) const;


// ─────────────────────────────────────────────────────────────
// Ability Grant Variables
// ─────────────────────────────────────────────────────────────
private:
	/** 세트 적용 시 부여할 어빌리티와 레벨 및 입력 태그 목록을 보관합니다. */
	UPROPERTY(EditAnywhere, Category = "변수|어빌리티")
	TArray<FORAbilitySet_GameplayAbility> GrantedGameplayAbilities;
};

#include "OGPlayerState.h"

#include "AbilitySystem/ORAbilitySet.h"
#include "AbilitySystem/ORAbilitySystemComponent.h"
#include "AbilitySystem/Attribute/ORAttributeSet.h"

AOGPlayerState::AOGPlayerState()
{
	// 캐릭터가 교체되어도 유지할 ASC와 능력치 세트를 플레이어 상태가 소유합니다.
	{
		ASC = CreateDefaultSubobject<UORAbilitySystemComponent>(TEXT("ASC"));
		AttributeSet = CreateDefaultSubobject<UORAttributeSet>(TEXT("AttributeSet"));
	}

	// 프로젝트의 기본 어빌리티 목록을 실행 시 로드하도록 연결합니다.
	DefaultAbilitySets.Add(TSoftObjectPtr<UORAbilitySet>(FSoftObjectPath(TEXT("/Game/_ORGEL/Blueprint/Character/Player/DA_PlayerAbilitySet.DA_PlayerAbilitySet"))));
}

void AOGPlayerState::BeginPlay()
{
	Super::BeginPlay();

	// 플레이어 상태가 먼저 시작되어 아바타가 없어도 소유자 정보를 초기화합니다.
	ASC->InitAbilityActorInfo(this, GetPawn());

	// 재빙의 시 중복 부여되지 않도록 플레이어 상태의 시작 시점에만 부여합니다.
	for (const TSoftObjectPtr<UORAbilitySet>& AbilitySetReference : DefaultAbilitySets)
	{
		if (const UORAbilitySet* AbilitySet = AbilitySetReference.LoadSynchronous())
		{
			AbilitySet->GiveToAbilitySystem(ASC, nullptr, this);
		}
	}
}

UAbilitySystemComponent* AOGPlayerState::GetAbilitySystemComponent() const
{
	return ASC;
}

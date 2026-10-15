#include "OGPlayerState.h"

#include "AbilitySystem/ORAbilitySet.h"
#include "AbilitySystem/ORAbilitySystemComponent.h"
#include "AbilitySystem/Attribute/ORHealthSet.h"
#include "AbilitySystem/Attribute/ORMovementSet.h"
#include "Data/ORCharacterStatData.h"

AOGPlayerState::AOGPlayerState()
{
	// 캐릭터가 교체되어도 유지할 ASC와 체력 속성 세트를 생성합니다.
	{
		ASC = CreateDefaultSubobject<UORAbilitySystemComponent>(TEXT("ASC"));
		HealthSet = CreateDefaultSubobject<UORHealthSet>(TEXT("AttributeSet"));
		MovementSet = CreateDefaultSubobject<UORMovementSet>(TEXT("MovementSet"));
	}

	// 기존 기본 어빌리티 세트를 실행 시 로드하도록 연결합니다.
	DefaultAbilitySet = TSoftObjectPtr<UORAbilitySet>(FSoftObjectPath(TEXT("/Game/_ORGEL/Blueprint/Character/Player/DA_PlayerAbilitySet.DA_PlayerAbilitySet")));
}

void AOGPlayerState::BeginPlay()
{
	// Blueprint 시작 처리와 어빌리티 부여 전에 최초 체력을 준비합니다.
	ASC->InitAbilityActorInfo(this, GetPawn());
	
	// 최초 시작 시 등록된 속성 세트의 기본 능력치를 초기화합니다.
	ASC->InitializeAttributeSets(*InitialStatData);
	
	Super::BeginPlay();

	// 플레이어 상태의 최초 시작 시점에만 기본 어빌리티를 부여합니다.
	if (const UORAbilitySet* AbilitySet = DefaultAbilitySet.LoadSynchronous())
	{
		AbilitySet->GiveToAbilitySystem(ASC, nullptr, this);
	}
	
}

UAbilitySystemComponent* AOGPlayerState::GetAbilitySystemComponent() const
{
	return ASC;
}
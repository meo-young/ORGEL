#include "OGEnemy.h"

#include "AbilitySystem/ORAbilitySystemComponent.h"
#include "AbilitySystem/Attribute/ORHealthSet.h"
#include "AbilitySystem/Attribute/ORMovementSet.h"
#include "Data/ORCharacterStatData.h"

AOGEnemy::AOGEnemy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// 몬스터 자신이 소유할 ASC와 체력 속성 세트를 생성합니다.
	{
		ASC = CreateDefaultSubobject<UORAbilitySystemComponent>(TEXT("ASC"));
		HealthSet = CreateDefaultSubobject<UORHealthSet>(TEXT("HealthSet"));
		MovementSet = CreateDefaultSubobject<UORMovementSet>(TEXT("MovementSet"));
	}
}

void AOGEnemy::BeginPlay()
{
	// Blueprint 시작 처리 전에 ASC와 공통 능력치를 준비합니다.
	ASC->InitAbilityActorInfo(this, this);

	// 생성자에서 생성되어 자동 등록된 속성 세트를 초기화합니다.
	ASC->InitializeAttributeSets(*InitialStatData);

	// 부모의 공통 사망 이벤트를 등록합니다.
	BindAttributeEvents();

	Super::BeginPlay();
}
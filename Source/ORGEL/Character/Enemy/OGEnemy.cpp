#include "OGEnemy.h"

#include "AbilitySystem/ORAbilitySystemComponent.h"
#include "AbilitySystem/Attribute/ORHealthSet.h"
#include "Data/ORCharacterStatData.h"

AOGEnemy::AOGEnemy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// 몬스터 자신이 소유할 ASC와 체력 속성 세트를 생성합니다.
	{
		ASC = CreateDefaultSubobject<UORAbilitySystemComponent>(TEXT("ASC"));
		HealthSet = CreateDefaultSubobject<UORHealthSet>(TEXT("HealthSet"));
	}
}

void AOGEnemy::BeginPlay()
{
	// Blueprint 시작 처리 전에 몬스터를 소유자와 아바타로 연결하고 체력을 준비합니다.
	ASC->InitAbilityActorInfo(this, this);
	ASC->AddAttributeSetSubobject(HealthSet.Get());
	HealthSet->InitializeHealth(InitialStatData->GetMaxHealth());
	
	// 초기 체력을 설정한 다음 체력 고갈 이벤트를 연결합니다.
	BindHealthEvents();

	Super::BeginPlay();
}
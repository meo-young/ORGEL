#include "OGCombatRoom.h"

#include "Character/Enemy/Normal/OGNormalEnemy.h"
#include "Character/Player/OGPlayer.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"
#include "NavigationSystem.h"
#include "ORGameplayTags.h"
#include "Define/Define.h"
#include "Kismet/GameplayStatics.h"

AOGCombatRoom::AOGCombatRoom()
{
	// 진입 이벤트만 사용하므로 Tick을 실행하지 않습니다.
	PrimaryActorTick.bCanEverTick = false;
	
	// 최초 진입 시 몬스터를 생성할 수 있도록 미진입 상태로 초기화합니다.
	RoomState = ORGameplayTags::Room_State_Unvisited;

	// 진입 감지와 스폰 범위를 하나의 박스로 관리합니다.
	{
		RoomArea = CreateDefaultSubobject<UBoxComponent>(TEXT("RoomArea"));
		SetRootComponent(RoomArea);

		RoomArea->SetBoxExtent(FVector(1000.0f, 1000.0f, 200.0f));
		RoomArea->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		RoomArea->SetCollisionObjectType(ECC_WorldDynamic);
		RoomArea->SetCollisionResponseToAllChannels(ECR_Ignore);
		RoomArea->SetCollisionResponseToChannel(ORCollisionChannels::Player, ECR_Overlap);
		RoomArea->SetGenerateOverlapEvents(true);
		RoomArea->SetCanEverAffectNavigation(false);
	}
}

void AOGCombatRoom::BeginPlay()
{
	Super::BeginPlay();

	// 플레이어의 방 진입을 감지하도록 이벤트를 연결합니다.
	RoomArea->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::HandleRoomOverlap);
}

void AOGCombatRoom::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// 방 종료 이후 사망 이벤트가 클리어 판정을 호출하지 않도록 연결을 해제합니다.
	for (const TWeakObjectPtr<AOGNormalEnemy>& EnemyReference : SpawnedEnemies)
	{
		if (AOGNormalEnemy* Enemy = EnemyReference.Get())
		{
			Enemy->OnDeathStarted.RemoveAll(this);
		}
	}

	// 액터 제거를 처치로 처리하지 않고 추적 참조만 정리합니다.
	SpawnedEnemies.Empty();

	Super::EndPlay(EndPlayReason);
}

void AOGCombatRoom::HandleEnemyDeath(AOGCharacter* Character)
{
	// 목록에서 실제 제거된 개체만 처리하여 중복 사망 알림을 무시합니다.
	AOGNormalEnemy* Enemy = CastChecked<AOGNormalEnemy>(Character);
	SpawnedEnemies.Remove(Enemy);
	
	// 사망한 개체의 이벤트 연결을 해제하고 클리어를 확인합니다.
	Enemy->OnDeathStarted.RemoveAll(this);
	
	if (SpawnedEnemies.Num() == 0)
	{
		TryClearRoom();
	}
}

void AOGCombatRoom::TryClearRoom()
{
	// 생성 중에는 판정을 미루고 전투 중 생존 목록이 비었을 때만 처리합니다.
	if (RoomState != ORGameplayTags::Room_State_Active)
	{
		return;
	}

	// 사망 애니메이션 완료를 기다리지 않고 마지막 사망 시작 시 클리어합니다.
	RoomState = ORGameplayTags::Room_State_Cleared;

	UE_LOG(LogTemp, Warning, TEXT("[%s] 모든 몬스터가 사망하여 방을 클리어했습니다."), *GetName());
}

void AOGCombatRoom::HandleRoomOverlap(UPrimitiveComponent*, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32, bool, const FHitResult&)
{
	// 최초 진입 이후에는 방을 나갔다 들어와도 다시 생성하지 않습니다.
	if (RoomState != ORGameplayTags::Room_State_Unvisited)
	{
		return;
	}
	
	// 생성 과정에서 진입 이벤트가 다시 발생해도 중복 실행되지 않도록 먼저 전환합니다.
	RoomState = ORGameplayTags::Room_State_Spawning;

	// 진입 사실을 출력하고 몬스터 생성을 시작합니다.
	UE_LOG(LogTemp, Warning, TEXT("[%s] 플레이어가 방에 진입했습니다"), *GetName());

	SpawnEnemies();
}

void AOGCombatRoom::SpawnEnemies()
{
	// 스폰 설정이 없는 경우 생성을 생략합니다.
	if (SpawnEntries.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] 몬스터 스폰 설정이 비어 있습니다."), *GetName());
		return;
	}

	// 생성 전에 전체 클래스와 수량을 확인합니다.
	for (const FORRoomEnemySpawnEntry& Entry : SpawnEntries)
	{
		if (!Entry.EnemyClass || Entry.Count <= 0)
		{
			UE_LOG(LogTemp, Error, TEXT("[%s] 몬스터 클래스 또는 생성 수가 올바르지 않습니다."), *GetName());
			return;
		}
	}

	// 무작위 후보를 이동 가능한 바닥 위치로 투영할 내비게이션 시스템을 조회합니다.
	UNavigationSystemV1* Navigation = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (!Navigation)
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] 내비게이션 시스템이 없습니다."), *GetName());
		return;
	}
	
	// 클래스별로 지정한 수량만큼 생성을 시도합니다.
	for (const FORRoomEnemySpawnEntry& Entry : SpawnEntries)
	{
		for (int32 Index = 0; Index < Entry.Count; ++Index)
		{
			if (!TrySpawnEnemy(Entry.EnemyClass, Navigation))
			{
				UE_LOG(LogTemp, Warning, TEXT("[%s] %s의 %d번째 개체를 생성할 위치를 찾지 못했습니다."), *GetName(), *GetNameSafe(Entry.EnemyClass.Get()), Index + 1);
			}
		}
	}
	
	RoomState = ORGameplayTags::Room_State_Active;
}

bool AOGCombatRoom::TrySpawnEnemy(TSubclassOf<AOGNormalEnemy> EnemyClass, UNavigationSystemV1* Navigation)
{
	const ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	
	// 몬스터 클래스별 캡슐 크기를 반영합니다.
	const AOGNormalEnemy* DefaultEnemy = EnemyClass->GetDefaultObject<AOGNormalEnemy>();
	const float CapsuleRadius = DefaultEnemy->GetCapsuleComponent()->GetScaledCapsuleRadius();
	const float CapsuleHalfHeight = DefaultEnemy->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	const FVector BoundsExtent = RoomArea->GetScaledBoxExtent();
	const FTransform AreaTransform = RoomArea->GetComponentTransform();
	const float MinDistanceSquared = FMath::Square(MinSpawnDistance);
	
	// 조건을 만족하는 위치를 제한된 횟수만큼 탐색합니다.
	for (int32 Attempt = 0; Attempt < MaxSpawnAttempts; ++Attempt)
	{
		// 캡슐 반지름만큼 경계에서 안쪽으로 들어온 범위에서 후보를 선택합니다.
		const FVector LocalCandidate(
			FMath::FRandRange(-BoundsExtent.X + CapsuleRadius, BoundsExtent.X - CapsuleRadius),
			FMath::FRandRange(-BoundsExtent.Y + CapsuleRadius, BoundsExtent.Y - CapsuleRadius),
			0.0f);

		const FVector WorldCandidate = AreaTransform.TransformPositionNoScale(LocalCandidate);

		// 후보 주변 NavMesh에서 실제 바닥 위치를 찾습니다.
		FNavLocation FloorLocation;
		if (!Navigation->ProjectPointToNavigation(WorldCandidate, FloorLocation, FVector(50.0f, 50.0f, BoundsExtent.Z)))
		{
			continue;
		}

		// 바닥 위치에서 캡슐 중심 높이만큼 올려 실제 생성 위치를 계산합니다.
		const FVector SpawnLocation = FloorLocation.Location + FVector::UpVector * (CapsuleHalfHeight + 2.0f);
		const FVector LocalSpawnLocation = AreaTransform.InverseTransformPositionNoScale(SpawnLocation);

		// NavMesh 투영으로 위치가 바뀌더라도 캡슐 전체가 방 영역 안에 있도록 검사합니다.
		if (FMath::Abs(LocalSpawnLocation.X) + CapsuleRadius > BoundsExtent.X ||
			FMath::Abs(LocalSpawnLocation.Y) + CapsuleRadius > BoundsExtent.Y ||
			FMath::Abs(LocalSpawnLocation.Z) + CapsuleHalfHeight > BoundsExtent.Z)
		{
			continue;
		}

		// 스폰 후보와 플레이어 중심 사이의 거리를 검사합니다.
		if (FVector::DistSquared(SpawnLocation, Player->GetActorLocation()) < MinDistanceSquared)
		{
			continue;
		}

		// 스폰 후보와 이 방에서 이미 생성한 모든 유효한 몬스터의 거리를 검사합니다.
		const bool bTooCloseToEnemy = SpawnedEnemies.ContainsByPredicate([&SpawnLocation, MinDistanceSquared](const TWeakObjectPtr<AOGNormalEnemy>& EnemyReference)
		{
			const AOGNormalEnemy* Enemy = EnemyReference.Get();
			return Enemy && FVector::DistSquared(SpawnLocation, Enemy->GetActorLocation()) < MinDistanceSquared;
		});

		if (bTooCloseToEnemy)
		{
			continue;
		}

		// 거리 조건과 별개로 실제 충돌이 발생하는 위치에는 생성하지 않습니다.
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding;

		AOGNormalEnemy* Enemy = GetWorld()->SpawnActor<AOGNormalEnemy>(EnemyClass, SpawnLocation, GetActorRotation(), SpawnParameters);
		if (!Enemy)
		{
			continue;
		}

		// 이후 생성되는 몬스터가 이번 개체와의 거리도 검사하도록 등록합니다.
		SpawnedEnemies.Add(TWeakObjectPtr<AOGNormalEnemy>(Enemy));
		Enemy->OnDeathStarted.AddUObject(this, &ThisClass::HandleEnemyDeath);
		return true;
	}

	// 시도 횟수 안에 유효한 위치를 찾지 못하면 실패를 전달합니다.
	return false;
}

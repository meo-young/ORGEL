#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "OGCombatRoom.generated.h"

class AOGCharacter;
class AOGPlayer;
class UNavigationSystemV1;
class AOGNormalEnemy;
class UBoxComponent;
class UPrimitiveComponent;

USTRUCT()
struct FORRoomEnemySpawnEntry
{
	GENERATED_BODY()

	/** 생성할 일반 몬스터 Blueprint 클래스를 지정합니다. */
	UPROPERTY(EditAnywhere, Category = "변수|스폰")
	TSubclassOf<AOGNormalEnemy> EnemyClass;

	/** 지정한 몬스터 클래스의 생성 수를 지정합니다. */
	UPROPERTY(EditAnywhere, Category = "변수|스폰", meta = (ClampMin = "1", UIMin = "1"))
	int32 Count = 1;
};

UCLASS()
class ORGEL_API AOGCombatRoom : public AActor
{
	GENERATED_BODY()

// ─────────────────────────────────────────────────────────────
// Lifecycle
// ─────────────────────────────────────────────────────────────
public:
	/** 진입 감지와 몬스터 스폰에 사용할 방 영역을 생성합니다. */
	AOGCombatRoom();

protected:
	/** 방 영역의 진입 이벤트를 연결합니다. */
	virtual void BeginPlay() override;

	/** 방 종료 시 몬스터의 사망 이벤트 연결을 해제합니다. */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

// ─────────────────────────────────────────────────────────────
// Room Entry
// ─────────────────────────────────────────────────────────────
private:
	/** 플레이어가 방에 진입하면 로그를 출력하고 설정된 몬스터를 생성합니다. */
	UFUNCTION()
	void HandleRoomOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


// ─────────────────────────────────────────────────────────────
// Spawn
// ─────────────────────────────────────────────────────────────
private:
	/** 구조체 배열에 지정된 클래스와 수량에 따라 일반 몬스터를 생성합니다. */
	void SpawnEnemies();

	/** 방 경계와 거리 및 충돌 조건을 만족하는 위치에 몬스터 한 개체를 생성합니다. */
	bool TrySpawnEnemy(TSubclassOf<AOGNormalEnemy> EnemyClass, UNavigationSystemV1* Navigation);

	
// ─────────────────────────────────────────────────────────────
// Room Clear
// ─────────────────────────────────────────────────────────────
private:
	/** 사망한 몬스터를 추적 목록에서 제거하고 클리어를 확인합니다. */
	void HandleEnemyDeath(AOGCharacter* Character);

	/** 전투 중 모든 몬스터가 사망했으면 클리어 상태로 전환합니다. */
	void TryClearRoom();
	

// ─────────────────────────────────────────────────────────────
// Component Variables
// ─────────────────────────────────────────────────────────────
private:
	/** 플레이어 진입 감지와 몬스터 스폰 범위를 공통으로 관리합니다. */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> RoomArea;


// ─────────────────────────────────────────────────────────────
// Spawn Variables
// ─────────────────────────────────────────────────────────────
private:
	/** 방 진입 시 생성할 일반 몬스터 클래스와 수량을 배열로 관리합니다. */
	UPROPERTY(EditAnywhere, Category = "변수|스폰")
	TArray<FORRoomEnemySpawnEntry> SpawnEntries;

	/** 스폰 후보와 플레이어 및 기존 몬스터 중심 사이의 최소 거리를 지정합니다. */
	UPROPERTY(EditAnywhere, Category = "변수|스폰", meta = (ClampMin = "0.0", Units = "cm"))
	float MinSpawnDistance = 300.0f;

	/** 몬스터 한 개체의 유효한 스폰 위치를 찾는 최대 시도 횟수를 지정합니다. */
	UPROPERTY(EditAnywhere, Category = "변수|스폰", meta = (ClampMin = "1", UIMin = "1"))
	int32 MaxSpawnAttempts = 50;

	/** 최초 진입과 생성 진행 상태를 구분하여 중복 스폰을 방지합니다. */
	UPROPERTY(VisibleInstanceOnly, Category = "변수|방상태")
	FGameplayTag RoomState;
	
	/** 이 방에서 생성한 몬스터를 참조하여 이후 스폰 후보와의 거리를 검사합니다. */
	TArray<TWeakObjectPtr<AOGNormalEnemy>> SpawnedEnemies;
};
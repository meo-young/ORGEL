#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ORGEL_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

// ─────────────────────────────────────────────────────────────
// Lifecycle
// ─────────────────────────────────────────────────────────────
public:
	/** 주기적으로 주변 상호작용 대상을 탐색하도록 설정합니다. */
	UInteractionComponent();

private:
	/** 탐색 주기에 맞춰 가장 가까운 상호작용 대상을 갱신합니다. */
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


// ─────────────────────────────────────────────────────────────
// Interaction Target
// ─────────────────────────────────────────────────────────────
public:
	/** 현재 위치를 기준으로 범위 안의 가장 가까운 대상을 다시 선정합니다. */
	void RefreshInteractionTarget();

	/** 마지막 탐색에서 선정한 대상을 반환하며 제거된 대상은 nullptr로 반환합니다. */
	AActor* GetCurrentTarget() const;


// ─────────────────────────────────────────────────────────────
// Interaction Variables
// ─────────────────────────────────────────────────────────────
private:
	/** 플레이어 중심과 대상 액터 위치 사이에 허용할 최대 거리를 지정합니다. */
	UPROPERTY(EditDefaultsOnly, Category = "변수|상호작용", meta = (ClampMin = "1.0", Units = "cm", AllowPrivateAccess = true))
	float InteractionRange = 150.0f;

	/** 탐색으로 선정한 대상을 수명을 연장하지 않는 약한 참조로 보관합니다. */
	UPROPERTY(Transient)
	TWeakObjectPtr<AActor> CurrentTarget;
	
};

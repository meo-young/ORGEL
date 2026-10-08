#include "InteractionComponent.h"

#include "CollisionQueryParams.h"
#include "CollisionShape.h"
#include "Engine/OverlapResult.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "Define/Define.h"

UInteractionComponent::UInteractionComponent()
{
	// 표시용 대상은 주기적으로 갱신하고 실제 실행 직전에는 별도로 재탐색합니다.
	{
		PrimaryComponentTick.bCanEverTick = true;
		PrimaryComponentTick.TickInterval = 0.1f;
	}
}

void UInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 플레이어와 대상의 이동을 현재 대상 선정에 반영합니다.
	RefreshInteractionTarget();
}

void UInteractionComponent::RefreshInteractionTarget()
{
	// 소유자 자신을 제외하고 상호작용 전용 Trace Channel로 주변 후보를 수집합니다.
	AActor* Owner = GetOwner();
	const FVector Origin = Owner->GetActorLocation();

	TArray<FOverlapResult> Overlaps;
	const FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(InteractionSearch), false, Owner);

	GetWorld()->OverlapMultiByChannel(Overlaps, Origin, FQuat::Identity, ORCollisionChannels::Interaction, FCollisionShape::MakeSphere(InteractionRange), QueryParams);

	// 하나의 액터에 여러 충돌 바디가 있어도 한 번만 거리 비교를 수행합니다.
	TSet<AActor*> VisitedActors;
	AActor* ClosestTarget = nullptr;
	float ClosestDistance = InteractionRange;

	for (const FOverlapResult& Overlap : Overlaps)
	{
		// 전용 채널에 Block으로 응답한 결과만 상호작용 후보로 사용합니다.
		if (!Overlap.bBlockingHit)
		{
			continue;
		}

		// 제거 중인 액터와 Interface 미구현 액터 및 중복 후보를 제외합니다.
		AActor* Candidate = Overlap.GetActor();
		if (!IsValid(Candidate) || !Cast<IInteractable>(Candidate) || VisitedActors.Contains(Candidate))
		{
			continue;
		}

		VisitedActors.Add(Candidate);

		// 충돌 바디 표면이 아닌 대상 액터 위치까지의 거리로 범위를 판정합니다.
		const float Distance = FVector::Dist(Origin, Candidate->GetActorLocation());
		if (Distance > InteractionRange)
		{
			continue;
		}

		// 가장 가까운 대상을 선택하고 동일 거리에서는 런타임 ID로 순서를 고정합니다.
		if (!ClosestTarget || Distance < ClosestDistance)
		{
			ClosestTarget = Candidate;
			ClosestDistance = Distance;
		}
	}

	// 후보가 없으면 이전 대상을 해제합니다.
	CurrentTarget = ClosestTarget;
}

AActor* UInteractionComponent::GetCurrentTarget() const
{
	return CurrentTarget.Get();
}
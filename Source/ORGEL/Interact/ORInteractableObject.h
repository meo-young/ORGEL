#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "ORInteractableObject.generated.h"

class USkeletalMeshComponent;

UCLASS(Abstract)
class ORGEL_API AORInteractableObject : public AActor, public IInteractable
{
	GENERATED_BODY()

// ─────────────────────────────────────────────────────────────
// Lifecycle
// ─────────────────────────────────────────────────────────────
public:
	/** 스켈레탈 메시를 생성하고 상호작용 검사 전용 충돌을 설정합니다. */
	AORInteractableObject();


// ─────────────────────────────────────────────────────────────
// Interaction
// ─────────────────────────────────────────────────────────────
public:
	/** 실행 조건을 검사한 뒤 파생 클래스의 상호작용 결과를 반환합니다. */
	virtual bool TryInteract(AActor* Interactor) override final;

protected:
	/** 상태를 변경하지 않고 공통 조건과 대상별 실행 조건을 확인합니다. */
	virtual bool CanInteract(const AActor* Interactor) const;

	/** 오브젝트별 동작을 처리하며 기본 구현은 Blueprint 이벤트에 전달합니다. */
	virtual bool PerformInteraction(AActor* Interactor);

	/** 실행 조건을 통과한 요청을 Blueprint 또는 C++ 구현으로 전달합니다. */
	UFUNCTION(BlueprintImplementableEvent)
	bool K2_PerformInteraction(AActor* Interactor);
	

// ─────────────────────────────────────────────────────────────
// Component
// ─────────────────────────────────────────────────────────────
protected:
	/** 오브젝트의 외형과 애니메이션 및 상호작용 탐색용 충돌을 제공하는 메시를 참조합니다. */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> SkeletalMeshComponent;
	
};
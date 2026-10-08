#include "ORInteractableObject.h"

#include "Components/SkeletalMeshComponent.h"
#include "Define/Define.h"

AORInteractableObject::AORInteractableObject()
{
	// 공통 액터 자체에는 매 프레임 처리를 사용하지 않도록 설정합니다.
	PrimaryActorTick.bCanEverTick = false;

	// 스켈레탈 메시를 루트로 생성하여 외형과 애니메이션을 제공하도록 설정합니다.
	{
		SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
		SetRootComponent(SkeletalMeshComponent);
	}

	// 직접 수행하는 상호작용 쿼리에만 반응하도록 충돌을 설정합니다.
	{
		SkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		SkeletalMeshComponent->SetCollisionObjectType(ECC_WorldDynamic);
		SkeletalMeshComponent->SetCollisionResponseToChannel(ORCollisionChannels::Interaction, ECR_Block);
		SkeletalMeshComponent->SetGenerateOverlapEvents(false);
	}
}

bool AORInteractableObject::TryInteract(AActor* Interactor)
{
	// 모든 실행 요청이 대상 내부의 조건 검사를 거치도록 처리합니다.
	if (!CanInteract(Interactor))
	{
		UE_LOG(LogTemp, Warning, TEXT("상호작용 실패"))
		return false;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("상호작용 수행"))

	// 조건을 통과한 요청만 오브젝트별 실제 동작으로 전달합니다.
	return PerformInteraction(Interactor);
}

bool AORInteractableObject::CanInteract(const AActor* Interactor) const
{
	// 유효한 외부 요청자만 허용하고 대상이 종료 중이면 실행을 차단합니다.
	return IsValid(Interactor) && Interactor != this && !IsActorBeingDestroyed();
}

bool AORInteractableObject::PerformInteraction(AActor* Interactor)
{
	// 기본 동작은 Blueprint에서 구현한 상호작용 이벤트에 전달합니다.
	return K2_PerformInteraction(Interactor);
}

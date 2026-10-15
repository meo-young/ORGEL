#include "ORAnimNotify_DeathFinished.h"

#include "Character/OGCharacter.h"
#include "Components/SkeletalMeshComponent.h"

void UORAnimNotify_DeathFinished::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	// 애니메이션 미리보기 등 캐릭터가 없는 환경에서는 완료 처리를 생략합니다.
	if (AOGCharacter* Character = Cast<AOGCharacter>(MeshComp->GetOwner()))
	{
		Character->FinishDeath();
	}
}
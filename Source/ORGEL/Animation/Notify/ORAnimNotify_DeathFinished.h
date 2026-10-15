#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "ORAnimNotify_DeathFinished.generated.h"

UCLASS()
class ORGEL_API UORAnimNotify_DeathFinished : public UAnimNotify
{
	GENERATED_BODY()

// ─────────────────────────────────────────────────────────────
// Animation Notify
// ─────────────────────────────────────────────────────────────
public:
	/** 사망 애니메이션의 마지막 포즈에서 캐릭터의 사망을 완료합니다. */
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	
};
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interactable.generated.h"

class AActor;

UINTERFACE(NotBlueprintable)
class ORGEL_API UInteractable : public UInterface
{
	GENERATED_BODY()
};

class ORGEL_API IInteractable
{
	GENERATED_BODY()

// ─────────────────────────────────────────────────────────────
// Interaction
// ─────────────────────────────────────────────────────────────
public:
	/** 최종 조건을 확인하고 상호작용을 실행하여 성공 여부를 반환합니다. */
	virtual bool TryInteract(AActor* Interactor) = 0;
	
};
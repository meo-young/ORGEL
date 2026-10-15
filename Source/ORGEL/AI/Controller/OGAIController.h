#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "OGAIController.generated.h"

UCLASS()
class ORGEL_API AOGAIController : public AAIController
{
	GENERATED_BODY()

// ─────────────────────────────────────────────────────────────
// Lifecycle
// ─────────────────────────────────────────────────────────────
public:
	/** 플레이어 접근 여부를 주기적으로 확인하도록 설정합니다. */
	AOGAIController();

};

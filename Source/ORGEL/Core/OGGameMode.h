#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "OGGameMode.generated.h"

UCLASS()
class ORGEL_API AOGGameMode : public AGameModeBase
{
	GENERATED_BODY()

// ─────────────────────────────────────────────────────────────
// Run
// ─────────────────────────────────────────────────────────────
public:
	/** 현재 런의 종료를 확정하고 월드 진행을 정지합니다. */
	void EndRun();
	
	/** 현재 맵을 다시 불러와 능력치와 런 상태를 초기화합니다. */
	void StartNewRun();
	
};
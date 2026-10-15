#include "OGGameMode.h"

#include "Kismet/GameplayStatics.h"

void AOGGameMode::EndRun()
{
	UE_LOG(LogTemp, Warning, TEXT("런이 종료됐습니다."));
}

void AOGGameMode::StartNewRun()
{
	// 에디터 실행 시 붙는 접두사를 제외하고 현재 맵 이름을 조회합니다.
	const FName LevelName(*UGameplayStatics::GetCurrentLevelName(this, true));

	// 현재 월드를 교체하여 플레이어 상태와 몬스터 및 런 내부 효과를 새로 생성합니다.
	UGameplayStatics::OpenLevel(this, LevelName, true);
}
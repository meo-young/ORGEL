#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/OGUserWidget.h"
#include "OGSettingWidget.generated.h"

UCLASS()
class ORGEL_API UOGSettingWidget : public UOGUserWidget
{
	GENERATED_BODY()

// ─────────────────────────────────────────────────────────────
// Input
// ─────────────────────────────────────────────────────────────
private:
	/** 자식 위젯의 포커스와 관계없이 ESC로 설정 화면을 닫고 게임을 재개합니다. */
	virtual FReply NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
};

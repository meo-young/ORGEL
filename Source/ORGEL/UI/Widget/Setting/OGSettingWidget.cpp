#include "OGSettingWidget.h"

#include "Character/Player/OGPlayerController.h"
#include "Input/Reply.h"
#include "InputCoreTypes.h"

FReply UOGSettingWidget::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	// ESC를 자식 위젯보다 먼저 소비하여 설정 화면 종료와 컨트롤러 입력이 중복 실행되지 않도록 처리합니다.
	if (InKeyEvent.GetKey() == EKeys::Escape)
	{
		// 키를 누른 채 유지할 때 발생하는 반복 이벤트는 전환 없이 소비합니다.
		if (!InKeyEvent.IsRepeat())
		{
			GetOwningPlayer<AOGPlayerController>()->ToggleSettings();
		}
		return FReply::Handled();
	}

	// 설정 화면의 나머지 키 입력은 기존 위젯 처리를 유지합니다.
	return Super::NativeOnPreviewKeyDown(InGeometry, InKeyEvent);
}

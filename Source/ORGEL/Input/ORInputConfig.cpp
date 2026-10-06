#include "ORInputConfig.h"

const UInputAction* UORInputConfig::FindNativeInputActionForTag(const FGameplayTag& InputTag) const
{
	// 요청한 태그에 정확히 일치하는 기본 입력 액션을 검색합니다.
	for (const FORInputAction& Action : NativeInputActions)
	{
		if (Action.InputAction && Action.InputTag == InputTag)
		{
			return Action.InputAction;
		}
	}

	// 매핑이 없는 기본 입력은 바인딩하지 않도록 처리합니다.
	return nullptr;
}

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "ORInputConfig.h"
#include "ORInputComponent.generated.h"

UCLASS()
class ORGEL_API UORInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

// ─────────────────────────────────────────────────────────────
// Input Binding
// ─────────────────────────────────────────────────────────────
public:
	/** 태그로 찾은 기본 입력을 지정한 함수에 연결합니다. */
	template <class UserClass, typename FuncType>
	void BindNativeAction(const UORInputConfig* InputConfig, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func);

	/** 어빌리티의 입력 시작과 종료를 태그 전달 함수에 연결합니다. */
	template <class UserClass, typename PressedFuncType, typename ReleasedFuncType>
	void BindAbilityActions(const UORInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc);
};

template <class UserClass, typename FuncType>
void UORInputComponent::BindNativeAction(const UORInputConfig* InputConfig, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func)
{
	// 설정에 포함된 기본 입력만 바인딩합니다.
	if (const UInputAction* Action = InputConfig->FindNativeInputActionForTag(InputTag))
	{
		BindAction(Action, TriggerEvent, Object, Func);
	}
}

template <class UserClass, typename PressedFuncType, typename ReleasedFuncType>
void UORInputComponent::BindAbilityActions(const UORInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc)
{
	// 유지 입력은 ASC에서 추적하며 완료와 취소를 모두 해제로 전달합니다.
	for (const FORInputAction& Action : InputConfig->AbilityInputActions)
	{
		if (Action.InputAction && Action.InputTag.IsValid())
		{
			BindAction(Action.InputAction, ETriggerEvent::Started, Object, PressedFunc, Action.InputTag);
			BindAction(Action.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, Action.InputTag);
			BindAction(Action.InputAction, ETriggerEvent::Canceled, Object, ReleasedFunc, Action.InputTag);
		}
	}
}

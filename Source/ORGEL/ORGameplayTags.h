#pragma once

#include "NativeGameplayTags.h"

namespace ORGameplayTags
{
// ─────────────────────────────────────────────────────────────
// Input
// ─────────────────────────────────────────────────────────────
	// 입력 액션을 기본 조작과 어빌리티에 연결합니다.
	ORGEL_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Move);
	ORGEL_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Dodge);
	ORGEL_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Attack);


// ─────────────────────────────────────────────────────────────
// Character Status
// ─────────────────────────────────────────────────────────────
	// 캐릭터의 현재 상태를 표현합니다.
	ORGEL_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Moving);
	ORGEL_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Dodging);
	ORGEL_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Invulnerable);


// ─────────────────────────────────────────────────────────────
// Movement Restrictions
// ─────────────────────────────────────────────────────────────
	// 캐릭터의 이동과 회전을 제한합니다.
	ORGEL_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Gameplay_MovementStopped);


// ─────────────────────────────────────────────────────────────
// Ability Cooldown
// ─────────────────────────────────────────────────────────────
	// 지정한 어빌리티의 재사용 대기 상태를 구분합니다.
	ORGEL_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Cooldown_Dodge);
}

#include "ORGameplayTags.h"

namespace ORGameplayTags
{
// ─────────────────────────────────────────────────────────────
// Input
// ─────────────────────────────────────────────────────────────
	// 입력 액션을 기본 조작과 어빌리티에 연결합니다.
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Move, "InputTag.Move");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Dodge, "InputTag.Dodge");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Attack, "InputTag.Attack");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Interact, "InputTag.Interact");


// ─────────────────────────────────────────────────────────────
// Character Status
// ─────────────────────────────────────────────────────────────
	// 캐릭터의 현재 상태를 표현합니다.
	UE_DEFINE_GAMEPLAY_TAG(Status_Moving, "Status.Moving");
	UE_DEFINE_GAMEPLAY_TAG(Status_Dodging, "Status.Dodging");
	UE_DEFINE_GAMEPLAY_TAG(Status_Invulnerable, "Status.Invulnerable");
	
	
// ─────────────────────────────────────────────────────────────
// SetByCaller Data
// ─────────────────────────────────────────────────────────────
	// 피해와 회복 효과에 전달할 수치의 식별자를 등록합니다.
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_Damage, "SetByCaller.Damage");
	UE_DEFINE_GAMEPLAY_TAG(SetByCaller_Healing, "SetByCaller.Healing");


// ─────────────────────────────────────────────────────────────
// Restrictions
// ─────────────────────────────────────────────────────────────
	// 캐릭터의 이동과 회전을 제한합니다.
	UE_DEFINE_GAMEPLAY_TAG(Gameplay_MovementStopped, "Gameplay.MovementStopped");
	
	// 행동 제한 상태에서 상호작용 어빌리티의 발동을 차단합니다.
	UE_DEFINE_GAMEPLAY_TAG(Gameplay_InteractionBlocked, "Gameplay.InteractionBlocked");


// ─────────────────────────────────────────────────────────────
// Ability Cooldown
// ─────────────────────────────────────────────────────────────
	// 지정한 어빌리티의 재사용 대기 상태를 구분합니다.
	UE_DEFINE_GAMEPLAY_TAG(Cooldown_Dodge, "Cooldown.Dodge");
}

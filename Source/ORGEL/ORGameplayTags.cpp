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
	UE_DEFINE_GAMEPLAY_TAG(Status_Dead, "Status.Dead");
	UE_DEFINE_GAMEPLAY_TAG(Status_DeathFinished, "Status.DeathFinished");
	
	
// ─────────────────────────────────────────────────────────────
// Invulnerable Status
// ─────────────────────────────────────────────────────────────
	UE_DEFINE_GAMEPLAY_TAG(Status_Invulnerable, "Status.Invulnerable");
	UE_DEFINE_GAMEPLAY_TAG(Status_Invulnerable_Dodge, "Status.Invulnerable.Dodge");
	UE_DEFINE_GAMEPLAY_TAG(Status_Invulnerable_Hit, "Status.Invulnerable.Hit");
	
	
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
	
	
// ─────────────────────────────────────────────────────────────
// Room State
// ─────────────────────────────────────────────────────────────
	// 방의 전투 진행 상태 태그를 등록합니다.
	UE_DEFINE_GAMEPLAY_TAG(Room_State_Unvisited, "Room.State.Unvisited");
	UE_DEFINE_GAMEPLAY_TAG(Room_State_Spawning, "Room.State.Spawning");
	UE_DEFINE_GAMEPLAY_TAG(Room_State_Active, "Room.State.Active");
	UE_DEFINE_GAMEPLAY_TAG(Room_State_Cleared, "Room.State.Cleared");
}

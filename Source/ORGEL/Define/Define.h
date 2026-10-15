#pragma once

#include "Engine/EngineTypes.h"

namespace ORCollisionChannels
{
	/** 상호작용 가능한 오브젝트를 탐색하는 전용 Trace Channel을 지정합니다. */
	inline constexpr ECollisionChannel Interaction = ECC_GameTraceChannel1;
	
	/** 플레이어를 식별하는 전용 Object Channel을 지정합니다. */
	inline constexpr ECollisionChannel Player = ECC_GameTraceChannel2;

	/** 적을 식별하는 전용 Object Channel을 지정합니다. */
	inline constexpr ECollisionChannel Enemy = ECC_GameTraceChannel3;
}
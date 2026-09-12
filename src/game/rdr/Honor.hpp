#pragma once
#include "game/rdr/ScriptGlobal.hpp"

namespace YimMenu
{
	// Story honor lives in a script global: the game's scripts keep it in
	// Global_40.f_11095.f_35 and only ever add to it, clamped to +-240 until
	// chapter 6 and +-320 after.
	inline constexpr auto StoryHonorGlobal = ScriptGlobal(40).At(11095).At(35);
	inline constexpr int StoryHonorMax     = 320;

	inline bool StoryHonorReady()
	{
		return StoryHonorGlobal.CanAccess(false);
	}

	inline int GetStoryHonor()
	{
		return StoryHonorReady() ? StoryHonorGlobal.As<int&>() : 0;
	}

	inline void SetStoryHonor(int value)
	{
		if (StoryHonorReady())
			StoryHonorGlobal.As<int&>() = value;
	}
}

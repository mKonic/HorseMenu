#include "core/commands/LoopedCommand.hpp"
#include "game/rdr/ScriptGlobal.hpp"

namespace YimMenu::Features
{
	// Story honor is a script global. The game's own scripts hold it as
	// Global_40.f_11095.f_35 and only ever change it by adding to it, clamped to
	// +-240 until chapter 6 and +-320 after, so writing the top of the scale
	// every tick pins honor at maximum no matter what Arthur gets up to.
	constexpr auto HonorValue = ScriptGlobal(40).At(11095).At(35);
	constexpr int MaxHonor    = 320;

	class KeepHonorMaxed : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnEnable() override
		{
			if (HonorValue.CanAccess(false))
				LOG(INFO) << "Honor was " << HonorValue.As<int&>() << ", pinning it to " << MaxHonor;
			else
				LOG(WARNING) << "Script globals aren't up yet, honor will be pinned once they are";
		}

		virtual void OnTick() override
		{
			if (HonorValue.CanAccess(false))
				HonorValue.As<int&>() = MaxHonor;
		}
	};

	static KeepHonorMaxed _KeepHonorMaxed{"keephonormaxed", "Keep Honor Maxed", "Holds story honor at the top of the bar, whatever you do"};
}

#include "core/commands/LoopedCommand.hpp"
#include "game/rdr/Honor.hpp"

namespace YimMenu::Features
{
	class KeepHonorMaxed : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnEnable() override
		{
			if (StoryHonorReady())
				LOG(INFO) << "Honor was " << GetStoryHonor() << ", pinning it to " << StoryHonorMax;
			else
				LOG(WARNING) << "Script globals aren't up yet, honor will be pinned once they are";
		}

		virtual void OnTick() override
		{
			SetStoryHonor(StoryHonorMax);
		}
	};

	static KeepHonorMaxed _KeepHonorMaxed{"keephonormaxed", "Keep Honor Maxed", "Holds story honor at the top of the bar, whatever you do"};
}

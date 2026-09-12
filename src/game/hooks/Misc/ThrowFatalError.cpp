#include "core/Trace.hpp"
#include "core/hooking/DetourHook.hpp"
#include "game/hooks/Hooks.hpp"

#include <mutex>
#include <set>

namespace YimMenu::Hooks
{
	void Misc::ThrowFatalError(int code, int fileHash, int fileLine)
	{
		TRACE_SCOPE("ThrowFatalError");

		// Upstream raises an access violation here so the game's crash handler
		// prints a stack. That handler stops every thread and never returns
		// under wine, which looks like the game freezing for good, so record
		// the error instead and let the game carry on. Once per error, since
		// the game can raise the same one every frame.
		static std::mutex mutex;
		static std::set<std::tuple<int, int, int>> seen;

		const auto error = std::make_tuple(code, fileHash, fileLine);
		bool first       = false;
		{
			std::lock_guard lock(mutex);
			first = seen.insert(error).second;
		}

		if (first)
			LOG(WARNING) << "Swallowed a game fatal error: code = " << HEX((uint32_t)code)
			             << " | fileHash = " << HEX((uint32_t)fileHash) << " | fileLine = " << (uint32_t)fileLine;
	}
}

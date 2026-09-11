#pragma once
// Debugging aid: each TRACE_SCOPE counts entries and exits of a hook, and
// TraceDump() writes the table to %APPDATA%\HorseMenu\trace-counters.txt. A
// hook that froze the game shows up as enters > leaves, and the dump keeps
// working while the game is stuck because the menu's own loop writes it.
#include <array>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <fstream>

namespace YimMenu
{
	struct TraceCounter
	{
		const char* name;
		std::atomic<std::uint64_t> enters;
		std::atomic<std::uint64_t> leaves;
	};

	inline std::array<TraceCounter*, 64>& TraceRegistry()
	{
		static std::array<TraceCounter*, 64> registry{};
		return registry;
	}

	inline std::atomic<int>& TraceRegistered()
	{
		static std::atomic<int> count{0};
		return count;
	}

	inline TraceCounter& TraceRegister(TraceCounter& counter)
	{
		const int slot = TraceRegistered().fetch_add(1);
		if (slot < static_cast<int>(TraceRegistry().size()))
			TraceRegistry()[slot] = &counter;
		return counter;
	}

	class TraceScope
	{
	public:
		explicit TraceScope(TraceCounter& counter) :
		    m_Counter(counter)
		{
			m_Counter.enters.fetch_add(1, std::memory_order_relaxed);
		}
		~TraceScope()
		{
			m_Counter.leaves.fetch_add(1, std::memory_order_relaxed);
		}

	private:
		TraceCounter& m_Counter;
	};

	inline void TraceDump()
	{
		using namespace std::chrono;
		static auto last  = steady_clock::now() - seconds(10);
		const auto now    = steady_clock::now();
		if (now - last < milliseconds(500))
			return;
		last = now;

		const char* appdata = std::getenv("appdata");
		if (!appdata)
			return;

		std::ofstream file(std::filesystem::path(appdata) / "HorseMenu" / "trace-counters.txt", std::ios::trunc);
		const int count = std::min<int>(TraceRegistered().load(), static_cast<int>(TraceRegistry().size()));
		for (int i = 0; i < count; i++)
			if (const auto* counter = TraceRegistry()[i])
				file << counter->name << " enters=" << counter->enters.load() << " leaves=" << counter->leaves.load() << "\n";
	}
}

#define TRACE_SCOPE(NAME)                                                             \
	static YimMenu::TraceCounter _trace_counter{NAME, 0, 0};                          \
	static YimMenu::TraceCounter& _trace_registered = YimMenu::TraceRegister(_trace_counter); \
	(void)_trace_registered;                                                          \
	YimMenu::TraceScope _trace_scope { _trace_counter }

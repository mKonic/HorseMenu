#pragma once
// Debugging aid: whitespace- or comma-separated flags in
// %APPDATA%\HorseMenu\testflags.txt, read once, so hook groups can be left out
// without rebuilding. Flags: no-render-hooks, no-wndproc, no-scripts.
#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <set>
#include <string>
#include <string_view>

namespace YimMenu
{
	inline bool TestFlag(std::string_view name)
	{
		static const std::set<std::string, std::less<>> flags = [] {
			std::set<std::string, std::less<>> parsed;
			if (const char* appdata = std::getenv("appdata"))
			{
				std::ifstream file(std::filesystem::path(appdata) / "HorseMenu" / "testflags.txt");
				for (std::string token; file >> token;)
				{
					std::erase(token, ',');
					if (!token.empty())
						parsed.insert(token);
				}
			}
			return parsed;
		}();

		return flags.contains(name);
	}
}

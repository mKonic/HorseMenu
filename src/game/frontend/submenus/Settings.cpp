#include "settings.hpp"

#include <unordered_set>

#include "core/TestFlags.hpp"
#include "core/commands/Commands.hpp"
#include "core/commands/HotkeySystem.hpp"
#include "core/commands/LoopedCommand.hpp"
#include "game/backend/Self.hpp"
#include "game/features/Features.hpp"
#include "game/frontend/items/Items.hpp"

namespace YimMenu::Submenus
{
	// Every command lands in one flat list here, so labels that are clear inside
	// their own submenu ("Health" under both Cores and Bars) read as duplicates.
	// Sort by label, tell same-label commands apart by their command name, and
	// leave out the online ones when the online hooks are off.
	static void Hotkeys()
	{
		struct Entry
		{
			joaat_t m_Hash;
			std::string m_Label;
		};

		static const std::vector<Entry> entries = [] {
			static const std::unordered_set<joaat_t> online_commands = {
			    "blipsprite"_J, "bring"_J, "bringall"_J, "cageplayercircus"_J,
			    "cageplayerlarge"_J, "cageplayersmall"_J, "cancelattachment"_J, "defensive"_J,
			    "deletehorse"_J, "deletevehicle"_J, "endparlay"_J, "explode"_J,
			    "hearall"_J, "icekick"_J, "increasebounty"_J, "kickhorse"_J,
			    "kill"_J, "lightning"_J, "maxhonor"_J, "maxhonorall"_J,
			    "minhonor"_J, "minhonorall"_J, "mountinstance"_J, "newsession"_J,
			    "newsessionposse"_J, "offensive"_J, "popkick"_J, "primaryicon"_J,
			    "remotebolas"_J, "revealall"_J, "rideonshoulders"_J, "secondaryicon"_J,
			    "sendstablemountevent"_J, "sendticker"_J, "slap"_J, "spank"_J,
			    "spawngoldchest"_J, "splitkick"_J, "spoofblip"_J, "spoofprimaryicon"_J,
			    "spoofsecondaryicon"_J, "stablemountevent"_J, "startparlay"_J, "tickermessage"_J,
			    "touchplayer"_J, "tpalltojail"_J, "tpalltowaypoint"_J, "tpbehindplayer"_J,
			    "tpintovehicle"_J, "tpplayertojail"_J, "tpplayertomadamnazar"_J, "tpplayertowaypoint"_J,
			    "tptoplayer"_J, "tptoplayercamp"_J, "voicechatoverride"_J};

			const bool skip_online = TestFlag("no-net");
			std::vector<Entry> list;

			for (auto& [hash, command] : Commands::GetCommands())
			{
				if (!g_HotkeySystem.m_CommandHotkeys.contains(hash) || (skip_online && online_commands.contains(hash)))
					continue;

				list.push_back({hash, command->GetLabel()});
			}

			std::ranges::sort(list, [](const Entry& a, const Entry& b) {
				return a.m_Label < b.m_Label;
			});

			for (std::size_t i = 0; i < list.size(); i++)
			{
				const bool shared = (i > 0 && list[i - 1].m_Label == list[i].m_Label)
				    || (i + 1 < list.size() && list[i + 1].m_Label == list[i].m_Label);

				if (shared)
					list[i].m_Label += " (" + Commands::GetCommand(list[i].m_Hash)->GetName() + ")";
			}

			return list;
		}();

		ImGui::BulletText("Hold the command name clicked to change its hotkey");
		ImGui::BulletText("Press any registered key to remove");

		ImGui::Spacing();

		static std::string filter;
		filter.resize(64);
		if (ImGui::InputTextWithHint("##hotkeyfilter", "Search", filter.data(), filter.capacity()))
			filter.resize(std::strlen(filter.c_str()));

		ImGui::Separator();
		ImGui::Spacing();

		const auto needle = std::string(filter.c_str()) | std::views::transform(::tolower) | std::ranges::to<std::string>();

		for (const auto& entry : entries)
		{
			if (!needle.empty())
			{
				const auto haystack = entry.m_Label | std::views::transform(::tolower) | std::ranges::to<std::string>();
				if (!haystack.contains(needle))
					continue;
			}

			ImGui::PushID(entry.m_Hash);
			HotkeySetter(entry.m_Hash, entry.m_Label).Draw();
			ImGui::Spacing();
			ImGui::PopID();
		}
	};

	Settings::Settings() :
	    Submenu::Submenu("Settings")
	{
		auto hotkeys           = std::make_shared<Category>("Hotkeys");
		auto gui               = std::make_shared<Category>("GUI");
		auto protections       = std::make_shared<Category>("Protection");
		auto syncGroup         = std::make_shared<Group>("Sync");
		auto networkEventGroup = std::make_shared<Group>("Network Events");
		auto scriptEventGroup  = std::make_shared<Group>("Script Events");
		auto playerEsp         = std::make_shared<Group>("Player ESP", 10);
		auto pedEsp            = std::make_shared<Group>("Ped ESP", 10);
		auto overlay            = std::make_shared<Group>("Overlay");
		auto context           = std::make_shared<Group>("Context Menu");
		auto misc              = std::make_shared<Group>("Misc");

		hotkeys->AddItem(std::make_shared<ImGuiItem>(Hotkeys));

		// Players
		playerEsp->AddItem(std::make_shared<BoolCommandItem>("espdrawplayers"_J));
		playerEsp->AddItem(std::make_shared<ConditionalItem>("espdrawplayers"_J, std::make_shared<BoolCommandItem>("espdrawdeadplayers"_J)));

		playerEsp->AddItem(std::make_shared<ConditionalItem>("espdrawplayers"_J, std::make_shared<BoolCommandItem>("espnameplayers"_J, "Player Name")));
		playerEsp->AddItem(std::make_shared<ConditionalItem>("espdrawplayers"_J, std::make_shared<ColorCommandItem>("namecolorplayers"_J)));

		playerEsp->AddItem(std::make_shared<ConditionalItem>("espdrawplayers"_J, std::make_shared<BoolCommandItem>("espdistanceplayers"_J, "Player Distance")));
		//playerEsp->AddItem(std::make_shared<ConditionalItem>("espdrawplayers"_J, std::make_shared<ColorCommandItem>("distancecolorplayers"_J))); // TO DO

		playerEsp->AddItem(std::make_shared<ConditionalItem>("espdrawplayers"_J, std::make_shared<BoolCommandItem>("espskeletonplayers"_J, "Player Skeleton")));
		playerEsp->AddItem(std::make_shared<ConditionalItem>("espdrawplayers"_J, std::make_shared<ColorCommandItem>("skeletoncolorplayers"_J)));

		// Peds
		pedEsp->AddItem(std::make_shared<BoolCommandItem>("espdrawpeds"_J));
		pedEsp->AddItem(std::make_shared<ConditionalItem>("espdrawpeds"_J, std::make_shared<BoolCommandItem>("espdrawdeadpeds"_J)));

		pedEsp->AddItem(std::make_shared<ConditionalItem>("espdrawpeds"_J, std::make_shared<BoolCommandItem>("espmodelspeds"_J, "Ped Hashes")));
		pedEsp->AddItem(std::make_shared<ConditionalItem>("espdrawpeds"_J, std::make_shared<ColorCommandItem>("hashcolorpeds"_J)));

		pedEsp->AddItem(std::make_shared<ConditionalItem>("espdrawpeds"_J, std::make_shared<BoolCommandItem>("espnetinfopeds"_J, "Ped Net Info")));
		pedEsp->AddItem(std::make_shared<ConditionalItem>("espdrawpeds"_J, std::make_shared<BoolCommandItem>("espscriptinfopeds"_J, "Ped Script Info")));

		pedEsp->AddItem(std::make_shared<ConditionalItem>("espdrawpeds"_J, std::make_shared<BoolCommandItem>("espdistancepeds"_J, "Ped Distance")));
		//pedEsp->AddItem(std::make_shared<ConditionalItem>("espdrawpeds"_J, std::make_shared<ColorCommandItem>("distancecolorpeds"_J))); // TO DO

		pedEsp->AddItem(std::make_shared<ConditionalItem>("espdrawpeds"_J, std::make_shared<BoolCommandItem>("espskeletonpeds"_J, "Ped Skeleton")));
		pedEsp->AddItem(std::make_shared<ConditionalItem>("espdrawpeds"_J, std::make_shared<ColorCommandItem>("skeletoncolorpeds"_J)));

		pedEsp->AddItem(std::make_shared<ConditionalItem>("espdrawpeds"_J, std::make_shared<BoolCommandItem>("espskeletonhorse"_J, "Horse Skeleton")));
		pedEsp->AddItem(std::make_shared<ConditionalItem>("espdrawpeds"_J, std::make_shared<ColorCommandItem>("skeletoncolorhorse"_J)));

		overlay->AddItem(std::make_shared<BoolCommandItem>("overlay"_J));
		overlay->AddItem(std::make_shared<ConditionalItem>("overlay"_J, std::make_shared<BoolCommandItem>("overlayfps"_J)));

		context->AddItem(std::make_shared<BoolCommandItem>("ctxmenu"_J));
		context->AddItem(std::make_shared<ConditionalItem>("ctxmenu"_J, std::make_shared<BoolCommandItem>("ctxmenuplayers"_J, "Players")));
		context->AddItem(std::make_shared<ConditionalItem>("ctxmenu"_J, std::make_shared<BoolCommandItem>("ctxmenupeds"_J, "Peds")));
		context->AddItem(std::make_shared<ConditionalItem>("ctxmenu"_J, std::make_shared<BoolCommandItem>("ctxmenuvehicles"_J, "Vehicles")));
		context->AddItem(std::make_shared<ConditionalItem>("ctxmenu"_J, std::make_shared<BoolCommandItem>("ctxmenuobjects"_J, "Objects")));

		syncGroup->AddItem(std::make_shared<BoolCommandItem>("blockspectate"_J));
		syncGroup->AddItem(std::make_shared<BoolCommandItem>("blockspectatesession"_J));
		syncGroup->AddItem(std::make_shared<BoolCommandItem>("blockattach"_J));
		syncGroup->AddItem(std::make_shared<BoolCommandItem>("blockvehflood"_J));

		networkEventGroup->AddItem(std::make_shared<BoolCommandItem>("blockexplosions"_J));
		networkEventGroup->AddItem(std::make_shared<BoolCommandItem>("blockptfx"_J));
		networkEventGroup->AddItem(std::make_shared<BoolCommandItem>("blockclearpedtasks"_J));
		networkEventGroup->AddItem(std::make_shared<BoolCommandItem>("blockscriptcommand"_J));
		networkEventGroup->AddItem(std::make_shared<BoolCommandItem>("userelaycxns"_J));

		scriptEventGroup->AddItem(std::make_shared<BoolCommandItem>("blockhonormanipulation"_J));
		scriptEventGroup->AddItem(std::make_shared<BoolCommandItem>("blockdefensive"_J));
		scriptEventGroup->AddItem(std::make_shared<BoolCommandItem>("blockoffensive"_J));
		scriptEventGroup->AddItem(std::make_shared<BoolCommandItem>("blockpresscharges"_J));
		scriptEventGroup->AddItem(std::make_shared<BoolCommandItem>("blockstartparlay"_J));
		scriptEventGroup->AddItem(std::make_shared<BoolCommandItem>("blockendparlay"_J));
		scriptEventGroup->AddItem(std::make_shared<BoolCommandItem>("blocktickerspam"_J));
		scriptEventGroup->AddItem(std::make_shared<BoolCommandItem>("blockstableevents"_J));
		scriptEventGroup->AddItem(std::make_shared<BoolCommandItem>("blockkickfrommissionlobby"_J));

		gui->AddItem(playerEsp);
		gui->AddItem(pedEsp);
		gui->AddItem(overlay);
		gui->AddItem(context);
		gui->AddItem(misc);

		protections->AddItem(syncGroup);
		protections->AddItem(networkEventGroup);
		protections->AddItem(scriptEventGroup);
		AddCategory(std::move(hotkeys));
		AddCategory(std::move(gui));
		AddCategory(std::move(protections));
	}
}
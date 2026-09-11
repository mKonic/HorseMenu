#include "core/Trace.hpp"
#include "core/hooking/DetourHook.hpp"
#include "game/hooks/Hooks.hpp"
#include "game/backend/ScriptPatches.hpp"

namespace YimMenu::Hooks
{
	int Script::ScriptVM(void* stack, void** globals, bool* globals_enabled, rage::scrProgram* program, rage::scrThreadContext* ctx)
	{
		TRACE_SCOPE("ScriptVM");
		ScriptPatches::OnScriptVMEnter(program);
		int res = BaseHook::Get<Script::ScriptVM, DetourHook<decltype(&ScriptVM)>>()->Original()(stack, globals, globals_enabled, program, ctx);
		ScriptPatches::OnScriptVMLeave(program);
		return res;
	}
}
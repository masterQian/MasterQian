#pragma once

#include "MasterQian.Meta.h"

#define MasterQianModuleName(_) MasterQian_System_Window_
#define MasterQianModuleNameString "MasterQian_System_Window_"
#ifdef _DEBUG
#define MasterQianLibString "MasterQian.System.Window.Debug.dll"
#else
#define MasterQianLibString "MasterQian.System.Window.dll"
#endif
#define MasterQianModuleVersion 20231229ULL
#pragma message("—————————— Please copy [" MasterQianLibString "] into your program folder ——————————")

namespace MasterQian::System::Window {
	namespace details {
		META_IMPORT_API(void, ShowTaskBar, mqbool);
		META_MODULE_BEGIN
			META_PROC_API(ShowTaskBar);
		META_MODULE_END
	}
#undef MasterQianModuleName
#undef MasterQianModuleNameString
#undef MasterQianLibString
#undef MasterQianModuleVersion

	/// <summary>
	/// 显示任务栏
	/// </summary>
	/// <param name="status">是否显示任务栏</param>
	inline void ShowTaskBar(bool status = true) noexcept {
		 details::MasterQian_System_Window_ShowTaskBar(status);
	}
}
#pragma once

#include "MasterQian.Meta.h"

#include <string>

#define MasterQianModuleName(_) MasterQian_System_Info_
#define MasterQianModuleNameString "MasterQian_System_Info_"
#ifdef _DEBUG
#define MasterQianLibString "MasterQian.System.Info.Debug.dll"
#else
#define MasterQianLibString "MasterQian.System.Info.dll"
#endif
#define MasterQianModuleVersion 20240102ULL
#pragma message("—————————— Please copy [" MasterQianLibString "] into your program folder ——————————")

namespace MasterQian::System::Info {
	namespace details {
		META_IMPORT_API(mqui32, GetScreenWidth);
		META_IMPORT_API(mqui32, GetScreenHeight);
		META_IMPORT_API(mqui32, GetTaskBarHeight);
		META_IMPORT_API(void, GetCurrentUserName, mqstr);
		META_MODULE_BEGIN
			META_PROC_API(GetScreenWidth);
			META_PROC_API(GetScreenHeight);
			META_PROC_API(GetTaskBarHeight);
			META_PROC_API(GetCurrentUserName);
		META_MODULE_END
	}
#undef MasterQianModuleName
#undef MasterQianModuleNameString
#undef MasterQianLibString
#undef MasterQianModuleVersion

	/// <summary>
	///  取屏幕宽度
	/// </summary>
	[[nodiscard]] inline mqui32 GetScreenWidth() noexcept {
		return details::MasterQian_System_Info_GetScreenWidth();
	}

	/// <summary>
	/// 取屏幕高度
	/// </summary>
	[[nodiscard]] inline mqui32 GetScreenHeight() noexcept {
		return details::MasterQian_System_Info_GetScreenHeight();
	}

	/// <summary>
	/// 取任务栏高度
	/// </summary>
	[[nodiscard]] inline mqui32 GetTaskBarHeight() noexcept {
		return details::MasterQian_System_Info_GetTaskBarHeight();
	}

	/// <summary>
	/// 取当前用户名
	/// </summary>
	[[nodiscard]] inline std::wstring GetCurrentUserName() noexcept {
		mqchar buf[260]{ };
		details::MasterQian_System_Info_GetCurrentUserName(buf);
		return buf;
	}
}
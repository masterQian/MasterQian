#pragma once

#include "MasterQian.Meta.h"
#include "MasterQian.Bin.h"

#define MasterQianModuleName(_) MasterQian_System_Process_
#define MasterQianModuleNameString "MasterQian_System_Process_"
#ifdef _DEBUG
#define MasterQianLibString "MasterQian.System.Process.Debug.dll"
#else
#define MasterQianLibString "MasterQian.System.Process.dll"
#endif
#define MasterQianModuleVersion 20231229ULL
#pragma message("—————————— Please copy [" MasterQianLibString "] into your program folder ——————————")

namespace MasterQian::System::Process {
	namespace details {
		META_IMPORT_API(mqcbytes, GetResource, mqui32, mqcstr, mqui32*);
		META_IMPORT_API(mqbool, SingleProcessLock);
		META_IMPORT_API(mqui32, Execute, mqcstr, mqcstr, mqbool, mqbool);
		META_MODULE_BEGIN
			META_PROC_API(GetResource);
			META_PROC_API(SingleProcessLock);
			META_PROC_API(Execute);
		META_MODULE_END
	}
#undef MasterQianModuleName
#undef MasterQianModuleNameString
#undef MasterQianLibString
#undef MasterQianModuleVersion

	/// <summary>
	/// 取资源，应为rc文件中对应的资源
	/// </summary>
	/// <param name="id">资源ID</param>
	/// <param name="type">资源类型，默认为FILE</param>
	/// <returns>资源字节集</returns>
	[[nodiscard]] inline BinView GetResource(mqui32 id, std::wstring_view type = L"FILE") noexcept {
		mqui32 res_size{ };
		auto mem{ details::MasterQian_System_Process_GetResource(id, type.data(), &res_size) };
		return { mem, static_cast<mqui64>(res_size) };
	}

	/// <summary>
	/// 单进程锁，保证应用程序仅有一个实例
	/// </summary>
	/// <returns>若运行的程序不是第一个实例则返回false</returns>
	[[nodiscard]] inline mqbool SingleProcessLock() noexcept {
		return details::MasterQian_System_Process_SingleProcessLock();
	}

	/// <summary>
	/// 执行进程
	/// </summary>
	/// <param name="fn">进程文件名</param>
	/// <param name="arg">参数</param>
	/// <param name="isWaiting">是否等待进程结束</param>
	/// <param name="isAdmin">是否以管理员身份运行</param>
	/// <returns>进程返回值，仅在等待时有效</returns>
	inline mqui32 Execute(std::wstring_view fn, std::wstring_view arg = L"",
		mqbool isWaiting = false, mqbool isAdmin = false) noexcept {
		return details::MasterQian_System_Process_Execute(fn.data(), arg.data(), isWaiting, isAdmin);
	}
}
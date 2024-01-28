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
#pragma message("�������������������� Please copy [" MasterQianLibString "] into your program folder ��������������������")

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
	/// ȡ��Դ��ӦΪrc�ļ��ж�Ӧ����Դ
	/// </summary>
	/// <param name="id">��ԴID</param>
	/// <param name="type">��Դ���ͣ�Ĭ��ΪFILE</param>
	/// <returns>��Դ�ֽڼ�</returns>
	[[nodiscard]] inline BinView GetResource(mqui32 id, std::wstring_view type = L"FILE") noexcept {
		mqui32 res_size{ };
		auto mem{ details::MasterQian_System_Process_GetResource(id, type.data(), &res_size) };
		return { mem, static_cast<mqui64>(res_size) };
	}

	/// <summary>
	/// ������������֤Ӧ�ó������һ��ʵ��
	/// </summary>
	/// <returns>�����еĳ����ǵ�һ��ʵ���򷵻�false</returns>
	[[nodiscard]] inline mqbool SingleProcessLock() noexcept {
		return details::MasterQian_System_Process_SingleProcessLock();
	}

	/// <summary>
	/// ִ�н���
	/// </summary>
	/// <param name="fn">�����ļ���</param>
	/// <param name="arg">����</param>
	/// <param name="isWaiting">�Ƿ�ȴ����̽���</param>
	/// <param name="isAdmin">�Ƿ��Թ���Ա�������</param>
	/// <returns>���̷���ֵ�����ڵȴ�ʱ��Ч</returns>
	inline mqui32 Execute(std::wstring_view fn, std::wstring_view arg = L"",
		mqbool isWaiting = false, mqbool isAdmin = false) noexcept {
		return details::MasterQian_System_Process_Execute(fn.data(), arg.data(), isWaiting, isAdmin);
	}
}
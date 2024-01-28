#pragma once

#include "MasterQian.Meta.h"
#include "MasterQian.Bin.h"

#define MasterQianModuleName(_) MasterQian_System_Clipboard_
#define MasterQianModuleNameString "MasterQian_System_Clipboard_"
#ifdef _DEBUG
#define MasterQianLibString "MasterQian.System.Clipboard.Debug.dll"
#else
#define MasterQianLibString "MasterQian.System.Clipboard.dll"
#endif
#define MasterQianModuleVersion 20231229ULL
#pragma message("—————————— Please copy [" MasterQianLibString "] into your program folder ——————————")

namespace MasterQian::System::Clipboard {
	namespace details {
		META_IMPORT_API(void, Clear);
		META_IMPORT_API(mqhandle, GetDataSize, mqui32, mqui64*);
		META_IMPORT_API(mqbool, GetData, mqhandle, mqmem, mqui64);
		META_IMPORT_API(mqbool, SetData, mqui32, mqcmem, mqui64);
		META_IMPORT_API(mqbool, SetFiles, mqcstr, mqui64);
		META_MODULE_BEGIN
			META_PROC_API(Clear);
			META_PROC_API(GetDataSize);
			META_PROC_API(GetData);
			META_PROC_API(SetData);
			META_PROC_API(SetFiles);
		META_MODULE_END
	}
#undef MasterQianModuleName
#undef MasterQianModuleNameString
#undef MasterQianLibString
#undef MasterQianModuleVersion

	/// <summary>
	/// 清空剪贴板
	/// </summary>
	inline void Clear() noexcept {
		details::MasterQian_System_Clipboard_Clear();
	}

	/// <summary>
	/// 取剪贴板文本
	/// </summary>
	[[nodiscard]] inline std::wstring GetString() noexcept {
		mqui64 size{ };
		std::wstring str;
		if (auto handle{ details::MasterQian_System_Clipboard_GetDataSize(13, &size) }) {
			str.resize((size - 1) >> 1);
			if (!details::MasterQian_System_Clipboard_GetData(handle, str.data(), size)) {
				str = { }; // move construct, free memory
			}
		}
		return str;
	}

	/// <summary>
	/// 置剪贴板文本
	/// </summary>
	/// <param name="sv">文本</param>
	inline bool SetString(std::wstring_view sv) noexcept {
		return details::MasterQian_System_Clipboard_SetData(13, sv.data(), (sv.size() + 1) << 1);
	}

	/// <summary>
	/// 取剪贴板位图
	/// </summary>
	/// <returns>BMP格式的字节集</returns>
	[[nodiscard]] inline Bin GetBitmap() noexcept {
		mqui64 size{ };
		Bin bin;
		if (auto handle{ details::MasterQian_System_Clipboard_GetDataSize(8, &size) }) {
			bin.resize(size + 2);
			auto pData{ bin.data() };
			if (details::MasterQian_System_Clipboard_GetData(handle, pData + 2, size)) {
				// BMP File
				// | 66 | 77 | size + 2 <4bytes> | 0 | 0 | 0 | 0 | 54 | 0 | 0 | 0 |
				// | <16bytes header> | 0 | 0 | 0 | 0 | size - 52 <4bytes> | <... content> |
				
				pData[0] = 66; // 'B'
				pData[1] = 77; // 'M'
				memcpy(pData + 26, pData + 14, 4); // Prevent overlapping copies
				memcpy(pData + 14, pData + 2, 12);
				*reinterpret_cast<mqi32*>(pData + 2) = static_cast<mqi32>(size + 2);
				memset(pData + 6, 0, 8);
				pData[10] = 54;
				memset(pData + 30, 0, 4);
				*reinterpret_cast<mqi32*>(pData + 34) = static_cast<mqi32>(size - 52);
			}
			else {
				bin = { }; // move construct, free memory
			}
		}
		return bin;
	}

	/// <summary>
	/// 置剪贴板文件，可用于在文件资源管理器复制粘贴等操作
	/// </summary>
	/// <param name="files">文件列表</param>
	inline bool SetFiles(std::vector<std::wstring> const& files) noexcept {
		std::wstring buf;
		for (auto& item : files) {
			buf += item;
			buf.push_back(L'\0');
		}
		return details::MasterQian_System_Clipboard_SetFiles(buf.data(), (buf.size() + 1) << 1);
	}

	/// <summary>
	/// 取剪贴板文件
	/// </summary>
	[[nodiscard]] inline std::vector<std::wstring> GetFiles() noexcept {
		mqui64 size{ };
		std::vector<std::wstring> files;
		if (auto handle{ details::MasterQian_System_Clipboard_GetDataSize(15, &size) }) {
			std::wstring buf(size >> 1, L'\0');
			if (details::MasterQian_System_Clipboard_GetData(handle, buf.data(), size)) {
				for (mqcstr p = buf.data() + 20 / sizeof(mqchar); *p; p += files.back().size() + 1) {
					files.emplace_back(p);
				}
			}
		}
		return files;
	}
}
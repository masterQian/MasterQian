#pragma once

#include "MasterQian.Meta.h"
#include "MasterQian.Bin.h"

#define MasterQianModuleName(_) MasterQian_Data_Transcode_
#define MasterQianModuleNameString "MasterQian_Data_Transcode_"
#ifdef _DEBUG
#define MasterQianLibString "MasterQian.Data.Transcode.Debug.dll"
#else
#define MasterQianLibString "MasterQian.Data.Transcode.dll"
#endif
#define MasterQianModuleVersion 20231229ULL
#pragma message("—————————— Please copy [" MasterQianLibString "] into your program folder ——————————")

namespace MasterQian::Data::Transcode {
	enum class CodePage : mqenum {
		ANSI = 0U, UTF8 = 65001U
	};

	namespace details {
		META_IMPORT_API(mqui32, ToStringImpl1, mqcmem, mqui32, CodePage);
		META_IMPORT_API(void, ToStringImpl2, mqcmem, mqui32, mqstr, mqui32, CodePage);
		META_IMPORT_API(mqui32, ToBinImpl1, mqcstr, mqui32, CodePage);
		META_IMPORT_API(void, ToBinImpl2, mqcstr, mqui32, mqmem, mqui32, CodePage);
		META_MODULE_BEGIN
			META_PROC_API(ToStringImpl1);
			META_PROC_API(ToStringImpl2);
			META_PROC_API(ToBinImpl1);
			META_PROC_API(ToBinImpl2);
		META_MODULE_END
	}
#undef MasterQianModuleName
#undef MasterQianModuleNameString
#undef MasterQianLibString
#undef MasterQianModuleVersion

	// 字节集转字符串
	[[nodiscard]] inline std::wstring ToString(BinView bv, CodePage cp = CodePage::UTF8) noexcept {
		std::wstring tmp;
		auto len{ details::MasterQian_Data_Transcode_ToStringImpl1(bv.data(), bv.size32(), cp) };
		if (len) {
			tmp.resize(len);
			details::MasterQian_Data_Transcode_ToStringImpl2(bv.data(), bv.size32(), tmp.data(), len, cp);
		}
		return tmp;
	}

	// 字符串转字节集
	[[nodiscard]] inline Bin ToBin(std::wstring_view sv, CodePage cp = CodePage::UTF8) noexcept {
		Bin tmp;
		auto len{ details::MasterQian_Data_Transcode_ToBinImpl1(sv.data(), static_cast<mqui32>(sv.size()), cp) };
		if (len) {
			tmp.resize(len);
			details::MasterQian_Data_Transcode_ToBinImpl2(sv.data(), static_cast<mqui32>(sv.size()), tmp.data(), len, cp);
		}
		return tmp;
	}
}
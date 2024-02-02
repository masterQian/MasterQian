module;
#include "MasterQian.Meta.h"
#define MasterQianModuleName(name) MasterQian_Data_##name
#define MasterQianModuleNameString(name) "MasterQian_Data_"#name
#ifdef _DEBUG
#define MasterQianLibString "MasterQian.Data.Debug.dll"
#else
#define MasterQianLibString "MasterQian.Data.dll"
#endif
#define MasterQianModuleVersion 20240131ULL
#pragma message("�������������������� Please copy [" MasterQianLibString "] into your program folder ��������������������")

export module MasterQian.Data;
export import MasterQian.Bin;

namespace MasterQian::Data {
	// �ı���������
	export enum class CodePage : mqenum {
		ANSI = 0U, UTF8 = 65001U
	};

	namespace details {
		META_IMPORT_API(mqui32, ToStringImpl1, mqcmem, mqui32, CodePage);
		META_IMPORT_API(void, ToStringImpl2, mqcmem, mqui32, mqstr, mqui32, CodePage);
		META_IMPORT_API(mqui32, ToBinImpl1, mqcstr, mqui32, CodePage);
		META_IMPORT_API(void, ToBinImpl2, mqcstr, mqui32, mqmem, mqui32, CodePage);
		META_IMPORT_API(mqui32, GetCRC32, mqcbytes, mqui32);
		META_IMPORT_API(void, GetMD5, mqcbytes, mqui32, mqstr);
		META_IMPORT_API(void, Base64Encode, mqcbytes, mqui64, mqstr);
		META_IMPORT_API(void, Base64Decode, mqcstr, mqui64, mqbytes);
		META_MODULE_BEGIN
			META_PROC_API(ToStringImpl1);
			META_PROC_API(ToStringImpl2);
			META_PROC_API(ToBinImpl1);
			META_PROC_API(ToBinImpl2);
			META_PROC_API(GetCRC32);
			META_PROC_API(GetMD5);
			META_PROC_API(Base64Encode);
			META_PROC_API(Base64Decode);
		META_MODULE_END
	}
}

export namespace MasterQian::Data {
	/// <summary>
	/// �ֽڼ�ת�ַ���
	/// </summary>
	/// <param name="bv">�ֽڼ�</param>
	/// <param name="cp">���룬Ĭ��ΪUTF8</param>
	/// <returns>�ַ���</returns>
	[[nodiscard]] inline std::wstring ToString(BinView bv, CodePage cp = CodePage::UTF8) noexcept {
		std::wstring tmp;
		auto len{ details::MasterQian_Data_ToStringImpl1(bv.data(), bv.size32(), cp) };
		if (len) {
			tmp.resize(len);
			details::MasterQian_Data_ToStringImpl2(bv.data(), bv.size32(), tmp.data(), len, cp);
		}
		return tmp;
	}

	/// <summary>
	/// �ַ���ת�ֽڼ�
	/// </summary>
	/// <param name="sv">�ַ���</param>
	/// <param name="cp">���룬Ĭ��ΪUTF8</param>
	/// <returns>�ֽڼ�</returns>
	[[nodiscard]] inline Bin ToBin(std::wstring_view sv, CodePage cp = CodePage::UTF8) noexcept {
		Bin tmp;
		auto len{ details::MasterQian_Data_ToBinImpl1(sv.data(), static_cast<mqui32>(sv.size()), cp) };
		if (len) {
			tmp.resize(len);
			details::MasterQian_Data_ToBinImpl2(sv.data(), static_cast<mqui32>(sv.size()), tmp.data(), len, cp);
		}
		return tmp;
	}

	/// <summary>
	/// ȡ��ϣֵ
	/// </summary>
	/// <param name="bv">�ֽڼ�</param>
	/// <returns>��ϣֵ</returns>
	[[nodiscard]] inline mqui64 HashValue(BinView bv) noexcept {
		return freestanding::hash_bytes(bv.data(), bv.size());
	}

	/// <summary>
	/// ȡCRC32У��ֵ
	/// </summary>
	/// <param name="bv">�ֽڼ�</param>
	/// <returns>CRC32У��ֵ</returns>
	[[nodiscard]] inline mqui32 CRC32(BinView bv) noexcept {
		return details::MasterQian_Data_GetCRC32(bv.data(), bv.size32());
	}

	/// <summary>
	/// ȡMD5ժҪ
	/// </summary>
	/// <param name="bv">�ֽڼ�</param>
	/// <returns>32λMD5ֵ����д</returns>
	[[nodiscard]] inline std::wstring MD5(BinView bv) noexcept {
		std::wstring md5(32, L'\0');
		details::MasterQian_Data_GetMD5(bv.data(), bv.size32(), md5.data());
		return md5;
	}

	/// <summary>
	/// Base64����
	/// </summary>
	/// <param name="bv">�ֽڼ�</param>
	/// <returns>Base64�����ַ���</returns>
	[[nodiscard]] inline std::wstring Base64Encode(BinView bv) noexcept {
		auto len{ bv.size() };
		auto num24Bit{ len / 3U };
		auto bPadding{ len > num24Bit * 3 };
		std::wstring base64((num24Bit + bPadding) << 2, L'\0');
		details::MasterQian_Data_Base64Encode(bv.data(), len, base64.data());
		return base64;
	}

	/// <summary>
	/// Base64����
	/// </summary>
	/// <param name="sv">�ַ���</param>
	/// <returns>Base64�����ֽڼ�</returns>
	[[nodiscard]] inline Bin Base64Decode(std::wstring_view sv) noexcept {
		mqui64 len{ sv.size() }, padSize{ }, retSize{ };
		if (len && len % 4 == 0) {
			auto num32Bit{ (len >> 2) - 1 };
			auto pEnd{ &sv.back() };
			if (*pEnd == L'=') padSize = (*(pEnd - 1) == L'=' ? 1 : 2);
			else ++num32Bit;
			retSize = num32Bit * 3 + padSize;
		}
		Bin bin(retSize);
		details::MasterQian_Data_Base64Decode(sv.data(), len, bin.data());
		return bin;
	}
}

export [[nodiscard]] inline MasterQian::Bin operator ""_ansi(mqcstr str, mqui64 size) noexcept {
	return MasterQian::Data::ToBin(std::wstring_view{ str, size }, MasterQian::Data::CodePage::ANSI);
}

export [[nodiscard]] inline MasterQian::Bin operator ""_utf8(mqcstr str, mqui64 size) noexcept {
	return MasterQian::Data::ToBin(std::wstring_view{ str, size }, MasterQian::Data::CodePage::UTF8);
}
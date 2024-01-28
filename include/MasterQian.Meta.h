#pragma once

// ���뻷��
#if !defined(__cplusplus) || !defined(__cpp_concepts) || _MSVC_LANG < 202002L
#error "Module requires at least C++20 standard compiler."
#endif

#ifndef _WIN64
#error "Module requires Windows x64."
#endif

// ���Ͷ���

/// <summary>
/// Windows DLL extern C 4�ֽ� Boolean����
/// </summary>
using mqbool = int;
/// <summary>
/// 8λ�з�������
/// </summary>
using mqi8 = signed char;
/// <summary>
/// 16λ�з�������
/// </summary>
using mqi16 = short;
/// <summary>
/// 32λ�з�������
/// </summary>
using mqi32 = int;
/// <summary>
/// 64λ�з�������
/// </summary>
using mqi64 = long long;
/// <summary>
/// 8λ�޷�������
/// </summary>
using mqui8 = unsigned char;
/// <summary>
/// 16λ�޷�������
/// </summary>
using mqui16 = unsigned short;
/// <summary>
/// 32λ�޷�������
/// </summary>
using mqui32 = unsigned int;
/// <summary>
/// 64λ�޷�������
/// </summary>
using mqui64 = unsigned long long;
/// <summary>
/// ���ֳ�����
/// </summary>
using mqword = unsigned short;
/// <summary>
/// ˫�ֳ�����
/// </summary>
using mqdword = unsigned long;
/// <summary>
/// ���ֳ�����
/// </summary>
using mqqword = unsigned long long;
/// <summary>
/// 32λ������
/// </summary>
using mqf32 = float;
/// <summary>
/// 64λ������
/// </summary>
using mqf64 = double;
/// <summary>
/// ö�ٻ�����
/// </summary>
using mqenum = mqui32;
/// <summary>
/// �ڴ��ַ
/// </summary>
using mqmem = void*;
/// <summary>
/// ֻ���ڴ��ַ
/// </summary>
using mqcmem = void const*;
/// <summary>
/// ���
/// </summary>
using mqhandle = void*;
/// <summary>
/// �ֽ�
/// </summary>
using mqbyte = unsigned char;
/// <summary>
/// �ֽ���
/// </summary>
using mqbytes = unsigned char*;
/// <summary>
/// ֻ���ֽ���
/// </summary>
using mqcbytes = unsigned char const*;
/// <summary>
/// ASCII�ַ�
/// </summary>
using mqchara = char;
/// <summary>
/// ASCII�ַ���
/// </summary>
using mqstra = char*;
/// <summary>
/// ֻ��ASCII�ַ���
/// </summary>
using mqcstra = char const*;
/// <summary>
/// ���ַ�
/// </summary>
using mqchar = wchar_t;
/// <summary>
/// ���ַ���
/// </summary>
using mqstr = wchar_t*;
/// <summary>
/// ֻ�����ַ���
/// </summary>
using mqcstr = wchar_t const*;
/// <summary>
/// ��������
/// </summary>
using mqproc = long long(__stdcall*)() noexcept;
/// <summary>
/// GUID
/// </summary>
struct mqguid {
	mqdword Data1;
	mqword Data2;
	mqword Data3;
	mqbyte Data4[8];
};

namespace MasterQian::Meta {
// ����Win32API
#define META_WINAPI(type, name, ...) extern __declspec(dllimport) type __stdcall name (__VA_ARGS__) noexcept

// ���Win32API�б�
	namespace details {
		META_WINAPI(mqhandle, LoadLibraryW, mqcstr);
		META_WINAPI(mqproc, GetProcAddress, mqhandle, mqcstra);
		META_WINAPI(mqi32, MessageBoxW, mqhandle, mqcstr, mqcstr, mqui32);
		META_WINAPI(__declspec(noreturn) void, ExitProcess, mqui32);
		META_WINAPI(mqstra, lstrcpyA, mqstra, mqcstra);
		META_WINAPI(mqstra, lstrcatA, mqstra, mqcstra);
		META_WINAPI(mqi32, lstrcmpW, mqcstr, mqcstr);
		META_WINAPI(mqi32, lstrcmpiW, mqcstr, mqcstr);
		META_WINAPI(mqstr, lstrcpynW, mqstr, mqcstr, mqui32);
		META_WINAPI(mqstr, lstrcpyW, mqstr, mqcstr);
		META_WINAPI(mqstr, lstrcatW, mqstr, mqcstr);
		META_WINAPI(mqui32, lstrlenW, mqcstr);
		META_WINAPI(mqi32, wsprintfW, mqstr, mqcstr, ...);
	}

// ������������
#pragma comment(linker,"/alternatename:__imp_?LoadLibraryW@details@Meta@MasterQian@@YAPEAXPEB_W@Z=__imp_LoadLibraryW")
#pragma comment(linker,"/alternatename:__imp_?GetProcAddress@details@Meta@MasterQian@@YAP6A_JX_EPEAXPEBD@Z=__imp_GetProcAddress")
#pragma comment(linker,"/alternatename:__imp_?MessageBoxW@details@Meta@MasterQian@@YAHPEAXPEB_W1I@Z=__imp_MessageBoxW")
#pragma comment(linker,"/alternatename:__imp_?ExitProcess@details@Meta@MasterQian@@YAXI@Z=__imp_ExitProcess")
#pragma comment(linker,"/alternatename:__imp_?lstrcpyA@details@Meta@MasterQian@@YAPEADPEADPEBD@Z=__imp_lstrcpyA")
#pragma comment(linker,"/alternatename:__imp_?lstrcatA@details@Meta@MasterQian@@YAPEADPEADPEBD@Z=__imp_lstrcatA")
#pragma comment(linker,"/alternatename:__imp_?lstrcmpW@details@Meta@MasterQian@@YAHPEB_W0@Z=__imp_lstrcmpW")
#pragma comment(linker,"/alternatename:__imp_?lstrcmpiW@details@Meta@MasterQian@@YAHPEB_W0@Z=__imp_lstrcmpiW")
#pragma comment(linker,"/alternatename:__imp_?lstrcpynW@details@Meta@MasterQian@@YAPEA_WPEA_WPEB_WI@Z=__imp_lstrcpynW")
#pragma comment(linker,"/alternatename:__imp_?lstrcpyW@details@Meta@MasterQian@@YAPEA_WPEA_WPEB_W@Z=__imp_lstrcpyW")
#pragma comment(linker,"/alternatename:__imp_?lstrcatW@details@Meta@MasterQian@@YAPEA_WPEA_WPEB_W@Z=__imp_lstrcatW")
#pragma comment(linker,"/alternatename:__imp_?lstrlenW@details@Meta@MasterQian@@YAIPEB_W@Z=__imp_lstrlenW")
#pragma comment(linker,"/alternatename:__imp_?wsprintfW@details@Meta@MasterQian@@YAHPEA_WPEB_WZZ=__imp_wsprintfW")


// ģ���ʼ��(������.h)
#define META_MODULE_BEGIN \
inline mqhandle MasterQianModuleName(_)##ModuleHandle = []() noexcept -> mqhandle { \
mqhandle handle { Meta::details::LoadLibraryW(L"" MasterQianLibString) }; \
mqcstr errMsg {}; \
if (handle) { \
	using VerFuncType = mqui64(__stdcall*)(); \
	if(auto VerFunc { (VerFuncType)Meta::details::GetProcAddress(handle, "MasterQianVersion") }) { \
		if(auto ver{ VerFunc() }; ver != MasterQianModuleVersion) { \
			errMsg = L"Header version mismatched library \"" MasterQianLibString "\" version !"; \
		} \
	} \
	else { \
		errMsg = L"The library file \"" MasterQianLibString "\" is damaged!"; \
	} \
} \
else { \
	errMsg = L"Missing \"" MasterQianLibString "\" !"; \
} \
if (errMsg) { \
	Meta::details::MessageBoxW(nullptr, errMsg, L"MasterQian", 16U); \
	Meta::details::ExitProcess(static_cast<mqui32>(-1)); \
}

#define META_MODULE_END \
return handle; \
}();


// ģ�鵼��API����(������.h)
#define META_IMPORT_API(type, name, ...) inline type(__stdcall* MasterQianModuleName(_)##name) (__VA_ARGS__);
// ģ��API��ȡ(������.h)
#define META_PROC_API(name) MasterQianModuleName(_)##name = (decltype(MasterQianModuleName(_)##name))Meta::details::GetProcAddress(handle, MasterQianModuleNameString #name)


// ����API(������.cpp)
#define META_EXPORT_API(type, name, ...) extern "C" __declspec(dllexport) type __stdcall MasterQianModuleName(_)##name (__VA_ARGS__)

// �����汾API(������.cpp)
#define META_EXPORT_API_VERSION(ver) extern "C" __declspec(dllexport) mqui64 __stdcall MasterQianVersion() { return ver; }


// ͨ�ýṹ
	
	namespace details {
		template <typename T>
		concept string_type = requires(T t) {
			t.data();
			t.size();
			typename T::value_type;
		};

		[[nodiscard]] inline constexpr mqui64 hash_bytes(mqcbytes data, mqui64 size) noexcept {
			mqui64 value{ 14695981039346656037ULL };
			for (size_t i{ }; i < size; ++i) {
				value ^= static_cast<mqui64>(data[i]);
				value *= 1099511628211ULL;
			}
			return value;
		}

		template<string_type T>
		[[nodiscard]] inline constexpr mqui64 hash_impl(T const& t) noexcept {
			return hash_bytes(static_cast<mqcbytes>(static_cast<mqcmem>(t.data())),
				static_cast<mqui64>(t.size() * sizeof(typename T::value_type)));
		}

		template<mqui64 N>
		inline constexpr mqui64 hash_impl(mqchar const(&t)[N]) noexcept {
			return hash_bytes(static_cast<mqcbytes>(static_cast<mqcmem>(t)),
				(N - 1ULL) * sizeof(mqchar));
		}

		template<typename T>
		inline constexpr mqui64 hash_impl(T const& t) noexcept {
			return hash_bytes(static_cast<mqcbytes>(static_cast<mqcmem>(&t)), sizeof(T));
		}
	}

	/// <summary>
	/// �칹��ϣ
	/// </summary>
	struct isomerism_hash {
		template<typename T>
		[[nodiscard]] constexpr auto operator () (T&& t) const noexcept {
			return details::hash_impl(static_cast<T&&>(t));
		}
		using is_transparent = void;
	};

	/// <summary>
	/// �칹���
	/// </summary>
	struct isomerism_equal {
		template <class T1, class T2>
		[[nodiscard]] constexpr auto operator()(T1&& t1, T2&& t2) const noexcept {
			return static_cast<T1&&>(t1) == static_cast<T2&&>(t2);
		}
		using is_transparent = void;
	};


// ͨ�ú���
	
	/// <summary>
	/// �ϳ�64λ
	/// </summary>
	/// <param name="low">��32λ</param>
	/// <param name="high">��32λ</param>
	[[nodiscard]] inline static constexpr mqui64 MakeQWORD(mqui32 low, mqui32 high) noexcept {
		return static_cast<mqui64>(low) | (static_cast<mqui64>(high) << 32ULL);
	}

	/// <summary>
	/// ȡ��32λ
	/// </summary>
	/// <param name="v">64λ</param>
	[[nodiscard]] inline static constexpr mqui32 LowQWORD(mqui64 v) noexcept {
		return static_cast<mqui32>(v & 0xFFFFFFFFULL);
	}

	/// <summary>
	/// ȡ��32λ
	/// </summary>
	/// <param name="v">64λ</param>
	[[nodiscard]] inline static constexpr mqui32 HighQWORD(mqui64 v) noexcept {
		return static_cast<mqui32>((v >> 32U) & 0xFFFFFFFFULL);
	}
}
#pragma once

// 编译环境
#if !defined(__cplusplus) || !defined(__cpp_concepts) || _MSVC_LANG < 202002L
#error "Module requires at least C++20 standard compiler."
#endif

#ifndef _WIN64
#error "Module requires Windows x64."
#endif

// 类型定义

/// <summary>
/// Windows DLL extern C 4字节 Boolean类型
/// </summary>
using mqbool = int;
/// <summary>
/// 8位有符号整数
/// </summary>
using mqi8 = signed char;
/// <summary>
/// 16位有符号整数
/// </summary>
using mqi16 = short;
/// <summary>
/// 32位有符号整数
/// </summary>
using mqi32 = int;
/// <summary>
/// 64位有符号整数
/// </summary>
using mqi64 = long long;
/// <summary>
/// 8位无符号整数
/// </summary>
using mqui8 = unsigned char;
/// <summary>
/// 16位无符号整数
/// </summary>
using mqui16 = unsigned short;
/// <summary>
/// 32位无符号整数
/// </summary>
using mqui32 = unsigned int;
/// <summary>
/// 64位无符号整数
/// </summary>
using mqui64 = unsigned long long;
/// <summary>
/// 单字长整数
/// </summary>
using mqword = unsigned short;
/// <summary>
/// 双字长整数
/// </summary>
using mqdword = unsigned long;
/// <summary>
/// 四字长整数
/// </summary>
using mqqword = unsigned long long;
/// <summary>
/// 32位浮点数
/// </summary>
using mqf32 = float;
/// <summary>
/// 64位浮点数
/// </summary>
using mqf64 = double;
/// <summary>
/// 枚举基类型
/// </summary>
using mqenum = mqui32;
/// <summary>
/// 内存地址
/// </summary>
using mqmem = void*;
/// <summary>
/// 只读内存地址
/// </summary>
using mqcmem = void const*;
/// <summary>
/// 句柄
/// </summary>
using mqhandle = void*;
/// <summary>
/// 字节
/// </summary>
using mqbyte = unsigned char;
/// <summary>
/// 字节流
/// </summary>
using mqbytes = unsigned char*;
/// <summary>
/// 只读字节流
/// </summary>
using mqcbytes = unsigned char const*;
/// <summary>
/// ASCII字符
/// </summary>
using mqchara = char;
/// <summary>
/// ASCII字符串
/// </summary>
using mqstra = char*;
/// <summary>
/// 只读ASCII字符串
/// </summary>
using mqcstra = char const*;
/// <summary>
/// 宽字符
/// </summary>
using mqchar = wchar_t;
/// <summary>
/// 宽字符串
/// </summary>
using mqstr = wchar_t*;
/// <summary>
/// 只读宽字符串
/// </summary>
using mqcstr = wchar_t const*;
/// <summary>
/// 函数调用
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
// 引入Win32API
#define META_WINAPI(type, name, ...) extern __declspec(dllimport) type __stdcall name (__VA_ARGS__) noexcept

// 最低Win32API列表
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

// 导出名重链接
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


// 模块初始化(仅用于.h)
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


// 模块导入API定义(仅用于.h)
#define META_IMPORT_API(type, name, ...) inline type(__stdcall* MasterQianModuleName(_)##name) (__VA_ARGS__);
// 模块API提取(仅用于.h)
#define META_PROC_API(name) MasterQianModuleName(_)##name = (decltype(MasterQianModuleName(_)##name))Meta::details::GetProcAddress(handle, MasterQianModuleNameString #name)


// 导出API(仅用于.cpp)
#define META_EXPORT_API(type, name, ...) extern "C" __declspec(dllexport) type __stdcall MasterQianModuleName(_)##name (__VA_ARGS__)

// 导出版本API(仅用于.cpp)
#define META_EXPORT_API_VERSION(ver) extern "C" __declspec(dllexport) mqui64 __stdcall MasterQianVersion() { return ver; }


// 通用结构
	
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
	/// 异构哈希
	/// </summary>
	struct isomerism_hash {
		template<typename T>
		[[nodiscard]] constexpr auto operator () (T&& t) const noexcept {
			return details::hash_impl(static_cast<T&&>(t));
		}
		using is_transparent = void;
	};

	/// <summary>
	/// 异构相等
	/// </summary>
	struct isomerism_equal {
		template <class T1, class T2>
		[[nodiscard]] constexpr auto operator()(T1&& t1, T2&& t2) const noexcept {
			return static_cast<T1&&>(t1) == static_cast<T2&&>(t2);
		}
		using is_transparent = void;
	};


// 通用函数
	
	/// <summary>
	/// 合成64位
	/// </summary>
	/// <param name="low">低32位</param>
	/// <param name="high">高32位</param>
	[[nodiscard]] inline static constexpr mqui64 MakeQWORD(mqui32 low, mqui32 high) noexcept {
		return static_cast<mqui64>(low) | (static_cast<mqui64>(high) << 32ULL);
	}

	/// <summary>
	/// 取低32位
	/// </summary>
	/// <param name="v">64位</param>
	[[nodiscard]] inline static constexpr mqui32 LowQWORD(mqui64 v) noexcept {
		return static_cast<mqui32>(v & 0xFFFFFFFFULL);
	}

	/// <summary>
	/// 取高32位
	/// </summary>
	/// <param name="v">64位</param>
	[[nodiscard]] inline static constexpr mqui32 HighQWORD(mqui64 v) noexcept {
		return static_cast<mqui32>((v >> 32U) & 0xFFFFFFFFULL);
	}
}
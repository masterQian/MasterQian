#include "../include/MasterQian.Meta.h"
#include <string>
import MasterQian.freestanding;
using namespace MasterQian;
#define MasterQianModuleName(name) MasterQian_Log_##name
META_EXPORT_API_VERSION(20240131ULL)

using LPSECURITY_ATTRIBUTES = struct SECURITY_ATTRIBUTES {
	mqui32 nLength;
	mqmem lpSecurityDescriptor;
	mqbool bInheritHandle;
}*;

using LPOVERLAPPED = struct OVERLAPPED {
	mqui64 Internal;
	mqui64 InternalHigh;
	union {
		struct {
			mqui32 Offset;
			mqui32 OffsetHigh;
		} DUMMYSTRUCTNAME;
		mqmem Pointer;
	} DUMMYUNIONNAME;
	mqhandle  hEvent;
}*;

constexpr mqui32 CREATE_ALWAYS{ 2U };
constexpr mqui32 GENERIC_WRITE{ 0x40000000U };
constexpr mqui32 FILE_ATTRIBUTE_NORMAL{ 0x00000080U };
mqhandle INVALID_HANDLE_VALUE{ reinterpret_cast<mqhandle>(-1) };
constexpr mqui32 FOREGROUND_BLUE{ 0x0001U };
constexpr mqui32 FOREGROUND_GREEN{ 0x0002U };
constexpr mqui32 FOREGROUND_RED{ 0x0004U };

META_WINAPI(mqhandle, GetStdHandle, mqui32);
META_WINAPI(mqbool, CloseHandle, mqhandle);
META_WINAPI(mqhandle, CreateFileW, mqcstr, mqui32, mqui32, LPSECURITY_ATTRIBUTES, mqui32, mqui32, mqhandle);
META_WINAPI(mqbool, WriteFile, mqhandle, mqcmem, mqui32, mqui32*, LPOVERLAPPED);
META_WINAPI(mqbool, WriteConsoleW, mqhandle, mqcmem, mqui32, mqui32*, mqmem);
META_WINAPI(mqbool, SetConsoleTextAttribute, mqhandle, mqui16);
META_WINAPI(void, OutputDebugStringW, mqcstr);

#pragma comment(linker,"/alternatename:__imp_?GetStdHandle@@YAPEAXI@Z=__imp_GetStdHandle")
#pragma comment(linker,"/alternatename:__imp_?CloseHandle@@YAHPEAX@Z=__imp_CloseHandle")
#pragma comment(linker,"/alternatename:__imp_?CreateFileW@@YAPEAXPEB_WIIPEAUSECURITY_ATTRIBUTES@@IIPEAX@Z=__imp_CreateFileW")
#pragma comment(linker,"/alternatename:__imp_?WriteFile@@YAHPEAXPEBXIPEAIPEAUOVERLAPPED@@@Z=__imp_WriteFile")
#pragma comment(linker,"/alternatename:__imp_?WriteConsoleW@@YAHPEAXPEBXIPEAI0@Z=__imp_WriteConsoleW")
#pragma comment(linker,"/alternatename:__imp_?SetConsoleTextAttribute@@YAHPEAXG@Z=__imp_SetConsoleTextAttribute")
#pragma comment(linker,"/alternatename:__imp_?OutputDebugStringW@@YAXPEB_W@Z=__imp_OutputDebugStringW")

enum class LogType : mqenum {
	STD_CONSOLE, DEBUG_CONSOLE, FILE,
};

enum class LogTag : mqenum {
	INFO, WARNING, ERROR
};

META_EXPORT_API(mqhandle, CreateLogger, LogType type, mqcmem arg) {
	switch (type) {
	case LogType::STD_CONSOLE: {
		return GetStdHandle(static_cast<mqui32>(-11));
	}
	case LogType::DEBUG_CONSOLE: {
		return reinterpret_cast<mqhandle>(5201314U);
	}
	case LogType::FILE: {
		if (arg) {
			auto hFile{ CreateFileW(static_cast<mqcstr>(arg), GENERIC_WRITE, 0,
				nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr) };
			return hFile != INVALID_HANDLE_VALUE ? hFile : nullptr;
		}
	}
	}
	return nullptr;
}

META_EXPORT_API(void, CloseLogger, LogType type, mqhandle handle) {
	if (type == LogType::FILE) {
		CloseHandle(handle);
	}
}

META_EXPORT_API(void, LoggerLog, LogType type, LogTag tag, mqhandle handle, mqcstr msg, mqui32 size) {
	switch (type) {
	case LogType::STD_CONSOLE: {
		switch (tag) {
		case LogTag::INFO: {
			SetConsoleTextAttribute(handle, FOREGROUND_GREEN);
			break;
		}
		case LogTag::WARNING: {
			SetConsoleTextAttribute(handle, FOREGROUND_BLUE);
			break;
		}
		case LogTag::ERROR: {
			SetConsoleTextAttribute(handle, FOREGROUND_RED);
			break;
		}
		}
		WriteConsoleW(handle, msg, size, nullptr, nullptr);
		SetConsoleTextAttribute(handle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		break;
	}
	case LogType::FILE: {
		auto len{ api::WideCharToMultiByte(65001U, 0, msg, size, nullptr, 0, nullptr, nullptr) };
		if (len > 0) {
			static std::string buffer;
			if (buffer.capacity() < len) {
				buffer.reserve(len);
			}
			api::WideCharToMultiByte(65001U, 0, msg, size, buffer.data(), len, nullptr, nullptr);
			WriteFile(handle, buffer.data(), len, nullptr, nullptr);
		}
		break;
	}
	case LogType::DEBUG_CONSOLE: {
		OutputDebugStringW(msg);
		break;
	}
	}
}
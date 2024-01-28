#include "../include/MasterQian.Meta.h"

#define _AMD64_
#include <minwindef.h>
#include <processenv.h>
#include <fileapi.h>
#include <handleapi.h>
#include <consoleapi.h>
#include <consoleapi2.h>
#include <stringapiset.h>
#include <debugapi.h>

#include <string>

#define MasterQianModuleName(_) MasterQian_System_Log_
META_EXPORT_API_VERSION(20240111ULL)

namespace details {
	enum class LogType : mqenum {
		STD_CONSOLE, DEBUG_CONSOLE, FILE,
	};

	enum class LogTag : mqenum {
		INFO, WARNING, ERROR
	};
}

META_EXPORT_API(mqhandle, CreateLogger, details::LogType type, mqcmem arg) {
	switch (type) {
	case details::LogType::STD_CONSOLE: {
		return GetStdHandle(static_cast<mqdword>(-11));
	}
	case details::LogType::DEBUG_CONSOLE: {
		return reinterpret_cast<mqhandle>(5201314U);
	}
	case details::LogType::FILE: {
		if (arg) {
			auto hFile{ CreateFileW(static_cast<mqcstr>(arg), GENERIC_WRITE, 0,
				nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL,
				nullptr) };
			return hFile != INVALID_HANDLE_VALUE ? hFile : nullptr;
		}
	}
	}
	return nullptr;
}

META_EXPORT_API(void, CloseLogger, details::LogType type, mqhandle handle) {
	if (type == details::LogType::FILE) {
		CloseHandle(handle);
	}
}

META_EXPORT_API(void, LoggerLog, details::LogType type, details::LogTag tag, mqhandle handle, mqcstr msg, mqui32 size) {
	switch (type) {
	case details::LogType::STD_CONSOLE: {
		switch (tag) {
		case details::LogTag::INFO: {
			SetConsoleTextAttribute(handle, FOREGROUND_GREEN);
			break;
		}
		case details::LogTag::WARNING: {
			SetConsoleTextAttribute(handle, FOREGROUND_BLUE);
			break;
		}
		case details::LogTag::ERROR: {
			SetConsoleTextAttribute(handle, FOREGROUND_RED);
			break;
		}
		}
		WriteConsoleW(handle, msg, size, nullptr, nullptr);
		SetConsoleTextAttribute(handle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		break;
	}
	case details::LogType::FILE: {
		auto len{ WideCharToMultiByte(CP_UTF8, 0, msg, size,
			nullptr, 0, nullptr, nullptr) };
		if (len > 0) {
			static std::string buffer;
			if (buffer.capacity() < len) {
				buffer.reserve(len);
			}
			WideCharToMultiByte(CP_UTF8, 0, msg, size, buffer.data(), len, nullptr, nullptr);
			WriteFile(handle, buffer.data(), len, nullptr, nullptr);
		}
		break;
	}
	case details::LogType::DEBUG_CONSOLE: {
		OutputDebugStringW(msg);
		break;
	}
	}
}
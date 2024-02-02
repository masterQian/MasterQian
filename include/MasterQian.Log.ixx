module;
#include "MasterQian.Meta.h"
#define MasterQianModuleName(name) MasterQian_Log_##name
#define MasterQianModuleNameString(name) "MasterQian_Log_"#name
#ifdef _DEBUG
#define MasterQianLibString "MasterQian.Log.Debug.dll"
#else
#define MasterQianLibString "MasterQian.Log.dll"
#endif
#define MasterQianModuleVersion 20240131ULL
#pragma message("—————————— Please copy [" MasterQianLibString "] into your program folder ——————————")

export module MasterQian.Log;
export import MasterQian.freestanding;
export import <string>;

export namespace MasterQian {
	// 日志类型
	enum class LogType : mqenum {
		STD_CONSOLE, // 控制台日志
		DEBUG_CONSOLE, // 调试窗口日志
		FILE, // 文件日志
	};

	// 日志标签
	enum class LogTag : mqenum {
		INFO, // 信息
		WARNING, // 警告
		ERR // 错误
	};
}

namespace MasterQian::details {
	META_IMPORT_API(mqhandle, CreateLogger, LogType, mqcmem);
	META_IMPORT_API(void, CloseLogger, LogType, mqhandle);
	META_IMPORT_API(void, LoggerLog, LogType, LogTag, mqhandle, mqcstr, mqui32);
	META_MODULE_BEGIN
		META_PROC_API(CreateLogger);
	META_PROC_API(CloseLogger);
	META_PROC_API(LoggerLog);
	META_MODULE_END

	template<typename T>
	concept can_to_wstring = requires (freestanding::remove_cvref<T> t) {
		{ std::to_wstring(t) } -> freestanding::same<std::wstring>;
	} && !requires(freestanding::remove_cvref<T> t) {
		std::wstring(t);
	};

	template<can_to_wstring T>
	inline void LoggerLogValue(std::wstring& buf, T&& t) {
		buf += std::to_wstring(freestanding::forward<T>(t));
	}

	inline void LoggerLogValue(std::wstring& buf, std::wstring_view sv) {
		buf += sv;
	}

	template<typename T>
	concept log_userstruct_func = requires (std::wstring & buf, freestanding::remove_cvref<T> const t) {
		{ Log(buf, t) } noexcept -> freestanding::same<void>;
	};

	template<typename T>
	concept log_userstruct_member_func = requires (std::wstring & buf, freestanding::remove_cvref<T> const t) {
		{ t.Log(buf) } noexcept -> freestanding::same<void>;
	};

	/// <summary>
	/// <para>用户可以通过实现Log接口来输出自定义类型</para>
	/// <para>以输出坐标Pos类为例</para>
	/// <example>
	/// <code>
	/// struct Pos {
	///		int x, y;
	///		inline void Log(std::wstring& buf) const noexcept {
	///			buf += std::to_wstring(x) + L"," + std::wstring(y);
	///		}
	/// }
	/// 或
	/// struct Pos {
	///		int x, y;
	/// }
	/// inline void Log(std::wstring& buf, Pos pos) noexcept {
	///		buf += std::to_wstring(pos.x) + L"," + std::wstring(pos.y);
	/// }
	/// </code>
	/// </example>
	/// </summary>
	template<typename T>
	requires (log_userstruct_func<T> || log_userstruct_member_func<T>)
	inline void LoggerLogValue(std::wstring& buf, T const& t) {
		if constexpr (log_userstruct_func<T>) {
			Log(buf, t);
		}
		else {
			t.Log(buf);
		}
	}
}

export namespace MasterQian {
	template<LogType type>
	struct Logger {
	private:
		mqhandle handle;

		static constexpr mqcstr LogTagString[] = { L"[Info] ", L"[Warning] ", L"[Error] " };
	public:
		/// <summary>
		/// 除文件日志需要以文件名作为参数其他均不需要
		/// </summary>
		/// <param name="arg">参数</param>
		Logger(mqcmem arg = nullptr) noexcept {
			handle = details::MasterQian_Log_CreateLogger(type, arg);
		}

		Logger(Logger const&) = delete;
		Logger& operator = (Logger const&) = delete;

		~Logger() noexcept {
			close();
		}

		/// <summary>
		/// 重定向
		/// </summary>
		/// <param name="arg">参数</param>
		void reset(mqcmem arg = nullptr) noexcept {
			close();
			handle = details::MasterQian_Log_CreateLogger(type, arg);
		}

		/// <summary>
		/// 关闭
		/// </summary>
		void close() noexcept {
			if (handle) {
				details::MasterQian_Log_CloseLogger(type, handle);
				handle = nullptr;
			}
		}

		/// <summary>
		/// 输出
		/// </summary>
		/// <typeparam name="tag">日志标签</typeparam>
		/// <typeparam name="newLine">自动换行</typeparam>
		template<LogTag tag = LogTag::INFO, bool newLine = true, typename... Args>
		void log(Args&&... args) const noexcept {
			if (handle) {
				std::wstring buf{ LogTagString[static_cast<mqui32>(tag)] };
				(details::LoggerLogValue(buf, freestanding::forward<Args>(args)), ...);
				if (newLine) {
					buf += (type == LogType::FILE ? L"\r\n" : L"\n");
				}
				details::MasterQian_Log_LoggerLog(type, tag, handle, buf.data(), static_cast<mqui32>(buf.size()));
			}
		}

		/// <summary>
		/// 输出信息
		/// </summary>
		template<bool newLine = true, typename... Args>
		void i(Args&&... args) const noexcept {
			log<LogTag::INFO, newLine>(freestanding::forward<Args>(args)...);
		}

		/// <summary>
		/// 输出警告
		/// </summary>
		template<bool newLine = true, typename... Args>
		void w(Args&&... args) const noexcept {
			log<LogTag::WARNING, newLine>(freestanding::forward<Args>(args)...);
		}

		/// <summary>
		/// 输出错误
		/// </summary>
		template<bool newLine = true, typename... Args>
		void e(Args&&... args) const noexcept {
			log<LogTag::ERR, newLine>(freestanding::forward<Args>(args)...);
		}
	};

	// 控制台日志，将在stdout输出
	using ConsoleLogger = Logger<LogType::STD_CONSOLE>;

	// 调试窗口日志，将在debug窗口输出
	using DebugLogger = Logger<LogType::DEBUG_CONSOLE>;

	// 文件日志，指定文件输出
	using FileLogger = Logger<LogType::FILE>;
}


/*    对基础类型输出支持    */

// mqguid
export inline void Log(std::wstring& buf, mqguid const& guid) noexcept {
	mqchar str[37]{ };
	MasterQian::api::wsprintfW(str, L"%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
		guid.Data1, guid.Data2, guid.Data3, guid.Data4[0], guid.Data4[1], guid.Data4[2],
		guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
	buf += str;
}

// mqpoint
export inline void Log(std::wstring& buf, mqpoint point) noexcept {
	mqchar str[64]{ };
	MasterQian::api::wsprintfW(str, L"(%u, %u)", point.x, point.y);
	buf += str;
}

// mqsize
export inline void Log(std::wstring& buf, mqsize size) noexcept {
	mqchar str[64]{ };
	MasterQian::api::wsprintfW(str, L"(%u, %u)", size.width, size.height);
	buf += str;
}

// mqpoint
export inline void Log(std::wstring& buf, mqrect rect) noexcept {
	mqchar str[128]{ };
	MasterQian::api::wsprintfW(str, L"(%u, %u, %u, %u)", rect.left, rect.top, rect.width, rect.height);
	buf += str;
}

// mqpoint
export inline void Log(std::wstring& buf, mqrange range) noexcept {
	mqchar str[128]{ };
	MasterQian::api::wsprintfW(str, L"(%u, %u, %u, %u)", range.left, range.top, range.right, range.bottom);
	buf += str;
}
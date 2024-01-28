#pragma once

#include "MasterQian.Meta.h"

#include <string>
#include <concepts>

#define MasterQianModuleName(_) MasterQian_System_Log_
#define MasterQianModuleNameString "MasterQian_System_Log_"
#ifdef _DEBUG
#define MasterQianLibString "MasterQian.System.Log.Debug.dll"
#else
#define MasterQianLibString "MasterQian.System.Log.dll"
#endif
#define MasterQianModuleVersion 20240111ULL
#pragma message("—————————— Please copy [" MasterQianLibString "] into your program folder ——————————")

namespace MasterQian::System {
	/// <summary>
	/// 日志类型
	/// </summary>
	enum class LogType : mqenum {
		// 控制台日志
		STD_CONSOLE,
		// 调试窗口日志
		DEBUG_CONSOLE,
		// 文件日志
		FILE,
	};

	// 日志标签
	enum class LogTag : mqenum {
		// 信息
		INFO,
		// 警告
		WARNING,
		// 错误
		ERR
	};

	namespace details {
		META_IMPORT_API(mqhandle, CreateLogger, LogType, mqcmem);
		META_IMPORT_API(void, CloseLogger, LogType, mqhandle);
		META_IMPORT_API(void, LoggerLog, LogType, LogTag, mqhandle, mqcstr, mqui32);
		META_MODULE_BEGIN
			META_PROC_API(CreateLogger);
		META_PROC_API(CloseLogger);
		META_PROC_API(LoggerLog);
		META_MODULE_END

		template<typename T>
		concept can_to_wstring = requires (std::remove_cvref_t<T> t) {
			{ std::to_wstring(t) } -> std::same_as<std::wstring>;
		} && !requires(std::remove_cvref_t<T> t) {
			std::wstring(t);
		};

		template<typename T>
		concept log_userstruct = requires (std::wstring & buf, std::remove_cvref_t<T> const t) {
			{ t.Log(buf) } noexcept -> std::same_as<void>;
		};

		template<can_to_wstring T>
		inline void LoggerLogValue(std::wstring& buf, T&& t) {
			buf += std::to_wstring(std::forward<T>(t));
		}

		inline void LoggerLogValue(std::wstring& buf, std::wstring_view sv) {
			buf += sv;
		}

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
		/// </code>
		/// </example>
		/// </summary>
		inline void LoggerLogValue(std::wstring& buf, log_userstruct auto const& t) {
			t.Log(buf);
		}

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
				handle = MasterQian_System_Log_CreateLogger(type, arg);
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
				handle = MasterQian_System_Log_CreateLogger(type, arg);
			}

			/// <summary>
			/// 关闭
			/// </summary>
			void close() noexcept {
				if (handle) {
					MasterQian_System_Log_CloseLogger(type, handle);
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
					(LoggerLogValue(buf, std::forward<Args>(args)), ...);
					if (newLine) {
						buf += (type == LogType::FILE ? L"\r\n" : L"\n");
					}
					MasterQian_System_Log_LoggerLog(type, tag, handle, buf.data(), static_cast<mqui32>(buf.size()));
				}
			}

			/// <summary>
			/// 输出信息
			/// </summary>
			template<bool newLine = true, typename... Args>
			void i(Args&&... args) const noexcept {
				log<LogTag::INFO, newLine>(std::forward<Args>(args)...);
			}

			/// <summary>
			/// 输出警告
			/// </summary>
			template<bool newLine = true, typename... Args>
			void w(Args&&... args) const noexcept {
				log<LogTag::WARNING, newLine>(std::forward<Args>(args)...);
			}

			/// <summary>
			/// 输出错误
			/// </summary>
			template<bool newLine = true, typename... Args>
			void e(Args&&... args) const noexcept {
				log<LogTag::ERR, newLine>(std::forward<Args>(args)...);
			}
		};
	}
#undef MasterQianModuleName
#undef MasterQianModuleNameString
#undef MasterQianLibString
#undef MasterQianModuleVersion

	/// <summary>
	/// 控制台日志，将在stdout输出
	/// </summary>
	using ConsoleLogger = details::Logger<LogType::STD_CONSOLE>;

	/// <summary>
	/// 调试窗口日志，将在debug窗口输出
	/// </summary>
	using DebugLogger = details::Logger<LogType::DEBUG_CONSOLE>;

	/// <summary>
	/// 文件日志，指定文件输出
	/// </summary>
	using FileLogger = details::Logger<LogType::FILE>;
}
#pragma once

#include "MasterQian.Meta.h"

#include <string>
#include <concepts>

#define MasterQianModuleVersion 20240112ULL
#undef MasterQianModuleVersion

namespace MasterQian {
	namespace details {
		struct mqsystemtime {
			mqui16 year{ };
			mqui16 month{ };
			mqui16 week{ };
			mqui16 day{ };
			mqui16 hour{ };
			mqui16 minute{ };
			mqui16 second{ };
			mqui16 millisecond{ };
		};

		constexpr mqui64 UnixDST{ 116444736000000000ULL };
		constexpr mqui64 BASE10POW[]{ 1ULL, 10ULL, 100ULL, 1000ULL, 10000ULL, 100000ULL, 1000000ULL, 10000000ULL };

		META_WINAPI(void, GetSystemTimeAsFileTime, mqui64*);
		META_WINAPI(mqbool, FileTimeToSystemTime, mqui64 const*, mqsystemtime*);
		META_WINAPI(mqbool, SystemTimeToFileTime, mqsystemtime const*, mqui64*);
		
#pragma comment(linker,"/alternatename:__imp_?GetSystemTimeAsFileTime@details@MasterQian@@YAXPEA_K@Z=__imp_GetSystemTimeAsFileTime")
#pragma comment(linker,"/alternatename:__imp_?FileTimeToSystemTime@details@MasterQian@@YAHPEB_KPEAUmqsystemtime@12@@Z=__imp_FileTimeToSystemTime")
#pragma comment(linker,"/alternatename:__imp_?SystemTimeToFileTime@details@MasterQian@@YAHPEBUmqsystemtime@12@PEA_K@Z=__imp_SystemTimeToFileTime")
	}

	/// <summary>
	/// ����ʱ��
	/// </summary>
	struct Time {
		mqui16 year;
		mqui8 month;
		mqui8 day;
		mqui8 hour;
		mqui8 minute;
		mqui8 second;
		mqui8 week;
		mqui16 millisecond;
		mqui16 microsecond;

		constexpr Time(mqui16 YY, mqui8 MM, mqui8 DD, mqui8 hh, mqui8 mm, mqui8 ss,
			mqui16 milli = 0U, mqui16 micro = 0U, mqui8 week = 0U) noexcept : year{ YY }, month{ MM }, day{ DD },
			hour{ hh }, minute{ mm }, second{ ss }, week{ week }, millisecond{ milli }, microsecond{ micro } {}

		/// <summary>���Ƚ����ڲ��Ƚ�ʱ��</summary>
		[[nodiscard]] constexpr bool compare_date(Time const& t) const noexcept {
			return year == t.year && month == t.month && day == t.day;
		}

		/// <summary>
		/// ��ʽ������2024-01-23 22:38:49.0123 2
		/// </summary>
		[[nodiscard]] std::wstring format() const noexcept {
			mqchar str[32ULL];
			MasterQian::Meta::details::wsprintfW(str, L"%04hu-%02u-%02u %02u:%02u:%02u.%03hu.%04hu %u",
				year, month, day, hour, minute, second, millisecond, microsecond, week);
			return str;
		}

		/// <summary>
		/// ��ʽ������20240123
		/// </summary>
		[[nodiscard]] std::wstring formatDate() const noexcept {
			mqchar str[16ULL];
			MasterQian::Meta::details::wsprintfW(str, L"%04hu%02u%02u", year, month, day);
			return str;
		}

		/// <summary>
		/// ��ʽ������223849
		/// </summary>
		[[nodiscard]] std::wstring formatTime() const noexcept {
			mqchar str[8ULL];
			MasterQian::Meta::details::wsprintfW(str, L"%02u%02u%02u", hour, minute, second);
			return str;
		}

		/// <summary>
		/// ��ʽ������20240123223849
		/// </summary>
		[[nodiscard]] std::wstring formatDateTime() const noexcept {
			mqchar str[32ULL];
			MasterQian::Meta::details::wsprintfW(str, L"%04hu%02u%02u%02u%02u%02u", year, month, day, hour, minute, second);
			return str;
		}
	};

	/// <summary>
	/// ʱ���
	/// </summary>
	struct Timestamp {
	private:
		mqui64 value{ };
	public:
		/// <summary>
		/// ʱ�䵥λ
		/// </summary>
		enum Type : mqui64 {
			// ��
			second = 10ULL,
			// ����
			millisecond = 13ULL,
			// ΢��
			microsecond = 17ULL,
		};

		// ʱ��
		enum Zone : mqui64 {
			// ����ʱ��
			CN_BEIJING = 288000000000ULL,
		};

		/// <summary>
		/// �Ե�ǰϵͳʱ�乹��
		/// </summary>
		Timestamp() noexcept {
			details::GetSystemTimeAsFileTime(&value);
			value -= details::UnixDST;
		}

		/// <summary>
		/// ��ʱ�������
		/// </summary>
		/// <param name="v">ʱ���</param>
		/// <param name="type">ʱ�䵥λ��Ĭ��Ϊ΢�룬��17λʱ���</param>
		constexpr Timestamp(mqui64 v, Type type = Type::microsecond) noexcept : value{ v } {
			value *= details::BASE10POW[static_cast<mqui64>(Type::microsecond) - static_cast<mqui64>(type)];
		}

#ifdef _CHRONO_
		/// <summary>
		/// ��time_point����
		/// </summary>
		/// <param name="t">time_point</param>
		/// <param name="isFileTime">�Ƿ����ļ�ʱ��</param>
		template <typename Clock>
		Timestamp(::std::chrono::time_point<Clock> t, bool isFileTime = false) noexcept {
			value = t.time_since_epoch().count();
			if (isFileTime) {
				value -= details::UnixDST;
			}
		}

		/// <summary>
		/// תtime_point
		/// </summary>
		/// <param name="isFileTime">�Ƿ����ļ�ʱ��</param>
		template<typename Clock>
		[[nodiscard]] ::std::chrono::time_point<Clock> tp(bool isFileTime = false) const noexcept {
			return ::std::chrono::time_point<Clock>(::std::chrono::time_point<Clock>::duration(
				isFileTime ? value + details::UnixDST : value));
		}
#endif

		/// <summary>
		/// ������ʱ�乹��
		/// </summary>
		/// <param name="t">����ʱ��</param>
		/// <param name="zone">ʱ����Ĭ��Ϊ����</param>
		Timestamp(Time const& t, Zone zone = Zone::CN_BEIJING) noexcept {
			details::mqsystemtime st{
				.year = t.year,
				.month = t.month,
				.week = t.week,
				.day = t.day,
				.hour = t.hour,
				.minute = t.minute,
				.second = t.second,
				.millisecond = t.millisecond
			};
			details::SystemTimeToFileTime(&st, &value);
			value -= details::UnixDST + zone - t.microsecond;
		}

		/// <summary>
		/// ȡʱ�����ֵ
		/// </summary>
		template<Type type = Type::microsecond>
		[[nodiscard]] constexpr mqui64 stamp() const noexcept {
			return value / details::BASE10POW[static_cast<mqui64>(Type::microsecond) - static_cast<mqui64>(type)];
		}

		[[nodiscard]] constexpr operator mqui64 () const noexcept {
			return value;
		}

		/// <summary>
		/// ȡ����ʱ��
		/// </summary>
		/// <param name="zone">ʱ��</param>
		/// <returns>����ʱ�����Ӧʱ����ʱ��</returns>
		Time local(Zone zone = Zone::CN_BEIJING) const noexcept {
			mqui64 zone_value = value + details::UnixDST + zone;
			details::mqsystemtime st;
			details::FileTimeToSystemTime(&zone_value, &st);
			return Time{ st.year, static_cast<mqui8>(st.month), static_cast<mqui8>(st.day),
				static_cast<mqui8>(st.hour), static_cast<mqui8>(st.minute), static_cast<mqui8>(st.second),
				st.millisecond, static_cast<mqui16>(zone_value % 10000ULL), static_cast<mqui8>(st.week)
			};
		}

		/// <summary>
		/// �����
		/// </summary>
		/// <param name="count">����</param>
		constexpr void add_day(mqi64 count) noexcept {
			value += count * 24ULL * 3600ULL * 1000ULL * 10000ULL;
		}

		constexpr Timestamp& operator -= (std::integral auto v) noexcept {
			value -= static_cast<mqui64>(v);
			return *this;
		}
		
		constexpr Timestamp operator - (std::integral auto v) const noexcept {
			Timestamp ts{ *this };
			ts -= v;
			return ts;
		}

		constexpr mqi64 operator - (Timestamp ts) const noexcept {
			return static_cast<mqi64>(value - ts.value);
		}
	};

	inline constexpr Timestamp operator + (std::integral auto v, Timestamp ts) noexcept {
		return { static_cast<mqui64>(v) + ts.stamp() };
	}
}
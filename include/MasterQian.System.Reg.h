#pragma once

#include "MasterQian.Meta.h"
#include "MasterQian.Bin.h"

#define MasterQianModuleName(_) MasterQian_System_Reg_
#define MasterQianModuleNameString "MasterQian_System_Reg_"
#ifdef _DEBUG
#define MasterQianLibString "MasterQian.System.Reg.Debug.dll"
#else
#define MasterQianLibString "MasterQian.System.Reg.dll"
#endif
#define MasterQianModuleVersion 20231229ULL
#pragma message("—————————— Please copy [" MasterQianLibString "] into your program folder ——————————")

namespace MasterQian::System {
	/// <summary>
	/// 键类型
	/// </summary>
	enum class KeyType : mqenum {
		// 空
		None = 0UL,
		// 字符串
		String = 1UL,
		_ES = 2UL,
		// 二进制
		Binary = 3UL,
		// 数字
		Number = 4UL,
		_BN = 5UL, _Link = 6UL,
		// 多行字符串
		MultiLineString = 7UL,
		_RL = 8UL, _FRD = 9UL, _RRL = 10UL, _N64 = 11UL
	};

	/// <summary>
	/// 根键
	/// </summary>
	enum class KeyBase : mqui64 {
		ClassesRoot = 0x80000000ULL,
		CurrentUser = 0x80000001ULL,
		LocalMachine = 0x80000002ULL,
		Users = 0x80000003ULL,
		CurrentConfig = 0x80000005ULL
	};

	namespace details {
		META_IMPORT_API(mqbool, Open, mqhandle*, mqbool, KeyBase, mqcstr);
		META_IMPORT_API(mqbool, Close, mqhandle*);
		META_IMPORT_API(mqbool, CreateItem, mqhandle, mqbool, mqcstr, mqhandle*);
		META_IMPORT_API(mqbool, DeleteItem, mqhandle, mqbool, mqcstr);
		META_IMPORT_API(mqbool, EnumItem, mqhandle, mqdword, mqstr);
		META_IMPORT_API(mqbool, HasKey, mqhandle, mqcstr);
		META_IMPORT_API(mqbool, GetValueTypeAndSize, mqhandle, mqcstr, KeyType*, mqdword*);
		META_IMPORT_API(mqbool, GetValue, mqhandle, mqcstr, mqmem, mqdword*);
		META_IMPORT_API(mqbool, SetValue, mqhandle, mqcstr, KeyType, mqcmem, mqdword);
		META_IMPORT_API(mqbool, DeleteKey, mqhandle, mqcstr);
		META_IMPORT_API(mqbool, GetKeyValueMaxSize, mqhandle, mqdword*, mqdword*);
		META_IMPORT_API(mqdword, EnumKey, mqhandle, mqdword, mqdword, mqstr, KeyType*);
		META_IMPORT_API(void, FlushEnvironment);
		META_MODULE_BEGIN
			META_PROC_API(Open);
			META_PROC_API(Close);
			META_PROC_API(CreateItem);
			META_PROC_API(DeleteItem);
			META_PROC_API(EnumItem);
			META_PROC_API(HasKey);
			META_PROC_API(GetValueTypeAndSize);
			META_PROC_API(GetValue);
			META_PROC_API(SetValue);
			META_PROC_API(DeleteKey);
			META_PROC_API(GetKeyValueMaxSize);
			META_PROC_API(EnumKey);
			META_PROC_API(FlushEnvironment);
		META_MODULE_END
	}
#undef MasterQianModuleName
#undef MasterQianModuleNameString
#undef MasterQianLibString
#undef MasterQianModuleVersion

	/// <summary>
	/// 注册表操作对象
	/// </summary>
	struct Reg {
	private:
		mqhandle mqhandle;
		mqbool bWOW64;
	public:
		Reg() : mqhandle{ }, bWOW64{ } {}

		/// <summary>
		/// 打开注册表
		/// </summary>
		/// <param name="base">根键</param>
		/// <param name="route">路径</param>
		/// <param name="wow64">是否是64位映像</param>
		Reg(KeyBase base, std::wstring_view route, mqbool wow64 = true) noexcept : mqhandle{ }, bWOW64 { wow64 } {
			Open(base, route);
		}

		~Reg() noexcept {
			Close();
		}

		Reg(Reg const&) = delete;
		Reg(Reg&&) = delete;
		Reg& operator = (Reg const&) = delete;
		Reg& operator = (Reg&&) = delete;

		/// <summary>
		/// 置WOW64
		/// </summary>
		/// <param name="wow64">是否是64位映像</param>
		void SetWOW64(mqbool wow64) noexcept {
			bWOW64 = wow64;
		}

		/// <summary>
		/// 是否是WOW64
		/// </summary>
		[[nodiscard]] mqbool IsWOW64() const noexcept {
			return bWOW64;
		}

		/// <summary>
		/// 是否打开
		/// </summary>
		[[nodiscard]] mqbool IsOpen() const noexcept {
			return mqhandle != nullptr;
		}

		/// <summary>
		/// 打开
		/// </summary>
		/// <param name="base">根键</param>
		/// <param name="route">路径</param>
		/// <returns>是否成功</returns>
		mqbool Open(KeyBase base, std::wstring_view route) noexcept {
			if (mqhandle) Close();
			return details::MasterQian_System_Reg_Open(&mqhandle, bWOW64, base, route.data());
		}

		/// <summary>
		/// 关闭
		/// </summary>
		/// <returns>是否成功</returns>
		mqbool Close() noexcept {
			return details::MasterQian_System_Reg_Close(&mqhandle);
		}

		/// <summary>
		/// 创建项
		/// </summary>
		/// <param name="route">路径</param>
		/// <param name="ret">返回的项，若为空表示忽略</param>
		/// <returns>是否成功</returns>
		mqbool CreateItem(std::wstring_view route, Reg* ret = nullptr) const noexcept {
			return mqhandle ? details::MasterQian_System_Reg_CreateItem(mqhandle, bWOW64, route.data(), ret ? &ret->mqhandle : nullptr) : false;
		}

		/// <summary>
		/// 删除项
		/// </summary>
		/// <param name="route">路径</param>
		/// <returns>是否成功</returns>
		mqbool DeleteItem(std::wstring_view route) const noexcept {
			return mqhandle ? details::MasterQian_System_Reg_DeleteItem(mqhandle, bWOW64, route.data()) : false;
		}

		/// <summary>
		/// 枚举项
		/// </summary>
		/// <returns>路径下的所有项集合</returns>
		[[nodiscard]] std::vector<std::wstring> EnumItem() const noexcept {
			std::vector<std::wstring> container;
			if (mqhandle) {
				mqchar buf[260]{ };
				for (mqdword index{ }; details::MasterQian_System_Reg_EnumItem(mqhandle, index, buf); ++index) {
					container.emplace_back(buf);
				}
			}
			return container;
		}

		/// <summary>
		/// 是否有键
		/// </summary>
		/// <param name="key">键名</param>
		[[nodiscard]] mqbool HasKey(std::wstring_view key) const noexcept {
			return mqhandle ? details::MasterQian_System_Reg_HasKey(mqhandle, key.data()) : false;
		}
		
		/// <summary>
		/// 取键类型
		/// </summary>
		/// <param name="key">键名</param>
		/// <returns>键类型</returns>
		[[nodiscard]] KeyType GetKeyType(std::wstring_view key) const noexcept {
			KeyType type{ };
			if (mqhandle) {
				mqdword size;
				details::MasterQian_System_Reg_GetValueTypeAndSize(mqhandle, key.data(), &type, &size);
			}
			return type;
		}

		/// <summary>
		/// 取二进制值
		/// </summary>
		/// <param name="key">键名</param>
		[[nodiscard]] Bin GetBinaryValue(std::wstring_view key) const noexcept {
			Bin bin;
			if (mqhandle) {
				KeyType type{ };
				mqdword size;
				if (details::MasterQian_System_Reg_GetValueTypeAndSize(mqhandle, key.data(), &type, &size)) {
					if (type == KeyType::Binary) {
						bin.resize(static_cast<mqui64>(size));
						details::MasterQian_System_Reg_GetValue(mqhandle, key.data(), bin.data(), &size);
					}
				}
			}
			return bin;
		}

		/// <summary>
		/// 取数字值
		/// </summary>
		/// <param name="key">键名</param>
		[[nodiscard]] mqui64 GetNumberValue(std::wstring_view key) const noexcept {
			mqui64 value{ };
			if (mqhandle) {
				KeyType type{ };
				mqdword size;
				if (details::MasterQian_System_Reg_GetValueTypeAndSize(mqhandle, key.data(), &type, &size)) {
					if (type == KeyType::Number || type == KeyType::_N64) {
						details::MasterQian_System_Reg_GetValue(mqhandle, key.data(), &value, &size);
					}
				}
			}
			return value;
		}

		/// <summary>
		/// 取字符串值
		/// </summary>
		/// <param name="key">键名</param>
		[[nodiscard]] std::wstring GetStringValue(std::wstring_view key) const noexcept {
			std::wstring str;
			if (mqhandle) {
				KeyType type{ };
				mqdword size;
				if (details::MasterQian_System_Reg_GetValueTypeAndSize(mqhandle, key.data(), &type, &size)) {
					switch (type) {
					case KeyType::String:
					case KeyType::_ES:
					case KeyType::MultiLineString: {
						if (size < 2) {
							size = 2;
						}
						str.resize(static_cast<mqui64>((size - 2) >> 1), L'\0');
						details::MasterQian_System_Reg_GetValue(mqhandle, key.data(), str.data(), &size);
						if (type == KeyType::MultiLineString) {
							for (auto& ch : str) {
								if (ch == L'\0') ch = L'\n';
							}
						}
						break;
					}
					case KeyType::Number:
					case KeyType::_N64: {
						mqui64 value{ };
						details::MasterQian_System_Reg_GetValue(mqhandle, key.data(), &value, &size);
						str = std::to_wstring(value);
						break;
					}
					}
				}
			}
			return str;
		}

		/// <summary>
		/// 置二进制值
		/// </summary>
		/// <param name="key">键名</param>
		/// <param name="value">键值</param>
		/// <returns>是否成功</returns>
		mqbool SetBinaryValue(std::wstring_view key, BinView value) const noexcept {
			return mqhandle ? details::MasterQian_System_Reg_SetValue(mqhandle, key.data(), KeyType::Binary, value.data(), value.size32()) : false;
		}

		/// <summary>
		/// 置数字值
		/// </summary>
		/// <param name="key">键名</param>
		/// <param name="value">键值</param>
		/// <returns>是否成功</returns>
		mqbool SetNumberValue(std::wstring_view key, mqui64 value) const noexcept {
			if (mqhandle) {
				if (value > 0xFFFFFFFFULL) {
					return details::MasterQian_System_Reg_SetValue(mqhandle, key.data(), KeyType::_N64, &value, sizeof(mqui64));
				}
				mqdword value32{ static_cast<mqdword>(value) };
				return details::MasterQian_System_Reg_SetValue(mqhandle, key.data(), KeyType::Number, &value32, sizeof(mqdword));
			}
			return false;
		}

		/// <summary>
		/// 置字符串值
		/// </summary>
		/// <param name="key">键名</param>
		/// <param name="value">键值</param>
		/// <param name="multiline">是否多行</param>
		/// <returns>是否成功</returns>
		mqbool SetStringValue(std::wstring_view key, std::wstring_view value, mqbool multiline = false) const noexcept {
			if (mqhandle) {
				if (multiline) {
					std::wstring tmp(value);
					for (auto& ch : tmp) {
						if (ch == L'\n') ch = L'\0';
					}
					return details::MasterQian_System_Reg_SetValue(mqhandle, key.data(), KeyType::MultiLineString, tmp.data(), static_cast<mqdword>(tmp.size() << 1));
				}
				return details::MasterQian_System_Reg_SetValue(mqhandle, key.data(), KeyType::String, value.data(), static_cast<mqdword>(value.size() << 1));
			}
			return false;
		}

		/// <summary>
		/// 删除键
		/// </summary>
		/// <param name="key">键名</param>
		/// <returns>是否成功</returns>
		mqbool DeleteKey(std::wstring_view key) const noexcept {
			return mqhandle ? details::MasterQian_System_Reg_DeleteKey(mqhandle, key.data()) : false;
		}

		/// <summary>
		/// 枚举键值对
		/// </summary>
		/// <returns>项下所有键值对的集合</returns>
		[[nodiscard]] std::vector<std::pair<std::wstring, KeyType>> EnumKey() const noexcept {
			std::vector<std::pair<std::wstring, KeyType>> container;
			if (mqhandle) {
				mqdword maxKeySize{ };
				if (details::MasterQian_System_Reg_GetKeyValueMaxSize(mqhandle, &maxKeySize, nullptr)) {
					std::wstring buf(maxKeySize, L'\0');
					KeyType type{ };
					for (mqdword keySize{ }, index{ }; (keySize = details::MasterQian_System_Reg_EnumKey(
						mqhandle, index, maxKeySize + 1, buf.data(), &type)) != 0; ++index) {
						container.emplace_back(buf.data(), type);
					}
				}
			}
			return container;
		}
	};

	/// <summary>
	/// 刷新环境变量，来使得所有应用程序收到修改后注册表的更新
	/// </summary>
	inline void FlushEnvironment() noexcept {
		details::MasterQian_System_Reg_FlushEnvironment();
	}
}
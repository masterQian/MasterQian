#pragma once

#include "MasterQian.Meta.h"

#include <string>
#include <vector>
#include <unordered_map>

#define MasterQianModuleName(_) MasterQian_DB_Sqlite_
#define MasterQianModuleNameString "MasterQian_DB_Sqlite_"
#ifdef _DEBUG
#define MasterQianLibString "MasterQian.DB.Sqlite.Debug.dll"
#else
#define MasterQianLibString "MasterQian.DB.Sqlite.dll"
#endif
#define MasterQianModuleVersion 20231229ULL
#pragma message("—————————— Please copy [" MasterQianLibString "] into your program folder ——————————")

namespace MasterQian::DB::Sqlite {
	/// <summary>
	/// Sqlite操作结果
	/// </summary>
	enum class Result : mqi32 {
		OK, ERROR, INTERNAL, PERM, ABORT, BUSY, LOCKED, MOMEM, READONLY, INTERRUPT,
		IOERR, CORRUPT, NOTFOUND, FULL, CANTOPEN, PROTOCOL, EMPTY, SCHEMA, TOOBIG,
		CONSTRAINT, MISMATCH, MISUSE, NOLFS, AUTH, FORMAT, RANGE, NOTADB,
		NOTICE, WARNING,
		ROW = 100, DONE,
	};

	namespace details {
		META_IMPORT_API(Result, Open, mqhandle*, mqcstr);
		META_IMPORT_API(Result, Close, mqhandle*);
		META_IMPORT_API(Result, LastResult, mqhandle);
		META_IMPORT_API(mqcstr, LastResultString, mqhandle);
		META_IMPORT_API(Result, Execute, mqhandle, mqcstr);
		META_IMPORT_API(mqhandle, QueryPrepare, mqhandle, mqcstr);
		META_IMPORT_API(Result, QueryFinalize, mqhandle);
		META_IMPORT_API(mqui32, QueryColumnCount, mqhandle);
		META_IMPORT_API(mqcstr, QueryColumnName, mqhandle, mqui32);
		META_IMPORT_API(mqbool, QueryHasRow, mqhandle);
		META_IMPORT_API(mqcstr, QueryRow, mqhandle, mqui32);
		META_MODULE_BEGIN
			META_PROC_API(Open);
			META_PROC_API(Close);
			META_PROC_API(LastResult);
			META_PROC_API(LastResultString);
			META_PROC_API(Execute);
			META_PROC_API(QueryPrepare);
			META_PROC_API(QueryFinalize);
			META_PROC_API(QueryColumnCount);
			META_PROC_API(QueryColumnName);
			META_PROC_API(QueryHasRow);
			META_PROC_API(QueryRow);
		META_MODULE_END

		// ugly C++ , I need to wait until C++26 to use heterogeneous search for new features?
		struct _ColDefHash {
			template<typename T>
			[[nodiscard]] constexpr auto operator () (T const& t) const noexcept {
				return std::hash<T>{}(t);
			}
			using is_transparent = void;
		};

		struct _ColDefEqual {
			template<typename T1, typename T2>
			[[nodiscard]] constexpr auto operator () (T1 const& t1, T2 const& t2) const noexcept {
				return t1 == t2;
			}
			using is_transparent = void;
		};
	}
#undef MasterQianModuleName
#undef MasterQianModuleNameString
#undef MasterQianLibString
#undef MasterQianModuleVersion

	using ColDef = std::unordered_map<std::wstring, mqui32, details::_ColDefHash, details::_ColDefEqual>;

	/// <summary>
	/// 表的一行
	/// </summary>
	struct Row : protected std::vector<std::wstring> {
		using BaseT = std::vector<std::wstring>;
	protected:
		ColDef& colName;

		inline static std::wstring _EMPTYSTRING{ };
	public:
		Row(mqui64 count, ColDef& colDef) : BaseT{ count }, colName{ colDef } {}

		using BaseT::begin;
		using BaseT::end;
		using BaseT::operator[];

		[[nodiscard]] std::wstring const& operator [] (std::wstring_view name) const noexcept {
			if (auto iter{ colName.find(name) }; iter != colName.cend()) {
				return BaseT::operator[](iter->second);
			}
			return _EMPTYSTRING;
		}

		[[nodiscard]] std::wstring& operator [] (std::wstring_view name) noexcept {
			if (auto iter{ colName.find(name) }; iter != colName.cend()) {
				return BaseT::operator[](iter->second);
			}
			return _EMPTYSTRING;
		}
	};

	/// <summary>
	/// 数据库表
	/// </summary>
	struct Table : protected std::vector<Row> {
		using BaseT = std::vector<Row>;
		ColDef colName;

		Table(mqhandle handle) : BaseT{} {
			if (handle) {
				auto count{ details::MasterQian_DB_Sqlite_QueryColumnCount(handle) };
				for (mqui32 i{ }; i < count; ++i) {
					colName.emplace(details::MasterQian_DB_Sqlite_QueryColumnName(handle, i), i);
				}
				while (details::MasterQian_DB_Sqlite_QueryHasRow(handle)) {
					Row& rv{ BaseT::emplace_back(count, colName) };
					for (mqui32 j{ }; j < count; ++j) {
						rv[j] = details::MasterQian_DB_Sqlite_QueryRow(handle, j);
					}
				}
				details::MasterQian_DB_Sqlite_QueryFinalize(handle);
			}
		}
		
		using BaseT::empty;
		using BaseT::size;
		using BaseT::begin;
		using BaseT::end;
		using BaseT::operator[];
	};

	/// <summary>
	/// Sqlite3数据库对象
	/// </summary>
	struct Sqlite3 {
	private:
		mqhandle handle{ };
	public:
		Sqlite3() noexcept = default;

		Sqlite3(std::wstring_view fn) noexcept {
			details::MasterQian_DB_Sqlite_Open(&handle, fn.data());
		}

		~Sqlite3() noexcept {
			details::MasterQian_DB_Sqlite_Close(&handle);
		}

		Sqlite3(Sqlite3 const&) = delete;
		Sqlite3& operator = (Sqlite3 const&) = delete;

		/// <summary>
		/// 打开数据库
		/// </summary>
		/// <param name="fn">Sqlite3数据库文件名</param>
		/// <returns>操作结果</returns>
		Result Open(std::wstring_view fn) noexcept {
			if (handle) {
				details::MasterQian_DB_Sqlite_Close(&handle);
			}
			return details::MasterQian_DB_Sqlite_Open(&handle, fn.data());
		}

		/// <summary>
		/// 关闭数据库
		/// </summary>
		/// <returns>操作结果</returns>
		Result Close() noexcept {
			return details::MasterQian_DB_Sqlite_Close(&handle);
		}

		/// <summary>
		/// 取最后结果
		/// </summary>
		/// <returns>操作结果</returns>
		[[nodiscard]] Result LastResult() const noexcept {
			return details::MasterQian_DB_Sqlite_LastResult(handle);
		}

		/// <summary>
		/// 取最后结果文本
		/// </summary>
		/// <returns>操作结果文本</returns>
		[[nodiscard]] std::wstring LastResultString() const noexcept {
			return details::MasterQian_DB_Sqlite_LastResultString(handle);
		}

		/// <summary>
		/// 取所有表名
		/// </summary>
		/// <returns>数据库内所有表名</returns>
		[[nodiscard]] std::vector<std::wstring> TablesName() const noexcept {
			std::vector<std::wstring> container;
			for (auto& row : Query(L"select name from sqlite_master where type = 'table'")) {
				container.emplace_back(row[0ULL]);
			}
			return container;
		}

		/// <summary>
		/// 执行SQL
		/// </summary>
		/// <param name="sql">sql语句</param>
		/// <returns>操作结果</returns>
		Result Execute(std::wstring_view sql) const noexcept {
			return details::MasterQian_DB_Sqlite_Execute(handle, sql.data());
		}

		/// <summary>
		/// 查询SQL
		/// </summary>
		/// <param name="sql">sql语句</param>
		/// <returns>查询表</returns>
		[[nodiscard]] Table Query(std::wstring_view sql) const noexcept {
			return Table{ details::MasterQian_DB_Sqlite_QueryPrepare(handle, sql.data()) };
		}
	};
}
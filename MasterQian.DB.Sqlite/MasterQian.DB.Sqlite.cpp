#include "../include/MasterQian.Meta.h"
#include "sqlite3.h"
import MasterQian.freestanding;
using namespace MasterQian;
#define MasterQianModuleName(name) MasterQian_DB_Sqlite_##name
META_EXPORT_API_VERSION(20240131ULL)

enum class Result : mqi32 {
	OK, ERROR, INTERNAL, PERM, ABORT, BUSY, LOCKED, MOMEM, READONLY, INTERRUPT,
	IOERR, CORRUPT, NOTFOUND, FULL, CANTOPEN, PROTOCOL, EMPTY, SCHEMA, TOOBIG,
	CONSTRAINT, MISMATCH, MISUSE, NOLFS, AUTH, FORMAT, RANGE, NOTADB,
	NOTICE, WARNING,
	ROW = 100, DONE,
};

inline constexpr Result rs_cast(mqi32 v) noexcept {
	return static_cast<Result>(v);
}

META_EXPORT_API(Result, Open, sqlite3** handle, mqcstr fn) {
	return rs_cast(sqlite3_open16(fn, handle));
}

META_EXPORT_API(Result, Close, sqlite3** handle) {
	auto result{ rs_cast(sqlite3_close(*handle)) };
	*handle = nullptr;
	return result;
}

META_EXPORT_API(Result, LastResult, sqlite3* handle) {
	return rs_cast(sqlite3_errcode(handle));
}

META_EXPORT_API(mqcstr, LastResultString, sqlite3* handle) {
	return static_cast<mqcstr>(sqlite3_errmsg16(handle));
}

META_EXPORT_API(Result, Execute, sqlite3* handle, mqcstr sql) {
	// Why doesn't sqlite3 provide the char16 version of sqlite3_exec?
	auto len{ api::WideCharToMultiByte(65001U, 0, sql, -1, nullptr, 0, nullptr, nullptr) };
	mqstra buf = new mqchara[len]{ };
	api::WideCharToMultiByte(65001U, 0, sql, -1, buf, len, nullptr, nullptr);
	auto result{ rs_cast(sqlite3_exec(handle, buf, nullptr, nullptr, nullptr)) };
	delete[] buf;
	return result;
}

META_EXPORT_API(sqlite3_stmt*, QueryPrepare, sqlite3* handle, mqcstr sql) {
	sqlite3_stmt* stmt{ };
	sqlite3_prepare16_v2(handle, sql, -1, &stmt, nullptr);
	return stmt;
}

META_EXPORT_API(Result, QueryFinalize, sqlite3_stmt* stmt) {
	return rs_cast(sqlite3_finalize(stmt));
}

META_EXPORT_API(mqui32, QueryColumnCount, sqlite3_stmt* stmt) {
	return static_cast<mqui32>(sqlite3_column_count(stmt));
}

META_EXPORT_API(mqcstr, QueryColumnName, sqlite3_stmt* stmt, mqui32 col) {
	return static_cast<mqcstr>(sqlite3_column_name16(stmt, static_cast<mqi32>(col)));
}

META_EXPORT_API(mqbool, QueryHasRow, sqlite3_stmt* stmt) {
	return sqlite3_step(stmt) == SQLITE_ROW;
}

META_EXPORT_API(mqcstr, QueryRow, sqlite3_stmt* stmt, mqui32 col) {
	auto str{ static_cast<mqcstr>(sqlite3_column_text16(stmt, static_cast<mqi32>(col))) };
	return str ? str : L"";
}
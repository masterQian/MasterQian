#include "../include/MasterQian.Meta.h"

#define MasterQianModuleName(_) MasterQian_Storage_Zip_
META_EXPORT_API_VERSION(20240110ULL)

#define _AMD64_

#include <windef.h>
#include <fileapi.h>
#include <handleapi.h>
#include <memoryapi.h>
#include <stringapiset.h>

#include <vector>

#include "zip.h"
#include "unzip.h"

#ifdef _DEBUG
#pragma comment(lib, "zlibstatd.lib")
#else
#pragma comment(lib, "zlibstat.lib")
#endif

namespace details {
	enum class OpenMode : mqenum {
		CREATE, // 不存在创建, 存在失败
		CREATE_OPEN, // 不存在创建, 存在打开
		CREATE_OVERRIDE, // 不存在创建, 存在覆盖
		OPEN_EXIST, // 不存在失败, 存在打开
	};

	static mqstr ATW(mqcstra str_a, mqstr str_w) noexcept {
		auto len{ MultiByteToWideChar(CP_ACP, 0, str_a, -1, nullptr, 0) };
		if (len > 0 && len < MAX_PATH) {
			MultiByteToWideChar(CP_ACP, 0, str_a, -1, str_w, len);
		}
		else {
			*str_w = L'\0';
		}
		return str_w;
	}

	static mqstra WTA(mqcstr str_w, mqstra str_a) noexcept {
		auto len{ WideCharToMultiByte(CP_ACP, 0, str_w, -1, nullptr, 0, nullptr, nullptr) };
		if (len > 0 && len < MAX_PATH) {
			WideCharToMultiByte(CP_ACP, 0, str_w, -1, str_a, len, nullptr, nullptr);
		}
		else {
			*str_a = L'\0';
		}
		return str_a;
	}
}

META_EXPORT_API(mqui32, CompressBound, mqui32 src_size) {
	return compressBound(src_size) + sizeof(mqui32); // 用于存储压缩前大小
}

META_EXPORT_API(mqui32, Compress, mqcbytes src, mqui32 src_size, mqbytes des, mqui32 des_size, mqbool speedFirstly) {
	mqdword size{ des_size };
	*reinterpret_cast<mqui32*>(des) = src_size;
	compress2(des + sizeof(mqui32), &size, src, src_size,
		speedFirstly ? Z_BEST_SPEED : Z_BEST_COMPRESSION);
	return size + sizeof(mqui32);
}

META_EXPORT_API(mqui32, UncompressBound, mqcbytes src, mqui32 src_size) {
	if (src_size >= sizeof(mqui32)) {
		return *reinterpret_cast<mqui32 const*>(src);
	}
	return 0ULL;
}

META_EXPORT_API(mqbool, Uncompress, mqcbytes src, mqui32 src_size, mqbytes des, mqui32 des_size) {
	mqdword size{ des_size };
	return uncompress(des, &size, src + sizeof(mqui32), src_size - sizeof(mqui32)) == Z_OK;
}

META_EXPORT_API(zipFile, ZipStart, mqcstr fn, details::OpenMode mode) {
	DWORD attribute{ GetFileAttributesW(fn) };
	bool isExists{ attribute != INVALID_FILE_ATTRIBUTES && attribute != FILE_ATTRIBUTE_DIRECTORY };
	mqchara ansi_fn[MAX_PATH]{ };
	switch (mode) {
	case details::OpenMode::CREATE: {
		if (!isExists) {
			return zipOpen64(details::WTA(fn, ansi_fn), APPEND_STATUS_CREATE);
		}
		break;
	}
	case details::OpenMode::CREATE_OPEN: {
		if (isExists) {
			return zipOpen64(details::WTA(fn, ansi_fn), APPEND_STATUS_ADDINZIP);
		}
		else {
			return zipOpen64(details::WTA(fn, ansi_fn), APPEND_STATUS_CREATE);
		}
	}
	case details::OpenMode::CREATE_OVERRIDE: {
		return zipOpen64(details::WTA(fn, ansi_fn), APPEND_STATUS_CREATE);
	}
	case details::OpenMode::OPEN_EXIST: {
		return zipOpen64(details::WTA(fn, ansi_fn), APPEND_STATUS_ADDINZIP);
	}
	}
	return nullptr;
}

META_EXPORT_API(void, ZipEnd, zipFile handle) {
	zipClose(handle, nullptr);
}

META_EXPORT_API(mqbool, ZipIsFolder, mqcstr path) {
	auto attribute{ GetFileAttributesW(path) };
	return attribute != INVALID_FILE_ATTRIBUTES && (attribute & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

META_EXPORT_API(mqbool, ZipIsFile, mqcstr path) {
	return MasterQian_Storage_Zip_ZipIsFolder(path) == 0;
}

META_EXPORT_API(mqbool, ZipAddBin, zipFile handle, mqcstr branch, mqcbytes mem, mqui32 size) {
	mqchara ansi_name[MAX_PATH]{ };
	if (zipOpenNewFileInZip64(handle, details::WTA(branch, ansi_name), nullptr, nullptr, 0U,
		nullptr, 0U, nullptr, Z_DEFLATED, Z_BEST_COMPRESSION, 0) == ZIP_OK) {
		if (zipWriteInFileInZip(handle, mem, size) == ZIP_OK) {
			if (zipCloseFileInZip(handle) == ZIP_OK) {
				return true;
			}
		}
	}
	return false;
}

META_EXPORT_API(mqbool, ZipAddFile, zipFile handle, mqcstr branch, mqcstr fn) {
	bool result{ };
	if (auto hFile{ CreateFileW(fn, GENERIC_READ, FILE_SHARE_READ,
		nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
		nullptr) }; hFile != INVALID_HANDLE_VALUE) {
		if (auto hFileMap{ CreateFileMappingW(hFile, nullptr,
			PAGE_READONLY, 0, 0, nullptr) }) {
			if (auto data{ MapViewOfFile(hFileMap, FILE_MAP_READ,
				0, 0, 0) }) {
				LARGE_INTEGER size{ };
				GetFileSizeEx(hFile, &size);
				result = MasterQian_Storage_Zip_ZipAddBin(handle, branch, static_cast<mqcbytes>(data), size.LowPart);
				UnmapViewOfFile(data);
			}
			CloseHandle(hFileMap);
		}
		CloseHandle(hFile);
	}
	return result;
}

META_EXPORT_API(mqbool, ZipAddFolder, zipFile handle, mqcstr branch, mqcstr path) {
	mqchar buf[MAX_PATH]{ }, findPath[MAX_PATH]{ }, new_branch[MAX_PATH]{ };
	MasterQian::Meta::details::lstrcpyW(findPath, path);
	MasterQian::Meta::details::lstrcatW(findPath, L"\\*");
	WIN32_FIND_DATAW data;
	HANDLE hFileFind{ FindFirstFileW(findPath, &data) };
	bool result{ true };
	if (hFileFind != INVALID_HANDLE_VALUE) {
		bool hasNext{ true };
		while (hasNext) {
			if (MasterQian::Meta::details::lstrcmpW(data.cFileName, L".") != 0 &&
				MasterQian::Meta::details::lstrcmpW(data.cFileName, L"..") != 0) {
				MasterQian::Meta::details::lstrcpyW(buf, path);
				MasterQian::Meta::details::lstrcatW(buf, L"\\");
				MasterQian::Meta::details::lstrcatW(buf, data.cFileName);
				MasterQian::Meta::details::lstrcpyW(new_branch, branch);
				MasterQian::Meta::details::lstrcatW(new_branch, data.cFileName);
				if (MasterQian_Storage_Zip_ZipIsFolder(buf)) {
					MasterQian::Meta::details::lstrcatW(new_branch, L"\\");
					result = result && MasterQian_Storage_Zip_ZipAddFolder(handle, new_branch, buf);
				}
				else {
					result = result && MasterQian_Storage_Zip_ZipAddFile(handle, new_branch, buf);
				}
			}
			hasNext = FindNextFileW(hFileFind, &data);
		}
		FindClose(hFileFind);
	}
	return result;
}

using ZipCallBack = void(__stdcall*)(mqmem, bool, mqcstr, mqcstr);

META_EXPORT_API(mqbool, ZipAddFolderWithCallback, zipFile handle, mqcstr branch, mqcstr path, ZipCallBack func, mqmem arg) {
	mqchar buf[MAX_PATH]{ }, findPath[MAX_PATH]{ }, new_branch[MAX_PATH]{ };
	MasterQian::Meta::details::lstrcpyW(findPath, path);
	MasterQian::Meta::details::lstrcatW(findPath, L"\\*");
	WIN32_FIND_DATAW data;
	HANDLE hFileFind{ FindFirstFileW(findPath, &data) };
	bool result{ true };
	if (hFileFind != INVALID_HANDLE_VALUE) {
		bool hasNext{ true };
		while (hasNext) {
			if (MasterQian::Meta::details::lstrcmpW(data.cFileName, L".") != 0 && MasterQian::Meta::details::lstrcmpW(data.cFileName, L"..") != 0) {
				MasterQian::Meta::details::lstrcpyW(buf, path);
				MasterQian::Meta::details::lstrcatW(buf, L"\\");
				MasterQian::Meta::details::lstrcatW(buf, data.cFileName);
				MasterQian::Meta::details::lstrcpyW(new_branch, branch);
				MasterQian::Meta::details::lstrcatW(new_branch, data.cFileName);
				if (MasterQian_Storage_Zip_ZipIsFolder(buf)) {
					MasterQian::Meta::details::lstrcatW(new_branch, L"\\");
					result = result && MasterQian_Storage_Zip_ZipAddFolderWithCallback(handle, new_branch, buf, func, arg);
				}
				else {
					result = result && MasterQian_Storage_Zip_ZipAddFile(handle, new_branch, buf);
					if (func) func(arg, result, new_branch, buf);
				}
			}
			hasNext = FindNextFileW(hFileFind, &data);
		}
		FindClose(hFileFind);
	}
	return result;
}

META_EXPORT_API(unzFile, UnZipStart, mqcstr fn) {
	mqchara ansi_fn[MAX_PATH]{ };
	return unzOpen64(details::WTA(fn, ansi_fn));
}

META_EXPORT_API(void, UnZipEnd, unzFile handle) {
	unzClose(handle);
}

META_EXPORT_API(ZPOS64_T, UnZipCount, unzFile handle) {
	unz_global_info64 global_info{ };
	unzGetGlobalInfo64(handle, &global_info);
	return global_info.number_entry;
}

static void SaveMultiFolderFile(mqcstra file_path, mqbytes mem, mqui32 size) {
	mqchara tmp[MAX_PATH]{ }, * p = tmp;
	MasterQian::Meta::details::lstrcpyA(tmp, file_path);
	while (*p++) {
		if (*p == '\\') {
			*p = '\0';
			if (GetFileAttributesA(tmp) == INVALID_FILE_ATTRIBUTES) {
				CreateDirectoryA(tmp, nullptr);
			}
			*p = '\\';
		}
	}
	if (auto hFile{ CreateFileA(file_path, GENERIC_WRITE, 0,
		nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL,
		nullptr) }; hFile != INVALID_HANDLE_VALUE) {
		WriteFile(hFile, mem, size, nullptr, nullptr);
		CloseHandle(hFile);
	}
}

META_EXPORT_API(mqbool, UnZipSave, unzFile handle, mqcstr path) {
	mqchara ansi_path[MAX_PATH]{ }, file_path[MAX_PATH]{ };
	details::WTA(path, ansi_path);
	MasterQian::Meta::details::lstrcatA(ansi_path, "\\");

	if (unzGoToFirstFile(handle) != UNZ_OK) {
		return false;
	}
	unz_file_info64 file_info;
	mqchara name[MAX_PATH];
	std::vector<mqbyte> fileData;
	if (unzGetCurrentFileInfo64(handle, &file_info, name, MAX_PATH, nullptr, 0U, nullptr, 0U) != UNZ_OK) {
		return false;
	}
	if (unzOpenCurrentFile(handle) != UNZ_OK) {
		return false;
	}
	fileData.reserve(file_info.uncompressed_size);
	if (unzReadCurrentFile(handle, fileData.data(), static_cast<mqui32>(file_info.uncompressed_size)) <= 0) {
		unzCloseCurrentFile(handle);
		return false;
	}
	MasterQian::Meta::details::lstrcpyA(file_path, ansi_path);
	MasterQian::Meta::details::lstrcatA(file_path, name);
	SaveMultiFolderFile(file_path, fileData.data(), static_cast<mqui32>(file_info.uncompressed_size));
	while (unzGoToNextFile(handle) == UNZ_OK) {
		if (unzGetCurrentFileInfo64(handle, &file_info, name, MAX_PATH, nullptr, 0U, nullptr, 0U) != UNZ_OK) {
			return false;
		}
		if (unzOpenCurrentFile(handle) != UNZ_OK) {
			return false;
		}
		fileData.reserve(file_info.uncompressed_size);
		if (unzReadCurrentFile(handle, fileData.data(), static_cast<mqui32>(file_info.uncompressed_size)) <= 0) {
			unzCloseCurrentFile(handle);
			return false;
		}
		MasterQian::Meta::details::lstrcpyA(file_path, ansi_path);
		MasterQian::Meta::details::lstrcatA(file_path, name);
		SaveMultiFolderFile(file_path, fileData.data(), static_cast<mqui32>(file_info.uncompressed_size));
	}
	return true;
}

using UnZipCallBack = void(__stdcall*)(mqmem, mqcstr, mqcstr);

META_EXPORT_API(mqbool, UnZipSaveWithCallback, unzFile handle, mqcstr path, UnZipCallBack func, mqmem arg) {
	mqchara ansi_path[MAX_PATH]{ }, file_path[MAX_PATH]{ };
	mqchar unicode_name[MAX_PATH]{ }, unicode_file_path[MAX_PATH]{ };
	details::WTA(path, ansi_path);
	MasterQian::Meta::details::lstrcatA(ansi_path, "\\");

	if (unzGoToFirstFile(handle) != UNZ_OK) {
		return false;
	}
	unz_file_info64 file_info;
	mqchara name[MAX_PATH];
	std::vector<mqbyte> fileData;
	if (unzGetCurrentFileInfo64(handle, &file_info, name, MAX_PATH, nullptr, 0U, nullptr, 0U) != UNZ_OK) {
		return false;
	}
	if (unzOpenCurrentFile(handle) != UNZ_OK) {
		return false;
	}
	fileData.reserve(file_info.uncompressed_size);
	if (unzReadCurrentFile(handle, fileData.data(), static_cast<mqui32>(file_info.uncompressed_size)) <= 0) {
		unzCloseCurrentFile(handle);
		return false;
	}
	MasterQian::Meta::details::lstrcpyA(file_path, ansi_path);
	MasterQian::Meta::details::lstrcatA(file_path, name);
	SaveMultiFolderFile(file_path, fileData.data(), static_cast<mqui32>(file_info.uncompressed_size));

	func(arg, details::ATW(name, unicode_name), details::ATW(file_path, unicode_file_path));
	while (unzGoToNextFile(handle) == UNZ_OK) {
		if (unzGetCurrentFileInfo64(handle, &file_info, name, MAX_PATH, nullptr, 0U, nullptr, 0U) != UNZ_OK) {
			return false;
		}
		if (unzOpenCurrentFile(handle) != UNZ_OK) {
			return false;
		}
		fileData.reserve(file_info.uncompressed_size);
		if (unzReadCurrentFile(handle, fileData.data(), static_cast<mqui32>(file_info.uncompressed_size)) <= 0) {
			unzCloseCurrentFile(handle);
			return false;
		}
		MasterQian::Meta::details::lstrcpyA(file_path, ansi_path);
		MasterQian::Meta::details::lstrcatA(file_path, name);
		SaveMultiFolderFile(file_path, fileData.data(), static_cast<mqui32>(file_info.uncompressed_size));
		func(arg, details::ATW(name, unicode_name), details::ATW(file_path, unicode_file_path));
	}
	return true;
}
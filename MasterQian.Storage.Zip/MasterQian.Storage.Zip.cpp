#include "../include/MasterQian.Meta.h"
#include "zip.h"
#include "unzip.h"
#ifdef _DEBUG
#pragma comment(lib, "zlibstatd.lib")
#else
#pragma comment(lib, "zlibstat.lib")
#endif
import MasterQian.freestanding;
using namespace MasterQian;
#define MasterQianModuleName(name) MasterQian_Storage_Zip_##name
META_EXPORT_API_VERSION(20240131ULL)

mqhandle INVALID_HANDLE_VALUE{ reinterpret_cast<mqhandle>(-1) };
constexpr mqui32 INVALID_FILE_ATTRIBUTES { static_cast<mqui32>(-1) };
constexpr mqui32 FILE_ATTRIBUTE_NORMAL{ 0x00000080U };
constexpr mqui32 FILE_ATTRIBUTE_DIRECTORY{ 0x00000010U };
constexpr mqui32 GENERIC_READ{ 0x80000000U };
constexpr mqui32 GENERIC_WRITE{ 0x40000000U };
constexpr mqui32 CREATE_ALWAYS{ 2U };
constexpr mqui32 OPEN_EXISTING{ 3U };
constexpr mqui32 FILE_SHARE_READ{ 0x00000001U };
constexpr mqui32 PAGE_READONLY{ 0x02U };
constexpr mqui32 FILE_MAP_READ{ 0x0004U };

using LPFILETIME = struct FILETIME {
	mqui32 dwLowDateTime;
	mqui32 dwHighDateTime;
}*;

using LPWIN32_FIND_DATAW = struct WIN32_FIND_DATAW {
	mqui32 dwFileAttributes;
	FILETIME ftCreationTime;
	FILETIME ftLastAccessTime;
	FILETIME ftLastWriteTime;
	mqui32 nFileSizeHigh;
	mqui32 nFileSizeLow;
	mqui32 dwReserved0;
	mqui32 dwReserved1;
	mqchar cFileName[api::PATH_MAX_SIZE];
	mqchar cAlternateFileName[14];
}*;

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
		};
		mqmem Pointer;
	};
	mqhandle hEvent;
}*;

META_WINAPI(mqhandle, FindFirstFileW, mqcstr, LPWIN32_FIND_DATAW);
META_WINAPI(mqbool, FindNextFileW, mqhandle, LPWIN32_FIND_DATAW);
META_WINAPI(mqbool, FindClose, mqhandle);
META_WINAPI(mqui32, GetFileAttributesW, mqcstr);
META_WINAPI(mqhandle, CreateFileW, mqcstr, mqui32, mqui32, LPSECURITY_ATTRIBUTES, mqui32, mqui32, mqhandle);
META_WINAPI(mqbool, CreateDirectoryW, mqcstr, LPSECURITY_ATTRIBUTES);
META_WINAPI(mqbool, WriteFile, mqhandle, mqcmem, mqui32, mqui32*, LPOVERLAPPED);
META_WINAPI(mqbool, GetFileSizeEx, mqhandle, mqui64*);
META_WINAPI(mqhandle, CreateFileMappingW, mqhandle, LPSECURITY_ATTRIBUTES, mqui32, mqui32, mqui32, mqcstr);
META_WINAPI(mqmem, MapViewOfFile, mqhandle, mqui32, mqui32, mqui32, mqui64);
META_WINAPI(mqbool, UnmapViewOfFile, mqcmem);
META_WINAPI(mqbool, CloseHandle, mqhandle);

#pragma comment(linker,"/alternatename:__imp_?CreateDirectoryW@@YAHPEB_WPEAUSECURITY_ATTRIBUTES@@@Z=__imp_CreateDirectoryW")
#pragma comment(linker,"/alternatename:__imp_?FindFirstFileW@@YAPEAXPEB_WPEAUWIN32_FIND_DATAW@@@Z=__imp_FindFirstFileW")
#pragma comment(linker,"/alternatename:__imp_?GetFileSizeEx@@YAHPEAXPEA_K@Z=__imp_GetFileSizeEx")
#pragma comment(linker,"/alternatename:__imp_?FindNextFileW@@YAHPEAXPEAUWIN32_FIND_DATAW@@@Z=__imp_FindNextFileW")
#pragma comment(linker,"/alternatename:__imp_?WriteFile@@YAHPEAXPEBXIPEAIPEAUOVERLAPPED@@@Z=__imp_WriteFile")
#pragma comment(linker,"/alternatename:__imp_?FindClose@@YAHPEAX@Z=__imp_FindClose")
#pragma comment(linker,"/alternatename:__imp_?CreateFileW@@YAPEAXPEB_WIIPEAUSECURITY_ATTRIBUTES@@IIPEAX@Z=__imp_CreateFileW")
#pragma comment(linker,"/alternatename:__imp_?GetFileAttributesW@@YAIPEB_W@Z=__imp_GetFileAttributesW")
#pragma comment(linker,"/alternatename:__imp_?UnmapViewOfFile@@YAHPEBX@Z=__imp_UnmapViewOfFile")
#pragma comment(linker,"/alternatename:__imp_?CloseHandle@@YAHPEAX@Z=__imp_CloseHandle")
#pragma comment(linker,"/alternatename:__imp_?CreateFileMappingW@@YAPEAXPEAXPEAUSECURITY_ATTRIBUTES@@IIIPEB_W@Z=__imp_CreateFileMappingW")
#pragma comment(linker,"/alternatename:__imp_?MapViewOfFile@@YAPEAXPEAXIII_K@Z=__imp_MapViewOfFile")

enum class OpenMode : mqenum {
	CREATE, // 不存在创建, 存在失败
	CREATE_OPEN, // 不存在创建, 存在打开
	CREATE_OVERRIDE, // 不存在创建, 存在覆盖
	OPEN_EXIST, // 不存在失败, 存在打开
};

static mqstr ATW(mqcstra str_a, mqstr str_w) noexcept {
	auto len{ api::MultiByteToWideChar(0U, 0, str_a, -1, nullptr, 0) };
	if (len > 0 && len < api::PATH_MAX_SIZE) {
		api::MultiByteToWideChar(0U, 0, str_a, -1, str_w, len);
	}
	else {
		*str_w = L'\0';
	}
	return str_w;
}

static mqstra WTA(mqcstr str_w, mqstra str_a) noexcept {
	auto len{ api::WideCharToMultiByte(0U, 0, str_w, -1, nullptr, 0, nullptr, nullptr) };
	if (len > 0 && len < api::PATH_MAX_SIZE) {
		api::WideCharToMultiByte(0U, 0, str_w, -1, str_a, len, nullptr, nullptr);
	}
	else {
		*str_a = L'\0';
	}
	return str_a;
}

META_EXPORT_API(mqui32, CompressBound, mqui32 src_size) {
	return compressBound(src_size) + sizeof(mqui32); // 用于存储压缩前大小
}

META_EXPORT_API(mqui32, Compress, mqcbytes src, mqui32 src_size, mqbytes des, mqui32 des_size, mqbool speedFirstly) {
	unsigned long size{ des_size };
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
	unsigned long size{ des_size };
	return uncompress(des, &size, src + sizeof(mqui32), src_size - sizeof(mqui32)) == Z_OK;
}

META_EXPORT_API(zipFile, ZipStart, mqcstr fn, OpenMode mode) {
	mqui32 attribute{ GetFileAttributesW(fn) };
	bool isExists{ attribute != INVALID_FILE_ATTRIBUTES && attribute != FILE_ATTRIBUTE_DIRECTORY };
	mqchara ansi_fn[api::PATH_MAX_SIZE]{ };
	switch (mode) {
	case OpenMode::CREATE: {
		if (!isExists) {
			return zipOpen64(WTA(fn, ansi_fn), APPEND_STATUS_CREATE);
		}
		break;
	}
	case OpenMode::CREATE_OPEN: {
		if (isExists) {
			return zipOpen64(WTA(fn, ansi_fn), APPEND_STATUS_ADDINZIP);
		}
		else {
			return zipOpen64(WTA(fn, ansi_fn), APPEND_STATUS_CREATE);
		}
	}
	case OpenMode::CREATE_OVERRIDE: {
		return zipOpen64(WTA(fn, ansi_fn), APPEND_STATUS_CREATE);
	}
	case OpenMode::OPEN_EXIST: {
		return zipOpen64(WTA(fn, ansi_fn), APPEND_STATUS_ADDINZIP);
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
	mqchara ansi_name[api::PATH_MAX_SIZE]{ };
	if (zipOpenNewFileInZip64(handle, WTA(branch, ansi_name), nullptr, nullptr, 0U,
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
				mqui64 size{ };
				GetFileSizeEx(hFile, &size);
				result = MasterQian_Storage_Zip_ZipAddBin(handle, branch, static_cast<mqcbytes>(data), freestanding::low32(size));
				UnmapViewOfFile(data);
			}
			CloseHandle(hFileMap);
		}
		CloseHandle(hFile);
	}
	return result;
}

META_EXPORT_API(mqbool, ZipAddFolder, zipFile handle, mqcstr branch, mqcstr path) {
	mqchar buf[api::PATH_MAX_SIZE]{ }, findPath[api::PATH_MAX_SIZE]{ }, new_branch[api::PATH_MAX_SIZE]{ };
	api::lstrcpyW(findPath, path);
	api::lstrcatW(findPath, L"\\*");
	WIN32_FIND_DATAW data;
	mqhandle hFileFind{ FindFirstFileW(findPath, &data) };
	bool result{ true };
	if (hFileFind != INVALID_HANDLE_VALUE) {
		bool hasNext{ true };
		while (hasNext) {
			if (api::lstrcmpW(data.cFileName, L".") != 0 &&
				api::lstrcmpW(data.cFileName, L"..") != 0) {
				api::lstrcpyW(buf, path);
				api::lstrcatW(buf, L"\\");
				api::lstrcatW(buf, data.cFileName);
				api::lstrcpyW(new_branch, branch);
				api::lstrcatW(new_branch, data.cFileName);
				if (MasterQian_Storage_Zip_ZipIsFolder(buf)) {
					api::lstrcatW(new_branch, L"\\");
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
	mqchar buf[api::PATH_MAX_SIZE]{ }, findPath[api::PATH_MAX_SIZE]{ }, new_branch[api::PATH_MAX_SIZE]{ };
	api::lstrcpyW(findPath, path);
	api::lstrcatW(findPath, L"\\*");
	WIN32_FIND_DATAW data;
	mqhandle hFileFind{ FindFirstFileW(findPath, &data) };
	bool result{ true };
	if (hFileFind != INVALID_HANDLE_VALUE) {
		bool hasNext{ true };
		while (hasNext) {
			if (api::lstrcmpW(data.cFileName, L".") != 0 && api::lstrcmpW(data.cFileName, L"..") != 0) {
				api::lstrcpyW(buf, path);
				api::lstrcatW(buf, L"\\");
				api::lstrcatW(buf, data.cFileName);
				api::lstrcpyW(new_branch, branch);
				api::lstrcatW(new_branch, data.cFileName);
				if (MasterQian_Storage_Zip_ZipIsFolder(buf)) {
					api::lstrcatW(new_branch, L"\\");
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
	mqchara ansi_fn[api::PATH_MAX_SIZE]{ };
	return unzOpen64(WTA(fn, ansi_fn));
}

META_EXPORT_API(void, UnZipEnd, unzFile handle) {
	unzClose(handle);
}

META_EXPORT_API(ZPOS64_T, UnZipCount, unzFile handle) {
	unz_global_info64 global_info{ };
	unzGetGlobalInfo64(handle, &global_info);
	return global_info.number_entry;
}

static void SaveMultiFolderFile(mqcstr file_path, mqbytes mem, mqui32 size) noexcept {
	mqchar tmp[api::PATH_MAX_SIZE]{ };
	api::lstrcpyW(tmp, file_path);
	for (auto p{ tmp }; *p; ++p) {
		if (*p == L'/') {
			*p = L'\\';
		}
		if (*p == L'\\') {
			*p = L'\0';
			if (GetFileAttributesW(tmp) == INVALID_FILE_ATTRIBUTES) {
				CreateDirectoryW(tmp, nullptr);
			}
			*p = L'\\';
		}
	}
	if (auto hFile{ CreateFileW(file_path, GENERIC_WRITE, 0,
		nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL,
		nullptr) }; hFile != INVALID_HANDLE_VALUE) {
		WriteFile(hFile, mem, size, nullptr, nullptr);
		CloseHandle(hFile);
	}
}

META_EXPORT_API(mqbool, UnZipSave, unzFile handle, mqcstr path) {
	mqchara ansi_path[api::PATH_MAX_SIZE]{ }, file_path[api::PATH_MAX_SIZE]{ };
	WTA(path, ansi_path);
	api::lstrcatA(ansi_path, "\\");

	if (unzGoToFirstFile(handle) != UNZ_OK) {
		return false;
	}
	unz_file_info64 file_info;
	mqchara name[api::PATH_MAX_SIZE];
	mqchar file_path_w[api::PATH_MAX_SIZE]{ };
	mqbuffer fileData;
	if (unzGetCurrentFileInfo64(handle, &file_info, name, api::PATH_MAX_SIZE, nullptr, 0U, nullptr, 0U) != UNZ_OK) {
		return false;
	}
	if (file_info.uncompressed_size) {
		if (unzOpenCurrentFile(handle) != UNZ_OK) {
			return false;
		}
		fileData.reserve(file_info.uncompressed_size);
		if (unzReadCurrentFile(handle, fileData.data(), static_cast<mqui32>(file_info.uncompressed_size)) <= 0) {
			unzCloseCurrentFile(handle);
			return false;
		}
		api::lstrcpyA(file_path, ansi_path);
		api::lstrcatA(file_path, name);
		SaveMultiFolderFile(ATW(file_path, file_path_w), fileData.data(), static_cast<mqui32>(file_info.uncompressed_size));
	}
	while (unzGoToNextFile(handle) == UNZ_OK) {
		if (unzGetCurrentFileInfo64(handle, &file_info, name, api::PATH_MAX_SIZE, nullptr, 0U, nullptr, 0U) != UNZ_OK) {
			return false;
		}
		if (file_info.uncompressed_size) {
			if (unzOpenCurrentFile(handle) != UNZ_OK) {
				return false;
			}
			fileData.reserve(file_info.uncompressed_size);
			if (unzReadCurrentFile(handle, fileData.data(), static_cast<mqui32>(file_info.uncompressed_size)) <= 0) {
				unzCloseCurrentFile(handle);
				return false;
			}
			api::lstrcpyA(file_path, ansi_path);
			api::lstrcatA(file_path, name);
			SaveMultiFolderFile(ATW(file_path, file_path_w), fileData.data(), static_cast<mqui32>(file_info.uncompressed_size));
		}
	}
	return true;
}

using UnZipCallBack = void(__stdcall*)(mqmem, mqcstr, mqcstr);

META_EXPORT_API(mqbool, UnZipSaveWithCallback, unzFile handle, mqcstr path, UnZipCallBack func, mqmem arg) {
	mqchara ansi_path[api::PATH_MAX_SIZE]{ }, file_path[api::PATH_MAX_SIZE]{ };
	mqchar unicode_name[api::PATH_MAX_SIZE]{ }, unicode_file_path[api::PATH_MAX_SIZE]{ };
	WTA(path, ansi_path);
	api::lstrcatA(ansi_path, "\\");

	if (unzGoToFirstFile(handle) != UNZ_OK) {
		return false;
	}
	unz_file_info64 file_info;
	mqchara name[api::PATH_MAX_SIZE];
	mqchar file_path_w[api::PATH_MAX_SIZE]{ };
	mqbuffer fileData;
	if (unzGetCurrentFileInfo64(handle, &file_info, name, api::PATH_MAX_SIZE, nullptr, 0U, nullptr, 0U) != UNZ_OK) {
		return false;
	}
	if (file_info.uncompressed_size) {
		if (unzOpenCurrentFile(handle) != UNZ_OK) {
			return false;
		}
		fileData.reserve(file_info.uncompressed_size);
		if (unzReadCurrentFile(handle, fileData.data(), static_cast<mqui32>(file_info.uncompressed_size)) <= 0) {
			unzCloseCurrentFile(handle);
			return false;
		}
		api::lstrcpyA(file_path, ansi_path);
		api::lstrcatA(file_path, name);
		SaveMultiFolderFile(ATW(file_path, file_path_w), fileData.data(), static_cast<mqui32>(file_info.uncompressed_size));

		func(arg, ATW(name, unicode_name), ATW(file_path, unicode_file_path));
	}
	while (unzGoToNextFile(handle) == UNZ_OK) {
		if (unzGetCurrentFileInfo64(handle, &file_info, name, api::PATH_MAX_SIZE, nullptr, 0U, nullptr, 0U) != UNZ_OK) {
			return false;
		}
		if (file_info.uncompressed_size) {
			if (unzOpenCurrentFile(handle) != UNZ_OK) {
				return false;
			}
			fileData.reserve(file_info.uncompressed_size);
			if (unzReadCurrentFile(handle, fileData.data(), static_cast<mqui32>(file_info.uncompressed_size)) <= 0) {
				unzCloseCurrentFile(handle);
				return false;
			}
			api::lstrcpyA(file_path, ansi_path);
			api::lstrcatA(file_path, name);
			SaveMultiFolderFile(ATW(file_path, file_path_w), fileData.data(), static_cast<mqui32>(file_info.uncompressed_size));
			func(arg, ATW(name, unicode_name), ATW(file_path, unicode_file_path));
		}
	}
	return true;
}
#include "../include/MasterQian.Meta.h"
#include <stack>
#include <vector>
import MasterQian.freestanding;
using namespace MasterQian;
#define MasterQianModuleName(name) MasterQian_Storage_Path_##name
META_EXPORT_API_VERSION(20240131ULL)

mqhandle INVALID_HANDLE_VALUE{ reinterpret_cast<mqhandle>(-1) };
constexpr mqui32 INVALID_FILE_ATTRIBUTES{ static_cast<mqui32>(-1) };
constexpr mqui32 FILE_ATTRIBUTE_NORMAL{ 0x00000080U };
constexpr mqui32 FILE_ATTRIBUTE_DIRECTORY{ 0x00000010U };
constexpr mqui32 GENERIC_READ{ 0x80000000U };
constexpr mqui32 GENERIC_WRITE{ 0x40000000U };
constexpr mqui32 CREATE_ALWAYS{ 2U };
constexpr mqui32 OPEN_EXISTING{ 3U };
constexpr mqui32 FILE_SHARE_READ{ 0x00000001U };
constexpr mqui32 PAGE_READONLY{ 0x02U };
constexpr mqui32 PAGE_READWRITE{ 0x04U };
constexpr mqui32 FILE_MAP_READ{ 0x0004U };
constexpr mqui32 FILE_MAP_WRITE{ 0x0002U };
constexpr mqui32 FOF_NOERRORUI{ 0x0400U };
constexpr mqui32 FOF_ALLOWUNDO{ 0x0040U };
constexpr mqui32 FOF_SILENT{ 0x0004U };
constexpr mqui32 FOF_NOCONFIRMATION{ 0x0010U };
constexpr mqui32 FO_DELETE{ 0x0003U };
constexpr mqui32 SHERB_NOCONFIRMATION{ 0x00000001U };
constexpr mqui32 SHERB_NOPROGRESSUI{ 0x00000002U };
constexpr mqui32 SHERB_NOSOUND{ 0x00000004U };

using HRESULT = mqui32;

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

using LPWIN32_FILE_ATTRIBUTE_DATA = struct WIN32_FILE_ATTRIBUTE_DATA {
	mqui32 dwFileAttributes;
	FILETIME ftCreationTime;
	FILETIME ftLastAccessTime;
	FILETIME ftLastWriteTime;
	mqui32 nFileSizeHigh;
	mqui32 nFileSizeLow;
}*;

using LPSECURITY_ATTRIBUTES = struct SECURITY_ATTRIBUTES {
	mqui32 nLength;
	mqmem lpSecurityDescriptor;
	mqbool bInheritHandle;
}*;

using GET_FILEEX_INFO_LEVELS = enum {
	GetFileExInfoStandard,
	GetFileExMaxInfoLevel
};

using FILEOP_FLAGS = mqui16;

using LPSHFILEOPSTRUCTW = struct SHFILEOPSTRUCTW {
	mqhandle hwnd;
	mqui32 wFunc;
	mqcstr pFrom;
	mqcstr pTo;
	FILEOP_FLAGS fFlags;
	mqbool fAnyOperationsAborted;
	mqmem hNameMappings;
	mqcstr lpszProgressTitle;
}*;

using LPSHQUERYRBINFO = struct SHQUERYRBINFO {
	mqui32 cbSize;
	mqui64 i64Size;
	mqui64 i64NumItems;
}*;

using LPLINKFILE_HEADER = struct LINKFILE_HEADER {
	mqui32 Tag; // 0x00
	mqguid LinkCLSID; // 0x04
	mqui32 Flags; // 0x14
	mqui32 FileAttributes; // 0x18
	FILETIME CreationTime; // 0x1c
	FILETIME AccessTime; // 0x24
	FILETIME WriteTime; // 0x2c
	mqui32 FileSize; // 0x34
	mqui32 IconIndex; // 0x38
	mqui32 ShowCommand; // 0x3c
	mqui32 Hotkey; // 0x40
	mqbyte retain[8]; // 0x44
}*;

META_WINAPI(mqhandle, FindFirstFileW, mqcstr, LPWIN32_FIND_DATAW);
META_WINAPI(mqbool, FindNextFileW, mqhandle, LPWIN32_FIND_DATAW);
META_WINAPI(mqbool, FindClose, mqhandle);
META_WINAPI(mqui32, GetFileAttributesW, mqcstr);
META_WINAPI(mqbool, GetFileAttributesExW, mqcstr, GET_FILEEX_INFO_LEVELS, mqmem);
META_WINAPI(mqhandle, CreateFileW, mqcstr, mqui32, mqui32, LPSECURITY_ATTRIBUTES, mqui32, mqui32, mqhandle);
META_WINAPI(mqbool, SetFileTime, mqhandle, FILETIME const*, FILETIME const*, FILETIME const*);
META_WINAPI(mqbool, CloseHandle, mqhandle);
META_WINAPI(mqhandle, CreateFileMappingW, mqhandle, LPSECURITY_ATTRIBUTES, mqui32, mqui32, mqui32, mqcstr);
META_WINAPI(mqmem, MapViewOfFile, mqhandle, mqui32, mqui32, mqui32, mqui64);
META_WINAPI(mqbool, UnmapViewOfFile, mqcmem);
META_WINAPI(mqbool, CreateDirectoryW, mqcstr, LPSECURITY_ATTRIBUTES);
META_WINAPI(mqbool, RemoveDirectoryW, mqcstr);
META_WINAPI(mqbool, CopyFileW, mqcstr, mqcstr, mqbool);
META_WINAPI(mqbool, MoveFileW, mqcstr, mqcstr);
META_WINAPI(mqbool, DeleteFileW, mqcstr);
META_WINAPI(mqui32, GetLogicalDrives);
META_WINAPI(mqbool, GetDiskFreeSpaceW, mqcstr, mqui32*, mqui32*, mqui32*, mqui32*);
META_WINAPI(mqui32, GetTempPathW, mqui32, mqstr);
META_WINAPI(mqui32, GetModuleFileNameW, mqhandle, mqstr, mqui32);
META_WINAPI(mqbool, SetCurrentDirectoryW, mqcstr);
META_WINAPI(mqi32, SHFileOperationW, LPSHFILEOPSTRUCTW);
META_WINAPI(HRESULT, SHQueryRecycleBinW, mqcstr, LPSHQUERYRBINFO);
META_WINAPI(HRESULT, SHEmptyRecycleBinW, mqhandle, mqcstr, mqui32);
META_WINAPI(mqbool, SHGetSpecialFolderPathW, mqhandle, mqstr, mqi32, mqbool);

#pragma comment(linker,"/alternatename:__imp_?CreateDirectoryW@@YAHPEB_WPEAUSECURITY_ATTRIBUTES@@@Z=__imp_CreateDirectoryW")
#pragma comment(linker,"/alternatename:__imp_?GetLogicalDrives@@YAIXZ=__imp_GetLogicalDrives")
#pragma comment(linker,"/alternatename:__imp_?FindFirstFileW@@YAPEAXPEB_WPEAUWIN32_FIND_DATAW@@@Z=__imp_FindFirstFileW")
#pragma comment(linker,"/alternatename:__imp_?FindNextFileW@@YAHPEAXPEAUWIN32_FIND_DATAW@@@Z=__imp_FindNextFileW")
#pragma comment(linker,"/alternatename:__imp_?GetDiskFreeSpaceW@@YAHPEB_WPEAI111@Z=__imp_GetDiskFreeSpaceW")
#pragma comment(linker,"/alternatename:__imp_?RemoveDirectoryW@@YAHPEB_W@Z=__imp_RemoveDirectoryW")
#pragma comment(linker,"/alternatename:__imp_?SetFileTime@@YAHPEAXPEBUFILETIME@@11@Z=__imp_SetFileTime")
#pragma comment(linker,"/alternatename:__imp_?GetModuleFileNameW@@YAIPEAXPEA_WI@Z=__imp_GetModuleFileNameW")
#pragma comment(linker,"/alternatename:__imp_?GetTempPathW@@YAIIPEA_W@Z=__imp_GetTempPathW")
#pragma comment(linker,"/alternatename:__imp_?FindClose@@YAHPEAX@Z=__imp_FindClose")
#pragma comment(linker,"/alternatename:__imp_?CreateFileW@@YAPEAXPEB_WIIPEAUSECURITY_ATTRIBUTES@@IIPEAX@Z=__imp_CreateFileW")
#pragma comment(linker,"/alternatename:__imp_?GetFileAttributesW@@YAIPEB_W@Z=__imp_GetFileAttributesW")
#pragma comment(linker,"/alternatename:__imp_?UnmapViewOfFile@@YAHPEBX@Z=__imp_UnmapViewOfFile")
#pragma comment(linker,"/alternatename:__imp_?SHFileOperationW@@YAHPEAUSHFILEOPSTRUCTW@@@Z=__imp_SHFileOperationW")
#pragma comment(linker,"/alternatename:__imp_?GetFileAttributesExW@@YAHPEB_WW4GET_FILEEX_INFO_LEVELS@@PEAX@Z=__imp_GetFileAttributesExW")
#pragma comment(linker,"/alternatename:__imp_?SHQueryRecycleBinW@@YAIPEB_WPEAUSHQUERYRBINFO@@@Z=__imp_SHQueryRecycleBinW")
#pragma comment(linker,"/alternatename:__imp_?DeleteFileW@@YAHPEB_W@Z=__imp_DeleteFileW")
#pragma comment(linker,"/alternatename:__imp_?CloseHandle@@YAHPEAX@Z=__imp_CloseHandle")
#pragma comment(linker,"/alternatename:__imp_?SHEmptyRecycleBinW@@YAIPEAXPEB_WI@Z=__imp_SHEmptyRecycleBinW")
#pragma comment(linker,"/alternatename:__imp_?SetCurrentDirectoryW@@YAHPEB_W@Z=__imp_SetCurrentDirectoryW")
#pragma comment(linker,"/alternatename:__imp_?CopyFileW@@YAHPEB_W0H@Z=__imp_CopyFileW")
#pragma comment(linker,"/alternatename:__imp_?CreateFileMappingW@@YAPEAXPEAXPEAUSECURITY_ATTRIBUTES@@IIIPEB_W@Z=__imp_CreateFileMappingW")
#pragma comment(linker,"/alternatename:__imp_?MapViewOfFile@@YAPEAXPEAXIII_K@Z=__imp_MapViewOfFile")
#pragma comment(linker,"/alternatename:__imp_?SHGetSpecialFolderPathW@@YAHPEAXPEA_WHH@Z=__imp_SHGetSpecialFolderPathW")
#pragma comment(linker,"/alternatename:__imp_?MoveFileW@@YAHPEB_W0@Z=__imp_MoveFileW")

constexpr mqui64 UnixDST{ 116444736000000000ULL };

using EnumFileCallback = void(__stdcall*)(mqmem arg, mqcstr path, WIN32_FIND_DATAW const* data) noexcept;
using EnumFolderCallback = void(__stdcall*)(mqmem arg, mqcstr path, WIN32_FIND_DATAW const* data, mqbool isStart) noexcept;
using EnumUserCallback = void(__stdcall*)(mqmem arg, mqcstr path) noexcept;

enum class PathCode : mqi32 {
	Desktop = 0x0000,
	Documents = 0x0005,
	Music = 0x000D,
	Video = 0x000E,
	Fonts = 0x0014,
	AppData = 0x001A,
	Windows = 0x0024,
	System = 0x0025,
	ProgramFiles = 0x0026,
	Picture = 0x0027,

	Running = 5201314,
	RunningName,
	Temp,
};

inline static bool Exists(mqui32 attr) {
	return attr != INVALID_FILE_ATTRIBUTES;
}

inline static bool IsFolder(mqui32 attr) {
	return (attr & FILE_ATTRIBUTE_DIRECTORY) != 0U;
}

inline static bool IsFile(mqui32 attr) {
	return !IsFolder(attr);
}

static void EnumFolderImpl(mqcstr path, EnumFileCallback func, mqmem arg, mqcstr ext) noexcept {
	mqchar find_path[api::PATH_MAX_SIZE]{ };
	(void)api::lstrcpynW(find_path, path, api::PATH_MAX_SIZE - api::lstrlenW(ext) - 1U); // 预留\*.ext空间
	api::lstrcatW(find_path, L"\\");
	api::lstrcatW(find_path, ext);
	WIN32_FIND_DATAW data{ };
	mqhandle handle{ FindFirstFileW(find_path, &data) };
	if (handle != INVALID_HANDLE_VALUE) {
		do {
			if (api::lstrcmpW(data.cFileName, L".") != 0
				&& api::lstrcmpW(data.cFileName, L"..") != 0) { // 跳过. 和..
				func(arg, path, &data);
			}
		} while (FindNextFileW(handle, &data));
		FindClose(handle);
	}
}

static void EnumFolderDepthImpl(mqcstr path, EnumFileCallback file_func, EnumFolderCallback folder_func, mqmem arg) noexcept {
	struct EnumFolderDepthStruct {
		mqhandle handle;
		mqchar path[api::PATH_MAX_SIZE];

		EnumFolderDepthStruct(mqcstr value) : handle{ } {
			api::lstrcpyW(path, value);
		}
	};

	std::stack<EnumFolderDepthStruct, std::vector<EnumFolderDepthStruct>> hStack;
	hStack.emplace(path); // 初始路径

	WIN32_FIND_DATAW data{ }; // 查找数据

	while (!hStack.empty()) {
		auto& topF{ hStack.top() };
		if (topF.handle == nullptr) { // 初次查找
			mqchar find_path[api::PATH_MAX_SIZE]{ }; // 查找路径
			(void)api::lstrcpynW(find_path, topF.path, api::PATH_MAX_SIZE - 2); // 预留\*空间
			api::lstrcatW(find_path, L"\\*");
			topF.handle = FindFirstFileW(find_path, &data);
			if (topF.handle == INVALID_HANDLE_VALUE) { // 查找失败
				hStack.pop(); // 出栈
			}
			else { // 跳过.
				FindNextFileW(topF.handle, &data); // 跳过..
			}
		}
		else { // 二次查找
			if (FindNextFileW(topF.handle, &data)) { // 查找成功
				if (IsFolder(data.dwFileAttributes)) { // 是目录
					mqchar subpath[api::PATH_MAX_SIZE]; // 子目录路径
					api::lstrcpyW(subpath, topF.path);
					api::lstrcatW(subpath, L"\\");
					api::lstrcatW(subpath, data.cFileName);

					if (folder_func) folder_func(arg, subpath, &data, true); // 进入目录前回调
					hStack.emplace(subpath); // 一定先临时拷贝再push, 否则会因为扩容丢失引用
				}
				else { // 是文件
					file_func(arg, topF.path, &data); // 调用回调
				}
			}
			else { // 到达结尾
				if (folder_func) folder_func(arg, topF.path, &data, false); // 退出目录后回调
				FindClose(topF.handle); // 关闭查找
				hStack.pop(); // 出栈
			}
		}
	}
}

META_EXPORT_API(mqui32, Attribute, mqcstr path) {
	return GetFileAttributesW(path);
}

META_EXPORT_API(mqui64, Size, mqcstr path) {
	WIN32_FILE_ATTRIBUTE_DATA attr{ .dwFileAttributes = INVALID_FILE_ATTRIBUTES };
	GetFileAttributesExW(path, GetFileExInfoStandard, &attr);
	if (Exists(attr.dwFileAttributes)) {
		if (IsFolder(attr.dwFileAttributes)) { // 目录大小
			mqui64 size{ };
			EnumFolderDepthImpl(path, [ ] (mqmem arg, mqcstr path, WIN32_FIND_DATAW const* data) noexcept {
				*static_cast<mqui64*>(arg) += freestanding::ui64(data->nFileSizeLow, data->nFileSizeHigh);
				}, nullptr, &size);
			return size;
		}
		else { // 文件大小
			return freestanding::ui64(attr.nFileSizeLow, attr.nFileSizeHigh);
		}
	}
	return 0ULL;
}

META_EXPORT_API(mqui64, GetTime, mqcstr path, mqui32 type) {
	WIN32_FILE_ATTRIBUTE_DATA attr{ .dwFileAttributes = INVALID_FILE_ATTRIBUTES };
	GetFileAttributesExW(path, GetFileExInfoStandard, &attr);
	mqui64 t{ };
	if (Exists(attr.dwFileAttributes)) {
		if (type == 1U) { // 修改时间
			t = freestanding::ui64(attr.ftLastWriteTime.dwLowDateTime, attr.ftLastWriteTime.dwHighDateTime);
		}
		else if (type == 2U) { // 访问时间
			t = freestanding::ui64(attr.ftLastAccessTime.dwLowDateTime, attr.ftLastAccessTime.dwHighDateTime);
		}
		else { // 创建时间
			t = freestanding::ui64(attr.ftCreationTime.dwLowDateTime, attr.ftCreationTime.dwHighDateTime);
		}
		return t - UnixDST;
	}
	return t;
}

META_EXPORT_API(mqbool, SetTime, mqcstr path, mqui32 type, mqui64 t) {
	if (auto attr = GetFileAttributesW(path); Exists(attr)) {
		mqui32 flag = IsFolder(attr) ? 0x02000000U : 0U;
		if (auto hFile{ CreateFileW(path, GENERIC_WRITE, 0,
			nullptr, OPEN_EXISTING, flag, nullptr) };
			hFile != INVALID_HANDLE_VALUE) {
			t += UnixDST;
			if (type == 1U) { // 修改时间
				SetFileTime(hFile, nullptr, nullptr, reinterpret_cast<FILETIME const*>(&t));
			}
			else if (type == 2U) { // 访问时间
				SetFileTime(hFile, nullptr, reinterpret_cast<FILETIME const*>(&t), nullptr);
			}
			else { // 创建时间
				SetFileTime(hFile, reinterpret_cast<FILETIME const*>(&t), nullptr, nullptr);
			}
			CloseHandle(hFile);
			return true;
		}
	}
	return false;
}

META_EXPORT_API(void, Read, mqcstr path, mqui64 size, mqbytes buf) {
	if (auto hFile{ CreateFileW(path, GENERIC_READ, FILE_SHARE_READ,
		nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
		nullptr) }; hFile != INVALID_HANDLE_VALUE) {
		if (auto hFileMap{ CreateFileMappingW(hFile, nullptr,
			PAGE_READONLY, freestanding::high32(size),
			freestanding::low32(size), nullptr) }) {
			if (auto data{ MapViewOfFile(hFileMap, FILE_MAP_READ,
				0, 0, size) }) {
				memcpy(buf, data, size);
				UnmapViewOfFile(data);
			}
			CloseHandle(hFileMap);
		}
		CloseHandle(hFile);
	}
}

META_EXPORT_API(mqbool, Write, mqcstr path, mqui64 size, mqcbytes buf) {
	bool result{ };
	if (auto hFile{ CreateFileW(path, GENERIC_READ | GENERIC_WRITE, 0,
		nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL,
		nullptr) }; hFile != INVALID_HANDLE_VALUE) {
		if (auto hFileMap{ CreateFileMappingW(hFile, nullptr,
			PAGE_READWRITE, freestanding::high32(size),
			freestanding::low32(size), nullptr) }) {
			if (auto data{ MapViewOfFile(hFileMap, FILE_MAP_WRITE,
				0, 0, size) }) {
				memcpy(data, buf, size);
				result = UnmapViewOfFile(data);
			}
			CloseHandle(hFileMap);
		}
		CloseHandle(hFile);
	}
	return result;
}

META_EXPORT_API(mqbool, Copy, mqcstr rootPath, mqcstr filename, mqcstr folder_path, mqbool overridable) {
	mqchar src[api::PATH_MAX_SIZE];
	api::lstrcpyW(src, rootPath);
	api::lstrcatW(src, L"\\");
	api::lstrcatW(src, filename);
	auto attr1 = GetFileAttributesW(src), attr2 = GetFileAttributesW(folder_path);
	if (Exists(attr1) && Exists(attr2) && IsFolder(attr2)) {
		mqchar des[api::PATH_MAX_SIZE];
		api::lstrcpyW(des, folder_path);
		api::lstrcatW(des, L"\\");
		api::lstrcatW(des, filename);
		if (IsFolder(attr1)) { // 目录
			CreateDirectoryW(des, nullptr); // 先创建复制后的根目录
			struct callback_wrapper {
				mqcstr des; // 目标目录
				mqui32 rootLen; // 根路径长度
				mqbool overridable; // 是否覆写
				bool result;
			}wrapper{ des, api::lstrlenW(src), overridable, true };
			// 遍历原目录
			EnumFolderDepthImpl(src,
				[ ] (mqmem arg, mqcstr path, WIN32_FIND_DATAW const* data) noexcept { // 文件回调
					auto wrapper{ static_cast<callback_wrapper*>(arg) };
					mqchar oldFile[api::PATH_MAX_SIZE], newFile[api::PATH_MAX_SIZE];
					api::lstrcpyW(oldFile, path);
					api::lstrcatW(oldFile, L"\\");
					api::lstrcatW(oldFile, data->cFileName);
					api::lstrcpyW(newFile, wrapper->des);
					api::lstrcatW(newFile, path + wrapper->rootLen);
					api::lstrcatW(newFile, L"\\");
					api::lstrcatW(newFile, data->cFileName);
					wrapper->result = wrapper->result && CopyFileW(oldFile, newFile, wrapper->overridable == 0);

				}, [ ] (mqmem arg, mqcstr path, WIN32_FIND_DATAW const* data, mqbool isStart) noexcept { // 目录回调
					if (isStart) { // 进入目录前创建目录
						auto wrapper{ static_cast<callback_wrapper*>(arg) };
						mqchar newFolder[api::PATH_MAX_SIZE];
						api::lstrcpyW(newFolder, wrapper->des);
						api::lstrcatW(newFolder, path + wrapper->rootLen);
						wrapper->result = wrapper->result && CreateDirectoryW(newFolder, nullptr);
					}
					}, &wrapper);
			return wrapper.result;
		}
		else { // 文件
			return CopyFileW(src, des, overridable == 0);
		}
	}
	return false;
}

META_EXPORT_API(mqbool, CopyRename, mqcstr src, mqcstr des, mqbool overridable) {
	return CopyFileW(src, des, overridable == 0);
}

META_EXPORT_API(mqbool, Move, mqcstr rootPath, mqcstr filename, mqcstr folder_path) {
	mqchar src[api::PATH_MAX_SIZE];
	api::lstrcpyW(src, rootPath);
	api::lstrcatW(src, L"\\");
	api::lstrcatW(src, filename);
	auto attr1 = GetFileAttributesW(src), attr2 = GetFileAttributesW(folder_path);
	if (Exists(attr1) && Exists(attr2) && IsFolder(attr2)) {
		mqchar des[api::PATH_MAX_SIZE];
		api::lstrcpyW(des, folder_path);
		api::lstrcatW(des, L"\\");
		api::lstrcatW(des, filename);
		if (IsFolder(attr1)) { // 目录
			if (MoveFileW(src, des)) { // 尝试移动同驱动器下的目录
				return true;
			}
			// 不同驱动器下手动转移
			CreateDirectoryW(des, nullptr); // 先创建移动后的根目录
			struct callback_wrapper {
				mqcstr des; // 目标目录
				mqui32 rootLen; // 根路径长度
				bool result;
			}wrapper{ des, api::lstrlenW(src), true };
			// 遍历原目录
			EnumFolderDepthImpl(src,
				[ ] (mqmem arg, mqcstr path, WIN32_FIND_DATAW const* data) noexcept { // 文件回调
					auto wrapper{ static_cast<callback_wrapper*>(arg) };
					mqchar oldFile[api::PATH_MAX_SIZE], newFile[api::PATH_MAX_SIZE];
					api::lstrcpyW(oldFile, path);
					api::lstrcatW(oldFile, L"\\");
					api::lstrcatW(oldFile, data->cFileName);
					api::lstrcpyW(newFile, wrapper->des);
					api::lstrcatW(newFile, path + wrapper->rootLen);
					api::lstrcatW(newFile, L"\\");
					api::lstrcatW(newFile, data->cFileName);
					wrapper->result = wrapper->result && MoveFileW(oldFile, newFile);

				}, [ ] (mqmem arg, mqcstr path, WIN32_FIND_DATAW const* data, mqbool isStart) noexcept { // 目录回调
					if (isStart) { // 进入目录前创建目录
						auto wrapper{ static_cast<callback_wrapper*>(arg) };
						mqchar newFolder[api::PATH_MAX_SIZE];
						api::lstrcpyW(newFolder, wrapper->des);
						api::lstrcatW(newFolder, path + wrapper->rootLen);
						wrapper->result = wrapper->result && CreateDirectoryW(newFolder, nullptr);
					}
					else { // 退出目录后删除原目录
						RemoveDirectoryW(path);
					}
					}, &wrapper);
			return wrapper.result;
		}
		else { // 文件
			return MoveFileW(src, des);
		}
	}
	return false;
}

META_EXPORT_API(mqbool, Rename, mqcstr src, mqcstr des) {
	return MoveFileW(src, des);
}

META_EXPORT_API(mqbool, Delete, mqcstr path) {
	if (auto attr = GetFileAttributesW(path); Exists(attr)) {
		if (IsFolder(attr)) { // 目录
			bool result{ true };
			EnumFolderDepthImpl(path,
				[ ] (mqmem arg, mqcstr path, WIN32_FIND_DATAW const* data) noexcept { // 文件回调
					bool& result{ *static_cast<bool*>(arg) };
					mqchar buf[api::PATH_MAX_SIZE];
					api::lstrcpyW(buf, path);
					api::lstrcatW(buf, L"\\");
					api::lstrcatW(buf, data->cFileName);
					result = result && DeleteFileW(buf);
				}, [ ] (mqmem arg, mqcstr path, WIN32_FIND_DATAW const* data, mqbool isStart) noexcept { // 目录回调
					if (!isStart) { // 退出目录后删除
						bool& result{ *static_cast<bool*>(arg) };
						result = result && RemoveDirectoryW(path);
					}
					}, &result);
			return result;
		}
		else { // 文件
			return DeleteFileW(path);
		}
	}
	return false;
}

META_EXPORT_API(mqbool, DeleteToRecycleBin, mqcstr path) {
	mqchar del_path[api::PATH_MAX_SIZE]{ };
	api::lstrcpyW(del_path, path); // 确保path以两个\0结尾
	SHFILEOPSTRUCTW shDelFile{ };
	shDelFile.hwnd = nullptr;
	shDelFile.fFlags = FOF_NOERRORUI | FOF_ALLOWUNDO | FOF_SILENT | FOF_NOCONFIRMATION;
	shDelFile.wFunc = FO_DELETE;
	shDelFile.pFrom = del_path;
	return SHFileOperationW(&shDelFile) == 0;
}

META_EXPORT_API(mqbool, EnumFolder, mqcstr path, EnumUserCallback func, mqmem arg, mqcstr ext) {
	if (auto attr = GetFileAttributesW(path); Exists(attr) && IsFolder(attr)) { // 目录
		struct callback_wrapper {
			EnumUserCallback func;
			mqmem arg;
		}wrapper{ func, arg };
		EnumFolderImpl(path, [ ] (mqmem arg, mqcstr path, WIN32_FIND_DATAW const* data) noexcept {
			mqchar buf[api::PATH_MAX_SIZE];
			api::lstrcpyW(buf, path);
			api::lstrcatW(buf, L"\\");
			api::lstrcatW(buf, data->cFileName);
			auto& wrapper{ *static_cast<callback_wrapper*>(arg) };
			wrapper.func(wrapper.arg, buf);
			}, &wrapper, ext);
		return true;
	}
	return false;
}

META_EXPORT_API(mqbool, EnumFolderDepth, mqcstr path, EnumUserCallback func, mqmem arg) {
	if (auto attr = GetFileAttributesW(path); Exists(attr) && IsFolder(attr)) { // 目录
		struct callback_wrapper {
			EnumUserCallback func;
			mqmem arg;
		}wrapper{ func, arg };
		EnumFolderDepthImpl(path,
			[ ] (mqmem arg, mqcstr path, WIN32_FIND_DATAW const* data) noexcept {
				mqchar buf[api::PATH_MAX_SIZE];
				api::lstrcpyW(buf, path);
				api::lstrcatW(buf, L"\\");
				api::lstrcatW(buf, data->cFileName);
				auto& wrapper{ *static_cast<callback_wrapper*>(arg) };
				wrapper.func(wrapper.arg, buf);
			}, [ ] (mqmem arg, mqcstr path, WIN32_FIND_DATAW const* data, mqbool isStart) noexcept {
				if (isStart) {
					auto& wrapper{ *static_cast<callback_wrapper*>(arg) };
					wrapper.func(wrapper.arg, path);
				}
				}, &wrapper);
		return true;
	}
	return false;
}

META_EXPORT_API(mqbool, CreateFolder, mqcstr path) {
	mqchar tmp[api::PATH_MAX_SIZE]{ }, * p = tmp;
	api::lstrcpyW(tmp, path);
	while (*p++) {
		if (*p == L'\\') {
			*p = L'\0';
			if (GetFileAttributesW(tmp) == INVALID_FILE_ATTRIBUTES) {
				if (!CreateDirectoryW(tmp, nullptr)) {
					return false;
				}
			}
			*p = L'\\';
		}
	}
	if (GetFileAttributesW(tmp) == INVALID_FILE_ATTRIBUTES) {
		if (!CreateDirectoryW(tmp, nullptr)) {
			return false;
		}
	}
	return true;
}

META_EXPORT_API(mqbool, ShortCutTarget, mqcstr path, mqstr target) {
	WIN32_FILE_ATTRIBUTE_DATA attr{ .dwFileAttributes = INVALID_FILE_ATTRIBUTES };
	GetFileAttributesExW(path, GetFileExInfoStandard, &attr);
	if (Exists(attr.dwFileAttributes) && IsFile(attr.dwFileAttributes)) {
		auto size{ freestanding::ui64(attr.nFileSizeLow, attr.nFileSizeHigh) };
		if (size <= sizeof(LINKFILE_HEADER) && size > 8192U) {
			return false;
		}
		mqbyte buf[8192U]{ }, * p{ }, * end = buf + size;
		MasterQian_Storage_Path_Read(path, size, buf);
		auto header{ reinterpret_cast<LINKFILE_HEADER*>(buf) };
		if (header->Tag != 0x4CU || (header->Flags & 2U) == 0U) {
			return false;
		}
		p = buf + sizeof(LINKFILE_HEADER);
		mqui16 size16{ *reinterpret_cast<mqui16*>(p) };
		p += sizeof(mqui16) + size16 + 44U;
		while (*p) ++p;
		if (p >= end) {
			return false;
		}
		mqchara targetPath[api::PATH_MAX_SIZE]{ };
		api::lstrcpyA(targetPath, reinterpret_cast<mqcstra>(p + 1U));
		auto len{ api::MultiByteToWideChar(0U, 0, targetPath, -1, nullptr, 0) };
		api::MultiByteToWideChar(0U, 0, targetPath, -1, target, len);
		return true;
	}
	return false;
}

META_EXPORT_API(mqui32, GetDriversBitmap, mqstr bm) {
	mqui32 value{ GetLogicalDrives() };
	mqui32 count{ };
	for (mqui16 i{ }; i < 26U; ++i) {
		if ((value & 1U) == 1U) {
			bm[count++] = L'A' + i;
		}
		value >>= 1U;
	}
	return count;
}

META_EXPORT_API(void, GetDriverSpaceInfo, mqchar name, mqstr path, mqf64* totalSpace, mqf64* freeSpace) {
	path[0] = name;
	path[1] = L':';
	path[2] = L'\0';
	mqui32 sector, bytes, cluster, all_cluster;
	GetDiskFreeSpaceW(path, &sector, &bytes, &cluster, &all_cluster);
	mqf64 m{ bytes / 1024.0 * sector };
	*totalSpace = all_cluster * m / 1024 / 1024;
	*freeSpace = cluster * m / 1024 / 1024;
}

META_EXPORT_API(mqui64, GetRecycleBinItemCount, mqchar driverName) {
	SHQUERYRBINFO info{ };
	info.cbSize = sizeof(SHQUERYRBINFO);
	mqchar path[api::PATH_MAX_SIZE];
	api::lstrcpyW(path, L"?:\\$Recycle.Bin\\");
	*path = driverName;
	SHQueryRecycleBinW(path, &info);
	return info.i64NumItems;
}

META_EXPORT_API(void, ClearRecycleBin, mqbool tip) {
	SHEmptyRecycleBinW(nullptr, nullptr, tip ? 0 : SHERB_NOCONFIRMATION | SHERB_NOPROGRESSUI | SHERB_NOSOUND);
}

META_EXPORT_API(void, GetStandardPath, PathCode spc, mqstr buf) {
	if (static_cast<mqi32>(spc) >= static_cast<mqi32>(PathCode::Running)) {
		switch (spc) {
		case PathCode::Running: {
			mqui32 len{ GetModuleFileNameW(nullptr, buf, api::PATH_MAX_SIZE) };
			if (len > 0U) {
				mqstr p{ buf };
				while (*p) ++p;
				while (*p != L'\\') --p;
				*p = L'\0';
			}
			break;
		}
		case PathCode::RunningName: {
			GetModuleFileNameW(nullptr, buf, api::PATH_MAX_SIZE);
			break;
		}
		case PathCode::Temp: {
			GetTempPathW(api::PATH_MAX_SIZE, buf);
			break;
		}
		}
	}
	else {
		SHGetSpecialFolderPathW(nullptr, buf, static_cast<mqi32>(spc), false);
	}
}

META_EXPORT_API(void, AutoCurrentFolder, mqcstr path) {
	SetCurrentDirectoryW(path);
}
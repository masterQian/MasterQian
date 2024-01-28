#include "../include/MasterQian.Meta.h"

#include <shlobj_core.h>

#include <stack>
#include <vector>

#define MasterQianModuleName(_) MasterQian_Storage_Path_
META_EXPORT_API_VERSION(20240113ULL)

namespace details {
	constexpr auto UnixDST{ 116444736000000000ULL };

	inline static bool Exists(mqui32 attr) {
		return attr != INVALID_FILE_ATTRIBUTES;
	}

	inline static bool IsFolder(mqui32 attr) {
		return (attr & FILE_ATTRIBUTE_DIRECTORY) != 0U;
	}

	inline static bool IsFile(mqui32 attr) {
		return !IsFolder(attr);
	}

	using EnumFileCallback = void(__stdcall*)(mqmem arg, mqcstr path, WIN32_FIND_DATAW const* data) noexcept;
	using EnumFolderCallback = void(__stdcall*)(mqmem arg, mqcstr path, WIN32_FIND_DATAW const* data, mqbool isStart) noexcept;
	using EnumUserCallback = void(__stdcall*)(mqmem arg, mqcstr path) noexcept;

	static void EnumFolderImpl(mqcstr path, EnumFileCallback func, mqmem arg, mqcstr ext) noexcept {
		mqchar find_path[MAX_PATH]{ };
		(void)lstrcpynW(find_path, path, MAX_PATH - lstrlenW(ext) - 1); // 预留\*.ext空间
		lstrcatW(find_path, L"\\");
		lstrcatW(find_path, ext);
		WIN32_FIND_DATAW data{ };
		mqhandle handle{ FindFirstFileW(find_path, &data) };
		if (handle != INVALID_HANDLE_VALUE) {
			do {
				if (lstrcmpW(data.cFileName, L".") != 0
					&& lstrcmpW(data.cFileName, L"..") != 0) { // 跳过. 和..
					func(arg, path, &data);
				}
			} while (FindNextFileW(handle, &data));
			FindClose(handle);
		}
	}

	static void EnumFolderDepthImpl(mqcstr path, EnumFileCallback file_func, EnumFolderCallback folder_func, mqmem arg) noexcept {
		struct EnumFolderDepthStruct {
			mqhandle handle;
			mqchar path[MAX_PATH];

			EnumFolderDepthStruct(mqcstr value) : handle{ } {
				lstrcpyW(path, value);
			}
		};

		std::stack<EnumFolderDepthStruct, std::vector<EnumFolderDepthStruct>> hStack;
		hStack.emplace(path); // 初始路径

		WIN32_FIND_DATAW data{ }; // 查找数据

		while (!hStack.empty()) {
			auto& topF{ hStack.top() };
			if (topF.handle == nullptr) { // 初次查找
				mqchar find_path[MAX_PATH]{ }; // 查找路径
				(void)lstrcpynW(find_path, topF.path, MAX_PATH - 2); // 预留\*空间
				lstrcatW(find_path, L"\\*");
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
						mqchar subpath[MAX_PATH]; // 子目录路径
						lstrcpyW(subpath, topF.path);
						lstrcatW(subpath, L"\\");
						lstrcatW(subpath, data.cFileName);

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
}

META_EXPORT_API(mqui32, Attribute, mqcstr path) {
	return GetFileAttributesW(path);
}

META_EXPORT_API(mqui64, Size, mqcstr path) {
	WIN32_FILE_ATTRIBUTE_DATA attr{ .dwFileAttributes = INVALID_FILE_ATTRIBUTES };
	GetFileAttributesExW(path, GetFileExInfoStandard, &attr);
	if (details::Exists(attr.dwFileAttributes)) {
		if (details::IsFolder(attr.dwFileAttributes)) { // 目录大小
			mqui64 size{ };
			details::EnumFolderDepthImpl(path, [ ] (mqmem arg, mqcstr path, WIN32_FIND_DATAW const* data) noexcept {
				*static_cast<mqui64*>(arg) += MasterQian::Meta::MakeQWORD(data->nFileSizeLow, data->nFileSizeHigh);
				}, nullptr, &size);
			return size;
		}
		else { // 文件大小
			return MasterQian::Meta::MakeQWORD(attr.nFileSizeLow, attr.nFileSizeHigh);
		}
	}
	return 0ULL;
}

META_EXPORT_API(mqui64, GetTime, mqcstr path, mqui32 type) {
	WIN32_FILE_ATTRIBUTE_DATA attr{ .dwFileAttributes = INVALID_FILE_ATTRIBUTES };
	GetFileAttributesExW(path, GetFileExInfoStandard, &attr);
	mqui64 t{ };
	if (details::Exists(attr.dwFileAttributes)) {
		if (type == 1U) { // 修改时间
			t = MasterQian::Meta::MakeQWORD(attr.ftLastWriteTime.dwLowDateTime, attr.ftLastWriteTime.dwHighDateTime);
		}
		else if (type == 2U) { // 访问时间
			t = MasterQian::Meta::MakeQWORD(attr.ftLastAccessTime.dwLowDateTime, attr.ftLastAccessTime.dwHighDateTime);
		}
		else { // 创建时间
			t = MasterQian::Meta::MakeQWORD(attr.ftCreationTime.dwLowDateTime, attr.ftCreationTime.dwHighDateTime);
		}
	}
	return t - details::UnixDST;
}

META_EXPORT_API(mqbool, SetTime, mqcstr path, mqui32 type, mqui64 t) {
	if (auto attr = GetFileAttributesW(path); details::Exists(attr)) {
		mqdword flag = details::IsFolder(attr) ? 0x02000000U : 0U;
		if (auto hFile{ CreateFileW(path, GENERIC_WRITE, 0,
			nullptr, OPEN_EXISTING, flag, nullptr) };
			hFile != INVALID_HANDLE_VALUE) {
			t += details::UnixDST;
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
			PAGE_READONLY, MasterQian::Meta::HighQWORD(size),
			MasterQian::Meta::LowQWORD(size), nullptr) }) {
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
			PAGE_READWRITE, MasterQian::Meta::HighQWORD(size),
			MasterQian::Meta::LowQWORD(size), nullptr) }) {
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
	mqchar src[MAX_PATH];
	lstrcpyW(src, rootPath);
	lstrcatW(src, L"\\");
	lstrcatW(src, filename);
	auto attr1 = GetFileAttributesW(src), attr2 = GetFileAttributesW(folder_path);
	if (details::Exists(attr1) && details::Exists(attr2) && details::IsFolder(attr2)) {
		mqchar des[MAX_PATH];
		lstrcpyW(des, folder_path);
		lstrcatW(des, L"\\");
		lstrcatW(des, filename);
		if (details::IsFolder(attr1)) { // 目录
			CreateDirectoryW(des, nullptr); // 先创建复制后的根目录
			struct callback_wrapper {
				mqcstr des; // 目标目录
				mqi32 rootLen; // 根路径长度
				mqbool overridable; // 是否覆写
				bool result;
			}wrapper{ des, lstrlenW(src), overridable, true };
			// 遍历原目录
			details::EnumFolderDepthImpl(src,
				[ ] (mqmem arg, mqcstr path, WIN32_FIND_DATAW const* data) noexcept { // 文件回调
					auto wrapper{ static_cast<callback_wrapper*>(arg) };
					mqchar oldFile[MAX_PATH], newFile[MAX_PATH];
					lstrcpyW(oldFile, path);
					lstrcatW(oldFile, L"\\");
					lstrcatW(oldFile, data->cFileName);
					lstrcpyW(newFile, wrapper->des);
					lstrcatW(newFile, path + wrapper->rootLen);
					lstrcatW(newFile, L"\\");
					lstrcatW(newFile, data->cFileName);
					wrapper->result = wrapper->result && CopyFileW(oldFile, newFile, !wrapper->overridable);

				}, [ ] (mqmem arg, mqcstr path, WIN32_FIND_DATAW const* data, mqbool isStart) noexcept { // 目录回调
					if (isStart) { // 进入目录前创建目录
						auto wrapper{ static_cast<callback_wrapper*>(arg) };
						mqchar newFolder[MAX_PATH];
						lstrcpyW(newFolder, wrapper->des);
						lstrcatW(newFolder, path + wrapper->rootLen);
						wrapper->result = wrapper->result && CreateDirectoryW(newFolder, nullptr);
					}
					}, & wrapper);
			return wrapper.result;
		}
		else { // 文件
			return CopyFileW(src, des, !overridable);
		}
	}
	return false;
}

META_EXPORT_API(mqbool, CopyRename, mqcstr src, mqcstr des, mqbool overridable) {
	return CopyFileW(src, des, !overridable);
}

META_EXPORT_API(mqbool, Move, mqcstr rootPath, mqcstr filename, mqcstr folder_path) {
	mqchar src[MAX_PATH];
	lstrcpyW(src, rootPath);
	lstrcatW(src, L"\\");
	lstrcatW(src, filename);
	auto attr1 = GetFileAttributesW(src), attr2 = GetFileAttributesW(folder_path);
	if (details::Exists(attr1) && details::Exists(attr2) && details::IsFolder(attr2)) {
		mqchar des[MAX_PATH];
		lstrcpyW(des, folder_path);
		lstrcatW(des, L"\\");
		lstrcatW(des, filename);
		if (details::IsFolder(attr1)) { // 目录
			if (MoveFileW(src, des)) { // 尝试移动同驱动器下的目录
				return true;
			}
			// 不同驱动器下手动转移
			CreateDirectoryW(des, nullptr); // 先创建移动后的根目录
			struct callback_wrapper {
				mqcstr des; // 目标目录
				mqi32 rootLen; // 根路径长度
				bool result;
			}wrapper{ des, lstrlenW(src), true };
			// 遍历原目录
			details::EnumFolderDepthImpl(src,
				[ ] (mqmem arg, mqcstr path, WIN32_FIND_DATAW const* data) noexcept { // 文件回调
					auto wrapper{ static_cast<callback_wrapper*>(arg) };
					mqchar oldFile[MAX_PATH], newFile[MAX_PATH];
					lstrcpyW(oldFile, path);
					lstrcatW(oldFile, L"\\");
					lstrcatW(oldFile, data->cFileName);
					lstrcpyW(newFile, wrapper->des);
					lstrcatW(newFile, path + wrapper->rootLen);
					lstrcatW(newFile, L"\\");
					lstrcatW(newFile, data->cFileName);
					wrapper->result = wrapper->result && MoveFileW(oldFile, newFile);

				}, [ ] (mqmem arg, mqcstr path, WIN32_FIND_DATAW const* data, mqbool isStart) noexcept { // 目录回调
					if (isStart) { // 进入目录前创建目录
						auto wrapper{ static_cast<callback_wrapper*>(arg) };
						mqchar newFolder[MAX_PATH];
						lstrcpyW(newFolder, wrapper->des);
						lstrcatW(newFolder, path + wrapper->rootLen);
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
	if (auto attr = GetFileAttributesW(path); details::Exists(attr)) {
		if (details::IsFolder(attr)) { // 目录
			bool result{ true };
			details::EnumFolderDepthImpl(path,
				[ ] (mqmem arg, mqcstr path, WIN32_FIND_DATAW const* data) noexcept { // 文件回调
					bool& result{ *static_cast<bool*>(arg) };
					mqchar buf[MAX_PATH];
					lstrcpyW(buf, path);
					lstrcatW(buf, L"\\");
					lstrcatW(buf, data->cFileName);
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
	mqchar del_path[MAX_PATH]{ };
	lstrcpyW(del_path, path); // 确保path以两个\0结尾
	SHFILEOPSTRUCTW shDelFile{ };
	shDelFile.hwnd = nullptr;
	shDelFile.fFlags = FOF_NOERRORUI | FOF_ALLOWUNDO | FOF_SILENT | FOF_NOCONFIRMATION;
	shDelFile.wFunc = FO_DELETE;
	shDelFile.pFrom = del_path;
	return SHFileOperationW(&shDelFile) == 0;
}

META_EXPORT_API(mqbool, EnumFolder, mqcstr path, details::EnumUserCallback func, mqmem arg, mqcstr ext) {
	if (auto attr = GetFileAttributesW(path); details::Exists(attr) && details::IsFolder(attr)) { // 目录
		struct callback_wrapper {
			details::EnumUserCallback func;
			mqmem arg;
		}wrapper{ func, arg };
		details::EnumFolderImpl(path, [ ] (mqmem arg, mqcstr path, WIN32_FIND_DATAW const* data) noexcept {
			mqchar buf[MAX_PATH];
			lstrcpyW(buf, path);
			lstrcatW(buf, L"\\");
			lstrcatW(buf, data->cFileName);
			auto& wrapper{ *static_cast<callback_wrapper*>(arg) };
			wrapper.func(wrapper.arg, buf);
			}, &wrapper, ext);
		return true;
	}
	return false;
}

META_EXPORT_API(mqbool, EnumFolderDepth, mqcstr path, details::EnumUserCallback func, mqmem arg) {
	if (auto attr = GetFileAttributesW(path); details::Exists(attr) && details::IsFolder(attr)) { // 目录
		struct callback_wrapper {
			details::EnumUserCallback func;
			mqmem arg;
		}wrapper{ func, arg };
		details::EnumFolderDepthImpl(path,
			[ ] (mqmem arg, mqcstr path, WIN32_FIND_DATAW const* data) noexcept {
				mqchar buf[MAX_PATH];
				lstrcpyW(buf, path);
				lstrcatW(buf, L"\\");
				lstrcatW(buf, data->cFileName);
				auto& wrapper{ *static_cast<callback_wrapper*>(arg) };
				wrapper.func(wrapper.arg, buf);
			}, [ ] (mqmem arg, mqcstr path, WIN32_FIND_DATAW const* data, mqbool isStart) noexcept {
				if (isStart) {
					auto& wrapper{ *static_cast<callback_wrapper*>(arg) };
					wrapper.func(wrapper.arg, path);
				}
				}, & wrapper);
		return true;
	}
	return false;
}

META_EXPORT_API(mqbool, CreateFolder, mqcstr path) {
	mqchar tmp[MAX_PATH]{ }, * p = tmp;
	lstrcpyW(tmp, path);
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
	bool result{ };
	if (auto hr{ CoInitialize(nullptr) }; SUCCEEDED(hr)) {
		IShellLinkW* psl{ };
		if (hr = CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER,
			IID_IShellLinkW, (LPVOID*)&psl); SUCCEEDED(hr)) {
			IPersistFile* ppf{ };
			if (hr = psl->QueryInterface(IID_IPersistFile, (LPVOID*)&ppf); SUCCEEDED(hr)) {
				if (hr = ppf->Load(path, STGM_READ); SUCCEEDED(hr)) {
					psl->GetPath(target, MAX_PATH, nullptr, SLGP_SHORTPATH);
					result = true;
				}
				ppf->Release();
			}
			psl->Release();
		}
		CoUninitialize();
	}
	return result;
}

META_EXPORT_API(mqui32, GetDriversBitmap, mqstr bm) {
	mqdword value{ GetLogicalDrives() };
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
	ULONG sector, bytes, cluster, all_cluster;
	GetDiskFreeSpaceW(path, &sector, &bytes, &cluster, &all_cluster);
	mqf64 m{ bytes / 1024.0 * sector };
	*totalSpace = all_cluster * m / 1024 / 1024;
	*freeSpace = cluster * m / 1024 / 1024;
}

META_EXPORT_API(mqui64, GetRecycleBinItemCount, mqchar driverName) {
	SHQUERYRBINFO info{ };
	info.cbSize = sizeof(SHQUERYRBINFO);
	mqchar path[MAX_PATH];
	lstrcpyW(path, L"?:\\$Recycle.Bin\\");
	*path = driverName;
	SHQueryRecycleBinW(path, &info);
	return static_cast<mqui64>(info.i64NumItems);
}

META_EXPORT_API(void, ClearRecycleBin, mqbool tip) {
	SHEmptyRecycleBinW(nullptr, nullptr, tip ? 0 : SHERB_NOCONFIRMATION | SHERB_NOPROGRESSUI | SHERB_NOSOUND);
}

META_EXPORT_API(void, GetStandardPath, details::PathCode spc, mqstr buf) {
	if (static_cast<mqi32>(spc) >= static_cast<mqi32>(details::PathCode::Running)) {
		switch (spc) {
		case details::PathCode::Running: {
			mqdword len{ GetModuleFileNameW(nullptr, buf, MAX_PATH) };
			if (len > 0U) {
				mqstr p{ buf };
				while (*p) ++p;
				while (*p != L'\\') --p;
				*p = L'\0';
			}
			break;
		}
		case details::PathCode::RunningName: {
			GetModuleFileNameW(nullptr, buf, MAX_PATH);
			break;
		}
		case details::PathCode::Temp: {
			GetTempPathW(MAX_PATH, buf);
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
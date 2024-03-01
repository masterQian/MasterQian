#include "../include/MasterQian.Meta.h"
import MasterQian.freestanding;
using namespace MasterQian;
#define MasterQianModuleName(name) MasterQian_System_##name
META_EXPORT_API_VERSION(20240301ULL)

constexpr mqui32 MOUSEEVENTF_ABSOLUTE{ 0x8000U };
constexpr mqui32 MOUSEEVENTF_LEFTDOWN{ 0x0002U };
constexpr mqui32 MOUSEEVENTF_LEFTUP{ 0x0004U };
constexpr mqui32 MOUSEEVENTF_RIGHTDOWN{ 0x0008U };
constexpr mqui32 MOUSEEVENTF_RIGHTUP{ 0x0010U };
constexpr mqui32 MOUSEEVENTF_MIDDLEDOWN{ 0x0020U };
constexpr mqui32 MOUSEEVENTF_MIDDLEUP{ 0x0040U };
constexpr mqui32 MOUSEEVENTF_WHEEL{ 0x0800U };
constexpr mqi32 WHEEL_DELTA{ 120 };
constexpr mqui32 KEYEVENTF_KEYUP{ 0x0002U };
constexpr mqui32 GMEM_MOVEABLE{ 0x0002U };
constexpr mqui32 GMEM_ZEROINIT{ 0x0040U };
constexpr mqui32 CF_HDROP{ 15U };
constexpr mqui32 EVENT_ALL_ACCESS{ 0x000F0000U | 0x00100000U | 0x3U };
constexpr mqui32 SEE_MASK_NOASYNC{ 0x00000100U };
constexpr mqui32 SEE_MASK_UNICODE{ 0x00004000U };
constexpr mqui32 SEE_MASK_NOCLOSEPROCESS{ 0x00000040U };
constexpr mqui32 WAIT_TIMEOUT{ 258U };
mqhandle HWND_TOPMOST{ reinterpret_cast<mqhandle>(-1) };
constexpr mqcstr TASKBAR_CLASS{ L"Shell_traywnd" };
constexpr mqui32 SWP_SHOWWINDOW{ 0x0040U };
constexpr mqui32 SWP_HIDEWINDOW{ 0x0080U };

using LPFILETIME = struct FILETIME {
	mqui32 dwLowDateTime;
	mqui32 dwHighDateTime;
}*;

using LPAPPBARDATA = struct APPBARDATA {
	mqui32 cbSize;
	mqhandle hWnd;
	mqui32 uCallbackMessage;
	mqui32 uEdge;
	mqrange rc;
	mqui64 lParam;
}*;

using LPSECURITY_ATTRIBUTES = struct SECURITY_ATTRIBUTES {
	mqui32 nLength;
	mqmem lpSecurityDescriptor;
	mqbool bInheritHandle;
}*;

using LPSHELLEXECUTEINFOW = struct SHELLEXECUTEINFOW {
	mqui32 cbSize;
	mqui32 fMask;
	mqhandle hwnd;
	mqcstr lpVerb;
	mqcstr lpFile;
	mqcstr lpParameters;
	mqcstr lpDirectory;
	mqi32 nShow;
	mqhandle hInstApp;
	mqmem lpIDList;
	mqcstr lpClass;
	mqhandle hkeyClass;
	mqui32 dwHotKey;
	union {
		mqhandle hIcon;
		mqhandle hMonitor;
	};
	mqhandle hProcess;
}*;

using HKEY = mqhandle;
using LPHKEY = HKEY*;
using LSTATUS = mqui32;

META_WINAPI(mqbool, GetCursorPos, mqpoint*);
META_WINAPI(mqbool, SetCursorPos, mqui32, mqui32);
META_WINAPI(void, mouse_event, mqui32, mqui32, mqui32, mqui32, mqui64);
META_WINAPI(void, keybd_event, mqbyte, mqbyte, mqui32, mqui64);
META_WINAPI(mqbool, OpenClipboard, mqhandle);
META_WINAPI(mqbool, EmptyClipboard);
META_WINAPI(mqbool, CloseClipboard);
META_WINAPI(mqhandle, GetClipboardData, mqui32);
META_WINAPI(mqhandle, SetClipboardData, mqui32, mqhandle);
META_WINAPI(mqui64, GlobalSize, mqhandle);
META_WINAPI(mqmem, GlobalLock, mqhandle);
META_WINAPI(mqbool, GlobalUnlock, mqhandle);
META_WINAPI(mqhandle, GlobalAlloc, mqui32, mqui64);
META_WINAPI(mqhandle, GlobalFree, mqhandle);
META_WINAPI(mqhandle, GlobalFree, mqhandle);
META_WINAPI(mqbool, GetClientRect, mqhandle, mqrange*);
META_WINAPI(mqhandle, GetDesktopWindow);
META_WINAPI(mqui64, SHAppBarMessage, mqui32, LPAPPBARDATA);
META_WINAPI(mqbool, GetUserNameW, mqstr, mqui32*);
META_WINAPI(mqbool, GetComputerNameW, mqstr, mqui32*);
META_WINAPI(mqhandle, FindResourceW, mqhandle, mqcstr, mqcstr);
META_WINAPI(mqhandle, LoadResource, mqhandle, mqhandle);
META_WINAPI(mqmem, LockResource, mqhandle);
META_WINAPI(mqui32, SizeofResource, mqhandle, mqhandle);
META_WINAPI(mqui32, GetModuleFileNameW, mqhandle, mqstr, mqui32);
META_WINAPI(mqhandle, OpenEventW, mqui32, mqbool, mqcstr);
META_WINAPI(mqhandle, CreateEventW, LPSECURITY_ATTRIBUTES, mqbool, mqbool, mqcstr);
META_WINAPI(mqbool, ShellExecuteExW, LPSHELLEXECUTEINFOW);
META_WINAPI(mqui32, WaitForSingleObject, mqhandle, mqui32);
META_WINAPI(mqbool, TerminateProcess, mqhandle, mqui32);
META_WINAPI(mqbool, GetExitCodeProcess, mqhandle, mqui32*);
META_WINAPI(mqbool, CloseHandle, mqhandle);
META_WINAPI(mqhandle, FindWindowW, mqcstr, mqcstr);
META_WINAPI(mqbool, SetWindowPos, mqhandle, mqhandle, mqi32, mqi32, mqi32, mqi32, mqui32);
META_WINAPI(LSTATUS, RegOpenKeyW, HKEY, mqcstr, LPHKEY);
META_WINAPI(LSTATUS, RegOpenCurrentUser, mqui32, LPHKEY);
META_WINAPI(LSTATUS, RegCloseKey, HKEY);
META_WINAPI(LSTATUS, RegCreateKeyExW, HKEY, mqcstr, mqui32, mqstr, mqui32, mqui32, LPSECURITY_ATTRIBUTES, LPHKEY, mqui32*);
META_WINAPI(LSTATUS, RegDeleteKeyExW, HKEY, mqcstr, mqui32, mqui32);
META_WINAPI(LSTATUS, RegEnumKeyW, HKEY, mqui32, mqstr, mqui32);
META_WINAPI(LSTATUS, RegQueryValueExW, HKEY, mqcstr, mqui32*, mqui32*, mqmem, mqui32*);
META_WINAPI(LSTATUS, RegSetValueExW, HKEY, mqcstr, mqui32, mqui32, mqcmem, mqui32);
META_WINAPI(LSTATUS, RegDeleteValueW, HKEY, mqcstr);
META_WINAPI(LSTATUS, RegQueryInfoKeyW, HKEY, mqstr, mqui32*, mqui32*, mqui32*, mqui32*, mqui32*, mqui32*, mqui32*, mqui32*, mqui32*, LPFILETIME);
META_WINAPI(LSTATUS, RegEnumValueW, HKEY, mqui32, mqstr, mqui32*, mqui32*, mqui32*, mqmem, mqui32*);
META_WINAPI(mqui64, SendMessageTimeoutW, mqhandle, mqui32, mqui64, mqui64, mqui32, mqui32, mqui64*);

#pragma comment(linker,"/alternatename:__imp_?GetCursorPos@@YAHPEAUmqpoint@@@Z=__imp_GetCursorPos")
#pragma comment(linker,"/alternatename:__imp_?SetCursorPos@@YAHII@Z=__imp_SetCursorPos")
#pragma comment(linker,"/alternatename:__imp_?mouse_event@@YAXIIII_K@Z=__imp_mouse_event")
#pragma comment(linker,"/alternatename:__imp_?keybd_event@@YAXEEI_K@Z=__imp_keybd_event")
#pragma comment(linker,"/alternatename:__imp_?EmptyClipboard@@YAHXZ=__imp_EmptyClipboard")
#pragma comment(linker,"/alternatename:__imp_?CloseClipboard@@YAHXZ=__imp_CloseClipboard")
#pragma comment(linker,"/alternatename:__imp_?OpenClipboard@@YAHPEAX@Z=__imp_OpenClipboard")
#pragma comment(linker,"/alternatename:__imp_?SetClipboardData@@YAPEAXIPEAX@Z=__imp_SetClipboardData")
#pragma comment(linker,"/alternatename:__imp_?GetClipboardData@@YAPEAXI@Z=__imp_GetClipboardData")
#pragma comment(linker,"/alternatename:__imp_?GlobalSize@@YA_KPEAX@Z=__imp_GlobalSize")
#pragma comment(linker,"/alternatename:__imp_?GlobalAlloc@@YAPEAXI_K@Z=__imp_GlobalAlloc")
#pragma comment(linker,"/alternatename:__imp_?GlobalFree@@YAPEAXPEAX@Z=__imp_GlobalFree")
#pragma comment(linker,"/alternatename:__imp_?GlobalLock@@YAPEAXPEAX@Z=__imp_GlobalLock")
#pragma comment(linker,"/alternatename:__imp_?GlobalUnlock@@YAHPEAX@Z=__imp_GlobalUnlock")
#pragma comment(linker,"/alternatename:__imp_?GetDesktopWindow@@YAPEAXXZ=__imp_GetDesktopWindow")
#pragma comment(linker,"/alternatename:__imp_?GetClientRect@@YAHPEAXPEAUmqrange@@@Z=__imp_GetClientRect")
#pragma comment(linker,"/alternatename:__imp_?GetUserNameW@@YAHPEA_WPEAI@Z=__imp_GetUserNameW")
#pragma comment(linker,"/alternatename:__imp_?GetComputerNameW@@YAHPEA_WPEAI@Z=__imp_GetComputerNameW")
#pragma comment(linker,"/alternatename:__imp_?SHAppBarMessage@@YA_KIPEAUAPPBARDATA@@@Z=__imp_SHAppBarMessage")
#pragma comment(linker,"/alternatename:__imp_?SizeofResource@@YAIPEAX0@Z=__imp_SizeofResource")
#pragma comment(linker,"/alternatename:__imp_?TerminateProcess@@YAHPEAXI@Z=__imp_TerminateProcess")
#pragma comment(linker,"/alternatename:__imp_?GetModuleFileNameW@@YAIPEAXPEA_WI@Z=__imp_GetModuleFileNameW")
#pragma comment(linker,"/alternatename:__imp_?WaitForSingleObject@@YAIPEAXI@Z=__imp_WaitForSingleObject")
#pragma comment(linker,"/alternatename:__imp_?OpenEventW@@YAPEAXIHPEB_W@Z=__imp_OpenEventW")
#pragma comment(linker,"/alternatename:__imp_?CreateEventW@@YAPEAXPEAUSECURITY_ATTRIBUTES@@HHPEB_W@Z=__imp_CreateEventW")
#pragma comment(linker,"/alternatename:__imp_?LockResource@@YAPEAXPEAX@Z=__imp_LockResource")
#pragma comment(linker,"/alternatename:__imp_?CloseHandle@@YAHPEAX@Z=__imp_CloseHandle")
#pragma comment(linker,"/alternatename:__imp_?LoadResource@@YAPEAXPEAX0@Z=__imp_LoadResource")
#pragma comment(linker,"/alternatename:__imp_?FindResourceW@@YAPEAXPEAXPEB_W1@Z=__imp_FindResourceW")
#pragma comment(linker,"/alternatename:__imp_?ShellExecuteExW@@YAHPEAUSHELLEXECUTEINFOW@@@Z=__imp_ShellExecuteExW")
#pragma comment(linker,"/alternatename:__imp_?GetExitCodeProcess@@YAHPEAXPEAI@Z=__imp_GetExitCodeProcess")
#pragma comment(linker,"/alternatename:__imp_?FindWindowW@@YAPEAXPEB_W0@Z=__imp_FindWindowW")
#pragma comment(linker,"/alternatename:__imp_?SetWindowPos@@YAHPEAX0HHHHI@Z=__imp_SetWindowPos")
#pragma comment(linker,"/alternatename:__imp_?RegQueryValueExW@@YAIPEAXPEB_WPEAI202@Z=__imp_RegQueryValueExW")
#pragma comment(linker,"/alternatename:__imp_?RegEnumValueW@@YAIPEAXIPEA_WPEAI2202@Z=__imp_RegEnumValueW")
#pragma comment(linker,"/alternatename:__imp_?RegOpenKeyW@@YAIPEAXPEB_WPEAPEAX@Z=__imp_RegOpenKeyW")
#pragma comment(linker,"/alternatename:__imp_?RegDeleteValueW@@YAIPEAXPEB_W@Z=__imp_RegDeleteValueW")
#pragma comment(linker,"/alternatename:__imp_?RegSetValueExW@@YAIPEAXPEB_WIIPEBXI@Z=__imp_RegSetValueExW")
#pragma comment(linker,"/alternatename:__imp_?RegCreateKeyExW@@YAIPEAXPEB_WIPEA_WIIPEAUSECURITY_ATTRIBUTES@@PEAPEAXPEAI@Z=__imp_RegCreateKeyExW")
#pragma comment(linker,"/alternatename:__imp_?RegQueryInfoKeyW@@YAIPEAXPEA_WPEAI22222222PEAUFILETIME@@@Z=__imp_RegQueryInfoKeyW")
#pragma comment(linker,"/alternatename:__imp_?RegOpenCurrentUser@@YAIIPEAPEAX@Z=__imp_RegOpenCurrentUser")
#pragma comment(linker,"/alternatename:__imp_?RegEnumKeyW@@YAIPEAXIPEA_WI@Z=__imp_RegEnumKeyW")
#pragma comment(linker,"/alternatename:__imp_?RegDeleteKeyExW@@YAIPEAXPEB_WII@Z=__imp_RegDeleteKeyExW")
#pragma comment(linker,"/alternatename:__imp_?RegCloseKey@@YAIPEAX@Z=__imp_RegCloseKey")
#pragma comment(linker,"/alternatename:__imp_?SendMessageTimeoutW@@YA_KPEAXI_K1IIPEA_K@Z=__imp_SendMessageTimeoutW")

/*    AutoDevice    */

META_EXPORT_API(mqpoint, GetMousePos) {
	mqpoint point{ };
	GetCursorPos(&point);
	return point;
}

META_EXPORT_API(void, SetMousePos, mqpoint point) {
	SetCursorPos(point.x, point.y);
}

META_EXPORT_API(void, MouseClick, mqpoint point, mqui32 mode) {
	SetCursorPos(point.x, point.y);
	switch (mode) {
	case 0: {
		mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
		break;
	}
	case 1: {
		mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
		mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
		break;
	}
	case 2: {
		mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MIDDLEDOWN | MOUSEEVENTF_MIDDLEUP, 0, 0, 0, 0);
		break;
	}
	case 3: {
		mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MIDDLEDOWN | MOUSEEVENTF_MIDDLEUP, 0, 0, 0, 0);
		mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MIDDLEDOWN | MOUSEEVENTF_MIDDLEUP, 0, 0, 0, 0);
		break;
	}
	case 4: {
		mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
		break;
	}
	case 5: {
		mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
		mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
		break;
	}
	}
}

META_EXPORT_API(void, MouseWheel, mqi32 len) {
	mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_WHEEL, 0, 0, -len * WHEEL_DELTA, 0);
}

META_EXPORT_API(void, KeyboardClick, mqui32 key1, mqui32 key2, mqui32 key3) {
	keybd_event(key1, 0, 0, 0);
	if (key2) {
		keybd_event(key2, 0, 0, 0);
	}
	if (key3) {
		keybd_event(key3, 0, 0, 0);
		keybd_event(key3, 0, KEYEVENTF_KEYUP, 0);
	}
	if (key2) {
		keybd_event(key2, 0, KEYEVENTF_KEYUP, 0);
	}
	keybd_event(key1, 0, KEYEVENTF_KEYUP, 0);
}

/*    Clipborad    */

META_EXPORT_API(void, ClearClipboard) {
	if (OpenClipboard(nullptr)) {
		EmptyClipboard();
		CloseClipboard();
	}
}

META_EXPORT_API(mqhandle, GetClipboardDataSize, mqui32 fm, mqui64* pLen) {
	mqhandle hClip{ };
	if (OpenClipboard(nullptr)) {
		hClip = GetClipboardData(fm);
		if (hClip) {
			*pLen = GlobalSize(hClip);
		}
		else {
			CloseClipboard();
		}
	}
	return hClip;
}

META_EXPORT_API(mqbool, GetClipboardData, mqhandle hClip, mqmem mem, mqui64 len) {
	bool result{ };
	if (auto data{ GlobalLock(hClip) }) {
		freestanding::copy(mem, data, len);
		GlobalUnlock(mem);
		result = true;
	}
	CloseClipboard();
	return result;
}

META_EXPORT_API(mqbool, SetClipboardData, mqui32 fm, mqcmem data, mqui64 len) {
	bool result{ };
	if (OpenClipboard(nullptr)) {
		EmptyClipboard();
		if (mqhandle hClip{ GlobalAlloc(GMEM_MOVEABLE, len) }) {
			if (auto mem{ GlobalLock(hClip) }) {
				freestanding::copy(mem, data, len);
				GlobalUnlock(hClip);
				SetClipboardData(fm, hClip);
				result = true;
			}
			else {
				GlobalFree(hClip);
			}
		}
		CloseClipboard();
	}
	return result;
}

META_EXPORT_API(mqbool, SetClipboardFiles, mqcstr filesData, mqui64 len) {
	struct ClipBoardFiles {
		mqui32 pFiles;
		mqpoint pt;
		mqbool fNC;
		mqbool fWide;
	};

	bool result{ };
	if (OpenClipboard(nullptr)) {
		EmptyClipboard();
		if (auto hClip{ GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, sizeof(ClipBoardFiles) + len) }) {
			if (auto cbf{ static_cast<ClipBoardFiles*>(GlobalLock(hClip)) }) {
				cbf->pFiles = sizeof(ClipBoardFiles);
				cbf->fWide = true;
				freestanding::copy(cbf + 1U, filesData, len);
				GlobalUnlock(hClip);
				SetClipboardData(CF_HDROP, hClip);
				result = true;
			}
			else {
				GlobalFree(hClip);
			}
		}
		CloseClipboard();
	}
	return result;
}


/*    Info    */

META_EXPORT_API(mqui32, GetScreenWidth) {
	static mqinit<mqui32> width{ [ ]() noexcept {
		mqrange rect;
		GetClientRect(GetDesktopWindow(), &rect);
		return rect.right - rect.left;
		} };
	return width;
}

META_EXPORT_API(mqui32, GetScreenHeight) {
	static mqinit<mqui32> height{ [ ]() noexcept {
		mqrange rect;
		GetClientRect(GetDesktopWindow(), &rect);
		return rect.bottom - rect.top;
		} };
	return height;
}

META_EXPORT_API(mqui32, GetTaskBarHeight) {
	static mqinit<mqui32> taskbar_height{ [ ]()noexcept {
		APPBARDATA data{ };
		data.cbSize = sizeof(APPBARDATA);
		SHAppBarMessage(5U, &data);
		if (data.uEdge == 1U || data.uEdge == 3U) {
			return data.rc.bottom - data.rc.top;
		}
		else {
			return data.rc.right - data.rc.left;
		}
		} };
	return taskbar_height;
}

META_EXPORT_API(void, GetCurrentUserName, mqstr name) {
	mqui32 count{ api::PATH_MAX_SIZE };
	GetUserNameW(name, &count);
}

META_EXPORT_API(void, GetCurrentComputerName, mqstr name) {
	mqui32 count{ api::PATH_MAX_SIZE };
	GetComputerNameW(name, &count);
}


/*    Process    */

META_EXPORT_API(mqcmem, GetResource, mqi32 id, mqcstr type, mqui32* size) {
	if (auto hSrc{ FindResourceW(nullptr,
		reinterpret_cast<mqcstr>(static_cast<mqui64>(static_cast<mqui16>(id))), type) }) {
		if (auto hGlobal{ LoadResource(nullptr, hSrc) }) {
			if (auto mem{ LockResource(hGlobal) }) {
				if (auto dwSize{ SizeofResource(nullptr, hSrc) }) {
					*size = dwSize;
					return mem;
				}
			}
		}
	}
	*size = 0U;
	return nullptr;
}

META_EXPORT_API(mqbool, SingleProcessLock) {
	mqchar buf[api::PATH_MAX_SIZE]{ };
	GetModuleFileNameW(nullptr, buf, api::PATH_MAX_SIZE);
	for (mqstr p = buf; *p; ++p) {
		if (*p >= 127U || *p == L'\\') {
			*p = L'0';
		}
	}
	if (OpenEventW(EVENT_ALL_ACCESS, false, buf)) {
		return false;
	}
	else {
		CreateEventW(nullptr, false, false, buf);
		return true;
	}
}

META_EXPORT_API(mqui32, Execute, mqcstr path, mqcstr arg, mqbool isWaiting, mqbool isAdmin) {
	SHELLEXECUTEINFOW info{ };
	info.cbSize = sizeof(SHELLEXECUTEINFOW);
	info.lpFile = path;
	info.lpParameters = arg;
	info.lpVerb = isAdmin ? L"runas" : nullptr;
	info.nShow = 10;
	info.fMask = SEE_MASK_NOASYNC | SEE_MASK_UNICODE;
	if (isWaiting) info.fMask |= SEE_MASK_NOCLOSEPROCESS;
	mqchar workpath[api::PATH_MAX_SIZE]{ }, * p = workpath;
	api::lstrcpyW(workpath, path);
	while (*p) ++p;
	while (p != workpath && *p != L'\\') --p;
	*p = L'\0';
	info.lpDirectory = workpath;
	mqui32 exitCode{ };
	if (ShellExecuteExW(&info)) {
		if (info.hProcess) {
			if (isWaiting) {
				if (WaitForSingleObject(info.hProcess, static_cast<mqui32>(-1)) == WAIT_TIMEOUT) {
					exitCode = static_cast<mqui32>(-1);
					TerminateProcess(info.hProcess, exitCode);
				}
				else {
					GetExitCodeProcess(info.hProcess, &exitCode);
				}
			}
			CloseHandle(info.hProcess);
		}
	}
	return exitCode;
}


/*    Window    */

META_EXPORT_API(void, ShowTaskBar, mqbool status) {
	SetWindowPos(FindWindowW(TASKBAR_CLASS, nullptr), HWND_TOPMOST, 0, 0, 0, 0, status ? SWP_SHOWWINDOW : SWP_HIDEWINDOW);
}


/*    Reg    */

inline static constexpr mqui32 GetRegSAM(mqbool wow64) noexcept {
	return wow64 ? 0xF013FU : 0xF023FU;
}

META_EXPORT_API(mqbool, RegOpen, LPHKEY pKey, mqbool wow64, HKEY base, mqcstr route) {
	if (base == reinterpret_cast<HKEY>(0x80000001ULL)) {
		HKEY tmp;
		if (RegOpenCurrentUser(GetRegSAM(wow64), &tmp) == 0U) {
			mqbool result{ RegOpenKeyW(tmp, route, pKey) == 0U };
			RegCloseKey(tmp);
			if (result) {
				return true;
			}
			else {
				*pKey = nullptr;
			}
		}
	}
	else {
		if (RegOpenKeyW(base, route, pKey) == 0U) {
			return true;
		}
		else {
			*pKey = nullptr;
		}
	}
	return false;
}

META_EXPORT_API(mqbool, RegClose, LPHKEY pKey) {
	mqbool result{ *pKey ? RegCloseKey(*pKey) == 0 : false };
	*pKey = nullptr;
	return result;
}

META_EXPORT_API(mqbool, RegCreateItem, HKEY hKey, mqbool wow64, mqcstr route, LPHKEY ret) {
	HKEY tmp{ };
	if (RegCreateKeyExW(hKey, route, 0U, nullptr, 0U, GetRegSAM(wow64), nullptr, &tmp, nullptr) == 0U) {
		if (ret) {
			if (*ret) {
				RegCloseKey(*ret);
			}
			*ret = tmp;
		}
		else {
			RegCloseKey(tmp);
		}
		return true;
	}
	return false;
}

META_EXPORT_API(mqbool, RegDeleteItem, HKEY hKey, mqbool wow64, mqcstr route) {
	return RegDeleteKeyExW(hKey, route, GetRegSAM(wow64), 0U) == 0U;
}

META_EXPORT_API(mqbool, RegEnumItem, HKEY hKey, mqui32 index, mqstr route) {
	return RegEnumKeyW(hKey, index, route, api::PATH_MAX_SIZE) == 0U;
}

META_EXPORT_API(mqbool, RegHasKey, HKEY hKey, mqcstr route) {
	return RegQueryValueExW(hKey, route, nullptr, nullptr, nullptr, nullptr) == 0U;
}

META_EXPORT_API(mqbool, RegGetValueTypeAndSize, HKEY hKey, mqcstr route, mqui32* type, mqui32* size) {
	return RegQueryValueExW(hKey, route, nullptr, type, nullptr, size) == 0U;
}

META_EXPORT_API(mqbool, RegGetValue, HKEY hKey, mqcstr route, mqbytes mem, mqui32* size) {
	return RegQueryValueExW(hKey, route, nullptr, nullptr, mem, size) == 0U;
}

META_EXPORT_API(mqbool, RegSetValue, HKEY hKey, mqcstr route, mqui32 type, mqcmem mem, mqui32 size) {
	return RegSetValueExW(hKey, route, 0U, type, mem, size) == 0U;
}

META_EXPORT_API(mqbool, RegDeleteKey, HKEY hKey, mqcstr route) {
	return RegDeleteValueW(hKey, route) == 0U;
}

META_EXPORT_API(mqbool, RegGetKeyValueMaxSize, HKEY hKey, mqui32* maxKeySize, mqui32* maxValueSize) {
	return RegQueryInfoKeyW(hKey, nullptr, nullptr, nullptr, nullptr, nullptr,
		nullptr, nullptr, maxKeySize, maxValueSize, nullptr, nullptr) == 0U;
}

META_EXPORT_API(mqui32, RegEnumKey, HKEY hKey, mqui32 index, mqui32 maxKeySize, mqstr route, mqui32* type) {
	return RegEnumValueW(hKey, index, route, &maxKeySize, nullptr, type, nullptr, nullptr) == 0U ? maxKeySize : 0U;
}

META_EXPORT_API(void, FlushEnvironment) {
	mqui64 dwReturnValue{ };
	SendMessageTimeoutW(reinterpret_cast<mqhandle>(0xFFFFU), 0x001AU, 0ULL, 
		reinterpret_cast<mqui64>(L"Environment"), 0x0002U, 5000U, &dwReturnValue);
}
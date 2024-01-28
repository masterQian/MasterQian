#include "../include/MasterQian.Meta.h"

#define _AMD64_
#include <windef.h>
#include <libloaderapi.h>
#include <synchapi.h>
#include <processthreadsapi.h>
#include <shellapi.h>
#include <handleapi.h>

#define MasterQianModuleName(_) MasterQian_System_Process_
META_EXPORT_API_VERSION(20231229ULL)

META_EXPORT_API(mqcmem, GetResource, mqi32 id, mqcstr type, mqui32* size) {
	if (auto hSrc{ FindResourceW(nullptr, (LPWSTR)(ULONG_PTR)(WORD)id, type) }) {
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
	mqchar buf[MAX_PATH]{ };
	GetModuleFileNameW(nullptr, buf, MAX_PATH);
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
	mqchar workpath[MAX_PATH]{ }, *p = workpath;
	MasterQian::Meta::details::lstrcpyW(workpath, path);
	while (*p) ++p;
	while (p != workpath && *p != L'\\') --p;
	*p = L'\0';
	info.lpDirectory = workpath;
	mqdword exitCode{ };
	if (ShellExecuteExW(&info)) {
		if (info.hProcess) {
			if (isWaiting) {
				if (WaitForSingleObject(info.hProcess, -1) == 258UL) { // WAIT_TIMEOUT
					exitCode = -1;
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
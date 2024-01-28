#include "../include/MasterQian.Meta.h"

#define _AMD64_
#include <windef.h>
#include <winbase.h>
#include <winuser.h>

#define MasterQianModuleName(_) MasterQian_System_Clipboard_
META_EXPORT_API_VERSION(20231229ULL)

META_EXPORT_API(void, Clear) {
	if (OpenClipboard(nullptr)) {
		EmptyClipboard();
		CloseClipboard();
	}
}

META_EXPORT_API(mqhandle, GetDataSize, mqui32 fm, mqui64* pLen) {
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

META_EXPORT_API(mqbool, GetData, mqhandle hClip, mqmem mem, mqui64 len) {
	bool result{ };
	if (auto data = GlobalLock(hClip)) {
		memcpy(mem, data, len);
		GlobalUnlock(mem);
		result = true;
	}
	CloseClipboard();
	return result;
}

META_EXPORT_API(mqbool, SetData, mqui32 fm, mqcmem data, mqui64 len) {
	bool result{ };
	if (OpenClipboard(nullptr)) {
		EmptyClipboard();
		if (mqhandle hClip{ GlobalAlloc(GMEM_MOVEABLE, len) }) {
			if (auto mem{ GlobalLock(hClip) }) {
				memcpy(mem, data, len);
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

struct ClipBoardFiles {
	mqdword pFiles;
	struct { mqdword x, y; } pt;
	mqbool fNC;
	mqbool fWide;
};

META_EXPORT_API(mqbool, SetFiles, mqcstr filesData, mqui64 len) {
	bool result{ };
	if (OpenClipboard(nullptr)) {
		EmptyClipboard();
		if (auto hClip{ GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, sizeof(ClipBoardFiles) + len) }) {
			if (auto cbf{ static_cast<ClipBoardFiles*>(GlobalLock(hClip)) }) {
				cbf->pFiles = sizeof(ClipBoardFiles);
				cbf->fWide = true;
				memcpy(cbf + 1, filesData, len);
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
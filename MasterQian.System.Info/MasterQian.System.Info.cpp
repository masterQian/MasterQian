#include "../include/MasterQian.Meta.h"

#define _AMD64_
#include <windef.h>
#include <winuser.h>
#include <winbase.h>
#include <shellapi.h>

#define MasterQianModuleName(_) MasterQian_System_Info_
META_EXPORT_API_VERSION(20240102ULL)

META_EXPORT_API(mqui32, GetScreenWidth) {
	static bool initialize{ };
	static mqui32 value{ };
	if (!initialize) {
		initialize = true;
		RECT rect;
		GetClientRect(GetDesktopWindow(), &rect);
		value = rect.right - rect.left;
	}
	return value;
}

META_EXPORT_API(mqui32, GetScreenHeight) {
	static bool initialize{ };
	static mqui32 value{ };
	if (!initialize) {
		initialize = true;
		RECT rect;
		GetClientRect(GetDesktopWindow(), &rect);
		value = rect.bottom - rect.top;
	}
	return value;
}

META_EXPORT_API(mqui32, GetTaskBarHeight) {
	static bool initialize{ };
	static mqui32 value{ };
	if (!initialize) {
		initialize = true;
		APPBARDATA data{ };
		data.cbSize = sizeof(APPBARDATA);
		SHAppBarMessage(5, &data);
		if (data.uEdge == 1 || data.uEdge == 3) value = data.rc.bottom - data.rc.top;
		else value = data.rc.right - data.rc.left;
	}
	return value;
}

META_EXPORT_API(void, GetCurrentUserName, mqstr name) {
	mqdword count{ MAX_PATH };
	GetUserNameW(name, &count);
}
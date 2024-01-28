#include "../include/MasterQian.Meta.h"

#define _AMD64_
#include <windef.h>
#include <winuser.h>

#define MasterQianModuleName(_) MasterQian_System_Window_
META_EXPORT_API_VERSION(20231229ULL)

META_EXPORT_API(void, ShowTaskBar, mqbool status) {
	SetWindowPos(FindWindowW(L"Shell_traywnd", nullptr), HWND_TOPMOST,
		0, 0, 0, 0, status ? SWP_SHOWWINDOW : SWP_HIDEWINDOW);
}
#include "../include/MasterQian.Meta.h"
#include <ShlObj_core.h>
import MasterQian.freestanding;
using namespace MasterQian;
#define MasterQianModuleName(name) MasterQian_System_##name

META_EXPORT_API(void, SetTaskBarProgress, HWND hwnd, TBPFLAG type, mqui32 value) {
	if (CoInitialize(nullptr) >= 0) {
		ITaskbarList3* tbl{ };
		CoCreateInstance(CLSID_TaskbarList, nullptr, CLSCTX_ALL,
			__uuidof(ITaskbarList3), reinterpret_cast<mqmem*>(&tbl));
		if (tbl) {
			tbl->SetProgressState(hwnd, type);
			if (type != TBPF_NOPROGRESS && type != TBPF_INDETERMINATE) {
				tbl->SetProgressValue(hwnd, value, 100ULL);
			}
			tbl->Release();
		}
		CoUninitialize();
	}
}
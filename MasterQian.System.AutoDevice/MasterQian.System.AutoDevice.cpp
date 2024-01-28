#include "../include/MasterQian.Meta.h"

#define _AMD64_
#include <windef.h>
#include <winuser.h>

#define MasterQianModuleName(_) MasterQian_System_AutoDevice_
META_EXPORT_API_VERSION(20231229ULL)

META_EXPORT_API(void, GetMousePos, mqui32* x, mqui32* y) {
	POINT point{ };
	if (GetCursorPos(&point)) {
		*x = point.x;
		*y = point.y;
	}
	else {
		*x = *y = 0U;
	}
}

META_EXPORT_API(void, MouseMove, mqui32 x, mqui32 y) {
	SetCursorPos(static_cast<mqi32>(x), static_cast<mqi32>(y));
}

META_EXPORT_API(void, MouseClick, mqui32 x, mqui32 y, mqui32 mode) {
	SetCursorPos(static_cast<mqi32>(x), static_cast<mqi32>(y));
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
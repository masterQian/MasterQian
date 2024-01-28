#pragma once

#include "MasterQian.Meta.h"

#define MasterQianModuleName(_) MasterQian_System_AutoDevice_
#define MasterQianModuleNameString "MasterQian_System_AutoDevice_"
#ifdef _DEBUG
#define MasterQianLibString "MasterQian.System.AutoDevice.Debug.dll"
#else
#define MasterQianLibString "MasterQian.System.AutoDevice.dll"
#endif
#define MasterQianModuleVersion 20231229ULL
#pragma message("—————————— Please copy [" MasterQianLibString "] into your program folder ——————————")

namespace MasterQian::System::AutoDevice {
	/// <summary>
	/// 鼠标位置
	/// </summary>
	struct MousePos {
		mqui32 x;
		mqui32 y;
	};

	/// <summary>
	/// 点击模式
	/// </summary>
	enum class MouseClickMode : mqenum {
		// 左键单击
		L,
		// 左键双击
		LD,
		// 中键单击
		M,
		// 中键双击
		MD,
		// 右键单击
		R,
		// 右键双击
		RD,
	};

	// 键代码
	enum class KeyCode : mqenum {
		KEY_None = 0U,
		KEY_Backspace = 8U, KEY_Tab,
		KEY_Enter = 13U,
		KEY_Shift = 16U, KEY_Ctrl, KEY_Alt, KEY_Pause, KEY_CapLock,
		KEY_Esc = 27U,
		KEY_Space = 32U, KEY_Pageup, KEY_Pagedown, KEY_End, KEY_Home, KEY_Left, KEY_Up, KEY_Right, KEY_Down,
		KEY_Insert = 45U, KEY_Delete,
		KEY_0 = 48U, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9,
		KEY_A = 65U, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_G, KEY_H, KEY_I, KEY_J, KEY_K, KEY_L, KEY_M,
		KEY_N, KEY_O, KEY_P, KEY_Q, KEY_R, KEY_S, KEY_T, KEY_U, KEY_V, KEY_W, KEY_X, KEY_Y, KEY_Z,
		KEY_WinLeft = 91U, KEY_WinRight,
		KEY_R0 = 96U, KEY_R1, KEY_R2, KEY_R3, KEY_R4, KEY_R5, KEY_R6, KEY_R7, KEY_R8, KEY_R9,
		KEY_Multiply = 106U, KEY_Add,
		KEY_Subtract = 109U, KEY_Del, KEY_Divide,
		KEY_F1 = 112U, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12,
		KEY_NumLock = 144U
	};

	namespace details {
		META_IMPORT_API(void, GetMousePos, mqui32*, mqui32*);
		META_IMPORT_API(void, MouseMove, mqui32, mqui32);
		META_IMPORT_API(void, MouseClick, mqui32, mqui32, MouseClickMode);
		META_IMPORT_API(void, MouseWheel, mqi32);
		META_IMPORT_API(void, KeyboardClick, KeyCode, KeyCode, KeyCode);
		META_MODULE_BEGIN
			META_PROC_API(GetMousePos);
			META_PROC_API(MouseMove);
			META_PROC_API(MouseClick);
			META_PROC_API(MouseWheel);
			META_PROC_API(KeyboardClick);
		META_MODULE_END
	}
#undef MasterQianModuleName
#undef MasterQianModuleNameString
#undef MasterQianLibString
#undef MasterQianModuleVersion

	/// <summary>
	/// 取鼠标位置
	/// </summary>
	/// <returns>鼠标位置</returns>
	[[nodiscard]] inline MousePos GetMousePos() noexcept {
		MousePos pos{ };
		details::MasterQian_System_AutoDevice_GetMousePos(&pos.x, &pos.y);
		return pos;
	}

	/// <summary>
	/// 鼠标模拟移动
	/// </summary>
	/// <param name="pos">鼠标位置</param>
	/// <returns></returns>
	inline void MouseMove(MousePos pos) noexcept {
		details::MasterQian_System_AutoDevice_MouseMove(pos.x, pos.y);
	}

	/// <summary>
	/// 鼠标模拟点击
	/// </summary>
	/// <param name="pos">鼠标位置</param>
	/// <typeparam name="mode">点击模式</typeparam>
	template<MouseClickMode mode>
	inline void MouseClick(MousePos pos) noexcept {
		details::MasterQian_System_AutoDevice_MouseClick(pos.x, pos.y, mode);
	}

	/// <summary>
	/// 鼠标模拟滑轮
	/// </summary>
	/// <param name="len">滚动距离，负数表示反方向滚动</param>
	inline void MouseWheel(mqi32 len) noexcept {
		details::MasterQian_System_AutoDevice_MouseWheel(len);
	}

	/// <summary>
	/// 键盘模拟按键，最多支持3个键组合
	/// </summary>
	/// <typeparam name="key1">键1</typeparam>
	/// <typeparam name="key1">键2</typeparam>
	/// <typeparam name="key1">键3</typeparam>
	template<KeyCode key1, KeyCode key2 = KeyCode::KEY_None, KeyCode key3 = KeyCode::KEY_None>
	inline void KeyboardClick() noexcept {
		details::MasterQian_System_AutoDevice_KeyboardClick(key1, key2, key3);
	}
}
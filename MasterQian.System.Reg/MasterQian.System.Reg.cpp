#include "../include/MasterQian.Meta.h"

#define _AMD64_
#include <windef.h>
#include <winuser.h>
#include <winreg.h>

#define MasterQianModuleName(_) MasterQian_System_Reg_
META_EXPORT_API_VERSION(20231229ULL)

namespace details {
	inline static constexpr REGSAM GetRegSAM(mqbool wow64) noexcept {
		return wow64 ? 0xF013F : 0xF023F;
	}

}

META_EXPORT_API(mqbool, Open, PHKEY pKey, mqbool wow64, HKEY base, mqcstr route) {
	if (base == (HKEY)0x80000001ULL) {
		HKEY tmp;
		if (RegOpenCurrentUser(details::GetRegSAM(wow64), &tmp) == 0) {
			mqbool result{ RegOpenKeyW(tmp, route, pKey) == 0 };
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
		if (RegOpenKeyW(base, route, pKey) == 0) {
			return true;
		}
		else {
			*pKey = nullptr;
		}
	}
	return false;
}

META_EXPORT_API(mqbool, Close, PHKEY pKey) {
	mqbool result{ *pKey ? RegCloseKey(*pKey) == 0 : false };
	*pKey = nullptr;
	return result;
}

META_EXPORT_API(mqbool, CreateItem, HKEY hKey, mqbool wow64, mqcstr route, PHKEY ret) {
	HKEY tmp{ };
	if (RegCreateKeyExW(hKey, route, 0, nullptr, 0,
		details::GetRegSAM(wow64), nullptr, &tmp, nullptr) == 0) {
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

META_EXPORT_API(mqbool, DeleteItem, HKEY hKey, mqbool wow64, mqcstr route) {
	return RegDeleteKeyExW(hKey, route, details::GetRegSAM(wow64), 0) == 0;
}

META_EXPORT_API(mqbool, EnumItem, HKEY hKey, mqdword index, mqstr route) {
	return RegEnumKeyW(hKey, index, route, MAX_PATH) == 0;
}

META_EXPORT_API(mqbool, HasKey, HKEY hKey, mqcstr route) {
	return RegQueryValueExW(hKey, route, 0, nullptr, nullptr, nullptr) == 0;
}

META_EXPORT_API(mqbool, GetValueTypeAndSize, HKEY hKey, mqcstr route, mqdword* type, mqdword* size) {
	return RegQueryValueExW(hKey, route, 0, type, nullptr, size) == 0;
}

META_EXPORT_API(mqbool, GetValue, HKEY hKey, mqcstr route, mqbytes mem, mqdword* size) {
	return RegQueryValueExW(hKey, route, 0, nullptr, mem, size) == 0;
}

META_EXPORT_API(mqbool, SetValue, HKEY hKey, mqcstr route, mqdword type, mqcbytes mem, mqdword size) {
	return RegSetValueExW(hKey, route, 0, type, mem, size) == 0;
}

META_EXPORT_API(mqbool, DeleteKey, HKEY hKey, mqcstr route) {
	return RegDeleteValueW(hKey, route) == 0;
}

META_EXPORT_API(mqbool, GetKeyValueMaxSize, HKEY hKey, mqdword* maxKeySize, mqdword* maxValueSize) {
	return RegQueryInfoKeyW(hKey, nullptr, nullptr, nullptr, nullptr, nullptr,
		nullptr, nullptr, maxKeySize, maxValueSize, nullptr, nullptr) == 0;
}

META_EXPORT_API(mqdword, EnumKey, HKEY hKey, mqdword index, mqdword maxKeySize, mqstr route, mqdword* type) {
	return RegEnumValueW(hKey, index, route, &maxKeySize, 0, type,
		nullptr, nullptr) == 0 ? maxKeySize : 0U;
}

META_EXPORT_API(void, FlushEnvironment) {
	static DWORD_PTR dwReturnValue{ };
	SendMessageTimeoutW(HWND_BROADCAST, WM_SETTINGCHANGE, 0,
		reinterpret_cast<LPARAM>(L"Environment"), SMTO_ABORTIFHUNG, 5000, &dwReturnValue);
}
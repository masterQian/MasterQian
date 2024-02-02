#pragma once

#include "MasterQian.Meta.h"

#define MasterQianModuleName(_) MasterQian_System_Window_
#define MasterQianModuleNameString "MasterQian_System_Window_"
#ifdef _DEBUG
#define MasterQianLibString "MasterQian.System.Window.Debug.dll"
#else
#define MasterQianLibString "MasterQian.System.Window.dll"
#endif
#define MasterQianModuleVersion 20231229ULL
#pragma message("！！！！！！！！！！ Please copy [" MasterQianLibString "] into your program folder ！！！！！！！！！！")

namespace MasterQian::System::Window {
	namespace details {
		
		META_MODULE_BEGIN
			
		META_MODULE_END
	}
#undef MasterQianModuleName
#undef MasterQianModuleNameString
#undef MasterQianLibString
#undef MasterQianModuleVersion

}
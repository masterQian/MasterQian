#pragma once

#include "MasterQian.Meta.h"

#include <string>

#define MasterQianModuleName(_) MasterQian_System_Info_
#define MasterQianModuleNameString "MasterQian_System_Info_"
#ifdef _DEBUG
#define MasterQianLibString "MasterQian.System.Info.Debug.dll"
#else
#define MasterQianLibString "MasterQian.System.Info.dll"
#endif
#define MasterQianModuleVersion 20240102ULL
#pragma message("！！！！！！！！！！ Please copy [" MasterQianLibString "] into your program folder ！！！！！！！！！！")

namespace MasterQian::System::Info {
	namespace details {
		META_IMPORT_API(mqui32, GetScreenWidth);
		
		META_MODULE_BEGIN
			META_PROC_API(GetScreenWidth);
			
		META_MODULE_END
	}
#undef MasterQianModuleName
#undef MasterQianModuleNameString
#undef MasterQianLibString
#undef MasterQianModuleVersion

	

	
}
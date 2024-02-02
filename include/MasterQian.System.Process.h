#pragma once

#include "MasterQian.Meta.h"
#include "MasterQian.Bin.h"

#define MasterQianModuleName(_) MasterQian_System_Process_
#define MasterQianModuleNameString "MasterQian_System_Process_"
#ifdef _DEBUG
#define MasterQianLibString "MasterQian.System.Process.Debug.dll"
#else
#define MasterQianLibString "MasterQian.System.Process.dll"
#endif
#define MasterQianModuleVersion 20231229ULL
#pragma message("！！！！！！！！！！ Please copy [" MasterQianLibString "] into your program folder ！！！！！！！！！！")

namespace MasterQian::System::Process {
	namespace details {
		
		META_MODULE_BEGIN
			
		META_MODULE_END
	}
#undef MasterQianModuleName
#undef MasterQianModuleNameString
#undef MasterQianLibString
#undef MasterQianModuleVersion

	
}
#pragma once

#include "MasterQian.Meta.h"
#include "MasterQian.Bin.h"

#define MasterQianModuleName(_) MasterQian_Data_Crypto_
#define MasterQianModuleNameString "MasterQian_Data_Crypto_"
#ifdef _DEBUG
#define MasterQianLibString "MasterQian.Data.Crypto.Debug.dll"
#else
#define MasterQianLibString "MasterQian.Data.Crypto.dll"
#endif
#define MasterQianModuleVersion 20231229ULL
#pragma message("！！！！！！！！！！ Please copy [" MasterQianLibString "] into your program folder ！！！！！！！！！！")

namespace MasterQian::Data::Crypto {
	namespace details {
		
		META_MODULE_BEGIN
			
		META_MODULE_END
	}
#undef MasterQianModuleName
#undef MasterQianModuleNameString
#undef MasterQianLibString
#undef MasterQianModuleVersion

	
}
#include <string>
#include <unordered_map>
import MasterQian.Log;
import MasterQian.Storage.Path;
import MasterQian.Time;
import MasterQian.System;
using namespace MasterQian;

ConsoleLogger logger;

#if 0
#else
#pragma optimize("",off)
#endif
int main() {
	logger.i(L"123"_utf8);
	return 0;
}
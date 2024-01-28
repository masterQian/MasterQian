#define BUILD_TEST 0
#define OPTIMIZE 0
#include "Test.h"

#include "../include/MasterQian.WinRT.h"

#if OPTIMIZE
#else
#pragma optimize("",off)
#endif
int main() {
	winrt::init_apartment();
	auto args = WinRT::Args::box(2, 3.5, L"123");
	auto [r1, _, r3] = args.unbox<int, void, winrt::hstring>();
	logger.log(r1, L" ", L" ", r3);

	return 0;
}
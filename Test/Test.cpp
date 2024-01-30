#define BUILD_TEST 0
#define OPTIMIZE 0
#include "Test.h"

#if OPTIMIZE
#else
#pragma optimize("",off)
#endif
int main() {
	return 0;
}
#include <cstdio>

#include <Windows.h>

int wmain(int argc, wchar_t** argv) {
    if (argc == 2ULL) {
        auto dll_path{ argv[1ULL] };
        auto handle{ LoadLibraryW(dll_path) };
        if (handle) {
            using VerFuncType = ULONGLONG(__stdcall*)();
            auto verFunc = (VerFuncType)GetProcAddress(handle, "MasterQianVersion");
            if (verFunc) {
                wprintf(L"%llu", verFunc());
                FreeLibrary(handle);
            }
            else {
                wprintf(L"damaged dLL");
            }
        }
        else {
            wprintf(L"missing dll");
        }
    }
    else {
        wprintf(L"missing dll path");
    }
    return 0;
}
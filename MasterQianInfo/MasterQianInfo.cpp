import MasterQian.Log;
using namespace MasterQian;

mqi32 wmain(mqi32 argc, mqchar** argv) {
    if (argc == 2ULL) {
        auto handle{ api::LoadLibraryW(argv[1ULL]) };
        if (handle) {
            if (auto verFunc = reinterpret_cast<mqui64(__stdcall*)()>(api::GetProcAddress(handle, "MasterQianVersion"))) {
                console.i(verFunc());
                api::FreeLibrary(handle);
            }
            else console.e(L"damaged dLL");
        }
        else console.e(L"missing dll");
    }
    else console.e(L"missing dll path");
    return 0;
}
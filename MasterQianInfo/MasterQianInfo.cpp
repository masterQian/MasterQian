import MasterQian.Log;
using namespace MasterQian;

mqi32 wmain(mqi32 argc, mqchar** argv) {
    ConsoleLogger logger;
    if (argc == 2ULL) {
        auto dll_path{ argv[1ULL] };
        auto handle{ api::LoadLibraryW(dll_path) };
        if (handle) {
            using VerFuncType = mqui64(__stdcall*)();
            auto verFunc = reinterpret_cast<VerFuncType>(api::GetProcAddress(handle, "MasterQianVersion"));
            if (verFunc) {
                logger.i(verFunc());
                api::FreeLibrary(handle);
            }
            else {
                logger.e(L"damaged dLL");
            }
        }
        else {
            logger.e(L"missing dll");
        }
    }
    else {
        logger.e(L"missing dll path");
    }
    return 0;
}
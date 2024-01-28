#include "../include/MasterQian.Meta.h"

#define _AMD64_
#include <stringapiset.h>

#include "benchmark.h"
#ifdef _DEBUG
#pragma comment(lib, "benchmarkd.lib")
#else
#pragma comment(lib, "benchmark.lib")
#endif
#pragma comment(lib, "shlwapi.lib")

#define MasterQianModuleName(_) MasterQian_Tool_Benchmark_
META_EXPORT_API_VERSION(20240114ULL)

namespace details {
	using Function = void(__stdcall*)(mqcmem arg) noexcept;
}

META_EXPORT_API(void, BenchmarkInitialize) {
	int argc{ 0 };
	::benchmark::Initialize(&argc, nullptr);
}

META_EXPORT_API(void, BenchmarkRun) {
	::benchmark::RunSpecifiedBenchmarks();
}

META_EXPORT_API(void, BenchmarkShutdown) {
	::benchmark::Shutdown();
}

META_EXPORT_API(void, BenchmarkAdd, details::Function func, mqcmem args, mqui64 argc, mqui64 sizeofarg, mqui64 count, mqcstr name) {
	if (argc) {
		auto len{ WideCharToMultiByte(CP_ACP, 0, name, -1, nullptr, 0, nullptr, nullptr) };
		std::string name_ansi;
		if (len > 0) {
			name_ansi.resize(static_cast<mqui64>(len));
			WideCharToMultiByte(CP_ACP, 0, name, -1, name_ansi.data(), len, nullptr, nullptr);
		}

		auto benchmark = ::benchmark::internal::RegisterBenchmarkInternal(
			new ::benchmark::internal::FunctionBenchmark(name_ansi,
				[ ] (::benchmark::State& state) {
					auto func{ reinterpret_cast<details::Function>(state.range(0ULL)) };
					auto arg{ reinterpret_cast<mqcmem>(state.range(1ULL)) };
					for (auto _ : state) {
						func(arg);
					}
				}));
		benchmark->Iterations(static_cast<mqi64>(count));
		for (mqui64 i{ }; i < argc; ++i) {
			benchmark->ArgPair(reinterpret_cast<mqi64>(func),
				reinterpret_cast<mqi64>(static_cast<mqcbytes>(args) + i * sizeofarg));
		}
	}
}
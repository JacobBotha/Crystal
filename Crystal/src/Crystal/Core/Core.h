#pragma once


#ifdef CL_PLATFORM_WINDOWS 
	#ifdef CL_BUILD_DLL
		#define CRYSTAL_API _declspec(dllexport)
	#else 
		#define CRYSTAL_API _declspec(dllimport)
	#endif // CL_BUID_DLL
#else
	#error Crystal only supports Windows!
#endif

#define BIT(x) (1 << x)

#ifdef CL_ENABLE_ASSERTS
	#define CL_ASSERT(x, ...) { if(!(x)) { CL_ERROR("Assertion Failed: {0}", __VA_ARGS__ ); __debugbreak(); } }
	#define CL_CORE_ASSERT(...) { if(!(x)) { CL_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__ ); __debugbreak(); } }
#else
	#define CL_ASSERT(x, ...)
	#define CL_CORE_ASSERT(x, ...)
#endif
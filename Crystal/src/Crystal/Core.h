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
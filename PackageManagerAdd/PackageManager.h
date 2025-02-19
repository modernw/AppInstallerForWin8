#pragma once
#ifdef PKGMGR_EXPORTS
#define PKGMGR_API __declspec(dllexport)
#else
#define PKGMGR_API __declspec(dllimport)
#endif

typedef void (*_PACKAGE_ADD_PROGRESS)(unsigned progress);
typedef _PACKAGE_ADD_PROGRESS PACKAGE_ADD_PROGRESS;

enum class InstallStatus
{
	Success,
	Error,
	Canceled
};

extern "C" PKGMGR_API InstallStatus AddPackageFromPath (LPCWSTR lpswPath, PACKAGE_ADD_PROGRESS callbProgress);

extern "C" PKGMGR_API LPCWSTR GetLastErrorCode ();

extern "C" PKGMGR_API LPCWSTR GetLastErrorDetailText ();

extern "C" PKGMGR_API size_t GetLastErrorCodeLength ();

extern "C" PKGMGR_API size_t GetLastErrorDetailTextLength ();
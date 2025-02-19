#pragma once
#ifdef SHORTCUT_EXPORTS
#define SHORUTCUT_API __declspec(dllexport)
#else
#define SHORUTCUT_API __declspec(dllimport)
#endif

#include <windef.h>

extern "C" SHORUTCUT_API HRESULT CreateShortcut (LPCWSTR shortcutPath, LPCWSTR targetPath, LPCWSTR appUserModelID);

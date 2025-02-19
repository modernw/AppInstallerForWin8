#pragma once
#ifdef TOASTNOTICE_EXPORTS
#define TOASTNOTICE_API __declspec(dllexport)
#else
#define TOASTNOTICE_API __declspec(dllimport)
#endif

#include <windef.h>

typedef void (*_NOTICE_PRESS_CALLBACK) (void);
typedef _NOTICE_PRESS_CALLBACK NOTICE_PRESS_CALLBACK;
// 使用 COM 创建的 IStream *
typedef HANDLE HISTREAM;

extern "C" TOASTNOTICE_API bool CreateToastNotification (LPCWSTR identityName, LPCWSTR title, LPCWSTR text, NOTICE_PRESS_CALLBACK callback, HISTREAM imgFile);
extern "C" TOASTNOTICE_API HRESULT CreateShortcutWithAppIdA (LPCSTR pszShortcutPath, LPCSTR pszTargetPath, LPCSTR pszAppId);
extern "C" TOASTNOTICE_API HRESULT CreateShortcutWithAppIdW (LPCWSTR pszShortcutPath, LPCWSTR pszTargetPath, LPCWSTR pszAppId);
#pragma once
#ifdef PRIREADER_EXPORTS
#define PRIREADER_API __declspec(dllexport)
#else
#define PRIREADER_API __declspec(dllimport)
#endif

#include <windef.h>

EXTERN_C LPWSTR GetStringValueFromPriSource (IStream *priFileStream, LPCWSTR srcName);
EXTERN_C LPWSTR GetResourceValueFromPriSource (IStream *priFileStream, LPCWSTR srcName);
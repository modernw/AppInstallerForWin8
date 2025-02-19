#include "stdafx.h"
#include "PriReader.h"

static HRESULT InitializeResourceManager (ComPtr<IResourceManager>& resourceManager)
{
	ComPtr<IResourceManagerStatics> resourceManagerStatics;
	HRESULT hr = RoInitialize (RO_INIT_MULTITHREADED);
	if (FAILED (hr))
	{
		return hr;
	}
	hr = RoGetActivationFactory (HStringReference (RuntimeClass_Windows_ApplicationModel_Resources_Core_ResourceManager).Get (), IID_PPV_ARGS (&resourceManagerStatics));
	if (FAILED (hr))
	{
		return hr;
	}
	// Windows 8 使用 `GetCurrent` 获取全局 ResourceManager
	hr = resourceManagerStatics->GetCurrent (&resourceManager);
	return hr;
}

static LPWSTR GetResourceValue (ComPtr<IResourceManager> resourceManager, LPCWSTR srcName)
{
	ComPtr<IResourceMap> resourceMap;
	HRESULT hr = resourceManager->get_MainResourceMap (&resourceMap);
	if (FAILED (hr))
	{
		return nullptr;
	}

	HSTRING hResourceName;
	hr = WindowsCreateString (srcName, static_cast<UINT32>(wcslen (srcName)), &hResourceName);
	if (FAILED (hr))
	{
		return nullptr;
	}

	ComPtr<IResourceCandidate> resourceCandidate;
	hr = resourceMap->GetValue (hResourceName, &resourceCandidate);
	WindowsDeleteString (hResourceName);
	if (FAILED (hr))
	{
		return nullptr;
	}
	HSTRING hResourceValue;
	hr = resourceCandidate->get_ValueAsString (&hResourceValue);
	if (FAILED (hr))
	{
		return nullptr;
	}
	const wchar_t* resourceValue = WindowsGetStringRawBuffer (hResourceValue, nullptr);
	LPWSTR result = (LPWSTR)calloc (wcslen (resourceValue) + 1, sizeof (wchar_t));
	if (result)
	{
		wcscpy_s (result, wcslen (resourceValue) + 1, resourceValue);
	}

	WindowsDeleteString (hResourceValue);
	return result;
}

EXTERN_C LPWSTR GetStringValueFromPriSource (IStream* priFileStream, LPCWSTR srcName)
{
	// Windows 8 不支持直接加载 PRI 文件流，因此省略 stream 参数
	ComPtr<IResourceManager> resourceManager;
	HRESULT hr = InitializeResourceManager (resourceManager);
	if (FAILED (hr))
	{
		return nullptr;
	}
	return GetResourceValue (resourceManager, srcName);
}

EXTERN_C LPWSTR GetResourceValueFromPriSource (IStream* priFileStream, LPCWSTR srcName)
{
	// Windows 8 仅支持默认 ResourceManager
	ComPtr<IResourceManager> resourceManager;
	HRESULT hr = InitializeResourceManager (resourceManager);
	if (FAILED (hr))
	{
		return nullptr;
	}
	return GetResourceValue (resourceManager, srcName);
}
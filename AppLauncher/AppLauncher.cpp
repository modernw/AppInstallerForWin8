// AppLauncher.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "AppLauncher.h"

HRESULT LaunchApp (LPCWSTR lpswStr, PDWORD pdwProcessId)
{
	if (!lpswStr) return E_INVALIDARG;
	if (FAILED (CoInitializeEx (NULL, COINIT_APARTMENTTHREADED))) return E_INVALIDARG;
	std::wstring strAppUserModelId (L"");
	if (lpswStr) strAppUserModelId += lpswStr;
	CComPtr<IApplicationActivationManager> spAppActivationManager;
	HRESULT hrResult = E_INVALIDARG;
	if (!strAppUserModelId.empty ())
	{
		// Instantiate IApplicationActivationManager
		hrResult = CoCreateInstance (CLSID_ApplicationActivationManager, NULL, CLSCTX_LOCAL_SERVER, IID_IApplicationActivationManager, (LPVOID*)&spAppActivationManager);
		if (SUCCEEDED (hrResult))
		{
			// This call ensures that the app is launched as the foreground window
			hrResult = CoAllowSetForegroundWindow (spAppActivationManager, NULL);
			// Launch the app
			if (SUCCEEDED (hrResult))
			{
				hrResult = spAppActivationManager->ActivateApplication (strAppUserModelId.c_str (), NULL, AO_NONE, pdwProcessId);
			}
		}
	}
	return hrResult;
}
#include "stdafx.h"
#include "ToastNotification.h"

bool SaveIStreamToTempFile (IStream* stream, wchar_t* tempFilePath, size_t pathLen)
{
	wchar_t tempPath [MAX_PATH];
	if (!GetTempPathW (MAX_PATH, tempPath)) return false;
	if (!GetTempFileNameW (tempPath, L"TST", 0, tempFilePath)) return false;
	std::ofstream outFile (tempFilePath, std::ios::binary);
	if (!outFile) return false;
	// 读取 IStream 数据
	STATSTG stat;
	HRESULT hr = stream->Stat (&stat, STATFLAG_NONAME);
	if (FAILED (hr)) return false;
	LARGE_INTEGER liZero = {};
	stream->Seek (liZero, STREAM_SEEK_SET, nullptr);
	ULARGE_INTEGER remaining = stat.cbSize;
	BYTE buffer [4096];
	ULONG bytesRead;
	while (remaining.QuadPart > 0) 
	{
		hr = stream->Read (buffer, min (sizeof (buffer), remaining.LowPart), &bytesRead);
		if (FAILED (hr) || bytesRead == 0) break;
		outFile.write ((char*)buffer, bytesRead);
		remaining.QuadPart -= bytesRead;
	}
	return outFile.good ();
}

// 线程安全删除临时文件
void DeleteFileThreadSafe (wchar_t* filePath) 
{
	Sleep (5000);  // 等待通知可能完成显示
	DeleteFileW (filePath);
}

bool CreateToastNotification (
	LPCWSTR identityName,
	LPCWSTR title,
	LPCWSTR text,
	NOTICE_PRESS_CALLBACK callback,
	HISTREAM imgFile
)
{
	ComPtr <IToastNotifier> notifier;
	ComPtr <IToastNotification> toast;
	ComPtr <IToastNotificationFactory> toastFactory;
	ComPtr <IXmlDocument> xmlDoc;
	HRESULT hr = RoInitialize (RO_INIT_MULTITHREADED);
	if (FAILED (hr) && hr != RPC_E_CHANGED_MODE) return false;
	ComPtr <IToastNotificationManagerStatics> toastManager;
	hr = RoGetActivationFactory (HStringReference (L"Windows.UI.Notifications.ToastNotificationManager").Get (),
		__uuidof(IToastNotificationManagerStatics),
		&toastManager);
	if (FAILED (hr)) return false;
	hr = toastManager->CreateToastNotifierWithId (HStringReference (identityName).Get (), &notifier);
	if (FAILED (hr)) return false;
	hr = RoActivateInstance (HStringReference (L"Windows.Data.Xml.Dom.XmlDocument").Get (), &xmlDoc);
	if (FAILED (hr)) return false;
	wchar_t xmlTemplateWithImage [] =
		L"<toast>"
		L"<visual>"
		L"<binding template='ToastImageAndText02'>"
		L"<image id='1' src='file://%s'/>"
		L"<text id='1'>%s</text>"
		L"<text id='2'>%s</text>"
		L"</binding>"
		L"</visual>"
		L"</toast>";
	wchar_t xmlTemplateNoImage [] =
		L"<toast>"
		L"<visual>"
		L"<binding template='ToastGeneric'>"
		L"<text>%s</text>"
		L"<text>%s</text>"
		L"</binding>"
		L"</visual>"
		L"</toast>";
	wchar_t xmlData [1024];
	wchar_t imagePath [MAX_PATH] = L"";
	if (imgFile != NULL) 
	{
		IStream* img = (IStream*)imgFile;
		STATSTG stat;
		hr = img->Stat (&stat, STATFLAG_DEFAULT);
		if (SUCCEEDED (hr) && stat.pwcsName) 
		{
			wcscpy_s (imagePath, stat.pwcsName);
			CoTaskMemFree (stat.pwcsName);
		}
		else 
		{
			if (!SaveIStreamToTempFile (img, imagePath, MAX_PATH)) return false;
			wchar_t* tempFileCopy = _wcsdup (imagePath);
			if (tempFileCopy)
			{
				CreateThread (nullptr, 0, (LPTHREAD_START_ROUTINE)DeleteFileThreadSafe, tempFileCopy, 0, nullptr);
			}
		}
		swprintf_s (xmlData, xmlTemplateWithImage, imagePath, title, text);
	}
	else 
	{
		swprintf_s (xmlData, xmlTemplateNoImage, title, text);
	}
	ComPtr <IXmlDocumentIO> xmlDocIO;
	hr = xmlDoc.As (&xmlDocIO);
	if (FAILED (hr)) return false;
	hr = xmlDocIO->LoadXml (HStringReference (xmlData).Get ());
	if (FAILED (hr)) return false;
	hr = RoGetActivationFactory (HStringReference (L"Windows.UI.Notifications.ToastNotification").Get (),
		__uuidof(IToastNotificationFactory),
		&toastFactory);
	if (FAILED (hr)) return false;
	hr = toastFactory->CreateToastNotification (xmlDoc.Get (), &toast);
	if (FAILED (hr)) return false;
	return SUCCEEDED (notifier->Show (toast.Get ()));
}

HRESULT CreateShortcutWithAppIdA (LPCSTR pszShortcutPath, LPCSTR pszTargetPath, LPCSTR pszAppId)
{
	HRESULT hr;
	// 初始化 COM
	hr = CoInitialize (NULL);
	if (FAILED (hr))
		return hr;
	// 创建 IShellLink 对象（ANSI版本）
	IShellLinkA* pShellLinkA = NULL;
	hr = CoCreateInstance (CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
		IID_IShellLinkA, (void**)&pShellLinkA);
	if (FAILED (hr))
	{
		CoUninitialize ();
		return hr;
	}
	// 设置快捷方式目标路径
	hr = pShellLinkA->SetPath (pszTargetPath);
	if (FAILED (hr))
	{
		pShellLinkA->Release ();
		CoUninitialize ();
		return hr;
	}
	// 设置 AppUserModelID
	IPropertyStore* pPropStore = NULL;
	hr = pShellLinkA->QueryInterface (IID_IPropertyStore, (void**)&pPropStore);
	if (SUCCEEDED (hr))
	{
		PROPVARIANT propvar;
		hr = InitPropVariantFromString (_bstr_t (pszAppId), &propvar);
		if (SUCCEEDED (hr))
		{
			// PKEY_AppUserModel_ID 定义在 propkey.h 中
			hr = pPropStore->SetValue (PKEY_AppUserModel_ID, propvar);
			if (SUCCEEDED (hr))
				hr = pPropStore->Commit ();
			PropVariantClear (&propvar);
		}
		pPropStore->Release ();
	}
	// 保存快捷方式文件（IPersistFile 要求传入宽字符串）
	IPersistFile* pPersistFile = NULL;
	hr = pShellLinkA->QueryInterface (IID_IPersistFile, (void**)&pPersistFile);
	if (SUCCEEDED (hr))
	{
		// 将 pszShortcutPath 从 ANSI 转换为 Unicode
		wchar_t wszShortcutPath [MAX_PATH];
		MultiByteToWideChar (CP_ACP, 0, pszShortcutPath, -1, wszShortcutPath, MAX_PATH);
		hr = pPersistFile->Save (wszShortcutPath, TRUE);
		pPersistFile->Release ();
	}
	pShellLinkA->Release ();
	CoUninitialize ();
	return hr;
}

HRESULT CreateShortcutWithAppIdW (LPCWSTR pszShortcutPath, LPCWSTR pszTargetPath, LPCWSTR pszAppId)
{
	HRESULT hr;
	hr = CoInitialize (NULL);
	if (FAILED (hr))
	{
		return hr;
	}
	IShellLinkW* pShellLinkW = NULL;
	hr = CoCreateInstance (CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLinkW, (void**)&pShellLinkW);
	if (FAILED (hr)) 
	{
		CoUninitialize ();
		return hr;
	}
	hr = pShellLinkW->SetPath (pszTargetPath);
	if (FAILED (hr)) 
	{
		pShellLinkW->Release ();
		CoUninitialize ();
		return hr;
	}
	IPropertyStore* pPropStore = NULL;
	hr = pShellLinkW->QueryInterface (IID_IPropertyStore, (void**)&pPropStore);
	if (SUCCEEDED (hr)) 
	{
		PROPVARIANT propvar;
		hr = InitPropVariantFromString (pszAppId, &propvar);
		if (SUCCEEDED (hr)) 
		{
			hr = pPropStore->SetValue (PKEY_AppUserModel_ID, propvar);
			if (SUCCEEDED (hr))
			{
				hr = pPropStore->Commit ();
			}
			PropVariantClear (&propvar);
		}
		pPropStore->Release ();
	}
	IPersistFile* pPersistFile = NULL;
	hr = pShellLinkW->QueryInterface (IID_IPersistFile, (void**)&pPersistFile);
	if (SUCCEEDED (hr))
	{
		hr = pPersistFile->Save (pszShortcutPath, TRUE);
		pPersistFile->Release ();
	}
	pShellLinkW->Release ();
	CoUninitialize ();
	return hr;
}
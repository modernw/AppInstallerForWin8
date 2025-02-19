#include "stdafx.h"
#include "Shortcut\tCreater.h"

HRESULT CreateShortcut (LPCWSTR shortcutPath, LPCWSTR targetPath, LPCWSTR appUserModelID)
{
	HRESULT hr;
	IShellLink* psl = nullptr;
	IPersistFile* ppf = nullptr;
	IPropertyStore* pps = nullptr;
	PROPVARIANT pv;
	hr = CoInitializeEx (nullptr, COINIT_APARTMENTTHREADED);
	if (FAILED (hr))
		return hr;
	// Create a ShellLink object
	hr = CoCreateInstance (CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_IShellLink, (void**)&psl);
	if (SUCCEEDED (hr))
	{
		// Set the path to the shortcut target
		psl->SetPath (targetPath);
		// Set the AppUserModelID property for the shortcut
		hr = psl->QueryInterface (IID_IPropertyStore, (void**)&pps);
		if (SUCCEEDED (hr))
		{
			hr = InitPropVariantFromString (appUserModelID, &pv);
			if (SUCCEEDED (hr))
			{
				hr = pps->SetValue (PKEY_AppUserModel_ID, pv);
				if (SUCCEEDED (hr))
				{
					hr = pps->Commit ();
				}
				PropVariantClear (&pv);
			}
			pps->Release ();
		}
		// Save the shortcut to disk
		hr = psl->QueryInterface (IID_IPersistFile, (void**)&ppf);
		if (SUCCEEDED (hr))
		{
			hr = ppf->Save (shortcutPath, TRUE);
			ppf->Release ();
		}
		psl->Release ();
	}
	return hr;
}
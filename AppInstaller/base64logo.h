#pragma once
#include "Base64Img.h"
#include <iostream>
#include <map>
#include <regex>
#include "pkgcode.h"
#include "priread.h"

std::string GetFileNameWithoutExtension (LPCSTR filePath) 
{
	char pathCopy [MAX_PATH];
	lstrcpynA (pathCopy, filePath, MAX_PATH);
	char *fileName = PathFindFileNameA (pathCopy);
	PathRemoveExtensionA (fileName);
	return std::string (fileName);
}
std::wstring GetFileNameWithoutExtension (LPCWSTR filePath) 
{
	wchar_t pathCopy [MAX_PATH];
	lstrcpynW (pathCopy, filePath, MAX_PATH);
	wchar_t *fileName = PathFindFileNameW (pathCopy);
	PathRemoveExtensionW (fileName);
	return std::wstring (fileName);
}
std::string GetFileNameWithoutExtension (const std::string &filePath) 
{
	return GetFileNameWithoutExtension (filePath.c_str ());
}
std::wstring GetFileNameWithoutExtension (const std::wstring &filePath) 
{
	return GetFileNameWithoutExtension (filePath.c_str ());
}

std::string GetLogoBase64FromReader (PackageReader &reader, IStream **getOutput = NULL, PriReader *priread = NULL)
{
	if (!&reader) return std::string ("");
	if (!reader.isAvailable ()) return std::string ("");
	std::wstring logoPath = reader.getPropertyLogo ();
	if (logoPath.empty () || logoPath.length () == 0) return std::string ("");
	PriReader *pri = ((priread != NULL) ? priread : new PriReader (reader.getPriFileStream ()));
	if (pri->isAvailable ())
	{
		std::wstring logoPathFromPri = pri->findFilePathValue (logoPath);
		if (!logoPathFromPri.empty () && logoPath.length () > 0) logoPath = logoPathFromPri;
	}
	if (priread == NULL) delete pri;
	pri = NULL;
	IStream *imgfile = reader.extractFileToStream (logoPath);
	if (getOutput) *getOutput = imgfile;
	if (!imgfile)
	{
		std::wstring fileName = GetFileNameWithoutExtension (logoPath);
		IAppxPackageReader *appxreader = reader.getAppxPackageReader ();
		CComPtr <IAppxFilesEnumerator> fe = NULL;
		HRESULT hr = appxreader->GetPayloadFiles (&fe);
		if (SUCCEEDED (hr) && fe)
		{
			std::vector <std::wstring> files;
			BOOL hasCurrent = FALSE;
			hr = fe->GetHasCurrent (FALSE);
			while (SUCCEEDED (hr) && hasCurrent)
			{
				CComPtr <IAppxFile> ifile = NULL;
				hr = fe->GetCurrent (&ifile);
				if (SUCCEEDED (hr) && ifile)
				{
					LPWSTR lpstr = NULL;
					if (SUCCEEDED (ifile->GetName (&lpstr)) && lpstr)
					{
						std::wstring strWillSearch = StringToUpper (StringTrim (lpstr));
						std::wstring strWillToSearch = StringToUpper (StringTrim (fileName));
						if (StrStrW (strWillSearch.c_str (), strWillToSearch.c_str ()))
						{
							bool isFind = LabelEqual (PathFindExtensionW (lpstr), L".png");
							if (!isFind) isFind = LabelEqual (PathFindExtensionW (lpstr), L".jpg");
							if (!isFind) isFind = LabelEqual (PathFindExtensionW (lpstr), L".jpeg");
							if (!isFind) isFind = LabelEqual (PathFindExtensionW (lpstr), L".bmp");
							if (isFind) 
							{
								std::wstring temp (L"");
								if (lpstr) temp += lpstr;
								if (temp.length () > 0) push_no_repeat (files, temp);
							}
						}
					}
				}
			}
			bool isFindSuit = false;
			{
				std::map <int, std::wstring> scaleFiles;
				std::wregex pattern (L"(scale-(\\d+))");
				for (auto it : files)
				{
					if (StrStrW (it.c_str (), L"contrast-")) continue;
					std::wsmatch match;
					if (std::regex_search (it, match, pattern)) 
					{
						int temp = StrToIntW (match [1].str ().c_str ());
						if (!temp)
						{
							scaleFiles [temp] = it;
						}
					}
				}
				if (scaleFiles.find (GetDPI ()) != scaleFiles.end ())
				{
					isFindSuit = true;
					logoPath = scaleFiles [GetDPI ()];
				}
				else
				{
					bool isFindScale = false;
					for (auto it : scaleFiles)
					{
						if (it.first >= GetDPI ())
						{
							isFindScale = true;
							logoPath = scaleFiles [GetDPI ()];
							break;
						}
					}
					if (!isFindScale)
					{
						for (auto it : scaleFiles)
						{
							if (it.first >= 100)
							{
								isFindScale = true;
								logoPath = scaleFiles [GetDPI ()];
								break;
							}
						}
					}
					if (!isFindScale)
					{
						for (auto it : scaleFiles)
						{
							isFindScale = true;
							logoPath = scaleFiles [GetDPI ()];
							break;
						}
					}
					isFindSuit = isFindScale;
				}
			}
			if (!isFindSuit)
			{
				std::map <int, std::wstring> scaleFiles;
				std::wregex pattern (L"(scale-(\\d+))");
				for (auto it : files)
				{
					if (StrStrW (it.c_str (), L"contrast-white")) continue;
					std::wsmatch match;
					if (std::regex_search (it, match, pattern))
					{
						int temp = StrToIntW (match [1].str ().c_str ());
						if (!temp)
						{
							scaleFiles [temp] = it;
						}
					}
				}
				if (scaleFiles.find (GetDPI ()) != scaleFiles.end ())
				{
					isFindSuit = true;
					logoPath = scaleFiles [GetDPI ()];
				}
				else
				{
					bool isFindScale = false;
					for (auto it : scaleFiles)
					{
						if (it.first >= GetDPI ())
						{
							isFindScale = true;
							logoPath = scaleFiles [GetDPI ()];
							break;
						}
					}
					if (!isFindScale)
					{
						for (auto it : scaleFiles)
						{
							if (it.first >= 100)
							{
								isFindScale = true;
								logoPath = scaleFiles [GetDPI ()];
								break;
							}
						}
					}
					if (!isFindScale)
					{
						for (auto it : scaleFiles)
						{
							isFindScale = true;
							logoPath = scaleFiles [GetDPI ()];
							break;
						}
					}
					isFindSuit = isFindScale;
				}
			}
			if (!isFindSuit)
			{
				std::map <int, std::wstring> scaleFiles;
				std::wregex pattern (L"(scale-(\\d+))");
				for (auto it : files)
				{
					if (StrStrW (it.c_str (), L"contrast-black")) continue;
					std::wsmatch match;
					if (std::regex_search (it, match, pattern))
					{
						int temp = StrToIntW (match [1].str ().c_str ());
						if (!temp)
						{
							scaleFiles [temp] = it;
						}
					}
				}
				if (scaleFiles.find (GetDPI ()) != scaleFiles.end ())
				{
					isFindSuit = true;
					logoPath = scaleFiles [GetDPI ()];
				}
				else
				{
					bool isFindScale = false;
					for (auto it : scaleFiles)
					{
						if (it.first >= GetDPI ())
						{
							isFindScale = true;
							logoPath = scaleFiles [GetDPI ()];
							break;
						}
					}
					if (!isFindScale)
					{
						for (auto it : scaleFiles)
						{
							if (it.first >= 100)
							{
								isFindScale = true;
								logoPath = scaleFiles [GetDPI ()];
								break;
							}
						}
					}
					if (!isFindScale)
					{
						for (auto it : scaleFiles)
						{
							isFindScale = true;
							logoPath = scaleFiles [GetDPI ()];
							break;
						}
					}
					isFindSuit = isFindScale;
				}
			}
			if (!isFindSuit)
			{
				for (auto it : files)
				{
					logoPath = std::wstring (L"") + it;
					break;
				}
			}
		}
		imgfile = reader.extractFileToStream (logoPath);
	}
	if (getOutput) *getOutput = imgfile;
	if (!imgfile) return std::string ("");
	std::string b64res ("");
	LPSTR szRes = StreamToBase64 (imgfile);
	if (!getOutput) 
	{
		if (imgfile) 
		{
			imgfile->Release ();
			imgfile = NULL;
		}
	}
	if (!szRes) return b64res;
	if (szRes) b64res += szRes;
	free (szRes);
	if (b64res.length () > 0)
	{
		b64res = "data:image/png;base64," + b64res;
	}
	return b64res;
}

std::string GetBase64FromPath (PackageReader &reader, LPCWSTR lpswImgPath, IStream **getOutput = NULL, PriReader *priread = NULL)
{
	if (!&reader) return std::string ("");
	if (!reader.isAvailable ()) return std::string ("");
	if (!lpswImgPath) return std::string ("");
	std::wstring logoPath = std::wstring (lpswImgPath);
	if (logoPath.empty () || logoPath.length () == 0) return std::string ("");
	PriReader *pri = ((priread != NULL) ? priread : new PriReader (reader.getPriFileStream ()));
	if (pri->isAvailable ())
	{
		std::wstring logoPathFromPri = pri->findFilePathValue (logoPath);
		if (!logoPathFromPri.empty () && logoPath.length () > 0) logoPath = logoPathFromPri;
	}
	if (priread == NULL) delete pri;
	pri = NULL;
	IStream *imgfile = reader.extractFileToStream (logoPath);
	if (getOutput) *getOutput = imgfile;
	if (!imgfile)
	{
		std::wstring fileName = GetFileNameWithoutExtension (logoPath);
		IAppxPackageReader *appxreader = reader.getAppxPackageReader ();
		CComPtr <IAppxFilesEnumerator> fe = NULL;
		HRESULT hr = appxreader->GetPayloadFiles (&fe);
		if (SUCCEEDED (hr) && fe)
		{
			std::vector <std::wstring> files;
			BOOL hasCurrent = FALSE;
			hr = fe->GetHasCurrent (FALSE);
			while (SUCCEEDED (hr) && hasCurrent)
			{
				CComPtr <IAppxFile> ifile = NULL;
				hr = fe->GetCurrent (&ifile);
				if (SUCCEEDED (hr) && ifile)
				{
					LPWSTR lpstr = NULL;
					if (SUCCEEDED (ifile->GetName (&lpstr)) && lpstr)
					{
						std::wstring strWillSearch = StringToUpper (StringTrim (lpstr));
						std::wstring strWillToSearch = StringToUpper (StringTrim (fileName));
						if (StrStrW (strWillSearch.c_str (), strWillToSearch.c_str ()))
						{
							bool isFind = LabelEqual (PathFindExtensionW (lpstr), L".png");
							if (!isFind) isFind = LabelEqual (PathFindExtensionW (lpstr), L".jpg");
							if (!isFind) isFind = LabelEqual (PathFindExtensionW (lpstr), L".jpeg");
							if (!isFind) isFind = LabelEqual (PathFindExtensionW (lpstr), L".bmp");
							if (isFind)
							{
								std::wstring temp (L"");
								if (lpstr) temp += lpstr;
								if (temp.length () > 0) push_no_repeat (files, temp);
							}
						}
					}
				}
			}
			bool isFindSuit = false;
			{
				std::map <int, std::wstring> scaleFiles;
				std::wregex pattern (L"(scale-(\\d+))");
				for (auto it : files)
				{
					if (StrStrW (it.c_str (), L"contrast-")) continue;
					std::wsmatch match;
					if (std::regex_search (it, match, pattern))
					{
						int temp = StrToIntW (match [1].str ().c_str ());
						if (!temp)
						{
							scaleFiles [temp] = it;
						}
					}
				}
				if (scaleFiles.find (GetDPI ()) != scaleFiles.end ())
				{
					isFindSuit = true;
					logoPath = scaleFiles [GetDPI ()];
				}
				else
				{
					bool isFindScale = false;
					for (auto it : scaleFiles)
					{
						if (it.first >= GetDPI ())
						{
							isFindScale = true;
							logoPath = scaleFiles [GetDPI ()];
							break;
						}
					}
					if (!isFindScale)
					{
						for (auto it : scaleFiles)
						{
							if (it.first >= 100)
							{
								isFindScale = true;
								logoPath = scaleFiles [GetDPI ()];
								break;
							}
						}
					}
					if (!isFindScale)
					{
						for (auto it : scaleFiles)
						{
							isFindScale = true;
							logoPath = scaleFiles [GetDPI ()];
							break;
						}
					}
					isFindSuit = isFindScale;
				}
			}
			if (!isFindSuit)
			{
				std::map <int, std::wstring> scaleFiles;
				std::wregex pattern (L"(scale-(\\d+))");
				for (auto it : files)
				{
					if (StrStrW (it.c_str (), L"contrast-white")) continue;
					std::wsmatch match;
					if (std::regex_search (it, match, pattern))
					{
						int temp = StrToIntW (match [1].str ().c_str ());
						if (!temp)
						{
							scaleFiles [temp] = it;
						}
					}
				}
				if (scaleFiles.find (GetDPI ()) != scaleFiles.end ())
				{
					isFindSuit = true;
					logoPath = scaleFiles [GetDPI ()];
				}
				else
				{
					bool isFindScale = false;
					for (auto it : scaleFiles)
					{
						if (it.first >= GetDPI ())
						{
							isFindScale = true;
							logoPath = scaleFiles [GetDPI ()];
							break;
						}
					}
					if (!isFindScale)
					{
						for (auto it : scaleFiles)
						{
							if (it.first >= 100)
							{
								isFindScale = true;
								logoPath = scaleFiles [GetDPI ()];
								break;
							}
						}
					}
					if (!isFindScale)
					{
						for (auto it : scaleFiles)
						{
							isFindScale = true;
							logoPath = scaleFiles [GetDPI ()];
							break;
						}
					}
					isFindSuit = isFindScale;
				}
			}
			if (!isFindSuit)
			{
				std::map <int, std::wstring> scaleFiles;
				std::wregex pattern (L"(scale-(\\d+))");
				for (auto it : files)
				{
					if (StrStrW (it.c_str (), L"contrast-black")) continue;
					std::wsmatch match;
					if (std::regex_search (it, match, pattern))
					{
						int temp = StrToIntW (match [1].str ().c_str ());
						if (!temp)
						{
							scaleFiles [temp] = it;
						}
					}
				}
				if (scaleFiles.find (GetDPI ()) != scaleFiles.end ())
				{
					isFindSuit = true;
					logoPath = scaleFiles [GetDPI ()];
				}
				else
				{
					bool isFindScale = false;
					for (auto it : scaleFiles)
					{
						if (it.first >= GetDPI ())
						{
							isFindScale = true;
							logoPath = scaleFiles [GetDPI ()];
							break;
						}
					}
					if (!isFindScale)
					{
						for (auto it : scaleFiles)
						{
							if (it.first >= 100)
							{
								isFindScale = true;
								logoPath = scaleFiles [GetDPI ()];
								break;
							}
						}
					}
					if (!isFindScale)
					{
						for (auto it : scaleFiles)
						{
							isFindScale = true;
							logoPath = scaleFiles [GetDPI ()];
							break;
						}
					}
					isFindSuit = isFindScale;
				}
			}
			if (!isFindSuit)
			{
				for (auto it : files)
				{
					logoPath = std::wstring (L"") + it;
					break;
				}
			}
		}
		imgfile = reader.extractFileToStream (logoPath);
	}
	if (getOutput) *getOutput = imgfile;
	if (!imgfile) return std::string ("");
	std::string b64res ("");
	LPSTR szRes = StreamToBase64 (imgfile);
	if (!getOutput)
	{
		if (imgfile)
		{
			imgfile->Release ();
			imgfile = NULL;
		}
	}
	if (!szRes) return b64res;
	if (szRes) b64res += szRes;
	free (szRes);
	if (b64res.length () > 0)
	{
		b64res = "data:image/png;base64," + b64res;
	}
	return b64res;
}

std::wstring GetBase64FromPathW (PackageReader &reader, LPCWSTR lpswImgPath, IStream **getOutput = NULL, PriReader *priread = NULL)
{
	if (!&reader) return std::wstring (L"");
	if (!reader.isAvailable ()) return std::wstring (L"");
	if (!lpswImgPath) return std::wstring (L"");
	std::wstring logoPath = std::wstring (lpswImgPath);
	if (logoPath.empty () || logoPath.length () == 0) return std::wstring (L"");
	PriReader *pri = ((priread != NULL) ? priread : new PriReader (reader.getPriFileStream ()));
	if (pri->isAvailable ())
	{
		std::wstring logoPathFromPri = pri->findFilePathValue (logoPath);
		if (!logoPathFromPri.empty () && logoPath.length () > 0) logoPath = logoPathFromPri;
	}
	if (priread == NULL) delete pri;
	pri = NULL;
	IStream *imgfile = reader.extractFileToStream (logoPath);
	if (getOutput) *getOutput = imgfile;
	if (!imgfile)
	{
		std::wstring fileName = GetFileNameWithoutExtension (logoPath);
		IAppxPackageReader *appxreader = reader.getAppxPackageReader ();
		CComPtr <IAppxFilesEnumerator> fe = NULL;
		HRESULT hr = appxreader->GetPayloadFiles (&fe);
		if (SUCCEEDED (hr) && fe)
		{
			std::vector <std::wstring> files;
			BOOL hasCurrent = FALSE;
			hr = fe->GetHasCurrent (FALSE);
			while (SUCCEEDED (hr) && hasCurrent)
			{
				CComPtr <IAppxFile> ifile = NULL;
				hr = fe->GetCurrent (&ifile);
				if (SUCCEEDED (hr) && ifile)
				{
					LPWSTR lpstr = NULL;
					if (SUCCEEDED (ifile->GetName (&lpstr)) && lpstr)
					{
						std::wstring strWillSearch = StringToUpper (StringTrim (lpstr));
						std::wstring strWillToSearch = StringToUpper (StringTrim (fileName));
						if (StrStrW (strWillSearch.c_str (), strWillToSearch.c_str ()))
						{
							bool isFind = LabelEqual (PathFindExtensionW (lpstr), L".png");
							if (!isFind) isFind = LabelEqual (PathFindExtensionW (lpstr), L".jpg");
							if (!isFind) isFind = LabelEqual (PathFindExtensionW (lpstr), L".jpeg");
							if (!isFind) isFind = LabelEqual (PathFindExtensionW (lpstr), L".bmp");
							if (isFind)
							{
								std::wstring temp (L"");
								if (lpstr) temp += lpstr;
								if (temp.length () > 0) push_no_repeat (files, temp);
							}
						}
					}
				}
			}
			bool isFindSuit = false;
			{
				std::map <int, std::wstring> scaleFiles;
				std::wregex pattern (L"(scale-(\\d+))");
				for (auto it : files)
				{
					if (StrStrW (it.c_str (), L"contrast-")) continue;
					std::wsmatch match;
					if (std::regex_search (it, match, pattern))
					{
						int temp = StrToIntW (match [1].str ().c_str ());
						if (!temp)
						{
							scaleFiles [temp] = it;
						}
					}
				}
				if (scaleFiles.find (GetDPI ()) != scaleFiles.end ())
				{
					isFindSuit = true;
					logoPath = scaleFiles [GetDPI ()];
				}
				else
				{
					bool isFindScale = false;
					for (auto it : scaleFiles)
					{
						if (it.first >= GetDPI ())
						{
							isFindScale = true;
							logoPath = scaleFiles [GetDPI ()];
							break;
						}
					}
					if (!isFindScale)
					{
						for (auto it : scaleFiles)
						{
							if (it.first >= 100)
							{
								isFindScale = true;
								logoPath = scaleFiles [GetDPI ()];
								break;
							}
						}
					}
					if (!isFindScale)
					{
						for (auto it : scaleFiles)
						{
							isFindScale = true;
							logoPath = scaleFiles [GetDPI ()];
							break;
						}
					}
					isFindSuit = isFindScale;
				}
			}
			if (!isFindSuit)
			{
				std::map <int, std::wstring> scaleFiles;
				std::wregex pattern (L"(scale-(\\d+))");
				for (auto it : files)
				{
					if (StrStrW (it.c_str (), L"contrast-white")) continue;
					std::wsmatch match;
					if (std::regex_search (it, match, pattern))
					{
						int temp = StrToIntW (match [1].str ().c_str ());
						if (!temp)
						{
							scaleFiles [temp] = it;
						}
					}
				}
				if (scaleFiles.find (GetDPI ()) != scaleFiles.end ())
				{
					isFindSuit = true;
					logoPath = scaleFiles [GetDPI ()];
				}
				else
				{
					bool isFindScale = false;
					for (auto it : scaleFiles)
					{
						if (it.first >= GetDPI ())
						{
							isFindScale = true;
							logoPath = scaleFiles [GetDPI ()];
							break;
						}
					}
					if (!isFindScale)
					{
						for (auto it : scaleFiles)
						{
							if (it.first >= 100)
							{
								isFindScale = true;
								logoPath = scaleFiles [GetDPI ()];
								break;
							}
						}
					}
					if (!isFindScale)
					{
						for (auto it : scaleFiles)
						{
							isFindScale = true;
							logoPath = scaleFiles [GetDPI ()];
							break;
						}
					}
					isFindSuit = isFindScale;
				}
			}
			if (!isFindSuit)
			{
				std::map <int, std::wstring> scaleFiles;
				std::wregex pattern (L"(scale-(\\d+))");
				for (auto it : files)
				{
					if (StrStrW (it.c_str (), L"contrast-black")) continue;
					std::wsmatch match;
					if (std::regex_search (it, match, pattern))
					{
						int temp = StrToIntW (match [1].str ().c_str ());
						if (!temp)
						{
							scaleFiles [temp] = it;
						}
					}
				}
				if (scaleFiles.find (GetDPI ()) != scaleFiles.end ())
				{
					isFindSuit = true;
					logoPath = scaleFiles [GetDPI ()];
				}
				else
				{
					bool isFindScale = false;
					for (auto it : scaleFiles)
					{
						if (it.first >= GetDPI ())
						{
							isFindScale = true;
							logoPath = scaleFiles [GetDPI ()];
							break;
						}
					}
					if (!isFindScale)
					{
						for (auto it : scaleFiles)
						{
							if (it.first >= 100)
							{
								isFindScale = true;
								logoPath = scaleFiles [GetDPI ()];
								break;
							}
						}
					}
					if (!isFindScale)
					{
						for (auto it : scaleFiles)
						{
							isFindScale = true;
							logoPath = scaleFiles [GetDPI ()];
							break;
						}
					}
					isFindSuit = isFindScale;
				}
			}
			if (!isFindSuit)
			{
				for (auto it : files)
				{
					logoPath = std::wstring (L"") + it;
					break;
				}
			}
		}
		imgfile = reader.extractFileToStream (logoPath);
	}
	if (getOutput) *getOutput = imgfile;
	if (!imgfile) return std::wstring (L"");
	std::wstring b64res (L"");
	LPWSTR szRes = StreamToBase64W (imgfile);
	if (!getOutput)
	{
		if (imgfile)
		{
			imgfile->Release ();
			imgfile = NULL;
		}
	}
	if (!szRes) return b64res;
	if (szRes) b64res += szRes;
	free (szRes);
	if (b64res.length () > 0)
	{
		b64res = L"data:image/png;base64," + b64res;
	}
	return b64res;
}
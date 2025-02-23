#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#ifndef _CRT_NON_CONFORMING_SWPRINTFS
#define _CRT_NON_CONFORMING_SWPRINTFS
#endif
#include <Windows.h>
#include <string>
#include <iostream>
#include <cstdarg>
#include <dwmapi.h>
#include <wingdi.h>
#include <ShObjIdl.h>
#include "pkgcode.h"
#include "resource.h"
#include "res2.h"
#include "PackageManager.h"
#include "base64logo.h"
#include "AppLauncher.h"
#include "ToastNotification.h"
#include "pkginfo.h"
#include "CertificateManager.h"
#include "initread.h"
#include <Psapi.h>
#include <iomanip>

using namespace std;

std::wstring GetRCString_cpp (UINT resID)
{
	size_t bufferSize = 256;
	wchar_t* buffer = nullptr;
	int length = 0;
	do
	{
		delete [] buffer;
		buffer = new wchar_t [bufferSize];
		length = LoadStringW (GetModuleHandle (nullptr), resID, buffer, bufferSize);
		if ((size_t)length >= bufferSize)
		{
			bufferSize += 20;
		}
	} while ((size_t)length >= bufferSize);
	std::wstring result = L"";
	result += buffer;
	delete [] buffer;
	return result;
}
#define GetRCString GetRCString_cpp
#define rcIcon(resID) LoadRCIcon (resID)

HRSRC FindResourceByName (LPCWSTR resourceName, LPCWSTR resourceType)
{
	return FindResourceW (GetModuleHandle (nullptr), resourceName, resourceType);
}
HRSRC FindResourceByName (const std::wstring &resourceName, LPCWSTR resourceType)
{
	return FindResourceByName (resourceName.c_str (), resourceType);
}

std::wstring StrPrintFormatW (LPCWSTR format, ...)
{
	va_list args;
	va_start (args, format);
	size_t sizeBuf = 256;
	WCHAR *buffer = (WCHAR *)malloc (sizeBuf * sizeof (WCHAR));
	if (!buffer)
	{
		va_end (args);
		return L"";
	}
	int written = -1;
	while (true)
	{
		written = vswprintf (buffer, sizeBuf, format, args);
		if (written >= 0 && static_cast<size_t>(written) < sizeBuf)
		{
			break;
		}
		sizeBuf += 256;
		wchar_t* newBuffer = (wchar_t*)realloc (buffer, sizeBuf * sizeof (wchar_t));
		if (!newBuffer)
		{
			free (buffer);
			va_end (args);
			return L"";
		}
		buffer = newBuffer;
	}
	va_end (args);
	std::wstring result (buffer);
	free (buffer);
	return result;
}

size_t EnumerateFilesW (const std::wstring &directory, const std::wstring &filter,
	std::vector <std::wstring> &outFiles, bool recursive = false)
{
	std::wstring searchPath = directory;
	if (!searchPath.empty () && searchPath.back () != L'\\')
	{
		searchPath += L'\\';
	}
	searchPath += filter;
	WIN32_FIND_DATAW findData;
	HANDLE hFind = FindFirstFileW (searchPath.c_str (), &findData);
	if (hFind != INVALID_HANDLE_VALUE) 
	{
		do {
			if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) 
			{
				outFiles.push_back (directory + L"\\" + findData.cFileName);
			}
		} while (FindNextFileW (hFind, &findData));
		FindClose (hFind);
	}
	if (recursive) {
		std::wstring subDirSearchPath = directory + L"\\*";
		hFind = FindFirstFileW (subDirSearchPath.c_str (), &findData);
		if (hFind != INVALID_HANDLE_VALUE) 
		{
			do {
				if ((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
					wcscmp (findData.cFileName, L".") != 0 && wcscmp (findData.cFileName, L"..") != 0) 
				{
					EnumerateFilesW (directory + L"\\" + findData.cFileName, filter, outFiles, true);
				}
			} while (FindNextFileW (hFind, &findData));
			FindClose (hFind);
		}
	}
	return outFiles.size ();
}

std::wstring GetFileDirectoryW (const std::wstring &filePath)
{
	std::wstring directory (L"");
	WCHAR *buf = (WCHAR *)calloc (filePath.capacity () + 1, sizeof (WCHAR));
	lstrcpyW (buf, filePath.c_str ());
	PathRemoveFileSpecW (buf);
	directory += buf;
	free (buf);
	return directory;
}

std::wstring g_pkgPath = L"";
pkgreader g_reader;
LPCWSTR g_idenName = L"Microsoft.DesktopAppInstaller";
PackageInfomation g_pkgInfo;

HRESULT SetCurrentAppUserModelID (PCWSTR appID)
{
	typedef HRESULT (WINAPI *SetAppUserModelIDFunc)(PCWSTR);
	HMODULE shell32 = LoadLibraryW (L"shell32.dll");
	if (!shell32) return E_FAIL;
	auto SetAppUserModelID = (SetAppUserModelIDFunc)GetProcAddress (shell32, "SetCurrentProcessExplicitAppUserModelID");
	if (!SetAppUserModelID)
	{
		FreeLibrary (shell32);
		return E_FAIL;
	}
	HRESULT hr = SetAppUserModelID (appID);
	FreeLibrary (shell32);
	return hr;
}

typedef struct _CMDARGUMENT
{
	std::vector <std::wstring> front;
	std::vector <std::wstring> body;
	std::vector <std::wstring> rear;
	int flag;
} CMDARGUMENT;

std::vector <CMDARGUMENT> cmdargs =
{
	{{L"", L"/", L"-"}, {L"HELP", L"?", L"H"}, {}, 5},
	{{L"", L"/", L"-"}, {L"ReadInfo", L"Read", L"Info", L"ReadOnly", L"ReadInfoOnly"}, {}, 1},
	{{L"", L"/", L"-"}, {L"LoadCertOnly", L"Load", L"LoadOnly", L"Cert", L"CertOnly", L"LoadCert"}, {}, 2},
	{{L"", L"/", L"-"}, {L"InstallOnly", L"Install", L"AddPackageOnly", L"Add", L"AddOnly", L"InstallOnly"}, {}, 3}
};

// 编号为大于 0 的数，失败返回非正数
int GetCmdArgSerial (std::wstring cmdparam)
{
#define tolabel(__std_wstring__) StringToUpper (StringTrim (__std_wstring__))
	std::wstring arg = tolabel (cmdparam);
	for (size_t cnt = 0; cnt < cmdargs.size (); cnt ++)
	{
		CMDARGUMENT &tca = cmdargs [cnt];
		std::wstring tempP = L"";
		for (size_t i = 0; i < tca.body.size (); i ++)
		{
			if (tca.rear.size () > 0)
			{
				if (tca.front.size () > 0)
				{
					for (size_t j = 0; j < tca.front.size (); j ++)
					{
						for (size_t k = 0; k < tca.rear.size (); k ++)
						{
							tempP = tolabel (tca.front [j]) + tolabel (tca.body [i]) + tolabel (tca.rear [k]);
							std::wstring t1;
							if (tolabel (tca.rear [k]).length () > 0) t1 = arg.substr (0, tempP.length ());
							else t1 = tempP;
							if (t1 == arg) return tca.flag;
						}
					}
				}
				else
				{
					for (size_t k = 0; k < tca.rear.size (); k ++)
					{
						tempP = tolabel (tca.body [i]) + tolabel (tca.rear [k]);
						std::wstring t1;
						if (tolabel (tca.rear [k]).length () > 0) t1 = arg.substr (0, tempP.length ());
						else t1 = tempP;
						if (t1 == arg) return tca.flag;
					}
				}
			}
			else
			{
				if (tca.front.size () > 0)
				{
					for (size_t j = 0; j < tca.front.size (); j ++)
					{
						tempP = tolabel (tca.front [j]) + tolabel (tca.body [i]);
						std::wstring &t1 = tempP;
						if (t1 == arg) return tca.flag;
					}
				}
				else
				{
					tempP = tolabel (tca.body [i]);
					std::wstring &t1 = tempP;
					if (t1 == arg) return tca.flag;
				}
			}
		}
	}
	return 0; // 返回 0 正好适用于 C/C++ 的逻辑判断
}
int GetCmdArgSerial (LPCWSTR cmdparam) { return GetCmdArgSerial (std::wstring (cmdparam)); }

void ReadPackageInfo ();
void TaskInstallPackage ();
void TaskInstallPackages (size_t serial, size_t total);
bool LoadCert ();
bool InstallPackage ();
void PrintHelpText ()
{
	wchar_t path [MAX_PATH] = {0};
	GetModuleFileNameW (NULL, path, MAX_PATH);
	std::wcout << "Usage: " << endl;
	std::wcout << L'\t' << PathFindFileNameW (path) << L" <package_file_path(-s)>" << endl;
	std::wcout << L"\t\t" << "Install package(-s)." << endl;
	std::wcout << L'\t' << PathFindFileNameW (path) << L" <package_file_path(-s)> [/readinfo] [/loadcert] [/install]" << endl;
	std::wcout << L"\t\t" << "/readinfo" << endl;
	std::wcout << L"\t\t\t" << "Read package(-s) infomation." << endl;
	std::wcout << L"\t\t" << "/loadcert" << endl;
	std::wcout << L"\t\t\t" << "Load certificate from package(-s)." << endl;
	std::wcout << L"\t\t" << "/install" << endl;
	std::wcout << L"\t\t\t" << "Install package(-s). (Only perform installation tasks)" << endl;
	std::wcout << L"\t\t" << L"These command line parameters can be used cumulatively." << endl;
	std::wcout << L'\t' << PathFindFileNameW (path) << L" /?" << endl;
	std::wcout << L"\t\t" << "Show help." << endl;
}
void middle (wstring s) 
{
	HANDLE hOutput = GetStdHandle (STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO bInfo;
	GetConsoleScreenBufferInfo (hOutput, &bInfo);
	int len = bInfo.dwSize.X / 2 - s.size () / 2;
	wprintf (L"\033[%dC", len);
	wcout << s;
}

// 返回真时代表执行 GUI 进程
bool ReadCommand (int argc, LPWSTR *argv)
{
	std::vector <std::wstring> vecObjSwFiles;
	bool bDisplayHelp = false, bReadInfoOnly = false, bLoadCertOnly = false, bInstallOnly = false;
	for (size_t cnt = 1; cnt < (size_t)argc; cnt ++)
	{
		int res = GetCmdArgSerial (argv [cnt]);
		if (res > 0)
		{
			switch (res)
			{
				case 1: bReadInfoOnly = true; break;
				case 2: bLoadCertOnly = true; break;
				case 3: bInstallOnly = true; break;
				case 5: bDisplayHelp = true; break;
			}
		}
		else
		{
			if (IsFileExistsW (argv [cnt])) push_no_repeat (vecObjSwFiles, argv [cnt]);
			else
			{
				wcerr << "Error: Unrecognized command line arguments: " << argv [cnt] << endl;
				return false;
			}
		}
	}
	if (bDisplayHelp) // 处理命令参数提示
	{
		PrintHelpText ();
		return false;
	}
	if (vecObjSwFiles.size () == 1)
	{
		g_pkgPath = vecObjSwFiles [0];
		if (bReadInfoOnly)
		{
			if (g_reader.create (g_pkgPath)) ReadPackageInfo ();
			else wcerr << "Error: Unavailable Package." << endl;
		}
		if (bLoadCertOnly)
		{
			LoadCert ();
		}
		if (bInstallOnly)
		{
			InstallPackage ();
		}
		if (bReadInfoOnly || bLoadCertOnly || bInstallOnly) return false;
		return true;
	}
	else if (vecObjSwFiles.size () <= 0)
	{
		wchar_t path [MAX_PATH] = {0};
		if (GetModuleFileNameW (NULL, path, MAX_PATH))
		{
			std::wstring cmdline = L"\"" + std::wstring (path) + L"\" /?";
			STARTUPINFOW si = {sizeof (STARTUPINFOW)};
			PROCESS_INFORMATION pi = {0};
			if (CreateProcessW (NULL, (LPWSTR)cmdline.c_str (), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
			{
				WaitForSingleObject (pi.hProcess, INFINITE);
				CloseHandle (pi.hProcess);
				CloseHandle (pi.hThread);
			}
		}
		return false;
	}
	else if (vecObjSwFiles.size () > 1) // 面对多个文件
	{
		wchar_t path [MAX_PATH] = {0};
		if (GetModuleFileNameW (NULL, path, MAX_PATH))
		{
			for (size_t cnt = 0; cnt < vecObjSwFiles.size (); cnt ++)
			{
				g_pkgPath = vecObjSwFiles [cnt];
				wstringstream ss;
				ss << (cnt + 1) << L" of " << vecObjSwFiles.size () << endl;
				middle (ss.str ().c_str ());
				std::wstring cmdline = L"\"" + std::wstring (path) + L"\" \"" + g_pkgPath + L'\"';
				if (bReadInfoOnly) cmdline += L" /READINFO";
				STARTUPINFOW si = {sizeof (STARTUPINFOW)};
				PROCESS_INFORMATION pi = {0};
				if (CreateProcessW (NULL, (LPWSTR)cmdline.c_str (), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
				{
					WaitForSingleObject (pi.hProcess, INFINITE);
					CloseHandle (pi.hProcess);
					CloseHandle (pi.hThread);
				}
			}
		}
	}
	return false;
}

// 返回真时代表执行 GUI 进程
bool ReadCommand (LPWSTR lpCmdLine)
{
	int iArgc = 0;
	LPWSTR *lpArgv = CommandLineToArgvW (lpCmdLine, &iArgc);
	return ReadCommand (iArgc, lpArgv);
}

void ProgressCallback (unsigned uPrecent)
{
	wcout << L'\r';
	wcout << StrPrintFormatW (GetRCString_cpp (PAGE_2_INSTALLING).c_str (), (int)uPrecent);
}

void ReadPackageInfo ()
{
	if (g_reader.isAvailable ())
	{
		wcout << GetRCString_cpp (PAGE_LOADING);
		g_pkgInfo.create (g_reader);
		wcout << L"\r";
		wcout << L"Package File: " << g_pkgPath << endl;
		wcout << L'\t' << L"Identity" << endl;
		wcout << L"\t\t" << L"Name: " << g_pkgInfo.getIdentityName () << endl;
		wcout << L"\t\t" << L"Publisher: " << g_pkgInfo.getIdentityPublisher () << endl;
		wcout << L"\t\t" << L"Version: " << g_pkgInfo.getIdentityVersion ().toStringW () << endl;
		std::vector <APPX_PACKAGE_ARCHITECTURE> archs;
		wcout << L"\t\t" << L"Processor Architecture (" << g_pkgInfo.getIdentityProcessorArchitecture (archs) << "): ";
		if (archs.size () > 0)
		{
			std::wstring temp;
			for (size_t cnt = 0; cnt < archs.size (); cnt ++)
			{
				switch (archs [cnt])
				{
					case APPX_PACKAGE_ARCHITECTURE_NEUTRAL: temp += L"Neutral"; break;
					case APPX_PACKAGE_ARCHITECTURE_X86: temp += L"x86"; break;
					case APPX_PACKAGE_ARCHITECTURE_X64: temp += L"x64"; break;
					case APPX_PACKAGE_ARCHITECTURE_ARM: temp += L"Arm"; break;
					default:
						if ((UINT)archs [cnt] == 12) temp += L"Arm64";
				}
				if (cnt + 1 != archs.size ()) temp += L", ";
			}
			wcout << temp;
		}
		wcout << endl;
		wcout << L'\t' << L"Properties" << endl;
		wcout << L"\t\t" << L"Name: " << g_pkgInfo.getPropertyName () << endl;
		wcout << L"\t\t" << L"Publisher: " << g_pkgInfo.getPropertyPublisher () << endl;
		{
			std::wstring &temp = g_pkgInfo.getPropertyDescription ();
			if (temp.size () > 0)
			{
				wcout << L"\t\t" << L"Description: " << temp << endl;
			}
		}
		{
			std::vector <std::wstring> langs;
			g_pkgInfo.getResourceLanguages (langs);
			if (langs.size () > 0)
			{
				wcout << L"\t" << L"Resources" << endl;
				wcout << L"\t\t" << L"Languages (" << langs.size () << "): ";
				for (size_t cnt = 0; cnt < langs.size (); cnt ++)
				{
					wcout << langs [cnt];
					if (cnt + 1 != langs.size ()) wcout << L", ";
				}
				wcout << endl;
			}
		}
		std::vector <appmap> apps;
		g_pkgInfo.getApplications (apps);
		wcout << L'\t' << L"Applications (" << apps.size () << ")" << endl;
		if (apps.size () > 0)
		{
			for (auto it : apps)
			{
				wcout << L"\t\t" << L"Application" << endl;
				wcout << L"\t\t\t" << L"App Model User ID: " << it.getAppUserModelID () << endl;
				wcout << L"\t\t\t" << L"ID: " << it [L"Id"] << endl;
			}
		}
		std::vector <std::wstring> caps;
		g_pkgInfo.getCapabilities (caps);
		wcout << L'\t' << L"Capabilities (" << caps.size () << ")" << endl;
		if (caps.size () > 0)
		{
			for (auto it : caps)
			{
				wcout << L"\t\t" << GetRCString_cpp (CapabilitiesNameToID (it)) <<
					L" ( " << it << L" )" << endl;
			}
		}
		std::vector <DEPINFO> deps;
		wcout << L'\t' << L"Dependencies (" << deps.size () << ")" << endl;
		for (auto it : deps)
		{
			wcout << L"\t\t" << L"Dependency" << endl;
			wcout << L"\t\t\t" << L"Name: " << it.name << endl;
			wcout << L"\t\t\t" << L"Publisher: " << it.publisher << endl;
			wcout << L"\t\t\t" << L"Version: " << it.versionMin.toStringW () << endl;
		}
	}
}

bool LoadCert ()
{
	bool res = false;
	wcout << GetRCString_cpp (PAGE_2_LOADCERT);
	if (res = LoadCertFromSignedFile (g_pkgPath.c_str ())) wcout << L"Succeeded! " << endl;
	else wcout << "Failed. " << endl;
	return res;
}

bool InstallPackage ()
{
	InstallStatus ires = AddPackageFromPath (g_pkgPath.c_str (), &ProgressCallback);
	wcout << endl;
	if (ires == InstallStatus::Success)
	{
		wcout << "Install Succeeded! " << endl;
	}
	else
	{
		wcerr << "\rError: Install Failed. Message: " << GetLastErrorDetailText () << endl;
	}
	return ires == InstallStatus::Success;
}

void TaskInstallPackage ()
{
	if (g_reader.create (g_pkgPath))
	{
		ReadPackageInfo ();
		wcout << StrPrintFormatW (GetRCString_cpp (PAGE_2_TITLE).c_str (), g_pkgInfo.getPropertyName ().c_str ()) << endl;
		wcout << GetRCString_cpp (PAGE_2_INSTALL);
		LoadCert ();
		InstallPackage ();
	}
	else
	{
		wcerr << "\rError: Install Failed. Message: Unavailable Package." << endl;
	}
}

void TaskInstallPackages (size_t serial, size_t total)
{
	wcout << serial << L" of " << total << endl;
	wcout << L'\t' << GetRCString_cpp (PAGE_LOADING);
	g_reader.destroy ();
	if (g_reader.create (g_pkgPath))
	{
		g_pkgInfo.create (g_reader);
		wcout << L"\r";
		wcout << L"\tPackage File: " << g_pkgPath << endl;
		wcout << L"\t\t" << L"Identity" << endl;
		wcout << L"\t\t\t" << L"Name: " << g_pkgInfo.getIdentityName () << endl;
		wcout << L"\t\t\t" << L"Publisher: " << g_pkgInfo.getIdentityPublisher () << endl;
		wcout << L"\t\t\t" << L"Version: " << g_pkgInfo.getIdentityVersion ().toStringW () << endl;
		std::vector <APPX_PACKAGE_ARCHITECTURE> archs;
		wcout << L"\t\t\t" << L"Processor Architecture (" << g_pkgInfo.getIdentityProcessorArchitecture (archs) << ") ";
		if (archs.size () > 0)
		{
			std::wstring temp;
			for (size_t cnt = 0; cnt < archs.size (); cnt ++)
			{
				switch (archs [cnt])
				{
					case APPX_PACKAGE_ARCHITECTURE_NEUTRAL: temp += L"Neutral"; break;
					case APPX_PACKAGE_ARCHITECTURE_X86: temp += L"x86"; break;
					case APPX_PACKAGE_ARCHITECTURE_X64: temp += L"x64"; break;
					case APPX_PACKAGE_ARCHITECTURE_ARM: temp += L"Arm"; break;
					default:
						if ((UINT)archs [cnt] == 12) temp += L"Arm64";
				}
				if (cnt + 1 != archs.size ()) temp += L", ";
			}
			wcout << temp;
		}
		wcout << endl;
		wcout << L"\t\t" << L"Properties" << endl;
		wcout << L"\t\t\t" << L"Name: " << g_pkgInfo.getPropertyName () << endl;
		wcout << L"\t\t\t" << L"Publisher: " << g_pkgInfo.getPropertyPublisher () << endl;
		{
			std::wstring &temp = g_pkgInfo.getPropertyDescription ();
			if (temp.size () > 0)
			{
				wcout << L"\t\t\t" << L"Description: " << temp << endl;
			}
		}
		std::vector <appmap> apps;
		g_pkgInfo.getApplications (apps);
		wcout << L"\t" << L"Applications (" << apps.size () << ")" << endl;
		if (apps.size () > 0)
		{
			for (auto it : apps)
			{
				wcout << L"\t\t\t" << L"Application" << endl;
				wcout << L"\t\t\t\t" << L"App Model User ID: " << it.getAppUserModelID () << endl;
			}
		}
		wcout << StrPrintFormatW (GetRCString_cpp (PAGE_2_TITLE).c_str (), g_pkgInfo.getPropertyName ().c_str ()) << endl;
		wcout << GetRCString_cpp (PAGE_2_LOADCERT);
		if (LoadCertFromSignedFile (g_pkgPath.c_str ())) wcout << L"\tSucceeded! " << endl;
		else wcout << "\tFailed. " << endl;
		wcout << GetRCString_cpp (PAGE_2_INSTALL);
		InstallStatus ires = AddPackageFromPath (g_pkgPath.c_str (), &ProgressCallback);
		wcout << endl;
		if (ires == InstallStatus::Success)
		{
			wcout << "\tInstall Succeeded! " << endl;
		}
		else
		{
			wcout << "\tInstall Failed. Message: " << GetLastErrorDetailText () << endl;
		}
	}
	else
	{
		wcout << "\r\tInstall Failed. Message: Unavailable Package." << endl;
	}
}

int wmain (int argc, LPWSTR *argv)
{
	
	if (argc <= 1)
	{
		PrintHelpText ();
		return 0;
	}
	setlocale (LC_ALL, "");
	std::wcout.imbue (std::locale ("", LC_CTYPE));
	SetCurrentProcessExplicitAppUserModelID (g_idenName);
	wchar_t currentPath [MAX_PATH];
	GetCurrentDirectory (MAX_PATH, currentPath);
	wchar_t executablePath [MAX_PATH];
	GetModuleFileName (NULL, executablePath, MAX_PATH);
	std::wstring executableDir (executablePath);
	executableDir = executableDir.substr (0, executableDir.find_last_of (L"\\/"));
	if (!wcscmp (currentPath, executableDir.c_str ()))
	{
		SetCurrentDirectory (executableDir.c_str ());
	}
	CoInitializeEx (NULL, COINIT_MULTITHREADED);
	if (ReadCommand (argc, argv))
	{
		TaskInstallPackage ();
	}
	g_reader.destroy ();
	return 0;
}
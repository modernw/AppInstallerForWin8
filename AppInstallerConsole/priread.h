#pragma once
// 由于难用 PRI 相关 API，只得使用 makepri 了
#include <Windows.h>
#include <string>
#include <iostream>
#include "localeex.h"
#include <vector>
#include "pkgcode.h"
#include "PriReader2.h"

std::string GetProgramRootDirectoryA ()
{
	char path [MAX_PATH];
	if (GetModuleFileNameA (NULL, path, MAX_PATH))
	{
		std::string dir (path);
		size_t pos = dir.find_last_of ("\\/");
		if (pos != std::string::npos)
		{
			dir = dir.substr (0, pos);
		}
		return dir;
	}
	return "";
}
std::wstring GetProgramRootDirectoryW ()
{
	wchar_t path [MAX_PATH];
	if (GetModuleFileNameW (NULL, path, MAX_PATH))
	{
		std::wstring dir (path);
		size_t pos = dir.find_last_of (L"\\/");
		if (pos != std::wstring::npos)
		{
			dir = dir.substr (0, pos);
		}
		return dir;
	}
	return L"";
}
std::string EnsureTrailingSlash (const std::string &path)
{
	if (path.empty ()) return path;  // 空路径直接返回

	char lastChar = path.back ();
	if (lastChar == '\\' || lastChar == '/')
		return path;  // 已有分隔符，直接返回
					  // 根据系统或原路径格式添加适当的分隔符
	char separator = (path.find ('/') != std::string::npos) ? '/' : '\\';
	return path + separator;
}
std::wstring EnsureTrailingSlash (const std::wstring &path)
{
	if (path.empty ()) return path;

	wchar_t lastChar = path.back ();
	if (lastChar == L'\\' || lastChar == L'/')
		return path;

	wchar_t separator = (path.find (L'/') != std::wstring::npos) ? L'/' : L'\\';
	return path + separator;
}
bool IsDirectoryExists (const std::string path) { return IsDirectoryExistsA (path.c_str ()); }
bool IsDirectoryExists (const std::wstring path) { return IsDirectoryExistsW (path.c_str ()); }
bool IsDirectoryExists (LPCSTR path) { return IsDirectoryExistsA (path); }
bool IsDirectoryExists (LPCWSTR path) { return IsDirectoryExistsW (path); }
std::string NormalizePath (const std::string &path)
{
	if (!path.empty () && path.back () == '\\')
		return path.substr (0, path.size () - 1);
	return path;
}
std::wstring NormalizePath (const std::wstring &path)
{
	if (!path.empty () && path.back () == L'\\')
		return path.substr (0, path.size () - 1);
	return path;
}
std::vector <std::string> EnumSubdirectories (const std::string &directory, bool includeParentPath)
{
	std::vector<std::string> subdirs;
	std::string normPath = NormalizePath (directory); 
	std::string searchPath = normPath + "\\*"; 
	WIN32_FIND_DATAA findData;
	HANDLE hFind = FindFirstFileA (searchPath.c_str (), &findData);
	if (hFind == INVALID_HANDLE_VALUE) return subdirs; 
	do
	{
		// 过滤 "." 和 ".."
		if (strcmp (findData.cFileName, ".") == 0 || strcmp (findData.cFileName, "..") == 0)
			continue;
		// 判断是否为目录
		if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (includeParentPath)
				subdirs.push_back (normPath + "\\" + findData.cFileName);
			else
				subdirs.push_back (findData.cFileName);
		}
	} while (FindNextFileA (hFind, &findData));
	FindClose (hFind);
	return subdirs;
}
std::vector <std::wstring> EnumSubdirectories (const std::wstring &directory, bool includeParentPath)
{
	std::vector<std::wstring> subdirs;
	std::wstring normPath = NormalizePath (directory); 
	std::wstring searchPath = normPath + L"\\*"; 
	WIN32_FIND_DATAW findData;
	HANDLE hFind = FindFirstFileW (searchPath.c_str (), &findData);
	if (hFind == INVALID_HANDLE_VALUE) return subdirs; 
	do
	{
		if (wcscmp (findData.cFileName, L".") == 0 || wcscmp (findData.cFileName, L"..") == 0)
			continue;
		if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (includeParentPath)
				subdirs.push_back (normPath + L"\\" + findData.cFileName);
			else
				subdirs.push_back (findData.cFileName);
		}
	} while (FindNextFileW (hFind, &findData));
	FindClose (hFind);
	return subdirs;
}
bool CheckDependency (const std::string &dllName)
{
	HMODULE hModule = LoadLibraryA (dllName.c_str ());
	if (hModule == NULL)
	{
		return false; 
	}
	FreeLibrary (hModule); 
	return true;
}
bool CheckDependency (const std::wstring &dllName)
{
	HMODULE hModule = LoadLibraryW (dllName.c_str ());
	if (hModule == NULL)
	{
		return false; 
	}
	FreeLibrary (hModule);
	return true;
}
bool CanRunExe (const std::string &exePath)
{
	STARTUPINFOA si = {sizeof (si)};
	PROCESS_INFORMATION pi;
	BOOL success = CreateProcessA (
		exePath.c_str (),   // 应用路径
		NULL,              // 命令行参数
		NULL,              // 安全属性
		NULL,              // 线程安全属性
		FALSE,             // 不继承句柄
		CREATE_NO_WINDOW,  // 不创建窗口
		NULL,              // 环境变量
		NULL,              // 当前目录
		&si,               // 启动信息
		&pi                // 进程信息
	);
	if (!success)
	{
		DWORD error = GetLastError ();
		return false; 
	}
	WaitForSingleObject (pi.hProcess, INFINITE);
	DWORD exitCode;
	GetExitCodeProcess (pi.hProcess, &exitCode);
	CloseHandle (pi.hProcess);
	CloseHandle (pi.hThread);
	return (exitCode == 0);
}
bool CanRunExe (const std::wstring& exePath)
{
	STARTUPINFOW si = {sizeof (si)};
	PROCESS_INFORMATION pi;
	BOOL success = CreateProcessW (
		exePath.c_str (),   // 应用路径
		NULL,              // 命令行参数
		NULL,              // 安全属性
		NULL,              // 线程安全属性
		FALSE,             // 不继承句柄
		CREATE_NO_WINDOW,  // 不创建窗口
		NULL,              // 环境变量
		NULL,              // 当前目录
		&si,               // 启动信息
		&pi                // 进程信息
	);
	if (!success)
	{
		DWORD error = GetLastError ();
		return false; 
	}
	WaitForSingleObject (pi.hProcess, INFINITE);
	DWORD exitCode;
	GetExitCodeProcess (pi.hProcess, &exitCode);
	CloseHandle (pi.hProcess);
	CloseHandle (pi.hThread);
	return (exitCode == 0);
}
bool IsProgramExecutable (const std::string &exePath)
{
	if (!IsFileExists (exePath))
	{
		return false;
	}
	if (!CanRunExe (exePath))
	{
		return false;
	}
	return true;
}
bool IsProgramExecutable (const std::wstring &exePath)
{
	if (!IsFileExists (exePath))
	{
		return false;
	}
	if (!CanRunExe (exePath))
	{
		return false;
	}
	return true;
}
std::wstring GetUniqueTempFilePathW () 
{
	// 获取临时文件夹路径
	wchar_t tempPath [MAX_PATH];
	if (GetTempPath (MAX_PATH, tempPath) == 0) 
	{
		return L"";
	}
	// 创建一个唯一的临时文件
	wchar_t tempFile [MAX_PATH];
	if (GetTempFileName (tempPath, L"TMP", 0, tempFile) == 0) 
	{
		return L"";
	}
	return tempFile;
}
std::string GetUniqueTempFilePathA () {
	// 获取临时文件夹路径
	char tempPath [MAX_PATH];
	if (GetTempPathA (MAX_PATH, tempPath) == 0) 
	{
		return "";
	}
	// 创建一个唯一的临时文件
	char tempFile [MAX_PATH];
	if (GetTempFileNameA (tempPath, "TMP", 0, tempFile) == 0) 
	{
		return "";
	}
	return tempFile;
}
int InStr (const std::string &text, const std::string &keyword, bool ignoreCase = false)
{
	std::string s1, s2;
	if (ignoreCase)
	{
		s1 = StringToUpper (text);
		s2 = StringToUpper (keyword);
	}
	else
	{
		s1 = text;
		s2 = keyword;
	}
	const char *found = StrStrIA (s1.c_str (), s2.c_str ());
	if (!found) 
	{
		return -1;
	}
	return found - text.c_str ();
}
int InStr (const std::wstring &text, const std::wstring &keyword, bool ignoreCase = false)
{
	std::wstring s1, s2;
	if (ignoreCase)
	{
		s1 = StringToUpper (text);
		s2 = StringToUpper (keyword);
	}
	else
	{
		s1 = text;
		s2 = keyword;
	}
	const WCHAR *found = StrStrIW (s1.c_str (), s2.c_str ());
	if (!found) 
	{
		return -1;
	}
	return found - text.c_str ();
}

template <typename StringType> StringType ReplaceBackslash (const StringType &input)
{
	StringType result = input;  // 复制输入字符串，避免修改原数据
	using CharType = typename StringType::value_type;  // 获取字符串的字符类型（char 或 wchar_t）
	std::replace (result.begin (), result.end (), static_cast<CharType> ('\\'), static_cast<CharType> ('/'));
	return result;
}

class PriReader
{
	private:
	HPRIREADER priReader;
	public:
	PriReader (LPCWSTR lpPriFilePath) { this->create (lpPriFilePath); }
	PriReader (const std::wstring objPriFilePath) { this->create (objPriFilePath); }
	PriReader (IStream *comISPriFile) { this->create (comISPriFile); }
	~PriReader () { this->destroy (); }
	PriReader (): priReader (NULL) {}
	void destroy ()
	{
		if (!priReader) return;
		DestroyPriReader (priReader);
		priReader = NULL;
	}
	bool isAvailable () { return priReader != nullptr; }
	bool create (const std::wstring priFile) { destroy (); return priReader = CreatePriReader (priFile.c_str ()); }
	bool create (LPCWSTR priFile) { destroy (); if (!priFile) return false; return this->create (std::wstring (priFile)); }
	bool create (IStream *priFStream) { destroy (); return priReader = CreatePriReaderFromStream (priFStream); }
	// 获取到的指针需要 free 手动释放。
	LPSTR findStringValue (LPCSTR lpMsName, LPCSTR defaultLocaleCode = NULL)
	{
		return PriReaderFindStringValueA (priReader, lpMsName, defaultLocaleCode);
	}
	// 获取到的指针需要 free 手动释放。
	LPWSTR findStringValue (LPCWSTR lpMsName, LPCWSTR defaultLocaleCode = NULL)
	{
		return PriReaderFindStringValueW (priReader, lpMsName, defaultLocaleCode);
	}
	std::string findStringValue (const std::string &objMsName, const std::string &defaultLocaleCode = "")
	{
		std::string t ("");
		LPSTR lpStr = findStringValue (objMsName.c_str (), ((!defaultLocaleCode.empty () && defaultLocaleCode.length () > 0) ? defaultLocaleCode.c_str () : NULL));
		if (lpStr) t += lpStr;
		if (lpStr) free (lpStr);
		return t;
	}
	std::wstring findStringValue (const std::wstring &objMsName, const std::wstring &defaultLocaleCode = L"")
	{
		std::wstring t (L"");
		LPWSTR lpStr = findStringValue (objMsName.c_str (), ((!defaultLocaleCode.empty () && defaultLocaleCode.length () > 0) ? defaultLocaleCode.c_str () : NULL));
		if (lpStr) t += lpStr;
		if (lpStr) free (lpStr);
		return t;
	}
	LPSTR findFilePathValue (LPCSTR lpMsName, int defaultScale = 0, ImageContrast defaultContrast = ImageContrast::none)
	{
		return PriReaderFindFilePathValueA (priReader, lpMsName, defaultScale, defaultContrast);
	}
	LPWSTR findFilePathValue (LPCWSTR lpMsName, int defaultScale = 0, ImageContrast defaultContrast = ImageContrast::none)
	{
		return PriReaderFindFilePathValueW (priReader, lpMsName, defaultScale, defaultContrast);
	}
	std::string findFilePathValue (const std::string &objMsName, int defaultScale = 0, ImageContrast defaultContrast = ImageContrast::none)
	{
		std::string t ("");
		LPSTR lpStr = findFilePathValue (objMsName.c_str (), defaultScale, defaultContrast);
		if (lpStr) t += lpStr;
		if (lpStr) free (lpStr);
		return t;
	}
	std::wstring findFilePathValue (const std::wstring &objMsName, int defaultScale = 0, ImageContrast defaultContrast = ImageContrast::none)
	{
		std::wstring t (L"");
		LPWSTR lpStr = findFilePathValue (objMsName.c_str (), defaultScale, defaultContrast);
		if (lpStr) t += lpStr;
		if (lpStr) free (lpStr);
		return t;
	}
};
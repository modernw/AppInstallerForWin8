// PriReader2.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "localeex.h"
#include "PriReader2.h"

template <typename CharT> std::basic_string <CharT> replace_substring
(
	const std::basic_string <CharT> &str,
	const std::basic_string <CharT> &from,
	const std::basic_string <CharT> &to
)
{
	if (from.empty ()) return str;
	std::basic_string  <CharT> result;
	size_t pos = 0;
	size_t start_pos;
	while ((start_pos = str.find (from, pos)) != std::basic_string<CharT>::npos) 
	{
		result.append (str, pos, start_pos - pos);
		result.append (to); 
		pos = start_pos + from.length ();
	}
	result.append (str, pos, str.length () - pos);
	return result;
}

int GetDPI ()
{
	HDC hDC = GetDC (NULL);
	int DPI_A = (int)(((double)GetDeviceCaps (hDC, 118) / (double)GetDeviceCaps (hDC, 8)) * 100);
	int DPI_B = (int)(((double)GetDeviceCaps (hDC, 88) / 96) * 100);
	ReleaseDC (NULL, hDC);
	if (DPI_A == 100) return DPI_B;
	else if (DPI_B == 100) return DPI_A;
	else if (DPI_A == DPI_B) return DPI_A;
	else return 0;
}
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
extern "C" bool IsFileExistsW (LPCWSTR filename)
{
	DWORD dwAttrib = GetFileAttributesW (filename);
	return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}
extern "C" bool IsFileExistsA (LPCSTR filename)
{
	DWORD dwAttrib = GetFileAttributesA (filename);
	return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}
bool IsFileExists (LPWSTR filePath) { return IsFileExistsW (filePath); }
bool IsFileExists (LPCSTR filePath) { return IsFileExistsA (filePath); }
bool IsFileExists (std::string filePath) { return IsFileExistsA (filePath.c_str ()); }
bool IsFileExists (std::wstring filePath) { return IsFileExistsW (filePath.c_str ()); }
bool IsDirectoryExistsA (LPCSTR path)
{
	DWORD attributes = GetFileAttributesA (path);
	return (attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY));
}
bool IsDirectoryExistsW (LPCWSTR path)
{
	DWORD attributes = GetFileAttributesW (path);
	return (attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY));
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
std::wstring StringTrim (const std::wstring &str)
{
	if (str.empty ()) return L"";
	LPCWSTR start = str.c_str ();
	LPCWSTR end = start + str.size () - 1;
#define isblankstrlabelw(wch) (wch == L' ' || wch == L'\t' || wch == L'\n')
	while (*start != L'\0' && isblankstrlabelw (*start)) ++start;
	while (end >= start && isblankstrlabelw (*end)) --end;
	if (start > end) return L"";
	return std::wstring (start, end + 1);
}
std::string StringTrim (const std::string &str)
{
	if (str.empty ()) return "";
	LPCSTR start = str.c_str ();
	LPCSTR end = start + str.size () - 1;
#define isblankstrlabela(wch) (wch == ' ' || wch == '\t' || wch == '\n')
	while (*start != L'\0' && isblankstrlabela (*start)) ++start;
	while (end >= start && isblankstrlabela (*end)) --end;
	if (start > end) return "";
	return std::string (start, end + 1);
}
namespace l0km
{
	template < typename E,
		typename TR = std::char_traits<E>,
		typename AL = std::allocator < E >>
		inline std::basic_string<E, TR, AL> toupper (const std::basic_string<E, TR, AL> &src)
	{
		std::basic_string<E, TR, AL> dst = src;
		static const std::locale loc;
		const std::ctype<E> &ctype = std::use_facet<std::ctype<E>> (loc);
		for (typename std::basic_string<E, TR, AL>::size_type i = 0; i < src.size (); ++i)
		{
			dst [i] = ctype.toupper (src [i]);
		}
		return dst;
	}

	template < typename E,
		typename TR = std::char_traits<E>,
		typename AL = std::allocator < E >>
		inline std::basic_string<E, TR, AL> tolower (const std::basic_string<E, TR, AL> &src)
	{
		std::basic_string<E, TR, AL> dst = src;
		static const std::locale loc;
		const std::ctype<E> &ctype = std::use_facet<std::ctype<E>> (loc);
		for (typename std::basic_string<E, TR, AL>::size_type i = 0; i < src.size (); ++i)
		{
			dst [i] = ctype.tolower (src [i]);
		}
		return dst;
	}
} // namespace l0km
#define StringToUpper l0km::toupper
#define StringToLower l0km::tolower
int LabelCompare (const std::wstring &l1, const std::wstring &l2)
{
	std::wstring s1 = StringToUpper (StringTrim (l1)),
		s2 = StringToUpper (StringTrim (l2));
	return lstrcmpW (s1.c_str (), s2.c_str ());
}
int LabelCompare (LPCWSTR l1, LPCWSTR l2)
{
	return LabelCompare (std::wstring (l1), std::wstring (l2));
}
int LabelCompare (const std::string &l1, const std::string &l2)
{
	std::string s1 = StringToUpper (StringTrim (l1)),
		s2 = StringToUpper (StringTrim (l2));
	return lstrcmpA (s1.c_str (), s2.c_str ());
}
int LabelCompare (LPCSTR l1, LPCSTR l2)
{
	return LabelCompare (std::string (l1), std::string (l2));
}
bool LabelEqual (const std::wstring &l1, const std::wstring &l2)
{
	return !LabelCompare (l1, l2);
}
bool LabelEqual (LPCWSTR l1, LPCWSTR l2)
{
	return !LabelCompare (l1, l2);
}
bool LabelEqual (const std::string &l1, const std::string &l2)
{
	return !LabelCompare (l1, l2);
}
bool LabelEqual (LPCSTR l1, LPCSTR l2)
{
	return !LabelCompare (l1, l2);
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

enum class ProcessorArchitecture
{
	x86 = 1,
	x64 = 2,
	arm = 3,
	arm64 = 4
};

typedef struct VERSION
{
	UINT16 build, maintenance, minor, major;
	VERSION (UINT16 major, UINT16 minor, UINT16 maintenance, UINT16 build);
	VERSION (UINT64 ver);
	VERSION ();
	virtual ~VERSION ();
	std::wstring toStringW ();
	std::string toString ();
	bool isEmpty ();
	UINT64 getUInt64 ();
	INT64 compare (VERSION &another);
	bool equals (VERSION &another);
	bool operator == (VERSION &another) { return this->equals (another); }
	bool operator > (VERSION &another) { return this->getUInt64 () > another.getUInt64 (); }
	bool operator < (VERSION &another) { return this->getUInt64 () < another.getUInt64 (); }
	bool operator >= (VERSION &another) { return !(*this < another); }
	bool operator <= (VERSION &another) { return !(*this > another); }
	bool operator != (VERSION &another) { return !this->equals (another); }
	static VERSION stringTo (const std::string &str);
	static VERSION stringTo (const std::wstring &str);
	static VERSION stringTo (LPCSTR str);
	static VERSION stringTo (LPCWSTR str);
	VERSION setFromString (const std::string &str)
	{
		VERSION v = stringTo (str);
		this->major = v.major;
		this->minor = v.minor;
		this->maintenance = v.maintenance;
		this->build = v.build;
		return *this;
	}
	VERSION setFromString (const std::wstring &str)
	{
		VERSION v = stringTo (str);
		this->major = v.major;
		this->minor = v.minor;
		this->maintenance = v.maintenance;
		this->build = v.build;
		return *this;
	}
	private:
	static std::vector <std::string> splitVersionString (const std::string &str);
	static std::vector <std::wstring> splitVersionString (const std::wstring &str);
} _VERSION, VERSION, Version, version;
VERSION::VERSION (UINT16 major, UINT16 minor, UINT16 maintenance, UINT16 build):
	major (major), minor (minor), maintenance (maintenance), build (build) {}
VERSION::VERSION (UINT64 verdata) : major ((verdata >> 0x30) & 0xFFFF), minor ((verdata >> 0x20) & 0xFFFF),
maintenance ((verdata >> 0x10) & 0xFFFF), build ((verdata) & 0xFFFF) {}
VERSION::VERSION () : major (0), minor (0), maintenance (0), build (0) {}
VERSION::~VERSION ()
{
	this->major = 0;
	this->minor = 0;
	this->maintenance = 0;
	this->build = 0;
}
std::wstring VERSION::toStringW ()
{
	std::wstringstream ss;
	ss << this->major << L'.' << this->minor << L'.' << this->maintenance << L'.' << this->build << std::ends;
	std::wstring st (L"");
	st += ss.str ();
	return st;
}
std::string VERSION::toString ()
{
	std::stringstream ss;
	ss << this->major << '.' << this->minor << '.' << this->maintenance << '.' << this->build << std::ends;
	std::string st ("");
	st += ss.str ();
	return st;
}
bool VERSION::isEmpty ()
{
	return this->getUInt64 () == 0;
}
UINT64 VERSION::getUInt64 ()
{
	UINT64 u64 = (((UINT64)major) << 0x30) | (((UINT64)minor) << 0x20) | (((UINT64)maintenance) << 0x10) | ((UINT64)build);
	return u64;
}
INT64 VERSION::compare (VERSION &another)
{
	UINT64 u1 = this->getUInt64 (), u2 = another.getUInt64 ();
	return u1 - u2;
}
bool VERSION::equals (VERSION &another)
{
	return !this->compare (another);
}
std::vector <std::string> VERSION::splitVersionString (const std::string &str)
{
	std::vector<std::string> result;
	std::stringstream ss (str);
	std::string segment;
	while (std::getline (ss, segment, '.'))  // 先按 '.'
	{
		size_t pos = 0;
		while ((pos = segment.find (',')) != std::string::npos)  // 再按 ','
		{
			result.push_back (segment.substr (0, pos));
			segment.erase (0, pos + 1);
		}
		result.push_back (segment);
	}
	return result;
}
std::vector <std::wstring> VERSION::splitVersionString (const std::wstring &str)
{
	std::vector<std::wstring> result;
	std::wstringstream ss (str);
	std::wstring segment;
	while (std::getline (ss, segment, L'.'))  // 先按 '.'
	{
		size_t pos = 0;
		while ((pos = segment.find (L',')) != std::wstring::npos)  // 再按 ','
		{
			result.push_back (segment.substr (0, pos));
			segment.erase (0, pos + 1);
		}
		result.push_back (segment);
	}
	return result;
}
VERSION VERSION::stringTo (const std::string &str)
{
	std::vector<std::string> parts = splitVersionString (str);
	UINT16 major = (parts.size () > 0) ? static_cast<UINT16>(std::stoi (parts [0])) : 0;
	UINT16 minor = (parts.size () > 1) ? static_cast<UINT16>(std::stoi (parts [1])) : 0;
	UINT16 maintenance = (parts.size () > 2) ? static_cast<UINT16>(std::stoi (parts [2])) : 0;
	UINT16 build = (parts.size () > 3) ? static_cast<UINT16>(std::stoi (parts [3])) : 0;
	return VERSION (major, minor, maintenance, build);
}
VERSION VERSION::stringTo (const std::wstring &str)
{
	std::vector<std::wstring> parts = splitVersionString (str);
	UINT16 major = (parts.size () > 0) ? static_cast<UINT16>(std::stoi (parts [0])) : 0;
	UINT16 minor = (parts.size () > 1) ? static_cast<UINT16>(std::stoi (parts [1])) : 0;
	UINT16 maintenance = (parts.size () > 2) ? static_cast<UINT16>(std::stoi (parts [2])) : 0;
	UINT16 build = (parts.size () > 3) ? static_cast<UINT16>(std::stoi (parts [3])) : 0;
	return VERSION (major, minor, maintenance, build);
}
VERSION VERSION::stringTo (LPCSTR str)
{
	return stringTo (std::string (str));
}
VERSION VERSION::stringTo (LPCWSTR str)
{
	return stringTo (std::wstring (str));
}

struct ProgramExeInfo
{
	std::wstring filePath;
	ProcessorArchitecture process;
	VERSION version;
	ProgramExeInfo (std::wstring filePath, ProcessorArchitecture process, VERSION version): filePath (filePath), process (process), version (version) {}
	ProgramExeInfo (): filePath (L""), process (ProcessorArchitecture::x86), version (0) {}
	bool operator == (ProgramExeInfo &another)
	{
		return LabelEqual (this->filePath, another.filePath) && this->process == another.process && this->version == another.version;
	}
	bool operator < (ProgramExeInfo &another)
	{
		if (this->version > another.version) return true;
		if (this->version < another.version) return false;
		return static_cast<int>(this->process) < static_cast<int>(another.process);
	}
};

ProcessorArchitecture GetProcessorArchitecture ()
{
	SYSTEM_INFO sysInfo;
	GetSystemInfo (&sysInfo);
	switch (sysInfo.wProcessorArchitecture)
	{
		case PROCESSOR_ARCHITECTURE_INTEL:
			return ProcessorArchitecture::x86; // 32-bit Intel
		case PROCESSOR_ARCHITECTURE_AMD64:
			return ProcessorArchitecture::x64; // 64-bit Intel/AMD
		case PROCESSOR_ARCHITECTURE_ARM:
			return ProcessorArchitecture::arm; // ARM
		case 12:
			return ProcessorArchitecture::arm64; // ARM64
		default:
			throw std::runtime_error ("Unknown processor architecture.");
	}
	return ProcessorArchitecture::x86;
}

template <typename StringType> StringType ReplaceBackslash (const StringType &input)
{
	StringType result = input;  // 复制输入字符串，避免修改原数据
	using CharType = typename StringType::value_type;  // 获取字符串的字符类型（char 或 wchar_t）
	std::replace (result.begin (), result.end (), static_cast<CharType> ('\\'), static_cast<CharType> ('/'));
	return result;
}

HRESULT ExtractFileFromStream (_In_ IStream* ifile, _In_ LPCWSTR outputFilePath)
{
	if (ifile == nullptr || outputFilePath == nullptr)
	{
		return E_INVALIDARG;
	}
	HRESULT hr = S_OK;
	IStream* outputStream = nullptr;
	ULARGE_INTEGER fileSizeLargeInteger = {0};
	STATSTG statstg = {0};
	// 获取输入流的大小
	hr = ifile->Stat (&statstg, STATFLAG_NONAME);
	if (SUCCEEDED (hr))
	{
		fileSizeLargeInteger.QuadPart = statstg.cbSize.QuadPart;
	}
	// 创建输出文件的 IStream
	if (SUCCEEDED (hr))
	{
		hr = SHCreateStreamOnFileEx (
			outputFilePath,
			STGM_CREATE | STGM_READWRITE | STGM_SHARE_DENY_NONE,
			0,
			TRUE,
			NULL,
			&outputStream);
	}
	// 复制输入流数据到输出流
	if (SUCCEEDED (hr))
	{
		hr = ifile->CopyTo (outputStream, fileSizeLargeInteger, nullptr, nullptr);
	}
	// 确保数据完全写入磁盘
	if (SUCCEEDED (hr) && outputStream)
	{
		hr = outputStream->Commit (STGC_DEFAULT); // 刷新缓存
	}
	// 释放资源
	if (outputStream)
	{
		outputStream->Release ();
	}
	return hr;
}

class PriReader
{
	private:
	static std::wstring makepri;
	std::wstring prifile = L"";
	std::wstring txmlpath = L"";
	pugi::xml_document *xmlreader;
	static bool initMakePri ()
	{
		if (IsFileExists (makepri)) return true; // 无需重复初始化
		std::wstring rootpath = EnsureTrailingSlash (EnsureTrailingSlash (GetProgramRootDirectoryW ()) + L"Tools\\MakePRI");
		if (!IsDirectoryExists (rootpath)) rootpath = L"Tools\\MakePRI";
		if (!IsDirectoryExists (rootpath)) return false;
		std::vector <std::wstring> vers = EnumSubdirectories (rootpath, false);
		if (!vers.size ()) return false;
		std::vector <ProgramExeInfo> einf;
		for (auto it : vers)
		{
			// x86
			{
				ProgramExeInfo pe;
				pe.filePath = EnsureTrailingSlash (EnsureTrailingSlash (rootpath) + it) + L"x86\\makepri.exe";
				pe.process = ProcessorArchitecture::x86;
				pe.version = VERSION::stringTo (it);
				if (IsFileExists (pe.filePath))
				{
					if (!pe.version.isEmpty ())
					{
						einf.push_back (pe);
					}
				}
			}
			// x64
			{
				ProgramExeInfo pe;
				pe.filePath = EnsureTrailingSlash (EnsureTrailingSlash (rootpath) + it) + L"x64\\makepri.exe";
				pe.process = ProcessorArchitecture::x64;
				pe.version = VERSION::stringTo (it);
				if (IsFileExists (pe.filePath))
				{
					if (!pe.version.isEmpty ())
					{
						einf.push_back (pe);
					}
				}
			}
			// arm
			{
				ProgramExeInfo pe;
				pe.filePath = EnsureTrailingSlash (EnsureTrailingSlash (rootpath) + it) + L"arm\\makepri.exe";
				pe.process = ProcessorArchitecture::arm;
				pe.version = VERSION::stringTo (it);
				if (IsFileExists (pe.filePath))
				{
					if (!pe.version.isEmpty ())
					{
						einf.push_back (pe);
					}
				}
			}
			// arm64
			{
				ProgramExeInfo pe;
				pe.filePath = EnsureTrailingSlash (EnsureTrailingSlash (rootpath) + it) + L"arm64\\makepri.exe";
				pe.process = ProcessorArchitecture::arm64;
				pe.version = VERSION::stringTo (it);
				if (IsFileExists (pe.filePath))
				{
					if (!pe.version.isEmpty ())
					{
						einf.push_back (pe);
					}
				}
			}
		}
		size_t n = einf.size ();
		for (size_t i = 0; i < n - 1; i ++)
		{
			for (size_t j = 0; j < n - 1 - i; j ++)
			{
				if (einf [j].version < einf [j + 1].version)
				{
					std::swap (einf [j], einf [j + 1]);
				}
			}
		}
		ProcessorArchitecture syspa = GetProcessorArchitecture ();
		for (auto it : einf)
		{
			switch (syspa)
			{
				case ProcessorArchitecture::x64:
					if (it.process == ProcessorArchitecture::x64)
					{
						if (IsProgramExecutable (it.filePath))
						{
							PriReader::makepri = it.filePath;
							return true;
						}
					}
				case ProcessorArchitecture::x86:
					if (it.process == ProcessorArchitecture::x86)
					{
						if (IsProgramExecutable (it.filePath))
						{
							PriReader::makepri = it.filePath;
							return true;
						}
					} break;
				case ProcessorArchitecture::arm64:
					if (it.process == ProcessorArchitecture::arm64)
					{
						if (IsProgramExecutable (it.filePath))
						{
							PriReader::makepri = it.filePath;
							return true;
						}
					}
				case ProcessorArchitecture::arm:
					if (it.process == ProcessorArchitecture::arm)
					{
						if (IsProgramExecutable (it.filePath))
						{
							PriReader::makepri = it.filePath;
							return true;
						}
					} break;
			}
		}
		return false;
	}
	bool isLanguageMatch (const std::string l1, const std::string l2)
	{
		bool isLanguageMatch = LabelEqual (l1, l2);
		if (!isLanguageMatch)
		{
			bool isLanguageMatch = (
				LocaleCodeToLcidA (l1.c_str ())
				==
				LocaleCodeToLcidA (l2.c_str ())
				);
		}
		if (!isLanguageMatch)
		{
			bool isLanguageMatch = LabelEqual (
				GetLocaleRestrictedCodeA (l1),
				GetLocaleRestrictedCodeA (l2)
			);
		}
		return isLanguageMatch;
	}
	bool isLanguageMatch (LPCSTR l1, LPCSTR l2)
	{
		return isLanguageMatch (std::string (l1), std::string (l2));
	}
	bool isLanguageMatch (const std::wstring l1, const std::wstring l2)
	{
		bool isLanguageMatch = LabelEqual (l1, l2);
		if (!isLanguageMatch)
		{
			bool isLanguageMatch = (
				LocaleCodeToLcidW (l1.c_str ())
				==
				LocaleCodeToLcidW (l2.c_str ())
				);
		}
		if (!isLanguageMatch)
		{
			bool isLanguageMatch = LabelEqual (
				GetLocaleRestrictedCodeW (l1),
				GetLocaleRestrictedCodeW (l2)
			);
		}
		return isLanguageMatch;
	}
	bool isLanguageMatch (LPCWSTR l1, LPCWSTR l2)
	{
		return isLanguageMatch (std::wstring (l1), std::wstring (l2));
	}
	LPSTR recFindStringValue (pugi::xml_node &subResMap, LPCSTR lpMsName, LPCSTR defaultLocaleCode)
	{
		for (pugi::xml_node namedRes = subResMap.child ("NamedResource"); namedRes; namedRes = namedRes.next_sibling ("NamedResource"))
		{
			bool isMatch = false;
			pugi::xml_attribute nameAttr = namedRes.attribute ("name");
			std::string resname = nameAttr.as_string ();
			isMatch = LabelEqual (resname, lpMsName);
			static const std::regex pattern ("^ms-resource:", std::regex_constants::icase);
			if (!isMatch)
			{
				std::string lpname2 = std::regex_replace (std::string (lpMsName), pattern, "");
				isMatch = LabelEqual (resname, lpname2);
			}
			if (!isMatch)
			{
				std::string objstr = std::regex_replace (std::string (lpMsName), pattern, "");
				std::string lpname2 = PathFindFileNameA (objstr.c_str ());
				isMatch = LabelEqual (resname, lpname2);
			}
			if (!isMatch)
			{
				pugi::xml_attribute uriAttr = namedRes.attribute ("uri");
				std::string resuri = uriAttr.as_string ();
				isMatch = (InStr (resuri, lpMsName, true) >= 0);
			}
			if (!isMatch)
			{
				pugi::xml_attribute uriAttr = namedRes.attribute ("uri");
				std::string resuri = uriAttr.as_string ();
				std::string objstr = std::regex_replace (std::string (lpMsName), pattern, "");
				objstr = replace_substring <char> (objstr, "\\", "/");
				isMatch = (InStr (resuri, objstr, true) >= 0);
			}
			if (isMatch)
			{
				std::map <std::string, std::string> langmap;
				for (pugi::xml_node candidate = namedRes.child ("Candidate"); candidate; candidate = candidate.next_sibling ("Candidate"))
				{
					pugi::xml_attribute typeAttr = candidate.attribute ("type");
					if (!LabelEqual (typeAttr.as_string (), "String")) continue;
					std::string valueGet = candidate.child ("Value").text ().get ();
					for (pugi::xml_node qualifierSet = candidate.child ("QualifierSet"); qualifierSet; qualifierSet = qualifierSet.next_sibling ("QualifierSet"))
					{
						for (pugi::xml_node qualifier = qualifierSet.child ("Qualifier"); qualifier; qualifier = qualifier.next_sibling ("Qualifier"))
						{
							if (!LabelEqual (qualifier.attribute ("name").as_string (), "Language")) continue;
							std::string value = qualifier.attribute ("value").as_string ();
							langmap [std::string (value)] = valueGet;
						}
					}
				}
				for (auto it : langmap)
				{
					bool isMatch = LabelEqual (it.first, defaultLocaleCode);
					if (!isMatch) isMatch = (
						LocaleCodeToLcidA (it.first.c_str ())
						==
						LocaleCodeToLcidA (defaultLocaleCode)
						);
					if (isMatch)
					{
						LPSTR lpbuf = (LPSTR)malloc (it.second.capacity () * sizeof (char));
						ZeroMemory (lpbuf, it.second.capacity ());
						lstrcpyA (lpbuf, it.second.c_str ());
						return lpbuf;
					}
				}
				for (auto it : langmap)
				{
					bool isMatch = isLanguageMatch (it.first, defaultLocaleCode);
					if (isMatch)
					{
						LPSTR lpbuf = (LPSTR)malloc (it.second.capacity () * sizeof (char));
						ZeroMemory (lpbuf, it.second.capacity ());
						lstrcpyA (lpbuf, it.second.c_str ());
						return lpbuf;
					}
				}
				for (auto it : langmap)
				{
					bool isMatch = LabelEqual (it.first, LcidToLocaleCodeA (GetSystemDefaultLCID ()));
					if (!isMatch) isMatch = (
						LocaleCodeToLcidA (it.first.c_str ())
						==
						LocaleCodeToLcidA (LcidToLocaleCodeA (GetSystemDefaultLCID ()).c_str ())
						);
					if (isMatch)
					{
						LPSTR lpbuf = (LPSTR)malloc (it.second.capacity () * sizeof (char));
						ZeroMemory (lpbuf, it.second.capacity ());
						lstrcpyA (lpbuf, it.second.c_str ());
						return lpbuf;
					}
				}
				for (auto it : langmap)
				{
					bool isMatch = isLanguageMatch (it.first, LcidToLocaleCodeA (GetSystemDefaultLCID ()));
					if (isMatch)
					{
						LPSTR lpbuf = (LPSTR)malloc (it.second.capacity () * sizeof (char));
						ZeroMemory (lpbuf, it.second.capacity ());
						lstrcpyA (lpbuf, it.second.c_str ());
						return lpbuf;
					}
				}
				for (auto it : langmap)
				{
					bool isMatch = isLanguageMatch (it.first, "en-us");
					if (isMatch)
					{
						LPSTR lpbuf = (LPSTR)malloc (it.second.capacity () * sizeof (char));
						ZeroMemory (lpbuf, it.second.capacity ());
						lstrcpyA (lpbuf, it.second.c_str ());
						return lpbuf;
					}
				}
				for (auto it : langmap)
				{
					bool isMatch = isLanguageMatch (it.first, "en");
					if (isMatch)
					{
						LPSTR lpbuf = (LPSTR)malloc (it.second.capacity () * sizeof (char));
						ZeroMemory (lpbuf, it.second.capacity ());
						lstrcpyA (lpbuf, it.second.c_str ());
						return lpbuf;
					}
				}
				for (auto it : langmap)
				{
					LPSTR lpbuf = (LPSTR)malloc (it.second.capacity () * sizeof (char));
					ZeroMemory (lpbuf, it.second.capacity ());
					lstrcpyA (lpbuf, it.second.c_str ());
					return lpbuf;
				}
			}
		}
		for (pugi::xml_node subtree = subResMap.child ("ResourceMapSubtree"); subtree; subtree = subtree.next_sibling ("ResourceMapSubtree"))
		{
			LPSTR result = recFindStringValue (subtree, lpMsName, defaultLocaleCode);
			if (result) return result; // 找到就返回
		}
		return NULL;
	}
	LPWSTR recFindStringValue (pugi::xml_node &subResMap, LPCWSTR lpMsName, LPCWSTR defaultLocaleCode)
	{
		for (pugi::xml_node namedRes = subResMap.child ("NamedResource"); namedRes; namedRes = namedRes.next_sibling ("NamedResource"))
		{
			bool isMatch = false;
			pugi::xml_attribute nameAttr = namedRes.attribute ("name");
			std::wstring resname = pugi::as_wide (nameAttr.as_string ());
			isMatch = LabelEqual (resname, lpMsName);
			static const std::wregex pattern (L"^ms-resource:", std::regex_constants::icase);
			if (!isMatch)
			{
				std::wstring lpname2 = std::regex_replace (std::wstring (lpMsName), pattern, L"");
				isMatch = LabelEqual (resname, lpname2);
			}
			if (!isMatch)
			{
				std::wstring objstr = std::regex_replace (std::wstring (lpMsName), pattern, L"");
				std::wstring lpname2 = PathFindFileNameW (objstr.c_str ());
				isMatch = LabelEqual (resname, lpname2);
			}
			if (!isMatch)
			{
				pugi::xml_attribute uriAttr = namedRes.attribute ("uri");
				std::wstring resuri = pugi::as_wide (uriAttr.as_string ());
				isMatch = (InStr (resuri, lpMsName, true) >= 0);
			}
			if (!isMatch)
			{
				std::wstring lpname2 = std::regex_replace (std::wstring (lpMsName), pattern, L"");
				pugi::xml_attribute uriAttr = namedRes.attribute ("uri");
				std::wstring resuri = pugi::as_wide (uriAttr.as_string ());
				isMatch = (InStr (resuri, lpname2, true) >= 0);
			}
			if (!isMatch)
			{
				pugi::xml_attribute uriAttr = namedRes.attribute ("uri");
				std::wstring resuri = pugi::as_wide (uriAttr.as_string ());
				std::wstring objstr = std::regex_replace (std::wstring (lpMsName), pattern, L"");
				objstr = replace_substring <WCHAR> (objstr, L"\\", L"/");
				isMatch = (InStr (resuri, objstr, true) >= 0);
			}
			if (isMatch)
			{
				std::map <std::wstring, std::wstring> langmap;
				for (pugi::xml_node candidate = namedRes.child ("Candidate"); candidate; candidate = candidate.next_sibling ("Candidate"))
				{
					pugi::xml_attribute typeAttr = candidate.attribute ("type");
					if (!LabelEqual (typeAttr.as_string (), "String")) continue;
					for (pugi::xml_node qualifierSet = candidate.child ("QualifierSet"); qualifierSet; qualifierSet = qualifierSet.next_sibling ("QualifierSet"))
					{
						std::wstring valueGet = pugi::as_wide (candidate.child ("Value").text ().get ());
						for (pugi::xml_node qualifier = qualifierSet.child ("Qualifier"); qualifier; qualifier = qualifier.next_sibling ("Qualifier"))
						{
							if (!LabelEqual (qualifier.attribute ("name").as_string (), "Language")) continue;
							std::wstring value = pugi::as_wide (qualifier.attribute ("value").as_string ());
							langmap [std::wstring (value)] = valueGet;
						}
					}
				}
				for (auto it : langmap)
				{
					bool isMatch = LabelEqual (it.first, defaultLocaleCode);
					if (!isMatch) {
						isMatch = (LocaleCodeToLcidW (it.first.c_str ()) == LocaleCodeToLcidW (defaultLocaleCode));
					}
					if (isMatch) {
						LPWSTR lpbuf = (LPWSTR)malloc ((it.second.length () + 1) * sizeof (wchar_t));
						ZeroMemory (lpbuf, (it.second.length () + 1) * sizeof (wchar_t));
						lstrcpyW (lpbuf, it.second.c_str ());
						return lpbuf;
					}
				}
				for (auto it : langmap)
				{
					if (isLanguageMatch (it.first, defaultLocaleCode))
					{
						LPWSTR lpbuf = (LPWSTR)malloc ((it.second.length () + 1) * sizeof (wchar_t));
						ZeroMemory (lpbuf, (it.second.length () + 1) * sizeof (wchar_t));
						lstrcpyW (lpbuf, it.second.c_str ());
						return lpbuf;
					}
				}
				for (auto it : langmap)
				{
					bool isMatch = LabelEqual (it.first, LcidToLocaleCodeW (GetSystemDefaultLCID ()));
					if (!isMatch)
					{
						isMatch = (LocaleCodeToLcidW (it.first.c_str ()) == LocaleCodeToLcidW (LcidToLocaleCodeW (GetSystemDefaultLCID ()).c_str ()));
					}
					if (isMatch)
					{
						LPWSTR lpbuf = (LPWSTR)malloc ((it.second.length () + 1) * sizeof (wchar_t));
						ZeroMemory (lpbuf, (it.second.length () + 1) * sizeof (wchar_t));
						lstrcpyW (lpbuf, it.second.c_str ());
						return lpbuf;
					}
				}
				for (auto it : langmap)
				{
					if (isLanguageMatch (it.first, LcidToLocaleCodeW (GetSystemDefaultLCID ())))
					{
						LPWSTR lpbuf = (LPWSTR)malloc ((it.second.length () + 1) * sizeof (wchar_t));
						ZeroMemory (lpbuf, (it.second.length () + 1) * sizeof (wchar_t));
						lstrcpyW (lpbuf, it.second.c_str ());
						return lpbuf;
					}
				}
				for (auto it : langmap)
				{
					if (isLanguageMatch (it.first, L"en-us"))
					{
						LPWSTR lpbuf = (LPWSTR)malloc ((it.second.length () + 1) * sizeof (wchar_t));
						ZeroMemory (lpbuf, (it.second.length () + 1) * sizeof (wchar_t));
						lstrcpyW (lpbuf, it.second.c_str ());
						return lpbuf;
					}
				}
				for (auto it : langmap)
				{
					if (isLanguageMatch (it.first, L"en"))
					{
						LPWSTR lpbuf = (LPWSTR)malloc ((it.second.length () + 1) * sizeof (wchar_t));
						ZeroMemory (lpbuf, (it.second.length () + 1) * sizeof (wchar_t));
						lstrcpyW (lpbuf, it.second.c_str ());
						return lpbuf;
					}
				}
				for (auto it : langmap)
				{
					LPWSTR lpbuf = (LPWSTR)malloc ((it.second.length () + 1) * sizeof (wchar_t));
					ZeroMemory (lpbuf, (it.second.length () + 1) * sizeof (wchar_t));
					lstrcpyW (lpbuf, it.second.c_str ());
					return lpbuf;
				}
			}
		}
		// 递归查找子树
		for (pugi::xml_node subtree = subResMap.child ("ResourceMapSubtree"); subtree; subtree = subtree.next_sibling ("ResourceMapSubtree"))
		{
			LPWSTR result = recFindStringValue (subtree, lpMsName, defaultLocaleCode);
			if (result) return result;
		}
		return NULL;
	}
	struct ImageFileInfo
	{
		int scale;
		ImageContrast contrast;
		size_t hash;
		ImageFileInfo (int scale = 100, ImageContrast contrast = ImageContrast::none, size_t hash = 0):
			scale (scale), contrast (contrast), hash (hash) {}
		bool operator < (const ImageFileInfo &right) const 
		{
			if (scale != right.scale)
				return scale < right.scale;
			if (contrast != right.contrast)
				return contrast < right.contrast;
			return hash < right.hash;  // 确保所有字段都参与排序
		}
		bool operator == (const ImageFileInfo &right) const
		{
			return this->scale == right.scale && this->contrast == right.contrast && this->hash == right.hash;
		}
		bool operator != (const ImageFileInfo &right) const { return !(*this == right); }
	};
	LPSTR recFindFilePathValue (pugi::xml_node &subResMap, LPCSTR lpMsName, int defaultScale = GetDPI (), ImageContrast defaultContrast = ImageContrast::none)
	{
		for (pugi::xml_node namedRes = subResMap.child ("NamedResource"); namedRes; namedRes = namedRes.next_sibling ("NamedResource"))
		{
			bool isMatch = false;
			pugi::xml_attribute nameAttr = namedRes.attribute ("name");
			std::string resname = nameAttr.as_string ();
			isMatch = LabelEqual (resname, lpMsName);
			static const std::regex pattern ("^ms-resource:", std::regex_constants::icase);
			if (!isMatch)
			{
				std::string lpname2 = std::regex_replace (std::string (lpMsName), pattern, "");
				isMatch = LabelEqual (resname, lpname2);
			}
			if (!isMatch)
			{
				LPCSTR filename = PathFindFileNameA (lpMsName);
				isMatch = LabelEqual (resname, filename);
			}
			if (!isMatch)
			{
				pugi::xml_attribute uriAttr = namedRes.attribute ("uri");
				std::string resuri = uriAttr.as_string ();
				std::string ptouri = ReplaceBackslash (std::string (lpMsName));
				isMatch = (InStr (resuri, ptouri) >= 0);
			}
			if (isMatch)
			{
				std::map <ImageFileInfo, std::string> imgmap;
				for (pugi::xml_node candidate = namedRes.child ("Candidate"); candidate; candidate = candidate.next_sibling ("Candidate"))
				{
					std::string valueGet = candidate.child ("Value").text ().get ();
					for (pugi::xml_node qualifierSet = candidate.child ("QualifierSet"); qualifierSet; qualifierSet = qualifierSet.next_sibling ("QualifierSet"))
					{
						ImageFileInfo tinfo;
						for (pugi::xml_node qualifier = candidate.child ("Qualifier"); qualifier; qualifier = qualifier.next_sibling ("Qualifier"))
						{
							std::string itemname = qualifier.attribute ("name").as_string ();
							if (LabelEqual (itemname, "Contrast"))
							{
								std::string conText = qualifier.attribute ("value").as_string ();
								if (LabelEqual (conText, "BLACK")) tinfo.contrast = ImageContrast::black;
								else if (LabelEqual (conText, "WHITE")) tinfo.contrast = ImageContrast::white;
							}
							else if (LabelEqual (itemname, "Scale"))
							{
								tinfo.scale = qualifier.attribute ("value").as_int ();
							}
						}
						imgmap [tinfo] = valueGet;
					}
				}
				for (auto it : imgmap)
				{
					if (defaultContrast == it.first.contrast)
					{
						if (it.first.scale == defaultScale)
						{
							LPSTR lpbuf = (LPSTR)calloc (it.second.capacity () + 1, sizeof (char));
							lstrcmpA (lpbuf, it.second.c_str ());
							return lpbuf;
						}
					}
				}
				for (auto it : imgmap)
				{
					if (defaultContrast == it.first.contrast)
					{
						if (it.first.scale >= defaultScale)
						{
							LPSTR lpbuf = (LPSTR)calloc (it.second.capacity () + 1, sizeof (char));
							lstrcmpA (lpbuf, it.second.c_str ());
							return lpbuf;
						}
					}
				}
				for (auto it : imgmap)
				{
					if (defaultContrast == it.first.contrast)
					{
						LPSTR lpbuf = (LPSTR)calloc (it.second.capacity () + 1, sizeof (char));
						lstrcmpA (lpbuf, it.second.c_str ());
						return lpbuf;
					}
				}
				for (auto it : imgmap)
				{
					if (it.first.contrast == ImageContrast::none)
					{
						if (it.first.scale == defaultScale)
						{
							LPSTR lpbuf = (LPSTR)calloc (it.second.capacity () + 1, sizeof (char));
							lstrcmpA (lpbuf, it.second.c_str ());
							return lpbuf;
						}
					}
				}
				for (auto it : imgmap)
				{
					if (it.first.contrast == ImageContrast::none)
					{
						if (it.first.scale >= defaultScale)
						{
							LPSTR lpbuf = (LPSTR)calloc (it.second.capacity () + 1, sizeof (char));
							lstrcmpA (lpbuf, it.second.c_str ());
							return lpbuf;
						}
					}
				}
				for (auto it : imgmap)
				{
					if (it.first.contrast == ImageContrast::none)
					{
						LPSTR lpbuf = (LPSTR)calloc (it.second.capacity () + 1, sizeof (char));
						lstrcmpA (lpbuf, it.second.c_str ());
						return lpbuf;
					}
				}
				for (auto it : imgmap)
				{
					if (it.first.contrast == ImageContrast::white)
					{
						if (it.first.scale == defaultScale)
						{
							LPSTR lpbuf = (LPSTR)calloc (it.second.capacity () + 1, sizeof (char));
							lstrcmpA (lpbuf, it.second.c_str ());
							return lpbuf;
						}
					}
				}
				for (auto it : imgmap)
				{
					if (it.first.contrast == ImageContrast::white)
					{
						if (it.first.scale >= defaultScale)
						{
							LPSTR lpbuf = (LPSTR)calloc (it.second.capacity () + 1, sizeof (char));
							lstrcmpA (lpbuf, it.second.c_str ());
							return lpbuf;
						}
					}
				}
				for (auto it : imgmap)
				{
					if (it.first.contrast == ImageContrast::white)
					{
						LPSTR lpbuf = (LPSTR)calloc (it.second.capacity () + 1, sizeof (char));
						lstrcmpA (lpbuf, it.second.c_str ());
						return lpbuf;
					}
				}
				for (auto it : imgmap)
				{
					if (it.first.contrast == ImageContrast::black)
					{
						if (it.first.scale == defaultScale)
						{
							LPSTR lpbuf = (LPSTR)calloc (it.second.capacity () + 1, sizeof (char));
							lstrcmpA (lpbuf, it.second.c_str ());
							return lpbuf;
						}
					}
				}
				for (auto it : imgmap)
				{
					if (it.first.contrast == ImageContrast::black)
					{
						if (it.first.scale >= defaultScale)
						{
							LPSTR lpbuf = (LPSTR)calloc (it.second.capacity () + 1, sizeof (char));
							lstrcmpA (lpbuf, it.second.c_str ());
							return lpbuf;
						}
					}
				}
				for (auto it : imgmap)
				{
					if (it.first.contrast == ImageContrast::black)
					{
						LPSTR lpbuf = (LPSTR)calloc (it.second.capacity () + 1, sizeof (char));
						lstrcmpA (lpbuf, it.second.c_str ());
						return lpbuf;
					}
				}
				for (auto it : imgmap)
				{
					LPSTR lpbuf = (LPSTR)calloc (it.second.capacity () + 1, sizeof (char));
					lstrcmpA (lpbuf, it.second.c_str ());
					return lpbuf;
				}
			}
		}
		for (pugi::xml_node subtree = subResMap.child ("ResourceMapSubtree"); subtree; subtree = subtree.next_sibling ("ResourceMapSubtree"))
		{
			LPSTR result = recFindFilePathValue (subtree, lpMsName, defaultScale, defaultContrast);
			if (result) return result; // 找到就返回
		}
		return NULL;
	}
	LPWSTR recFindFilePathValue (pugi::xml_node &subResMap, LPCWSTR lpMsName, int defaultScale = GetDPI (), ImageContrast defaultContrast = ImageContrast::none)
	{
		for (pugi::xml_node namedRes = subResMap.child ("NamedResource"); namedRes; namedRes = namedRes.next_sibling ("NamedResource"))
		{
			bool isMatch = false;
			pugi::xml_attribute nameAttr = namedRes.attribute ("name");
			std::wstring resname = pugi::as_wide (nameAttr.as_string ());
			isMatch = LabelEqual (resname, lpMsName);
			static const std::wregex pattern (L"^ms-resource:", std::regex_constants::icase);
			if (!isMatch)
			{
				std::wstring lpname2 = std::regex_replace (std::wstring (lpMsName), pattern, L"");
				isMatch = LabelEqual (resname, lpname2);
			}
			if (!isMatch)
			{
				LPCWSTR filename = PathFindFileNameW (lpMsName);
				isMatch = LabelEqual (resname, filename);
			}
			if (!isMatch)
			{
				pugi::xml_attribute uriAttr = namedRes.attribute ("uri");
				std::wstring resuri = pugi::as_wide (uriAttr.as_string ());
				std::wstring ptouri = ReplaceBackslash (std::wstring (lpMsName));
				isMatch = (InStr (resuri, ptouri) >= 0);
			}
			if (isMatch)
			{
				std::map <ImageFileInfo, std::wstring> imgmap;
				for (pugi::xml_node candidate = namedRes.child ("Candidate"); candidate; candidate = candidate.next_sibling ("Candidate"))
				{
					std::wstring valueGet (L"");
					valueGet += pugi::as_wide (candidate.child ("Value").text ().get ());
					bool isNoAdd = false;
					for (pugi::xml_node qualifierSet = candidate.child ("QualifierSet"); qualifierSet; qualifierSet = qualifierSet.next_sibling ("QualifierSet"))
					{
						ImageFileInfo tinfo;
						for (pugi::xml_node qualifier = qualifierSet.child ("Qualifier"); qualifier; qualifier = qualifier.next_sibling ("Qualifier"))
						{
							std::string itemname = qualifier.attribute ("name").as_string ();
							if (LabelEqual (itemname, "Contrast"))
							{
								std::string conText = qualifier.attribute ("value").as_string ();
								if (LabelEqual (conText, "BLACK")) tinfo.contrast = ImageContrast::black;
								else if (LabelEqual (conText, "WHITE")) tinfo.contrast = ImageContrast::white;
								else if (LabelEqual (conText, "HIGH")) tinfo.contrast = ImageContrast::white;
							}
							else if (LabelEqual (itemname, "Scale"))
							{
								tinfo.scale = qualifier.attribute ("value").as_int ();
							}
							else if (LabelEqual (itemname, "TargetSize"))
							{
								tinfo.scale = (int)((double)qualifier.attribute ("value").as_int () * 2);
								isNoAdd = true;
							}
							else if (LabelEqual (itemname, "AlternateForm"))
							{
								std::string alfText = qualifier.attribute ("value").as_string ();
								std::hash <std::string> hasher;
								tinfo.hash = hasher (alfText);
							}
						}
						if (!isNoAdd) if (valueGet.length () > 0) imgmap [tinfo] = valueGet;
					}
				}
				for (auto it : imgmap)
				{
					if (defaultContrast == it.first.contrast)
					{
						if (it.first.scale == defaultScale)
						{
							LPWSTR lpbuf = (LPWSTR)calloc (it.second.capacity () + 1, sizeof (WCHAR));
							StrCpyW (lpbuf, it.second.c_str ());
							return lpbuf; 
						}
					}
				}
				for (auto it : imgmap)
				{
					if (defaultContrast == it.first.contrast)
					{
						if (it.first.scale >= defaultScale)
						{
							LPWSTR lpbuf = (LPWSTR)calloc (it.second.capacity () + 1, sizeof (WCHAR));
							StrCpyW (lpbuf, it.second.c_str ());
							return lpbuf;
						}
					}
				}
				for (auto it : imgmap)
				{
					if (defaultContrast == it.first.contrast)
					{
						if (it.first.scale >= 100)
						{
							LPWSTR lpbuf = (LPWSTR)calloc (it.second.capacity () + 1, sizeof (WCHAR));
							StrCpyW (lpbuf, it.second.c_str ());
							return lpbuf;
						}
					}
				}
				for (auto it : imgmap)
				{
					if (defaultContrast == it.first.contrast)
					{
						LPWSTR lpbuf = (LPWSTR)calloc (it.second.capacity () + 1, sizeof (WCHAR));
						StrCpyW (lpbuf, it.second.c_str ());
						return lpbuf;
					}
				}
				for (auto it : imgmap)
				{
					if (it.first.contrast == ImageContrast::none)
					{
						if (it.first.scale == defaultScale)
						{
							LPWSTR lpbuf = (LPWSTR)calloc (it.second.capacity () + 1, sizeof (WCHAR));
							StrCpyW (lpbuf, it.second.c_str ());
							return lpbuf;
						}
					}
				}
				for (auto it : imgmap)
				{
					if (it.first.contrast == ImageContrast::none)
					{
						if (it.first.scale >= defaultScale)
						{
							LPWSTR lpbuf = (LPWSTR)calloc (it.second.capacity () + 1, sizeof (WCHAR));
							StrCpyW (lpbuf, it.second.c_str ());
							return lpbuf;
						}
					}
				}
				for (auto it : imgmap)
				{
					if (it.first.contrast == ImageContrast::none)
					{
						if (it.first.scale >= 100)
						{
							LPWSTR lpbuf = (LPWSTR)calloc (it.second.capacity () + 1, sizeof (WCHAR));
							StrCpyW (lpbuf, it.second.c_str ());
							return lpbuf;
						}
					}
				}
				for (auto it : imgmap)
				{
					if (it.first.contrast == ImageContrast::none)
					{
						LPWSTR lpbuf = (LPWSTR)calloc (it.second.capacity () + 1, sizeof (WCHAR));
						StrCpyW (lpbuf, it.second.c_str ());
						return lpbuf;
					}
				}
				for (auto it : imgmap)
				{
					if (it.first.contrast == ImageContrast::white)
					{
						if (it.first.scale == defaultScale)
						{
							LPWSTR lpbuf = (LPWSTR)calloc (it.second.capacity () + 1, sizeof (WCHAR));
							StrCpyW (lpbuf, it.second.c_str ());
							return lpbuf;
						}
					}
				}
				for (auto it : imgmap)
				{
					if (it.first.contrast == ImageContrast::white)
					{
						if (it.first.scale >= defaultScale)
						{
							LPWSTR lpbuf = (LPWSTR)calloc (it.second.capacity () + 1, sizeof (WCHAR));
							StrCpyW (lpbuf, it.second.c_str ());
							return lpbuf;
						}
					}
				}
				for (auto it : imgmap)
				{
					if (it.first.contrast == ImageContrast::white)
					{
						if (it.first.scale >= 100)
						{
							LPWSTR lpbuf = (LPWSTR)calloc (it.second.capacity () + 1, sizeof (WCHAR));
							StrCpyW (lpbuf, it.second.c_str ());
							return lpbuf;
						}
					}
				}
				for (auto it : imgmap)
				{
					if (it.first.contrast == ImageContrast::white)
					{
						LPWSTR lpbuf = (LPWSTR)calloc (it.second.capacity () + 1, sizeof (WCHAR));
						StrCpyW (lpbuf, it.second.c_str ());
						return lpbuf;
					}
				}
				for (auto it : imgmap)
				{
					if (it.first.contrast == ImageContrast::black)
					{
						if (it.first.scale == defaultScale)
						{
							LPWSTR lpbuf = (LPWSTR)calloc (it.second.capacity () + 1, sizeof (WCHAR));
							StrCpyW (lpbuf, it.second.c_str ());
							return lpbuf;
						}
					}
				}
				for (auto it : imgmap)
				{
					if (it.first.contrast == ImageContrast::black)
					{
						if (it.first.scale >= defaultScale)
						{
							LPWSTR lpbuf = (LPWSTR)calloc (it.second.capacity () + 1, sizeof (WCHAR));
							StrCpyW (lpbuf, it.second.c_str ());
							return lpbuf;
						}
					}
				}
				for (auto it : imgmap)
				{
					if (it.first.contrast == ImageContrast::black)
					{
						if (it.first.scale >= 100)
						{
							LPWSTR lpbuf = (LPWSTR)calloc (it.second.capacity () + 1, sizeof (WCHAR));
							StrCpyW (lpbuf, it.second.c_str ());
							return lpbuf;
						}
					}
				}
				for (auto it : imgmap)
				{
					if (it.first.contrast == ImageContrast::black)
					{
						LPWSTR lpbuf = (LPWSTR)calloc (it.second.capacity () + 1, sizeof (WCHAR));
						StrCpyW (lpbuf, it.second.c_str ());
						return lpbuf;
					}
				}
				for (auto it : imgmap)
				{
					LPWSTR lpbuf = (LPWSTR)calloc (it.second.capacity () + 1, sizeof (WCHAR));
					StrCpyW (lpbuf, it.second.c_str ());
					return lpbuf;
				}
			}
		}
		for (pugi::xml_node subtree = subResMap.child ("ResourceMapSubtree"); subtree; subtree = subtree.next_sibling ("ResourceMapSubtree"))
		{
			LPWSTR result = recFindFilePathValue (subtree, lpMsName, defaultScale, defaultContrast);
			if (result) return result; // 找到就返回
		}
		return NULL;
	}
	public:
	PriReader (LPCWSTR lpPriFilePath) { this->create (lpPriFilePath); }
	PriReader (const std::wstring objPriFilePath) { this->create (objPriFilePath); }
	PriReader (IStream *comISPriFile) { this->create (comISPriFile); }
	~PriReader () { this->destroy (); }
	PriReader (): prifile (L""), txmlpath (L"") { xmlreader = nullptr; }
	void destroy ()
	{
		if (xmlreader) delete xmlreader;
		xmlreader = nullptr;
		if (IsFileExists (txmlpath)) DeleteFileW (txmlpath.c_str ());
		txmlpath = L"";
		prifile = L"";
	}
	bool isAvailable () { return xmlreader != nullptr; }
	bool create (const std::wstring priFile)
	{
		destroy ();
		if (!PriReader::initMakePri ()) return false;
		std::wstring outputXmlFile;
		do
		{
			outputXmlFile = GetUniqueTempFilePathW () + L".xml";
		} while (IsFileExists (outputXmlFile));
		std::wstring quotedMakepri = L"\"" + makepri + L"\"";
		std::wstring quotedPriFile = L"\"" + priFile + L"\"";
		std::wstring quotedOutputXmlFile = L"\"" + outputXmlFile + L"\"";
		// 构造命令行
		std::wstring command = quotedMakepri + L" dump /if " + quotedPriFile + L" /of " + quotedOutputXmlFile + L" /dt detailed";
		// 创建匿名管道用于获取 stdout
		SECURITY_ATTRIBUTES sa;
		sa.nLength = sizeof (SECURITY_ATTRIBUTES);
		sa.bInheritHandle = TRUE;  // 允许子进程继承句柄
		sa.lpSecurityDescriptor = nullptr;
		HANDLE hReadPipe, hWritePipe;
		if (!CreatePipe (&hReadPipe, &hWritePipe, &sa, 0))
		{
			return false;  // 创建管道失败
		}
		STARTUPINFO si = {sizeof (si)};
		PROCESS_INFORMATION pi = {0};
		si.dwFlags = STARTF_USESTDHANDLES;
		si.hStdOutput = hWritePipe;  // 重定向标准输出
		si.hStdError = hWritePipe;   // 重定向错误输出
		si.hStdInput = NULL;
		// 执行命令
		if (CreateProcessW (nullptr, const_cast<LPWSTR>(command.c_str ()), nullptr, nullptr, TRUE, CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi))
		{
			CloseHandle (hWritePipe);  // 关闭写入端，防止 ReadFile 阻塞
			std::wstring output;
			CHAR buffer [4098];  // 读取缓冲区
			DWORD bytesRead;
			while (ReadFile (hReadPipe, buffer, sizeof (buffer) - 1, &bytesRead, nullptr) && bytesRead > 0)
			{
				buffer [bytesRead] = '\0'; // 确保字符串终止
				output += (WCHAR *)buffer;
				ZeroMemory (buffer, 4098);
			}
			// 输出调试信息（ANSI 版）
			// 等待进程结束
			WaitForSingleObject (pi.hProcess, INFINITE);
			while (ReadFile (hReadPipe, buffer, sizeof (buffer) - 1, &bytesRead, nullptr) && bytesRead > 0)
			{
				buffer [bytesRead] = '\0'; // 确保字符串终止
				output += (WCHAR *)buffer;
				ZeroMemory (buffer, 4098);
			}
			CloseHandle (hReadPipe);
			std::wstring debugoutput (L"");
			debugoutput += L"Command Line:\n" + command + L"\n";
			debugoutput += L"Process Output:\n" + output + L"\n";
			OutputDebugStringW (debugoutput.c_str ());
			// 检查输出文件是否存在
			if (IsFileExists (outputXmlFile))
			{
				CloseHandle (pi.hProcess);
				CloseHandle (pi.hThread);
			}
			else
			{
				CloseHandle (pi.hProcess);
				CloseHandle (pi.hThread);
				return false; // 失败
			}
		}
		else
		{
			CloseHandle (hReadPipe);
			CloseHandle (hWritePipe);
			return false; // 进程创建失败
		}
		txmlpath = outputXmlFile;
		xmlreader = new pugi::xml_document ();
		if (!xmlreader->load_file (outputXmlFile.c_str ()))
		{
			this->destroy ();
			return false;
		}
		return true;
	}
	bool create (LPCWSTR priFile) { destroy (); if (!priFile) return false; return this->create (std::wstring (priFile)); }
	bool create (IStream *priFStream)
	{
		destroy ();
		if (!PriReader::initMakePri ()) return false;
		if (!priFStream) return false;
		std::wstring tempPriFile;
		do
		{
			tempPriFile = GetUniqueTempFilePathW () + L".pri";
		} while (IsFileExists (tempPriFile));
		HRESULT hr = ExtractFileFromStream (priFStream, tempPriFile.c_str ());
		if (FAILED (hr))
		{
			if (IsFileExists (tempPriFile)) DeleteFileW (tempPriFile.c_str ());
			return false;
		}
		bool res = this->create (tempPriFile);
		DeleteFileW (tempPriFile.c_str ());
		return res;
	}
	// 获取到的指针需要 free 手动释放。
	LPSTR findStringValue (LPCSTR lpMsName, LPCSTR defaultLocaleCode = LcidToLocaleCodeA (GetUserDefaultLCID ()).c_str ())
	{
		pugi::xml_node root = xmlreader->child ("PriInfo");
		if (!root) return NULL;
		pugi::xml_node resmap = root.child ("ResourceMap");
		if (!resmap) return NULL;
		resmap.child ("ResourceMapSubtree");
		for (pugi::xml_node subtree = resmap.child ("ResourceMapSubtree"); subtree; subtree = subtree.next_sibling ("ResourceMapSubtree"))
		{
			std::string resMapName = subtree.attribute ("name").as_string ();
			if (!LabelEqual (resMapName, "resources")) continue;
			LPSTR result = recFindStringValue (subtree, lpMsName, defaultLocaleCode);
			if (result) return result;
		}
		for (pugi::xml_node subtree = resmap.child ("ResourceMapSubtree"); subtree; subtree = subtree.next_sibling ("ResourceMapSubtree"))
		{
			std::string resMapName = subtree.attribute ("name").as_string ();
			LPSTR result = recFindStringValue (subtree, lpMsName, defaultLocaleCode);
			if (result) return result;
		}
		return NULL;
	}
	// 获取到的指针需要 free 手动释放。
	LPWSTR findStringValue (LPCWSTR lpMsName, LPCWSTR defaultLocaleCode = LcidToLocaleCodeW (GetUserDefaultLCID ()).c_str ())
	{
		pugi::xml_node root = xmlreader->child ("PriInfo");
		if (!root) return NULL;
		pugi::xml_node resmap = root.child ("ResourceMap");
		if (!resmap) return NULL;
		for (pugi::xml_node subtree = resmap.child ("ResourceMapSubtree"); subtree; subtree = subtree.next_sibling ("ResourceMapSubtree"))
		{
			std::string resMapName = subtree.attribute ("name").as_string ();
			if (LabelEqual (resMapName, "Files")) continue;
			if (!LabelEqual (resMapName, "resources")) continue;
			LPWSTR result = recFindStringValue (subtree, lpMsName, defaultLocaleCode);
			if (result && lstrlenW (result) > 0) return result;
		}
		for (pugi::xml_node subtree = resmap.child ("ResourceMapSubtree"); subtree; subtree = subtree.next_sibling ("ResourceMapSubtree"))
		{
			std::string resMapName = subtree.attribute ("name").as_string ();
			if (LabelEqual (resMapName, "Files")) continue;
			// if (!LabelEqual (resMapName, "resources")) continue;
			LPWSTR result = recFindStringValue (subtree, lpMsName, defaultLocaleCode);
			if (result && lstrlenW (result) > 0) return result;
		}
		return NULL;
	}
	std::string findStringValue (const std::string &objMsName, const std::string &defaultLocaleCode = LcidToLocaleCodeA (GetUserDefaultLCID ()))
	{
		std::string t ("");
		LPSTR lpStr = findStringValue (objMsName.c_str (), defaultLocaleCode.c_str ());
		if (lpStr) t += lpStr;
		if (lpStr) free (lpStr);
		return t;
	}
	std::wstring findStringValue (const std::wstring &objMsName, const std::wstring &defaultLocaleCode = LcidToLocaleCodeW (GetUserDefaultLCID ()))
	{
		std::wstring t (L"");
		LPWSTR lpStr = findStringValue (objMsName.c_str (), defaultLocaleCode.c_str ());
		if (lpStr) t += lpStr;
		if (lpStr) free (lpStr);
		return t;
	}
	LPSTR findFilePathValue (LPCSTR lpMsName, int defaultScale = GetDPI (), ImageContrast defaultContrast = ImageContrast::none)
	{
		pugi::xml_node root = xmlreader->child ("PriInfo");
		if (!root) return NULL;
		pugi::xml_node resmap = root.child ("ResourceMap");
		if (!resmap) return NULL;
		resmap.child ("ResourceMapSubtree");
		for (pugi::xml_node subtree = resmap.child ("ResourceMapSubtree"); subtree; subtree = subtree.next_sibling ("ResourceMapSubtree"))
		{
			std::string resMapName = subtree.attribute ("name").as_string ();
			if (!LabelEqual (resMapName, "Files")) continue;
			LPSTR result = recFindFilePathValue (subtree, lpMsName, defaultScale, defaultContrast);
			if (result) return result;
		}
		return NULL;
	}
	LPWSTR findFilePathValue (LPCWSTR lpMsName, int defaultScale = GetDPI (), ImageContrast defaultContrast = ImageContrast::none)
	{
		pugi::xml_node root = xmlreader->child ("PriInfo");
		if (!root) return NULL;
		pugi::xml_node resmap = root.child ("ResourceMap");
		if (!resmap) return NULL;
		resmap.child ("ResourceMapSubtree");
		for (pugi::xml_node subtree = resmap.child ("ResourceMapSubtree"); subtree; subtree = subtree.next_sibling ("ResourceMapSubtree"))
		{
			std::string resMapName = subtree.attribute ("name").as_string ();
			if (!LabelEqual (resMapName, "Files")) continue;
			LPWSTR result = recFindFilePathValue (subtree, lpMsName, defaultScale, defaultContrast);
			if (result) return result;
		}
		return NULL;
	}
	std::string findFilePathValue (const std::string &objMsName, int defaultScale = GetDPI (), ImageContrast defaultContrast = ImageContrast::none)
	{
		std::string t ("");
		LPSTR lpStr = findFilePathValue (objMsName.c_str (), defaultScale, defaultContrast);
		if (lpStr) t += lpStr;
		if (lpStr) free (lpStr);
		return t;
	}
	std::wstring findFilePathValue (const std::wstring &objMsName, int defaultScale = GetDPI (), ImageContrast defaultContrast = ImageContrast::none)
	{
		std::wstring t (L"");
		LPWSTR lpStr = findFilePathValue (objMsName.c_str (), defaultScale, defaultContrast);
		if (lpStr) t += lpStr;
		if (lpStr) free (lpStr);
		return t;
	}
};
std::wstring PriReader::makepri = L"";

HPRIREADER CreatePriReader (LPCWSTR priFilePath)
{
	if (!priFilePath) return NULL;
	PriReader *ptr = new PriReader ();
	bool res = ptr->create (priFilePath);
	if (!res)
	{
		delete ptr;
		return NULL;
	}
	return ptr;
}
HPRIREADER CreatePriReaderFromStream (HISTREAM isPriFile)
{
	if (!isPriFile) return NULL;
	PriReader *ptr = new PriReader ();
	bool res = ptr->create ((IStream *)isPriFile);
	if (!res)
	{
		delete ptr;
		return NULL;
	}
	return ptr;
}
void DestroyPriReader (HPRIREADER hpr)
{
	if (!hpr) return;
	PriReader *ptr = (PriReader *)hpr;
	delete hpr;
	return;
}
// 返回的字符串只能肯定不是宽字符串，而不确定具体的编码方式（可能为 ANSI 也有可能为 UTF-8）
// 获取到的字符串需手动释放（调用 free ()）
LPSTR PriReaderFindStringValueA (HPRIREADER hPriReader, LPCSTR lpMsName, LPCSTR lpDefaultLocaleCode)
{
	if (!hPriReader || !lpMsName) return NULL;
	PriReader *ptr = (PriReader *)hPriReader;
	if (lpDefaultLocaleCode) return ptr->findStringValue (lpMsName, lpDefaultLocaleCode);
	else return ptr->findStringValue (lpMsName);
}
LPWSTR PriReaderFindStringValueW (HPRIREADER hPriReader, LPCWSTR lpMsName, LPCWSTR lpDefaultLocaleCode)
{
	if (!hPriReader || !lpMsName) return NULL;
	PriReader *ptr = (PriReader *)hPriReader;
	if (lpDefaultLocaleCode) return ptr->findStringValue (lpMsName, lpDefaultLocaleCode);
	else return ptr->findStringValue (lpMsName);
}
LPSTR PriReaderFindFilePathValueA (HPRIREADER hPriReader, LPCSTR lpMsName, int defaultScale, ImageContrast defaultContrast)
{
	if (!hPriReader || !lpMsName) return NULL;
	PriReader *ptr = (PriReader *)hPriReader;
	if (defaultScale <= 0) defaultScale = GetDPI ();
	if ((int)defaultContrast) defaultContrast = ImageContrast::none;
	return ptr->findFilePathValue (lpMsName, defaultScale, defaultContrast);
}
LPWSTR PriReaderFindFilePathValueW (HPRIREADER hPriReader, LPCWSTR lpMsName, int defaultScale, ImageContrast defaultContrast)
{
	if (!hPriReader || !lpMsName) return NULL;
	PriReader *ptr = (PriReader *)hPriReader;
	if (defaultScale <= 0) defaultScale = GetDPI ();
	if ((int)defaultContrast) defaultContrast = ImageContrast::none;
	return ptr->findFilePathValue (lpMsName, defaultScale, defaultContrast);
}

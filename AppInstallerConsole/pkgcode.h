#pragma once
#include <map>
#include <string>
#include <vector>
#include <Shlwapi.h>
#include <AppxPackaging.h>
#include <map>
#include <strsafe.h>
#include <sstream>
#include <atlbase.h>
template <class t, class t1> void push_no_repeat (std::vector <t> &vec, t1 &member)
{
	for (size_t cnt = 0; cnt < vec.size (); cnt ++) if (member == vec [cnt]) return;
	vec.push_back (member);
}
template <class t> void push_no_repeat (std::vector <t> &vec, t &member)
{
	for (size_t cnt = 0; cnt < vec.size (); cnt ++) if (member == vec [cnt]) return;
	vec.push_back (member);
}
#define spush_no_repeat push_no_repeat

typedef IAppxPackageReader *IAPPXREADER;
typedef IAppxBundleReader *IBUNDLEREADER;

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

void GetCapabilitiesName (APPX_CAPABILITIES caps, std::vector <std::wstring> &output)
{
	if (!&output) return;
	ULONG packageCap = (ULONG)caps;
	if (packageCap & APPX_CAPABILITY_INTERNET_CLIENT) spush_no_repeat (output, L"internetClient");
	if (packageCap & APPX_CAPABILITY_INTERNET_CLIENT_SERVER) spush_no_repeat (output, L"internetClientServer");
	if (packageCap & APPX_CAPABILITY_PRIVATE_NETWORK_CLIENT_SERVER) spush_no_repeat (output, L"privateNetworkClientServer");
	if (packageCap & APPX_CAPABILITY_DOCUMENTS_LIBRARY) spush_no_repeat (output, L"documentsLibrary");
	if (packageCap & APPX_CAPABILITY_PICTURES_LIBRARY) spush_no_repeat (output, L"picturesLibrary");
	if (packageCap & APPX_CAPABILITY_VIDEOS_LIBRARY) spush_no_repeat (output, L"videosLibrary");
	if (packageCap & APPX_CAPABILITY_MUSIC_LIBRARY) spush_no_repeat (output, L"musicLibrary");
	if (packageCap & APPX_CAPABILITY_ENTERPRISE_AUTHENTICATION) spush_no_repeat (output, L"enterpriseAuthentication");
	if (packageCap & APPX_CAPABILITY_SHARED_USER_CERTIFICATES) spush_no_repeat (output, L"sharedUserCertificates");
	if (packageCap & APPX_CAPABILITY_REMOVABLE_STORAGE) spush_no_repeat (output, L"removableStorage");
	if (packageCap & 1024) spush_no_repeat (output, L"appointments"); //APPX_CAPABILITY_APPOINTMENTS
	if (packageCap & 2048) spush_no_repeat (output, L"contacts"); //APPX_CAPABILITY_CONTACTS
	if (packageCap & 0x00001000) spush_no_repeat (output, L"phoneCall"); //APPX_CAPABILITY_PHONE_CALL
	if (packageCap & 0x00002000) spush_no_repeat (output, L"blockedChatMessages"); //APPX_CAPABILITY_BLOCKED_CHAT_MESSAGES
	if (packageCap & 0x00004000) spush_no_repeat (output, L"objects3D"); //APPX_CAPABILITY_OBJECTS3D
	if (packageCap & 0x00008000) spush_no_repeat (output, L"allJoyn"); //APPX_CAPABILITY_ALLJOYN
	if (packageCap & 0x00010000) spush_no_repeat (output, L"codeGeneration"); //APPX_CAPABILITY_CODE_GENERATION
	if (packageCap & 0x00020000) spush_no_repeat (output, L"remoteSystem"); //APPX_CAPABILITY_REMOTE_SYSTEM
	if (packageCap & 0x00040000) spush_no_repeat (output, L"userAccountInformation"); //APPX_CAPABILITY_USER_ACCOUNT_INFORMATION
	if (packageCap & 0x00080000) spush_no_repeat (output, L"extendedExecution"); //APPX_CAPABILITY_EXTENDED_EXECUTION
	if (packageCap & 0x00100000) spush_no_repeat (output, L"location"); //APPX_CAPABILITY_LOCATION
	if (packageCap & 0x00200000) spush_no_repeat (output, L"microphone"); //APPX_CAPABILITY_MICROPHONE
	if (packageCap & 0x00400000) spush_no_repeat (output, L"proximity"); //APPX_CAPABILITY_PROXIMITY
	if (packageCap & 0x00800000) spush_no_repeat (output, L"webcam"); //APPX_CAPABILITY_WEBCAM
	if (packageCap & 0x01000000) spush_no_repeat (output, L"spatialPerception"); //APPX_CAPABILITY_SPATIAL_PERCEPTION
	if (packageCap & 0x02000000) spush_no_repeat (output, L"appointmentsSystem"); //APPX_CAPABILITY_APPOINTMENTS_SYSTEM
	if (packageCap & 0x04000000) spush_no_repeat (output, L"contactsSystem"); //APPX_CAPABILITY_CONTACTS_SYSTEM
	if (packageCap & 0x08000000) spush_no_repeat (output, L"phoneCallSystem"); //APPX_CAPABILITY_PHONE_CALL_SYSTEM
	if (packageCap & 0x10000000) spush_no_repeat (output, L"smsSend"); //APPX_CAPABILITY_SMS_SEND
	if (packageCap & 0x20000000) spush_no_repeat (output, L"userDataTasks"); //APPX_CAPABILITY_USER_DATA_TASKS
	if (packageCap & 0x40000000) spush_no_repeat (output, L"userDataTasksSystem"); //APPX_CAPABILITY_USER_DATA_TASKS_SYSTEM
	if (packageCap & 0x80000000) spush_no_repeat (output, L"userDataAccounts"); //APPX_CAPABILITY_USER_DATA_ACCOUNTS
}

HRESULT GetBundleReader (_In_ LPCWSTR inputFileName, _Outptr_ IAppxBundleReader** bundleReader)
{
	HRESULT hr = S_OK;
	IAppxBundleFactory* appxBundleFactory = NULL;
	IStream* inputStream = NULL;
	hr = CoCreateInstance (__uuidof(AppxBundleFactory), NULL, CLSCTX_INPROC_SERVER, __uuidof(IAppxBundleFactory), (LPVOID*)(&appxBundleFactory));
	if (SUCCEEDED (hr))
	{
		hr = SHCreateStreamOnFileEx (inputFileName, STGM_READ | STGM_SHARE_DENY_NONE, 0, FALSE, NULL, &inputStream);
	}
	if (SUCCEEDED (hr))
	{
		hr = appxBundleFactory->CreateBundleReader (inputStream, bundleReader);
	}
	if (inputStream != NULL)
	{
		inputStream->Release ();
		inputStream = NULL;
	}
	if (appxBundleFactory != NULL)
	{
		appxBundleFactory->Release ();
		appxBundleFactory = NULL;
	}
	return hr;
}
HRESULT GetPackageReader (_In_ LPCWSTR inputFileName, _Outptr_ IAppxPackageReader** reader)
{
	HRESULT hr = S_OK;
	IAppxFactory* appxFactory = NULL;
	IStream* inputStream = NULL;
	hr = CoCreateInstance (__uuidof(AppxFactory), NULL, CLSCTX_INPROC_SERVER, __uuidof(IAppxFactory), (LPVOID*)(&appxFactory));
	if (SUCCEEDED (hr))
	{
		hr = SHCreateStreamOnFileEx (inputFileName, STGM_READ | STGM_SHARE_DENY_NONE, 0, FALSE, NULL, &inputStream);
	}
	if (SUCCEEDED (hr)) hr = appxFactory->CreatePackageReader (inputStream, reader);
	if (inputStream != NULL)
	{
		inputStream->Release ();
		inputStream = NULL;
	}
	if (appxFactory != NULL)
	{
		appxFactory->Release ();
		appxFactory = NULL;
	}
	return hr;
}
HRESULT GetAppxPackageReader (_In_ IStream* inputStream, _Outptr_ IAppxPackageReader** packageReader)
{
	HRESULT hr = S_OK;
	IAppxFactory* appxFactory = NULL;
	hr = CoCreateInstance (__uuidof(AppxFactory), NULL, CLSCTX_INPROC_SERVER, __uuidof(IAppxFactory), (LPVOID*)(&appxFactory));
	if (SUCCEEDED (hr))
	{
		hr = appxFactory->CreatePackageReader (inputStream, packageReader);
	}
	if (appxFactory != NULL)
	{
		appxFactory->Release ();
		appxFactory = NULL;
	}
	return hr;
}

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
VERSION::VERSION (): major (0), minor (0), maintenance (0), build (0) {}
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
std::wstring GetPrerequistOSVersionText (VERSION &ver)
{
	if (ver >= VERSION (10, 0, 22621, 0)) return std::wstring (L"Windows 11 Version 22H2");
	else if (ver >= VERSION (10, 0, 22159, 0)) return std::wstring (L"Windows 11 Initial Release (Version 21H2)");
	else if (ver >= VERSION (10, 0, 19645, 0)) return std::wstring (L"Windows 10 Insider Preview (Build 19645)");
	else if (ver >= VERSION (10, 0, 19541, 0)) return std::wstring (L"Windows 10 Insider Preview (Build 19541)");
	else if (ver >= VERSION (10, 0, 19045, 0)) return std::wstring (L"Windows 10 Version 22H2");
	else if (ver >= VERSION (10, 0, 19044, 0)) return std::wstring (L"Windows 10 Version 21H2 (November 2021 Update)");
	else if (ver >= VERSION (10, 0, 19043, 0)) return std::wstring (L"Windows 10 Version 21H2 (May 2021 Update)");
	else if (ver >= VERSION (10, 0, 19042, 0)) return std::wstring (L"Windows 10 Version 20H2");
	else if (ver >= VERSION (10, 0, 18362, 0)) return std::wstring (L"Windows 10 May 2019 Update");
	else if (ver >= VERSION (10, 0, 17763, 0)) return std::wstring (L"Windows 10 October 2018 Update");
	else if (ver >= VERSION (10, 0, 17134, 0)) return std::wstring (L"Windows 10 April 2018 Update");
	else if (ver >= VERSION (10, 0, 16299, 0)) return std::wstring (L"Windows 10 Fall Creators Update");
	else if (ver >= VERSION (10, 0, 15063, 0)) return std::wstring (L"Windows 10 Creators Update");
	else if (ver >= VERSION (10, 0, 14393, 0)) return std::wstring (L"Windows 10 Anniversary Update");
	else if (ver >= VERSION (10, 0, 10240, 0)) return std::wstring (L"Windows 10");
	else if (ver >= VERSION (10, 0, 0, 0)) return std::wstring (L"Windows 10");
	else if (ver >= VERSION (6, 4, 9807, 0)) return std::wstring (L"Windows 10 Technical Preview");
	else if (ver >= VERSION (6, 3, 0, 0)) return std::wstring (L"Windows 8.1");
	else if (ver >= VERSION (6, 2, 0, 0)) return std::wstring (L"Windows 8");
	return std::wstring (L"");
}

std::vector <std::wstring> applicationItems =
{
	L"Id",
	L"DisplayName",
	L"BackgroundColor",
	L"ForegroundText",
	L"ShortName"
};
typedef std::wstring strlabel, StringLabel;
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

typedef class ApplicationMap: public std::map <std::wstring, std::wstring>
{
	public:
	std::wstring getAppUserModelID ();
	friend bool operator == (ApplicationMap &a1, ApplicationMap &a2)
	{
		const LPWSTR lpKey = L"AppUserModelID";
		std::wstring wKey (lpKey);
		if (a1.find (wKey) == a1.end () && a2.find (wKey) == a2.end ()) return true;
		else if (a1.find (wKey) != a1.end () && a2.find (wKey) != a2.end ())
		{
			return a1 [wKey] == a2 [wKey];
		}
		else return false;
	}
	friend bool operator != (ApplicationMap &a1, ApplicationMap &a2)
	{
		return !(a1 == a2);
	}
} appmap;
std::wstring ApplicationMap::getAppUserModelID ()
{
	if (this->find (L"AppUserModelID") == this->end ()) return std::wstring (L"");
	else return (*this) [std::wstring (L"AppUserModelID")];
}

class DEPINFO
{
	public:
	std::wstring name;
	std::wstring publisher;
	VERSION versionMin;
	DEPINFO (LPWSTR name, LPWSTR publisher, UINT64 versionLimit);
	DEPINFO (IAppxManifestPackageDependency *dependency);
	DEPINFO ();
	bool operator == (DEPINFO &another) { return this->name == another.name; }
	bool isEmpty () { return name.length () == 0; }
};
DEPINFO::DEPINFO (LPWSTR name, LPWSTR publisher, UINT64 versionLimit):
	name (name), publisher (publisher), versionMin (*((VERSION *)&versionLimit))
{
}
DEPINFO::DEPINFO (IAppxManifestPackageDependency *dependency)
{
	LPWSTR deName = NULL;
	LPWSTR dePub = NULL;
	UINT64 deMinVer;
	dependency->GetName (&deName);
	dependency->GetPublisher (&dePub);
	dependency->GetMinVersion (&deMinVer);
	name = std::wstring (L"");
	publisher = std::wstring (L"");
	if (deName) name += deName;
	if (dePub) publisher += dePub;
	versionMin = VERSION (deMinVer);
	if (deName) CoTaskMemFree (deName);
	if (dePub) CoTaskMemFree (dePub);
}
DEPINFO::DEPINFO (): name (L""), publisher (L""), versionMin (0) {}

class AppxReader
{
	private:
	std::wstring m_filePath = L"";
	IAPPXREADER m_appxReader = NULL;
	public:
	// 每一次的创建都是先销毁再创建，所以创建失败了之前成功创建的内容也会被销毁。
	AppxReader (LPCWSTR swFilePath);
	AppxReader (const std::wstring swFilePath);
	AppxReader (IStream *objcomFile);
	AppxReader (IAppxFile *objcomFile);
	AppxReader ();
	~AppxReader ();
	bool create (LPCWSTR swFilePath);
	bool create (const std::wstring &objswFile);
	bool create (IStream *objcomFile);
	bool create (IAppxFile *objAppxFile);
	// 注意：当执行销毁操作时导入的读取器也会销毁！此方法是很危险的操作
	bool create (IAppxPackageReader *objAppxReader);
	bool destroy ();
	// 由于此读取器不在以文件为主要读取对象，所以如果要判断文件是否可用，使用此。
	bool isFileExists ();
	// 判断读取器是否可用，不鉴别是否以文件的形式打开。
	bool isAvailable ();
	// 用于以文件打开的读取器。
	bool isFileReaderAvailable ();
	// 无需手动释放，该指针会随着此对象的销毁而释放
	const IAPPXREADER getPackageReader () const { return this->m_appxReader; }
	// 无需手动释放，该指针会随着此对象的销毁而释放
	LPCWSTR getFilePath ();
	IAppxManifestReader *getManifest ();
	IAppxManifestPackageId *getIdentity ();
	std::wstring getIdentityName ();
	std::wstring getIdentityPublisher ();
	VERSION getIdentityVersion ();
	size_t getIdentityProcessorArchitecture (std::vector <APPX_PACKAGE_ARCHITECTURE> &arrOut);
	size_t getResourceLanguages (std::vector <std::wstring> &vec);
	IAppxManifestProperties *getProperties ();
	std::wstring getPropertiesStringValue (LPCWSTR lpswName);
	std::wstring getPropertiesStringValue (const std::wstring objswName);
	HRESULT getPropertiesBoolValue (LPCWSTR lpswName, BOOL &bOutput);
	HRESULT getPropertiesBoolValue (const std::wstring objswName, BOOL &bOutput);
	std::wstring getPropertyName ();
	std::wstring getPropertyDescription ();
	std::wstring getPropertyPublisher ();
	std::wstring getPropertyLogo ();
	UINT64 getPrerequisite (LPCWSTR lpswName);
	UINT64 getPrerequisite (const std::wstring objswName);
	VERSION getPrerequisiteOSMinVersion ();
	IAppxManifestApplicationsEnumerator *getApplicationsEnumerator ();
	bool isPackageApplication ();
	size_t getApplications (std::vector <appmap> &output);
	size_t getApplicationUserModelIDs (std::vector <std::wstring> &output);
	size_t getApplicationIDs (std::vector <std::wstring> &output);
	size_t getApplicationVisualElementsBackgroundColor (std::vector <std::wstring> &output);
	APPX_CAPABILITIES getCapabilitiesFlag ();
	size_t getDeviceCapabilities (std::vector <std::wstring> &output);
	// getCapabilitiesFlag 与 getDeviceCapabilities 结合
	size_t getCapabilities (std::vector <std::wstring> &output);
	IAppxManifestPackageDependenciesEnumerator *getDependenciesEnumerator ();
	size_t getDependencies (std::vector <DEPINFO> &output);
	IStream *getPriFileStream ();
	IStream *extractFileToStream (LPCWSTR lpswFilePath);
	IStream *extractFileToStream (std::wstring objswFilePath);
};
AppxReader::AppxReader (LPCWSTR swFilePath) { this->create (swFilePath); }
AppxReader::AppxReader (std::wstring swFilePath) { this->create (swFilePath); }
AppxReader::AppxReader (): m_appxReader (NULL), m_filePath (L"") {}
AppxReader::AppxReader (IStream *objcomFile) : m_filePath (L"")
{
	this->create (objcomFile);
}
AppxReader::AppxReader (IAppxFile *objcomFile) : m_filePath (L"")
{
	this->create (objcomFile);
}
AppxReader::~AppxReader () { this->destroy (); }
bool AppxReader::isFileExists ()
{
	return !this->m_filePath.empty () && this->m_filePath.length () > 0 && IsFileExists (this->m_filePath);
}
bool AppxReader::isAvailable ()
{
	return this->m_appxReader != NULL;
}
bool AppxReader::isFileReaderAvailable ()
{
	return this->isFileExists () && this->isAvailable ();
}
bool AppxReader::destroy ()
{
	if (!this->isAvailable ()) return false; // 表示已销毁等
	if (this->m_appxReader) this->m_appxReader->Release ();
	this->m_appxReader = NULL;
	this->m_filePath = std::wstring (L"");
	return true;
}
bool AppxReader::create (LPCWSTR swFilePath)
{
	this->destroy ();
	if (!swFilePath) return false;
	if (!lstrlenW (swFilePath)) return false;
	if (!IsFileExists (swFilePath)) return false;
	HRESULT hr = GetPackageReader (swFilePath, &this->m_appxReader);
	if (FAILED (hr))
	{
		if (this->m_appxReader) this->m_appxReader->Release ();
		this->m_appxReader = NULL;
		return false;
	}
	if (!this->m_appxReader) return false;
	this->m_filePath = L"";
	this->m_filePath += std::wstring (swFilePath);
	return this->isAvailable ();
}
bool AppxReader::create (const std::wstring &objswFile) { return this->create (objswFile.c_str ()); }
bool AppxReader::create (IStream *objcomFile)
{
	this->destroy ();
	if (!objcomFile) return false;
	HRESULT hr = GetAppxPackageReader (objcomFile, &this->m_appxReader);
	if (!this->m_appxReader) return false;
	if (FAILED (hr))
	{
		if (this->m_appxReader) this->m_appxReader->Release ();
		this->m_appxReader = NULL;
		return false;
	}
	return true;
}
bool AppxReader::create (IAppxFile *objAppxFile)
{
	this->destroy ();
	IStream *ifile = NULL;
	if (!objAppxFile) return false;
	HRESULT hr = objAppxFile->GetStream (&ifile);
	if (FAILED (hr))
	{
		if (ifile) ifile->Release ();
		return false;
	}
	return this->create (ifile);
}
bool AppxReader::create (IAppxPackageReader *objAppxReader)
{
	this->destroy ();
	if (!objAppxReader) return false;
	this->m_appxReader = objAppxReader;
	return objAppxReader != NULL;
}
LPCWSTR AppxReader::getFilePath () { return this->m_filePath.c_str (); }
IAppxManifestReader *AppxReader::getManifest ()
{
	if (!this->isAvailable ()) return NULL;
	IAppxManifestReader *manifest = NULL;
	HRESULT hr = this->m_appxReader->GetManifest (&manifest);
	if (!manifest) return NULL;
	if (FAILED (hr))
	{
		if (manifest) manifest->Release ();
		return NULL;
	}
	return manifest;
}
IAppxManifestPackageId *AppxReader::getIdentity ()
{
	if (!this->isAvailable ()) return NULL;
	IAppxManifestReader *manifest = this->getManifest ();
	if (!manifest) return NULL;
	IAppxManifestPackageId *packageId = NULL;
	HRESULT hr = manifest->GetPackageId (&packageId);
	if (manifest) { manifest->Release (); manifest = NULL; }
	if (!packageId) return NULL;
	if (FAILED (hr))
	{
		if (packageId) packageId->Release ();
		return NULL;
	}
	return packageId;
}
std::wstring AppxReader::getIdentityName ()
{
	std::wstring temp (L"");
	IAppxManifestPackageId *packageId = this->getIdentity ();
	if (!packageId) return temp;
	LPWSTR lpwzName = NULL;
	HRESULT hr = packageId->GetName (&lpwzName);
	if (FAILED (hr))
	{
		if (lpwzName) CoTaskMemFree (lpwzName);
		packageId->Release ();
		return temp;
	}
	if (lpwzName) temp += lpwzName;
	if (lpwzName) CoTaskMemFree (lpwzName);
	packageId->Release ();
	return temp;
}
std::wstring AppxReader::getIdentityPublisher ()
{
	std::wstring temp (L"");
	IAppxManifestPackageId *packageId = this->getIdentity ();
	if (!packageId) return temp;
	LPWSTR lpwzName = NULL;
	HRESULT hr = packageId->GetPublisher (&lpwzName);
	if (FAILED (hr))
	{
		if (lpwzName) CoTaskMemFree (lpwzName);
		packageId->Release ();
		return temp;
	}
	if (lpwzName) temp += lpwzName;
	if (lpwzName) CoTaskMemFree (lpwzName);
	packageId->Release ();
	return temp;
}
VERSION AppxReader::getIdentityVersion ()
{
	VERSION temp (0);
	IAppxManifestPackageId *packageId = this->getIdentity ();
	if (!packageId) return temp;
	UINT64 u64 = 0;
	HRESULT hr = packageId->GetVersion (&u64);
	temp = VERSION (u64);
	packageId->Release ();
	return temp;
}
size_t AppxReader::getIdentityProcessorArchitecture (std::vector <APPX_PACKAGE_ARCHITECTURE> &arrOut)
{
	if (!&arrOut) return 0;
	arrOut.clear ();
	IAppxManifestPackageId *packageId = this->getIdentity ();
	if (!packageId) return 0;
	APPX_PACKAGE_ARCHITECTURE apa = APPX_PACKAGE_ARCHITECTURE_NEUTRAL;
	packageId->GetArchitecture (&apa);
	push_no_repeat (arrOut, apa);
	return arrOut.size ();
}
size_t AppxReader::getResourceLanguages (std::vector <std::wstring> &vec)
{
	if (!&vec) return 0;
	vec.clear ();
	CComPtr <IAppxManifestReader> manifest = NULL;
	HRESULT hr = this->m_appxReader->GetManifest (&manifest);
	if (FAILED (hr)) return 0;
	CComPtr <IAppxManifestReader2> manifest2 = NULL;
	hr = manifest->QueryInterface (__uuidof(IAppxManifestReader2), (void**)&manifest2);
	if (FAILED (hr)) return 0;
	CComPtr <IAppxManifestQualifiedResourcesEnumerator> qres = NULL;
	hr = manifest2->GetQualifiedResources (&qres);
	if (FAILED (hr)) return 0;
	BOOL hasCurrent = FALSE;
	hr = qres->GetHasCurrent (&hasCurrent);
	while (SUCCEEDED (hr) && hasCurrent)
	{
		CComPtr <IAppxManifestQualifiedResource> res = NULL;
		hr = qres->GetCurrent (&res);
		if (FAILED (hr)) goto Loop_MoveNext;
		LPWSTR lpswLCode = NULL;
		hr = res->GetLanguage (&lpswLCode);
		if (FAILED (hr)) goto Loop_MoveNext;
		if (lpswLCode && lstrlenW (lpswLCode) > 0)
		{
			std::wstring tempStr (lpswLCode), tempLowerStr;
			tempLowerStr = StringToLower (tempStr);
			push_no_repeat (vec, lpswLCode);
		}
	Loop_MoveNext:
		hr = qres->MoveNext (&hasCurrent);
	}
	return vec.size ();
}
IAppxManifestProperties *AppxReader::getProperties ()
{
	IAppxManifestReader *manifest = this->getManifest ();
	if (!manifest) return NULL;
	IAppxManifestProperties *properties = NULL;
	HRESULT hr = manifest->GetProperties (&properties);
	if (manifest) { manifest->Release (); manifest = NULL; }
	if (!properties) return NULL;
	if (FAILED (hr)) { if (properties) properties->Release (); return NULL; }
	return properties;
}
std::wstring AppxReader::getPropertiesStringValue (LPCWSTR lpswName)
{
	std::wstring temp (L"");
	if (!lpswName) return NULL;
	IAppxManifestProperties *properties = this->getProperties ();
	if (!properties) return temp;
	LPWSTR lpswValue = NULL;
	HRESULT hr = properties->GetStringValue (lpswName, &lpswValue);
	properties->Release ();
	if (lpswValue) { temp += lpswValue; CoTaskMemFree (lpswValue); lpswValue = NULL; }
	return temp;
}
std::wstring AppxReader::getPropertiesStringValue (const std::wstring objswName)
{
	return this->getPropertiesStringValue (objswName.c_str ());
}
HRESULT AppxReader::getPropertiesBoolValue (LPCWSTR lpswName, BOOL &bOutput)
{
	HRESULT hr = E_FAIL;
	if (!lpswName || !&bOutput) return hr;
	IAppxManifestProperties *properties = this->getProperties ();
	if (!properties) return hr;
	hr = properties->GetBoolValue (lpswName, &bOutput);
	properties->Release ();
	return hr;
}
HRESULT AppxReader::getPropertiesBoolValue (const std::wstring objswName, BOOL &bOutput)
{
	return this->getPropertiesBoolValue (objswName.c_str (), bOutput);
}
std::wstring AppxReader::getPropertyName ()
{
	return this->getPropertiesStringValue (L"DisplayName");
}
std::wstring AppxReader::getPropertyDescription ()
{
	return this->getPropertiesStringValue (L"Description");
}
std::wstring AppxReader::getPropertyPublisher ()
{
	return this->getPropertiesStringValue (L"PublisherDisplayName");
}
std::wstring AppxReader::getPropertyLogo ()
{
	return this->getPropertiesStringValue (L"Logo");
}
// 失败返回 NULL
UINT64 AppxReader::getPrerequisite (LPCWSTR lpswName)
{
	IAppxManifestReader *manifest = this->getManifest ();
	if (!manifest) return NULL;
	UINT64 u64t = 0;
	HRESULT hr = manifest->GetPrerequisite (lpswName, &u64t);
	if (manifest) manifest->Release ();
	if (FAILED (hr)) return NULL;
	return u64t;
}
// 失败返回 NULL
UINT64 AppxReader::getPrerequisite (const std::wstring objswName)
{
	return this->getPrerequisite (objswName.c_str ());
}
VERSION AppxReader::getPrerequisiteOSMinVersion ()
{
	UINT64 temp = this->getPrerequisite (L"OSMinVersion");
	return VERSION (temp);
}
IAppxManifestApplicationsEnumerator *AppxReader::getApplicationsEnumerator ()
{
	IAppxManifestReader *manifest = this->getManifest ();
	if (!manifest) return NULL;
	IAppxManifestApplicationsEnumerator *temp = NULL;
	HRESULT hr = manifest->GetApplications (&temp);
	manifest->Release ();
	if (!temp) return NULL;
	if (FAILED (hr)) { if (temp) temp->Release (); return NULL; }
	return temp;
}
bool AppxReader::isPackageApplication ()
{
	IAppxManifestApplicationsEnumerator *aenum = this->getApplicationsEnumerator ();
	if (!aenum) return false;
	BOOL hasCurrent = FALSE;
	HRESULT hr = aenum->GetHasCurrent (&hasCurrent);
	aenum->Release ();
	return SUCCEEDED (hr) && hasCurrent;
}
size_t AppxReader::getApplications (std::vector <appmap> &output)
{
	if (!&output) return false;
	output.clear ();
	CComPtr <IAppxManifestApplicationsEnumerator> aenum = this->getApplicationsEnumerator ();
	if (!aenum) return 0;
	BOOL hasCurrent = FALSE;
	HRESULT hr = aenum->GetHasCurrent (&hasCurrent);
	while (SUCCEEDED (hr) && hasCurrent)
	{
		CComPtr <IAppxManifestApplication> app = NULL;
		hr = aenum->GetCurrent (&app);
		if (SUCCEEDED (hr))
		{
			appmap map;
			LPWSTR lpswTemp = NULL;
			hr = app->GetAppUserModelId (&lpswTemp);
			if (!lpswTemp) { hr = aenum->MoveNext (&hasCurrent); continue; }
			if (SUCCEEDED (hr))
			{
				{
					std::wstring objT (L"");
					if (lpswTemp) objT += lpswTemp;
					if (objT.length () <= 0)
					{
						if (lpswTemp) CoTaskMemFree (lpswTemp);
						lpswTemp = NULL;
						hr = aenum->MoveNext (&hasCurrent);
						continue;
					}
					map [StringLabel (L"AppUserModelID")] = objT;
				}
				for (auto it : applicationItems)
				{
					LPWSTR lpswTemp2 = NULL;
					hr = app->GetStringValue (it.c_str (), &lpswTemp2);
					if (FAILED (hr) || !lpswTemp2)
					{
						if (lpswTemp2) CoTaskMemFree (lpswTemp2);
						continue;
					}
					if (lstrlenW (lpswTemp2) <= 0)
					{
						if (lpswTemp2) CoTaskMemFree (lpswTemp2);
						continue;
					}
					std::wstring objT (L"");
					if (lpswTemp2) objT += lpswTemp2;
					map [StringLabel (it)] = objT;
				}
				push_no_repeat (output, map);
			}
			if (!lpswTemp) CoTaskMemFree (lpswTemp);
		}
		hr = aenum->MoveNext (&hasCurrent);
	}
	return output.size ();
}
size_t AppxReader::getApplicationUserModelIDs (std::vector <std::wstring> &output)
{
	if (!&output) return 0;
	output.clear ();
	std::vector <appmap> maps;
	this->getApplications (maps);
	for (auto it : maps)
	{
		std::wstring t (L"");
		t += it.getAppUserModelID ();
		push_no_repeat (output, t);
	}
	return output.size ();
}
size_t AppxReader::getApplicationIDs (std::vector <std::wstring> &output)
{
	if (!&output) return 0;
	output.clear ();
	std::vector <appmap> maps;
	this->getApplications (maps);
	for (auto it : maps)
	{
		std::wstring t (L"");
		t += it [StringLabel (L"Id")];
		push_no_repeat (output, t);
	}
	return output.size ();
}
size_t AppxReader::getApplicationVisualElementsBackgroundColor (std::vector <std::wstring> &output)
{
	if (!&output) return 0;
	output.clear ();
	std::vector <appmap> maps;
	this->getApplications (maps);
	for (auto it : maps)
	{
		std::wstring t (L"");
		t += it [StringLabel (L"BackgroundColor")];
		push_no_repeat (output, t);
	}
	return output.size ();
}
APPX_CAPABILITIES AppxReader::getCapabilitiesFlag ()
{
	CComPtr <IAppxManifestReader> manifest = this->getManifest ();
	if (!manifest) return (APPX_CAPABILITIES)NULL;
	APPX_CAPABILITIES temp;
	HRESULT hr = manifest->GetCapabilities (&temp);
	if (FAILED (hr)) return (APPX_CAPABILITIES)NULL;
	return temp;
}
size_t AppxReader::getDeviceCapabilities (std::vector <std::wstring> &output)
{
	if (!&output) return 0;
	output.clear ();
	CComPtr <IAppxManifestReader> manifest = this->getManifest ();
	if (!manifest) return 0;
	CComPtr <IAppxManifestDeviceCapabilitiesEnumerator> denum = NULL;
	HRESULT hr = manifest->GetDeviceCapabilities (&denum);
	if (!denum) return 0;
	if (FAILED (hr)) return 0;
	BOOL hasCurrent = FALSE;
	hr = denum->GetHasCurrent (&hasCurrent);
	while (SUCCEEDED (hr) && hasCurrent)
	{
		LPWSTR lpswT = NULL;
		hr = denum->GetCurrent (&lpswT);
		if (lpswT)
		{
			if (SUCCEEDED (hr))
			{
				std::wstring objswT (L"");
				if (lpswT) objswT += lpswT;
				if (objswT.length () > 0) push_no_repeat (output, objswT);
				if (lpswT) CoTaskMemFree (lpswT);
			}
		}
		hr = denum->MoveNext (&hasCurrent);
	}
	return output.size ();
}
size_t AppxReader::getCapabilities (std::vector <std::wstring> &output)
{
	if (!&output) return 0;
	output.clear ();
	std::vector <std::wstring> dcap;
	GetCapabilitiesName (this->getCapabilitiesFlag (), output);
	this->getDeviceCapabilities (dcap);
	for (auto it : dcap)
	{
		std::wstring temp (L"");
		temp += it;
		push_no_repeat (output, temp);
	}
	return output.size ();
}
IAppxManifestPackageDependenciesEnumerator *AppxReader::getDependenciesEnumerator ()
{
	CComPtr <IAppxManifestReader> manifest = this->getManifest ();
	if (!manifest) return NULL;
	IAppxManifestPackageDependenciesEnumerator *deps;
	HRESULT hr = manifest->GetPackageDependencies (&deps);
	if (!deps) return NULL;
	if (FAILED (hr))
	{
		if (deps) deps->Release ();
		return NULL;
	}
	return deps;
}
size_t AppxReader::getDependencies (std::vector <DEPINFO> &output)
{
	if (!&output) return 0;
	output.clear ();
	CComPtr <IAppxManifestPackageDependenciesEnumerator> deps = this->getDependenciesEnumerator ();
	if (!deps) return 0;
	BOOL hasCurrent = FALSE;
	HRESULT hr = deps->GetHasCurrent (&hasCurrent);
	while (SUCCEEDED (hr) && hasCurrent)
	{
		CComPtr <IAppxManifestPackageDependency> dep = NULL;
		hr = deps->GetCurrent (&dep);
		if (SUCCEEDED (hr))
		{
			DEPINFO dinfo (dep);
			push_no_repeat (output, dinfo);
		}
		hr = deps->MoveNext (&hasCurrent);
	}
	return output.size ();
}
IStream *AppxReader::extractFileToStream (LPCWSTR lpswFilePath)
{
	if (!lpswFilePath) return false;
	if (lstrlenW (lpswFilePath) <= 0) return false;
	CComPtr <IAppxFile> ifile = NULL;
	HRESULT hr = this->m_appxReader->GetPayloadFile (lpswFilePath, &ifile);
	if (!ifile) return NULL;
	if (FAILED (hr)) return NULL;
	IStream *sfile = NULL;
	hr = ifile->GetStream (&sfile);
	if (!sfile) return NULL;
	if (FAILED (hr)) return NULL;
	return sfile;
}
IStream *AppxReader::extractFileToStream (std::wstring objswFilePath)
{
	return this->extractFileToStream (objswFilePath.c_str ());
}
IStream *AppxReader::getPriFileStream ()
{
	return this->extractFileToStream (L"resources.pri");
}

class BundleReader
{
	private:
	std::wstring m_filePath = L"";
	IBUNDLEREADER m_bundleReader = NULL;
	public:
	// 每一次的创建都是先销毁再创建，所以创建失败了之前成功创建的内容也会被销毁。
	BundleReader (LPCWSTR swFilePath);
	BundleReader (const std::wstring swFilePath);
	BundleReader ();
	~BundleReader ();
	bool create (LPCWSTR swFilePath);
	bool create (const std::wstring &objswFile);
	// 注意：当执行销毁操作时导入的读取器也会销毁！此方法是很危险的操作
	bool destroy ();
	// 由于此读取器不在以文件为主要读取对象，所以如果要判断文件是否可用，使用此。
	bool isFileExists ();
	// 判断读取器是否可用，不鉴别是否以文件的形式打开。
	bool isAvailable ();
	// 用于以文件打开的读取器。
	bool isFileReaderAvailable ();
	// 无需手动释放，该指针会随着此对象的销毁而释放
	const IBUNDLEREADER getPackageReader () const { return this->m_bundleReader; }
	// 需手动释放
	IAPPXREADER getAppxPackageReader ();
	// 无需手动释放，该指针会随着此对象的销毁而释放
	LPCWSTR getFilePath ();
	IAppxBundleManifestReader *getManifest ();
	// 获取子包的清单，优先为应用包，其次为资源包/依赖包
	IAppxManifestReader *getAppxManifest ();
	IAppxManifestPackageId *getIdentity ();
	IAppxManifestPackageId *getAppxIdentity ();
	std::wstring getIdentityName ();
	std::wstring getIdentityPublisher ();
	VERSION getIdentityVersion ();
	size_t getIdentityProcessorArchitecture (std::vector <APPX_PACKAGE_ARCHITECTURE> &arrOut);
	size_t getResourceLanguages (std::vector <std::wstring> &vec);
	IAppxManifestProperties *getProperties ();
	std::wstring getPropertiesStringValue (LPCWSTR lpswName);
	std::wstring getPropertiesStringValue (const std::wstring objswName);
	HRESULT getPropertiesBoolValue (LPCWSTR lpswName, BOOL &bOutput);
	HRESULT getPropertiesBoolValue (const std::wstring objswName, BOOL &bOutput);
	std::wstring getPropertyName ();
	std::wstring getPropertyDescription ();
	std::wstring getPropertyPublisher ();
	std::wstring getPropertyLogo ();
	UINT64 getPrerequisite (LPCWSTR lpswName);
	UINT64 getPrerequisite (const std::wstring objswName);
	VERSION getPrerequisiteOSMinVersion ();
	IAppxManifestApplicationsEnumerator *getApplicationsEnumerator ();
	bool isPackageApplication ();
	size_t getApplications (std::vector <appmap> &output);
	size_t getApplicationUserModelIDs (std::vector <std::wstring> &output);
	size_t getApplicationIDs (std::vector <std::wstring> &output);
	size_t getApplicationVisualElementsBackgroundColor (std::vector <std::wstring> &output);
	APPX_CAPABILITIES getCapabilitiesFlag ();
	size_t getDeviceCapabilities (std::vector <std::wstring> &output);
	// getCapabilitiesFlag 与 getDeviceCapabilities 结合
	size_t getCapabilities (std::vector <std::wstring> &output);
	IAppxManifestPackageDependenciesEnumerator *getDependenciesEnumerator ();
	size_t getDependencies (std::vector <DEPINFO> &output);
	IStream *getPriFileStream ();
	IStream *extractFileToStream (LPCWSTR lpswFilePath);
	IStream *extractFileToStream (std::wstring objswFilePath);
};
BundleReader::BundleReader (LPCWSTR swFilePath) { this->create (swFilePath); }
BundleReader::BundleReader (const std::wstring swFilePath) { this->create (swFilePath); }
BundleReader::BundleReader (): m_filePath (L""), m_bundleReader (NULL) {}
BundleReader::~BundleReader () { this->destroy (); }
bool BundleReader::isFileExists ()
{
	return !this->m_filePath.empty () && this->m_filePath.length () > 0 && IsFileExists (this->m_filePath);
}
bool BundleReader::isAvailable ()
{
	return this->m_bundleReader != NULL;
}
bool BundleReader::isFileReaderAvailable ()
{
	return this->isFileExists () && this->isAvailable ();
}
bool BundleReader::destroy ()
{
	if (!this->isAvailable ()) return false; // 表示已销毁等
	if (this->m_bundleReader) this->m_bundleReader->Release ();
	this->m_bundleReader = NULL;
	this->m_filePath = std::wstring (L"");
	return true;
}
bool BundleReader::create (LPCWSTR swFilePath)
{
	this->destroy ();
	if (!swFilePath) return false;
	if (!lstrlenW (swFilePath)) return false;
	if (!IsFileExists (swFilePath)) return false;
	HRESULT hr = GetBundleReader (swFilePath, &this->m_bundleReader);
	if (FAILED (hr))
	{
		if (this->m_bundleReader) this->m_bundleReader->Release ();
		this->m_bundleReader = NULL;
		return false;
	}
	if (!this->m_bundleReader) return false;
	this->m_filePath = L"";
	this->m_filePath += std::wstring (swFilePath);
	return this->isAvailable ();
}
bool BundleReader::create (const std::wstring &objswFile) { return this->create (objswFile.c_str ()); }
LPCWSTR BundleReader::getFilePath () { return this->m_filePath.c_str (); }
IAppxBundleManifestReader *BundleReader::getManifest ()
{
	if (!this->isAvailable ()) return NULL;
	IAppxBundleManifestReader *bmanifest = NULL;
	HRESULT hr = this->m_bundleReader->GetManifest (&bmanifest);
	if (!bmanifest) return NULL;
	if (FAILED (hr))
	{
		if (bmanifest) bmanifest->Release ();
		return NULL;
	}
	return bmanifest;
}
IAPPXREADER BundleReader::getAppxPackageReader ()
{
	CComPtr<IAppxBundleManifestReader> bmanifest = this->getManifest ();
	if (!bmanifest) return NULL;
	CComPtr<IAppxBundleManifestPackageInfoEnumerator> pienum = NULL;
	HRESULT hr = bmanifest->GetPackageInfoItems (&pienum);
	if (!pienum || FAILED (hr)) return NULL;
	BOOL hasCurrent = FALSE;
	hr = pienum->GetHasCurrent (&hasCurrent);
	while (SUCCEEDED (hr) && hasCurrent)
	{
		CComPtr<IAppxBundleManifestPackageInfo> pinfo = NULL;
		hr = pienum->GetCurrent (&pinfo);
		if (pinfo && SUCCEEDED (hr))
		{
			APPX_BUNDLE_PAYLOAD_PACKAGE_TYPE ftype = APPX_BUNDLE_PAYLOAD_PACKAGE_TYPE_RESOURCE;
			hr = pinfo->GetPackageType (&ftype);
			if (SUCCEEDED (hr) && ftype == APPX_BUNDLE_PAYLOAD_PACKAGE_TYPE_APPLICATION)
			{
				LPWSTR lpswSubPkgName = NULL;
				hr = pinfo->GetFileName (&lpswSubPkgName);
				if (lpswSubPkgName && SUCCEEDED (hr))
				{
					CComPtr<IAppxFile> ifile = NULL;
					hr = this->m_bundleReader->GetPayloadPackage (lpswSubPkgName, &ifile);
					CoTaskMemFree (lpswSubPkgName);
					if (ifile && SUCCEEDED (hr))
					{
						CComPtr <IStream> isfile = NULL;
						hr = ifile->GetStream (&isfile);
						if (SUCCEEDED (hr) && isfile)
						{
							IAPPXREADER areader = NULL;
							hr = GetAppxPackageReader (isfile, &areader);
							if (FAILED (hr))
							{
								if (areader) areader->Release ();
								return NULL;
							}
							return areader;
						}
					}
				}
			}
		}
		hr = pienum->MoveNext (&hasCurrent);
	}
	hr = pienum->GetHasCurrent (&hasCurrent);
	while (SUCCEEDED (hr) && hasCurrent)
	{
		CComPtr<IAppxBundleManifestPackageInfo> pinfo = NULL;
		hr = pienum->GetCurrent (&pinfo);
		if (pinfo && SUCCEEDED (hr))
		{
			LPWSTR lpswSubPkgName = NULL;
			hr = pinfo->GetFileName (&lpswSubPkgName);
			if (lpswSubPkgName && SUCCEEDED (hr))
			{
				CComPtr<IAppxFile> ifile = NULL;
				hr = this->m_bundleReader->GetPayloadPackage (lpswSubPkgName, &ifile);
				CoTaskMemFree (lpswSubPkgName);
				if (ifile && SUCCEEDED (hr))
				{
					CComPtr <IStream> isfile = NULL;
					hr = ifile->GetStream (&isfile);
					if (SUCCEEDED (hr) && isfile)
					{
						IAPPXREADER areader = NULL;
						hr = GetAppxPackageReader (isfile, &areader);
						if (FAILED (hr))
						{
							if (areader) areader->Release ();
							return NULL;
						}
						return areader;
					}
				}
			}
		}
		hr = pienum->MoveNext (&hasCurrent);
	}
	return NULL;
}
IAppxManifestReader* BundleReader::getAppxManifest ()
{
	CComPtr<IAppxBundleManifestReader> bmanifest = this->getManifest ();
	if (!bmanifest) return NULL;
	CComPtr<IAppxBundleManifestPackageInfoEnumerator> pienum = NULL;
	HRESULT hr = bmanifest->GetPackageInfoItems (&pienum);
	if (!pienum || FAILED (hr)) return NULL;
	BOOL hasCurrent = FALSE;
	hr = pienum->GetHasCurrent (&hasCurrent);
	while (SUCCEEDED (hr) && hasCurrent)
	{
		CComPtr<IAppxBundleManifestPackageInfo> pinfo = NULL;
		hr = pienum->GetCurrent (&pinfo);
		if (pinfo && SUCCEEDED (hr))
		{
			APPX_BUNDLE_PAYLOAD_PACKAGE_TYPE ftype = APPX_BUNDLE_PAYLOAD_PACKAGE_TYPE_RESOURCE;
			hr = pinfo->GetPackageType (&ftype);
			if (SUCCEEDED (hr) && ftype == APPX_BUNDLE_PAYLOAD_PACKAGE_TYPE_APPLICATION)
			{
				LPWSTR lpswSubPkgName = NULL;
				hr = pinfo->GetFileName (&lpswSubPkgName);
				if (lpswSubPkgName && SUCCEEDED (hr))
				{
					CComPtr<IAppxFile> ifile = NULL;
					hr = this->m_bundleReader->GetPayloadPackage (lpswSubPkgName, &ifile);
					CoTaskMemFree (lpswSubPkgName);
					if (ifile && SUCCEEDED (hr))
					{
						AppxReader areader (ifile);
						IAppxManifestReader* amanifest = areader.getManifest ();
						if (amanifest) return amanifest;
					}
				}
			}
		}
		hr = pienum->MoveNext (&hasCurrent);
	}
	hr = pienum->GetHasCurrent (&hasCurrent);
	while (SUCCEEDED (hr) && hasCurrent)
	{
		CComPtr<IAppxBundleManifestPackageInfo> pinfo = NULL;
		hr = pienum->GetCurrent (&pinfo);
		if (pinfo && SUCCEEDED (hr))
		{
			LPWSTR lpswSubPkgName = NULL;
			hr = pinfo->GetFileName (&lpswSubPkgName);
			if (lpswSubPkgName && SUCCEEDED (hr))
			{
				CComPtr<IAppxFile> ifile = NULL;
				hr = this->m_bundleReader->GetPayloadPackage (lpswSubPkgName, &ifile);
				CoTaskMemFree (lpswSubPkgName);
				if (ifile && SUCCEEDED (hr))
				{
					AppxReader areader (ifile);
					IAppxManifestReader* amanifest = areader.getManifest ();
					if (amanifest) return amanifest;
				}
			}
		}
		hr = pienum->MoveNext (&hasCurrent);
	}
	return NULL;
}
IAppxManifestPackageId *BundleReader::getIdentity ()
{
	CComPtr <IAppxBundleManifestReader> bmanifest = this->getManifest ();
	if (!bmanifest) return NULL;
	IAppxManifestPackageId *packageId = NULL;
	HRESULT hr = bmanifest->GetPackageId (&packageId);
	if (!packageId) return NULL;
	if (FAILED (hr))
	{
		packageId->Release ();
		return NULL;
	}
	return packageId;
}
IAppxManifestPackageId *BundleReader::getAppxIdentity ()
{
	CComPtr <IAppxManifestReader> amanifest = this->getAppxManifest ();
	if (!amanifest) return NULL;
	IAppxManifestPackageId *packageId = NULL;
	HRESULT hr = amanifest->GetPackageId (&packageId);
	if (!packageId) return NULL;
	if (FAILED (hr))
	{
		packageId->Release ();
		return NULL;
	}
	return packageId;
}
std::wstring BundleReader::getIdentityName ()
{
	std::wstring t (L"");
	CComPtr <IAppxManifestPackageId> packageId = this->getAppxIdentity ();
	if (!packageId) packageId = this->getIdentity ();
	if (!packageId) return t;
	LPWSTR lpswT = NULL;
	HRESULT hr = packageId->GetName (&lpswT);
	if (!lpswT) return t;
	if (FAILED (hr)) { if (lpswT) CoTaskMemFree (lpswT); return t; }
	if (lpswT) t += lpswT;
	if (lpswT) CoTaskMemFree (lpswT);
	return t;
}
std::wstring BundleReader::getIdentityPublisher ()
{
	std::wstring t (L"");
	CComPtr <IAppxManifestPackageId> packageId = this->getAppxIdentity ();
	if (!packageId) packageId = this->getIdentity ();
	if (!packageId) return t;
	LPWSTR lpswT = NULL;
	HRESULT hr = packageId->GetPublisher (&lpswT);
	if (!lpswT) return t;
	if (FAILED (hr)) { if (lpswT) CoTaskMemFree (lpswT); return t; }
	if (lpswT) t += lpswT;
	if (lpswT) CoTaskMemFree (lpswT);
	return t;
}
VERSION BundleReader::getIdentityVersion ()
{
	VERSION v (0);
	CComPtr <IAppxManifestPackageId> packageId = this->getAppxIdentity ();
	if (!packageId) packageId = this->getIdentity ();
	if (!packageId) return v;
	UINT64 u64 = 0;
	HRESULT hr = packageId->GetVersion (&u64);
	v = VERSION (u64);
	return v;
}
size_t BundleReader::getIdentityProcessorArchitecture (std::vector <APPX_PACKAGE_ARCHITECTURE> &arrOut)
{
	CComPtr<IAppxBundleManifestReader> bmanifest = this->getManifest ();
	if (!bmanifest) return 0;
	CComPtr<IAppxBundleManifestPackageInfoEnumerator> pienum = NULL;
	HRESULT hr = bmanifest->GetPackageInfoItems (&pienum);
	if (!pienum || FAILED (hr)) return 0;
	BOOL hasCurrent = FALSE;
	hr = pienum->GetHasCurrent (&hasCurrent);
	while (SUCCEEDED (hr) && hasCurrent)
	{
		CComPtr<IAppxBundleManifestPackageInfo> pinfo = NULL;
		hr = pienum->GetCurrent (&pinfo);
		if (pinfo && SUCCEEDED (hr))
		{
			APPX_BUNDLE_PAYLOAD_PACKAGE_TYPE ftype = APPX_BUNDLE_PAYLOAD_PACKAGE_TYPE_RESOURCE;
			hr = pinfo->GetPackageType (&ftype);
			if (SUCCEEDED (hr) && ftype == APPX_BUNDLE_PAYLOAD_PACKAGE_TYPE_APPLICATION)
			{
				LPWSTR lpswSubPkgName = NULL;
				hr = pinfo->GetFileName (&lpswSubPkgName);
				if (lpswSubPkgName && SUCCEEDED (hr))
				{
					CComPtr<IAppxFile> ifile = NULL;
					hr = this->m_bundleReader->GetPayloadPackage (lpswSubPkgName, &ifile);
					CoTaskMemFree (lpswSubPkgName);
					if (ifile && SUCCEEDED (hr))
					{
						AppxReader areader (ifile);
						std::vector<APPX_PACKAGE_ARCHITECTURE> tarr;
						areader.getIdentityProcessorArchitecture (tarr);
						for (auto it : tarr) push_no_repeat (arrOut, it);
					}
				}
			}
		}
		hr = pienum->MoveNext (&hasCurrent);
	}
	return arrOut.size ();
}
size_t BundleReader::getResourceLanguages (std::vector <std::wstring> &vec)
{
	CComPtr <IAppxBundleManifestReader> bmanifest = this->getManifest ();
	if (!bmanifest) return 0;
	CComPtr<IAppxBundleManifestPackageInfoEnumerator> pienum = NULL;
	HRESULT hr = bmanifest->GetPackageInfoItems (&pienum);
	if (!pienum || FAILED (hr)) return 0;
	BOOL hasCurrent = FALSE;
	hr = pienum->GetHasCurrent (&hasCurrent);
	while (SUCCEEDED (hr) && hasCurrent)
	{
		CComPtr<IAppxBundleManifestPackageInfo> pinfo = NULL;
		hr = pienum->GetCurrent (&pinfo);
		if (pinfo && SUCCEEDED (hr))
		{
			LPWSTR lpswSubPkgName = NULL;
			hr = pinfo->GetFileName (&lpswSubPkgName);
			if (lpswSubPkgName && SUCCEEDED (hr))
			{
				CComPtr<IAppxFile> ifile = NULL;
				hr = this->m_bundleReader->GetPayloadPackage (lpswSubPkgName, &ifile);
				CoTaskMemFree (lpswSubPkgName);
				if (ifile && SUCCEEDED (hr))
				{
					AppxReader areader (ifile);
					std::vector<std::wstring> tarr;
					areader.getResourceLanguages (tarr);
					for (auto it : tarr) push_no_repeat (vec, it);
				}
			}
		}
		hr = pienum->MoveNext (&hasCurrent);
	}
	return vec.size ();
}
IAppxManifestProperties *BundleReader::getProperties ()
{
	CComPtr <IAppxManifestReader> manifest = this->getAppxManifest ();
	if (!manifest) return NULL;
	IAppxManifestProperties *properties = NULL;
	HRESULT hr = manifest->GetProperties (&properties);
	if (!properties) return NULL;
	if (FAILED (hr)) { if (properties) properties->Release (); return NULL; }
	return properties;
}
std::wstring BundleReader::getPropertiesStringValue (LPCWSTR lpswName)
{
	std::wstring temp (L"");
	if (!lpswName) return NULL;
	IAppxManifestProperties *properties = this->getProperties ();
	if (!properties) return temp;
	LPWSTR lpswValue = NULL;
	HRESULT hr = properties->GetStringValue (lpswName, &lpswValue);
	properties->Release ();
	if (lpswValue) { temp += lpswValue; CoTaskMemFree (lpswValue); lpswValue = NULL; }
	return temp;
}
std::wstring BundleReader::getPropertiesStringValue (const std::wstring objswName)
{
	return this->getPropertiesStringValue (objswName.c_str ());
}
HRESULT BundleReader::getPropertiesBoolValue (LPCWSTR lpswName, BOOL &bOutput)
{
	HRESULT hr = E_FAIL;
	if (!lpswName || !&bOutput) return hr;
	IAppxManifestProperties *properties = this->getProperties ();
	if (!properties) return hr;
	hr = properties->GetBoolValue (lpswName, &bOutput);
	properties->Release ();
	return hr;
}
HRESULT BundleReader::getPropertiesBoolValue (const std::wstring objswName, BOOL &bOutput)
{
	return this->getPropertiesBoolValue (objswName.c_str (), bOutput);
}
std::wstring BundleReader::getPropertyName ()
{
	return this->getPropertiesStringValue (L"DisplayName");
}
std::wstring BundleReader::getPropertyDescription ()
{
	return this->getPropertiesStringValue (L"Description");
}
std::wstring BundleReader::getPropertyPublisher ()
{
	return this->getPropertiesStringValue (L"PublisherDisplayName");
}
std::wstring BundleReader::getPropertyLogo ()
{
	return this->getPropertiesStringValue (L"Logo");
}
UINT64 BundleReader::getPrerequisite (LPCWSTR lpswName)
{
	IAppxManifestReader *manifest = this->getAppxManifest ();
	if (!manifest) return NULL;
	UINT64 u64t = 0;
	HRESULT hr = manifest->GetPrerequisite (lpswName, &u64t);
	if (manifest) manifest->Release ();
	if (FAILED (hr)) return NULL;
	return u64t;
}
UINT64 BundleReader::getPrerequisite (const std::wstring objswName)
{
	return this->getPrerequisite (objswName.c_str ());
}
VERSION BundleReader::getPrerequisiteOSMinVersion ()
{
	UINT64 temp = this->getPrerequisite (L"OSMinVersion");
	return VERSION (temp);
}
IAppxManifestApplicationsEnumerator *BundleReader::getApplicationsEnumerator ()
{
	IAppxManifestReader *manifest = this->getAppxManifest ();
	if (!manifest) return NULL;
	IAppxManifestApplicationsEnumerator *temp = NULL;
	HRESULT hr = manifest->GetApplications (&temp);
	manifest->Release ();
	if (!temp) return NULL;
	if (FAILED (hr)) { if (temp) temp->Release (); return NULL; }
	return temp;
}
bool BundleReader::isPackageApplication ()
{
	IAppxManifestApplicationsEnumerator *aenum = this->getApplicationsEnumerator ();
	if (!aenum) return false;
	BOOL hasCurrent = FALSE;
	HRESULT hr = aenum->GetHasCurrent (&hasCurrent);
	aenum->Release ();
	return SUCCEEDED (hr) && hasCurrent;
}
size_t BundleReader::getApplications (std::vector<appmap>& output)
{
	if (!&output) return 0;
	output.clear ();
	CComPtr<IAppxManifestApplicationsEnumerator> aenum = this->getApplicationsEnumerator ();
	if (!aenum) return 0;
	BOOL hasCurrent = FALSE;
	HRESULT hr = aenum->GetHasCurrent (&hasCurrent);
	while (SUCCEEDED (hr) && hasCurrent)
	{
		CComPtr<IAppxManifestApplication> app = NULL;
		hr = aenum->GetCurrent (&app);
		if (app && SUCCEEDED (hr))
		{
			appmap map;
			LPWSTR lpswTemp = NULL;
			hr = app->GetAppUserModelId (&lpswTemp);
			if (lpswTemp && SUCCEEDED (hr))
			{
				std::wstring objT = lpswTemp;
				CoTaskMemFree (lpswTemp);
				if (!objT.empty ())
				{
					map [StringLabel (L"AppUserModelID")] = objT;
					for (auto it : applicationItems)
					{
						LPWSTR lpswTemp2 = NULL;
						hr = app->GetStringValue (it.c_str (), &lpswTemp2);
						if (lpswTemp2 && SUCCEEDED (hr))
						{
							std::wstring objT2 = lpswTemp2;
							CoTaskMemFree (lpswTemp2);
							if (!objT2.empty ())
							{
								map [StringLabel (it)] = objT2;
							}
						}
					}
					push_no_repeat (output, map);
				}
			}
		}
		hr = aenum->MoveNext (&hasCurrent);
	}
	return output.size ();
}
size_t BundleReader::getApplicationUserModelIDs (std::vector <std::wstring> &output)
{
	if (!&output) return 0;
	output.clear ();
	std::vector <appmap> maps;
	this->getApplications (maps);
	for (auto it : maps)
	{
		std::wstring t (L"");
		t += it.getAppUserModelID ();
		push_no_repeat (output, t);
	}
	return output.size ();
}
size_t BundleReader::getApplicationIDs (std::vector <std::wstring> &output)
{
	if (!&output) return 0;
	output.clear ();
	std::vector <appmap> maps;
	this->getApplications (maps);
	for (auto it : maps)
	{
		std::wstring t (L"");
		t += it [StringLabel (L"Id")];
		push_no_repeat (output, t);
	}
	return output.size ();
}
size_t BundleReader::getApplicationVisualElementsBackgroundColor (std::vector <std::wstring> &output)
{
	if (!&output) return 0;
	output.clear ();
	std::vector <appmap> maps;
	this->getApplications (maps);
	for (auto it : maps)
	{
		std::wstring t (L"");
		t += it [StringLabel (L"BackgroundColor")];
		push_no_repeat (output, t);
	}
	return output.size ();
}
APPX_CAPABILITIES BundleReader::getCapabilitiesFlag ()
{
	CComPtr <IAppxManifestReader> manifest = this->getAppxManifest ();
	if (!manifest) return (APPX_CAPABILITIES)NULL;
	APPX_CAPABILITIES temp;
	HRESULT hr = manifest->GetCapabilities (&temp);
	if (FAILED (hr)) return (APPX_CAPABILITIES)NULL;
	return temp;
}
size_t BundleReader::getDeviceCapabilities (std::vector <std::wstring> &output)
{
	if (!&output) return 0;
	output.clear ();
	CComPtr<IAppxManifestReader> manifest = this->getAppxManifest ();
	if (!manifest) return 0;
	CComPtr<IAppxManifestDeviceCapabilitiesEnumerator> denum = NULL;
	HRESULT hr = manifest->GetDeviceCapabilities (&denum);
	if (!denum || FAILED (hr)) return 0;
	BOOL hasCurrent = FALSE;
	hr = denum->GetHasCurrent (&hasCurrent);
	while (SUCCEEDED (hr) && hasCurrent)
	{
		LPWSTR lpswT = NULL;
		hr = denum->GetCurrent (&lpswT);
		if (lpswT && SUCCEEDED (hr))
		{
			std::wstring objswT = lpswT;
			CoTaskMemFree (lpswT);
			if (!objswT.empty ()) push_no_repeat (output, objswT);
		}
		hr = denum->MoveNext (&hasCurrent);
	}
	return output.size ();
}
size_t BundleReader::getCapabilities (std::vector <std::wstring> &output)
{
	if (!&output) return 0;
	output.clear ();
	std::vector <std::wstring> dcap;
	GetCapabilitiesName (this->getCapabilitiesFlag (), output);
	this->getDeviceCapabilities (dcap);
	for (auto it : dcap)
	{
		std::wstring temp (L"");
		temp += it;
		push_no_repeat (output, temp);
	}
	return output.size ();
}
IAppxManifestPackageDependenciesEnumerator *BundleReader::getDependenciesEnumerator ()
{
	CComPtr <IAppxManifestReader> manifest = this->getAppxManifest ();
	if (!manifest) return NULL;
	IAppxManifestPackageDependenciesEnumerator *deps;
	HRESULT hr = manifest->GetPackageDependencies (&deps);
	if (!deps) return NULL;
	if (FAILED (hr))
	{
		if (deps) deps->Release ();
		return NULL;
	}
	return deps;
}
size_t BundleReader::getDependencies (std::vector <DEPINFO> &output)
{
	if (!&output) return 0;
	output.clear ();
	CComPtr <IAppxManifestPackageDependenciesEnumerator> deps = this->getDependenciesEnumerator ();
	if (!deps) return 0;
	BOOL hasCurrent = FALSE;
	HRESULT hr = deps->GetHasCurrent (&hasCurrent);
	while (SUCCEEDED (hr) && hasCurrent)
	{
		CComPtr <IAppxManifestPackageDependency> dep = NULL;
		hr = deps->GetCurrent (&dep);
		if (SUCCEEDED (hr))
		{
			DEPINFO dinfo (dep);
			push_no_repeat (output, dinfo);
		}
		hr = deps->MoveNext (&hasCurrent);
	}
	return output.size ();
}
IStream *BundleReader::extractFileToStream (LPCWSTR lpswFilePath)
{
	CComPtr<IAppxBundleManifestReader> bmanifest = this->getManifest ();
	if (!bmanifest) return NULL;
	CComPtr<IAppxBundleManifestPackageInfoEnumerator> pienum = NULL;
	HRESULT hr = bmanifest->GetPackageInfoItems (&pienum);
	if (!pienum || FAILED (hr)) return NULL;
	BOOL hasCurrent = FALSE;
	hr = pienum->GetHasCurrent (&hasCurrent);
	while (SUCCEEDED (hr) && hasCurrent)
	{
		CComPtr<IAppxBundleManifestPackageInfo> pinfo = NULL;
		hr = pienum->GetCurrent (&pinfo);
		if (pinfo && SUCCEEDED (hr))
		{
			APPX_BUNDLE_PAYLOAD_PACKAGE_TYPE ftype = APPX_BUNDLE_PAYLOAD_PACKAGE_TYPE_RESOURCE;
			hr = pinfo->GetPackageType (&ftype);
			if (SUCCEEDED (hr) && ftype == APPX_BUNDLE_PAYLOAD_PACKAGE_TYPE_APPLICATION)
			{
				LPWSTR lpswSubPkgName = NULL;
				hr = pinfo->GetFileName (&lpswSubPkgName);
				if (lpswSubPkgName && SUCCEEDED (hr))
				{
					CComPtr<IAppxFile> ifile = NULL;
					hr = this->m_bundleReader->GetPayloadPackage (lpswSubPkgName, &ifile);
					CoTaskMemFree (lpswSubPkgName);

					if (ifile && SUCCEEDED (hr))
					{
						AppxReader areader (ifile);
						IStream *isfile = areader.extractFileToStream (lpswFilePath);
						if (isfile) return isfile;
					}
				}
				else if (lpswSubPkgName)
				{
					CoTaskMemFree (lpswSubPkgName);
				}
			}
		}
		hr = pienum->MoveNext (&hasCurrent);
	}
	while (SUCCEEDED (hr) && hasCurrent)
	{
		CComPtr<IAppxBundleManifestPackageInfo> pinfo = NULL;
		hr = pienum->GetCurrent (&pinfo);
		if (pinfo && SUCCEEDED (hr))
		{
			LPWSTR lpswSubPkgName = NULL;
			hr = pinfo->GetFileName (&lpswSubPkgName);
			if (lpswSubPkgName && SUCCEEDED (hr))
			{
				CComPtr<IAppxFile> ifile = NULL;
				hr = this->m_bundleReader->GetPayloadPackage (lpswSubPkgName, &ifile);
				CoTaskMemFree (lpswSubPkgName);
				if (ifile && SUCCEEDED (hr))
				{
					AppxReader areader (ifile);
					IStream *isfile = areader.extractFileToStream (lpswFilePath);
					if (isfile) return isfile;
				}
			}
			else if (lpswSubPkgName)
			{
				CoTaskMemFree (lpswSubPkgName);
			}
		}
		hr = pienum->MoveNext (&hasCurrent);
	}
	return NULL;
}
IStream *BundleReader::extractFileToStream (std::wstring objswFilePath)
{
	return this->extractFileToStream (objswFilePath.c_str ());
}
IStream *BundleReader::getPriFileStream ()
{
	return this->extractFileToStream (L"resources.pri");
}

typedef enum _PKGTYPE { PKG_UNKNOWN = 0, PKG_SINGLE = 1, PKG_BUNDLE = 2 } PKGTYPE, pkgtype;

typedef class PackageReader
{
	private:
	AppxReader single;
	BundleReader bundle;
	public:
	PackageReader (LPCWSTR lpwzFile);
	PackageReader (std::wstring objwzFile);
	PackageReader ();
	bool create (LPCWSTR lpwzFile);
	bool create (std::wstring objwzFile);
	bool destroy ();
	bool isAvailable ();
	~PackageReader ();
	pkgtype getType ();
	// 失败返回长度为 0 或空的文本。
	std::wstring getIdentityName ();
	// 失败返回长度为 0 或空的文本。
	std::wstring getIdentityPublisher ();
	// 失败返回都为 0 的版本号
	VERSION getIdentityVersion ();
	size_t getIdentityProcessorArchitecture (std::vector <APPX_PACKAGE_ARCHITECTURE> &arrOut);
	size_t getResourceLanguages (std::vector <std::wstring> &vec);
	LPCWSTR getFilePath ();
	IAppxManifestReader *getAppxManifest ();
	IAppxBundleManifestReader *getBundleManifest ();
	// 如果此读取器的读取的包的类型为 Bundle 则指针需要手动释放。除此之外无需手动释放，会随着对象的销毁而销毁
	IAPPXREADER getAppxPackageReader ()
	{
		switch (this->getType ())
		{
			case PKG_SINGLE: return const_cast <IAPPXREADER> (single.getPackageReader ());
			case PKG_BUNDLE: return bundle.getAppxPackageReader ();
		}
		return NULL;
	}
	// 获取到的指针无需手动释放，会随着对象的销毁而销毁
	IBUNDLEREADER getBundlePackageReader ()
	{
		if (this->getType () == PKG_BUNDLE) return const_cast <IBUNDLEREADER> (bundle.getPackageReader ());
		else return NULL;
	}
	std::wstring getPropertyName ();
	std::wstring getPropertyDescription ();
	std::wstring getPropertyPublisher ();
	std::wstring getPropertyLogo ();
	// 失败返回都为 0 的版本号
	VERSION getPrerequisiteOSMinVersion ();
	// 判断当前包是否为应用包而不是资源包或依赖项。如果为捆绑包的话判断是否子包中存在应用包。
	bool isPackageApplication ();
	size_t getApplications (std::vector <appmap> &output);
	size_t getApplicationUserModelIDs (std::vector <std::wstring> &output);
	size_t getApplicationIDs (std::vector <std::wstring> &output);
	size_t getApplicationVisualElementsBackgroundColor (std::vector <std::wstring> &output);
	size_t getCapabilities (std::vector <std::wstring> &output);
	size_t getDependencies (std::vector <DEPINFO> &output);
	IStream *getPriFileStream ();
	IStream *extractFileToStream (LPCWSTR lpswFilePath);
	IStream *extractFileToStream (std::wstring objswFilePath);
} pkgreader;
PackageReader::PackageReader (LPCWSTR lpwzFile) { create (lpwzFile); }
PackageReader::PackageReader (std::wstring objwzFile) { create (objwzFile); }
PackageReader::PackageReader (): single (), bundle () {}
bool PackageReader::create (LPCWSTR lpwzFile)
{
	this->destroy ();
	return bundle.create (lpwzFile) || single.create (lpwzFile);
}
bool PackageReader::create (std::wstring objwzFile) { return this->create (objwzFile.c_str ()); }
bool PackageReader::destroy ()
{
	if (!isAvailable ()) return false;
	bool res = single.destroy ();
	res = bundle.destroy () || res;
	return res;
}
bool PackageReader::isAvailable ()
{
	return single.isFileReaderAvailable () || bundle.isFileReaderAvailable ();
}
PackageReader::~PackageReader () { destroy (); }
pkgtype PackageReader::getType ()
{
	if (single.isFileReaderAvailable ()) return PKG_SINGLE;
	else if (bundle.isFileReaderAvailable ()) return PKG_BUNDLE;
	else return PKG_UNKNOWN;
}
IAppxManifestReader *PackageReader::getAppxManifest ()
{
	switch (this->getType ())
	{
		case PKG_SINGLE: return single.getManifest ();
		case PKG_BUNDLE: return bundle.getAppxManifest ();
	}
	return NULL;
}
IAppxBundleManifestReader *PackageReader::getBundleManifest ()
{
	if (this->getType () == PKG_BUNDLE) return bundle.getManifest ();
	return NULL;
}
// 失败返回长度为 0 或空的文本。
std::wstring PackageReader::getIdentityName ()
{
	std::wstring t (L"");
	if (single.isFileReaderAvailable ()) return single.getIdentityName ();
	if (bundle.isFileReaderAvailable ()) return bundle.getIdentityName ();
	return t;
}
// 失败返回长度为 0 或空的文本。
std::wstring PackageReader::getIdentityPublisher ()
{
	std::wstring t (L"");
	if (single.isFileReaderAvailable ()) return single.getIdentityPublisher ();
	if (bundle.isFileReaderAvailable ()) return bundle.getIdentityPublisher ();
	return t;
}
// 失败返回都为 0 的版本号
VERSION PackageReader::getIdentityVersion ()
{
	VERSION v (0);
	if (single.isFileReaderAvailable ()) return single.getIdentityVersion ();
	if (bundle.isFileReaderAvailable ()) return bundle.getIdentityVersion ();
	return v;
}
size_t PackageReader::getIdentityProcessorArchitecture (std::vector <APPX_PACKAGE_ARCHITECTURE> &arrOut)
{

	if (single.isFileReaderAvailable ()) return single.getIdentityProcessorArchitecture (arrOut);
	if (bundle.isFileReaderAvailable ()) return bundle.getIdentityProcessorArchitecture (arrOut);
	return 0;
}
size_t PackageReader::getResourceLanguages (std::vector <std::wstring> &vec)
{
	if (single.isFileReaderAvailable ()) return single.getResourceLanguages (vec);
	if (bundle.isFileReaderAvailable ()) return bundle.getResourceLanguages (vec);
	return 0;
}
LPCWSTR PackageReader::getFilePath ()
{
	switch (this->getType ())
	{
		case PKG_SINGLE: return this->single.getFilePath ();
		case PKG_BUNDLE: return this->bundle.getFilePath ();
	}
	return NULL;
}
std::wstring PackageReader::getPropertyName ()
{
	if (single.isFileReaderAvailable ()) return single.getPropertyName ();
	if (bundle.isFileReaderAvailable ()) return bundle.getPropertyName ();
	return std::wstring (L"");
}
std::wstring PackageReader::getPropertyDescription ()
{
	if (single.isFileReaderAvailable ()) return single.getPropertyDescription ();
	if (bundle.isFileReaderAvailable ()) return bundle.getPropertyDescription ();
	return std::wstring (L"");
}
std::wstring PackageReader::getPropertyPublisher ()
{
	if (single.isFileReaderAvailable ()) return single.getPropertyPublisher ();
	if (bundle.isFileReaderAvailable ()) return bundle.getPropertyPublisher ();
	return std::wstring (L"");
}
std::wstring PackageReader::getPropertyLogo ()
{
	if (single.isFileReaderAvailable ()) return single.getPropertyLogo ();
	if (bundle.isFileReaderAvailable ()) return bundle.getPropertyLogo ();
	return std::wstring (L"");
}
// 失败返回都为 0 的版本号
VERSION PackageReader::getPrerequisiteOSMinVersion ()
{
	VERSION v (0);
	if (single.isFileReaderAvailable ()) return single.getPrerequisiteOSMinVersion ();
	if (bundle.isFileReaderAvailable ()) return bundle.getPrerequisiteOSMinVersion ();
	return v;
}
// 判断当前包是否为应用包而不是资源包或依赖项。如果为捆绑包的话判断是否子包中存在应用包。
bool PackageReader::isPackageApplication ()
{
	if (single.isFileReaderAvailable ()) return single.isPackageApplication ();
	if (bundle.isFileReaderAvailable ()) return bundle.isPackageApplication ();
	return false;
}
size_t PackageReader::getApplications (std::vector <appmap> &output)
{
	if (single.isFileReaderAvailable ()) return single.getApplications (output);
	if (bundle.isFileReaderAvailable ()) return bundle.getApplications (output);
	return 0;
}
size_t PackageReader::getApplicationUserModelIDs (std::vector <std::wstring> &output)
{
	if (single.isFileReaderAvailable ()) return single.getApplicationUserModelIDs (output);
	if (bundle.isFileReaderAvailable ()) return bundle.getApplicationUserModelIDs (output);
	return 0;
}
size_t PackageReader::getApplicationIDs (std::vector <std::wstring> &output)
{
	if (single.isFileReaderAvailable ()) return single.getApplicationIDs (output);
	if (bundle.isFileReaderAvailable ()) return bundle.getApplicationIDs (output);
	return 0;
}
size_t PackageReader::getApplicationVisualElementsBackgroundColor (std::vector <std::wstring> &output)
{
	if (single.isFileReaderAvailable ()) return single.getApplicationVisualElementsBackgroundColor (output);
	if (bundle.isFileReaderAvailable ()) return bundle.getApplicationVisualElementsBackgroundColor (output);
	return 0;
}
size_t PackageReader::getCapabilities (std::vector <std::wstring> &output)
{
	if (single.isFileReaderAvailable ()) return single.getCapabilities (output);
	if (bundle.isFileReaderAvailable ()) return bundle.getCapabilities (output);
	return 0;
}
size_t PackageReader::getDependencies (std::vector <DEPINFO> &output)
{
	if (single.isFileReaderAvailable ()) return single.getDependencies (output);
	if (bundle.isFileReaderAvailable ()) return bundle.getDependencies (output);
	return 0;
}
IStream *PackageReader::getPriFileStream ()
{
	if (single.isFileReaderAvailable ()) return single.getPriFileStream ();
	if (bundle.isFileReaderAvailable ()) return bundle.getPriFileStream ();
	return NULL;
}
IStream *PackageReader::extractFileToStream (LPCWSTR lpswFilePath)
{
	if (single.isFileReaderAvailable ()) return single.extractFileToStream (lpswFilePath);
	if (bundle.isFileReaderAvailable ()) return bundle.extractFileToStream (lpswFilePath);
	return NULL;
}
IStream *PackageReader::extractFileToStream (std::wstring objswFilePath)
{
	if (single.isFileReaderAvailable ()) return single.extractFileToStream (objswFilePath);
	if (bundle.isFileReaderAvailable ()) return bundle.extractFileToStream (objswFilePath);
	return NULL;
}

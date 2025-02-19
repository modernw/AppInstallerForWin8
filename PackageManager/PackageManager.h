// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 PACKAGEMANAGER_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// PACKAGEMANAGER_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#ifdef PACKAGEMANAGER_EXPORTS
#define PACKAGEMANAGER_API __declspec(dllexport)
#else
#define PACKAGEMANAGER_API __declspec(dllimport)
#endif

// 此类是从 PackageManager.dll 导出的
class PACKAGEMANAGER_API CPackageManager {
public:
	CPackageManager(void);
	// TODO:  在此添加您的方法。
};

extern PACKAGEMANAGER_API int nPackageManager;

PACKAGEMANAGER_API int fnPackageManager(void);

typedef void (*_PACKAGE_ADD_PROGRESS)(unsigned progress);
typedef _PACKAGE_ADD_PROGRESS PACKAGE_ADD_PROGRESS;

enum class InstallStatus
{
	Success,
	Error,
	Canceled
};

EXTERN_C PACKAGEMANAGER_API InstallStatus AddPackageFromPath (LPCWSTR lpswPath, PACKAGE_ADD_PROGRESS callbProgress);

EXTERN_C PACKAGEMANAGER_API LPCWSTR GetLastErrorCode ();

EXTERN_C PACKAGEMANAGER_API LPCWSTR GetLastErrorDetailText ();
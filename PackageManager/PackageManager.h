// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� PACKAGEMANAGER_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// PACKAGEMANAGER_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef PACKAGEMANAGER_EXPORTS
#define PACKAGEMANAGER_API __declspec(dllexport)
#else
#define PACKAGEMANAGER_API __declspec(dllimport)
#endif

// �����Ǵ� PackageManager.dll ������
class PACKAGEMANAGER_API CPackageManager {
public:
	CPackageManager(void);
	// TODO:  �ڴ�������ķ�����
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
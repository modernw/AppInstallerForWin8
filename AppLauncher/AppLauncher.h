// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� APPLAUNCHER_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// APPLAUNCHER_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef APPLAUNCHER_EXPORTS
#define APPLAUNCHER_API __declspec(dllexport)
#else
#define APPLAUNCHER_API __declspec(dllimport)
#endif

EXTERN_C APPLAUNCHER_API HRESULT LaunchApp (LPCWSTR strAppUserModelId, PDWORD pdwProcessId);

// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� CERTIFICATEMANAGER_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// CERTIFICATEMANAGER_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef CERTIFICATEMANAGER_EXPORTS
#define CERTIFICATEMANAGER_API __declspec(dllexport)
#else
#define CERTIFICATEMANAGER_API __declspec(dllimport)
#endif

#define func_certmgr CERTIFICATEMANAGER_API
#define func_c_certmgr extern "C" CERTIFICATEMANAGER_API

func_c_certmgr bool LoadCertFromCertFile (LPCWSTR lpCertFile);

func_c_certmgr bool LoadCertFromSignedFile (LPCWSTR lpSignedFile);

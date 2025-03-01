// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� BASE64IMG_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// BASE64IMG_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef BASE64IMG_EXPORTS
#define BASE64IMG_API __declspec(dllexport)
#else
#define BASE64IMG_API __declspec(dllimport)
#endif

typedef HANDLE HISTREAM;
typedef char *LPSTR;

extern "C" BASE64IMG_API LPSTR StreamToBase64 (HISTREAM ifile);

extern "C" BASE64IMG_API LPWSTR StreamToBase64W (HISTREAM ifile);

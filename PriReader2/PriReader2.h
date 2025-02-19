// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� PRIREADER2_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// PRIREADER2_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef PRIREADER2_EXPORTS
#define PRIREADER2_API __declspec(dllexport)
#else
#define PRIREADER2_API __declspec(dllimport)
#endif

#define class_prireader2 class PRIREADER2_API
#define struct_prireader2 struct PRIREADER2_API
#define enum_prireader2 enum PRIREADER2_API
#define enum_class_prireader2 enum class PRIREADER2_API
#define func_prireader2 PRIREADER2_API
#define func_c_prireader2 extern "C" func_prireader2

func_c_prireader2 int GetDPI ();
func_c_prireader2 bool IsDirectoryExistsA (LPCSTR path);
func_c_prireader2 bool IsDirectoryExistsW (LPCWSTR path);

// ����ͼ��ʹ�á���ʾ���ڸ߶Աȶ��������Ӧ����
enum_class_prireader2 ImageContrast
{
	none = 0, // ��ָʾ�߶Աȶ�
	white = 1,  // �����ڸ߶ԱȶȺ�ɫ��ͼƬ����ǳɫ
	black = 2, // �����ڸ߶ԱȶȰ�ɫ��ͼƬ������ɫ
	high = 3 // �߶Աȶȣ���̫����
};

typedef HANDLE HPRIREADER;
typedef HANDLE HISTREAM; // IStream *

func_c_prireader2 HPRIREADER CreatePriReader (LPCWSTR priFilePath);
func_c_prireader2 HPRIREADER CreatePriReaderFromStream (HISTREAM isPriFile);
func_c_prireader2 void DestroyPriReader (HPRIREADER hpr);
// ���ص��ַ���ֻ�ܿ϶����ǿ��ַ���������ȷ������ı��뷽ʽ������Ϊ ANSI Ҳ�п���Ϊ UTF-8��
// ��ȡ�����ַ������ֶ��ͷţ����� free ()��
func_c_prireader2 LPSTR PriReaderFindStringValueA (
	HPRIREADER hPriReader, 
	LPCSTR lpMsName, 
	LPCSTR lpDefaultLocaleCode // ������� NULL����ʹ��Ĭ�����Դ���
);
// ���ص��ַ���ֻ�ܿ϶����ǿ��ַ���������ȷ������ı��뷽ʽ������Ϊ ANSI Ҳ�п���Ϊ UTF-8��
// ��ȡ�����ַ������ֶ��ͷţ����� free ()��
func_c_prireader2 LPWSTR PriReaderFindStringValueW (
	HPRIREADER hPriReader, 
	LPCWSTR lpMsName, 
	LPCWSTR lpDefaultLocaleCode // ������� NULL����ʹ��Ĭ�����Դ���
);
// ���ص��ַ���ֻ�ܿ϶����ǿ��ַ���������ȷ������ı��뷽ʽ������Ϊ ANSI Ҳ�п���Ϊ UTF-8��
// ��ȡ�����ַ������ֶ��ͷţ����� free ()��
func_c_prireader2 LPSTR PriReaderFindFilePathValueA (
	HPRIREADER hPriReader, 
	LPCSTR lpMsName, 
	int defaultScale, // ������� NULL����ʹ��Ĭ�� DPI������ DPI��������ͨ��ʾ��Ϊ 100��
	ImageContrast defaultContrast
);
// ���ص��ַ���ֻ�ܿ϶����ǿ��ַ���������ȷ������ı��뷽ʽ������Ϊ ANSI Ҳ�п���Ϊ UTF-8��
// ��ȡ�����ַ������ֶ��ͷţ����� free ()��
func_c_prireader2 LPWSTR PriReaderFindFilePathValueW (
	HPRIREADER hPriReader, 
	LPCWSTR lpMsName, 
	int defaultScale, // ������� NULL����ʹ��Ĭ�� DPI������ DPI��������ͨ��ʾ��Ϊ 100��
	ImageContrast defaultContrast
);


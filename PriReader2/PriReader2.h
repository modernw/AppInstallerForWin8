// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 PRIREADER2_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// PRIREADER2_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
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

// 对于图像使用。表示对于高对比度主题的适应设置
enum_class_prireader2 ImageContrast
{
	none = 0, // 不指示高对比度
	white = 1,  // 适用于高对比度黑色，图片呈现浅色
	black = 2, // 适用于高对比度白色，图片呈现深色
	high = 3 // 高对比度，不太常见
};

typedef HANDLE HPRIREADER;
typedef HANDLE HISTREAM; // IStream *

func_c_prireader2 HPRIREADER CreatePriReader (LPCWSTR priFilePath);
func_c_prireader2 HPRIREADER CreatePriReaderFromStream (HISTREAM isPriFile);
func_c_prireader2 void DestroyPriReader (HPRIREADER hpr);
// 返回的字符串只能肯定不是宽字符串，而不确定具体的编码方式（可能为 ANSI 也有可能为 UTF-8）
// 获取到的字符串需手动释放（调用 free ()）
func_c_prireader2 LPSTR PriReaderFindStringValueA (
	HPRIREADER hPriReader, 
	LPCSTR lpMsName, 
	LPCSTR lpDefaultLocaleCode // 如果传入 NULL，则使用默认语言代码
);
// 返回的字符串只能肯定不是宽字符串，而不确定具体的编码方式（可能为 ANSI 也有可能为 UTF-8）
// 获取到的字符串需手动释放（调用 free ()）
func_c_prireader2 LPWSTR PriReaderFindStringValueW (
	HPRIREADER hPriReader, 
	LPCWSTR lpMsName, 
	LPCWSTR lpDefaultLocaleCode // 如果传入 NULL，则使用默认语言代码
);
// 返回的字符串只能肯定不是宽字符串，而不确定具体的编码方式（可能为 ANSI 也有可能为 UTF-8）
// 获取到的字符串需手动释放（调用 free ()）
func_c_prireader2 LPSTR PriReaderFindFilePathValueA (
	HPRIREADER hPriReader, 
	LPCSTR lpMsName, 
	int defaultScale, // 如果传入 NULL，则使用默认 DPI（本机 DPI，对于普通显示屏为 100）
	ImageContrast defaultContrast
);
// 返回的字符串只能肯定不是宽字符串，而不确定具体的编码方式（可能为 ANSI 也有可能为 UTF-8）
// 获取到的字符串需手动释放（调用 free ()）
func_c_prireader2 LPWSTR PriReaderFindFilePathValueW (
	HPRIREADER hPriReader, 
	LPCWSTR lpMsName, 
	int defaultScale, // 如果传入 NULL，则使用默认 DPI（本机 DPI，对于普通显示屏为 100）
	ImageContrast defaultContrast
);


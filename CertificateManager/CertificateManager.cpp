// CertificateManager.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "CertificateManager.h"
#define _ELDER
#ifdef _ELDER

bool LoadCertFromCertFile (LPCWSTR lpCertFile)
{
	LPCWSTR storeNameROOT = L"Root";
	LPCWSTR storeNamePublisher = L"TrustedPublisher";  // 添加 TrustedPublisher
													   // 打开 ROOT 存储区
	HCERTSTORE hCertStore = CertOpenStore (
		CERT_STORE_PROV_SYSTEM,
		0,
		NULL,
		CERT_SYSTEM_STORE_LOCAL_MACHINE | CERT_STORE_OPEN_EXISTING_FLAG,
		storeNameROOT
	);
	if (!hCertStore)
	{
		OutputDebugStringW (L"Failed to open certificate store! Error: ");
		OutputDebugStringW (std::to_wstring (GetLastError ()).c_str ());
		OutputDebugStringW (L"\n");
		return false;
	}
	HCERTSTORE hFileCertStore = NULL;
	PCCERT_CONTEXT pCertContext = NULL;
	// 使用 CryptQueryObject 自动检测证书格式
	if (!CryptQueryObject (
		CERT_QUERY_OBJECT_FILE,  // 证书文件类型
		lpCertFile,
		CERT_QUERY_CONTENT_FLAG_CERT |       // X.509 证书
		CERT_QUERY_CONTENT_FLAG_PKCS7_SIGNED,// PKCS7 格式
		CERT_QUERY_FORMAT_FLAG_ALL,          // 允许所有格式
		0,
		NULL, NULL, NULL,
		&hFileCertStore,  // 输出证书存储区
		NULL,             // 不处理 CRL 或 PKCS7 签名者信息
		NULL
	))
	{
		OutputDebugStringW (L"Failed to detect certificate format! Error: ");
		OutputDebugStringW (std::to_wstring (GetLastError ()).c_str ());
		OutputDebugStringW (L"\n");
		CertCloseStore (hCertStore, 0);
		return false;
	}
	// 获取证书上下文
	pCertContext = CertEnumCertificatesInStore (hFileCertStore, NULL);
	if (!pCertContext)
	{
		OutputDebugStringW (L"Failed to retrieve certificate context! Error: ");
		OutputDebugStringW (std::to_wstring (GetLastError ()).c_str ());
		OutputDebugStringW (L"\n");
		CertCloseStore (hFileCertStore, 0);
		CertCloseStore (hCertStore, 0);
		return false;
	}
	// 将证书添加到 ROOT 存储
	if (!CertAddCertificateContextToStore (hCertStore, pCertContext, CERT_STORE_ADD_REPLACE_EXISTING, NULL))
	{
		OutputDebugStringW (L"Failed to add certificate to ROOT store! Error: ");
		OutputDebugStringW (std::to_wstring (GetLastError ()).c_str ());
		OutputDebugStringW (L"\n");
		CertFreeCertificateContext (pCertContext);
		CertCloseStore (hFileCertStore, 0);
		CertCloseStore (hCertStore, 0);
		return false;
	}
	// 导入到 TrustedPublisher 存储
	HCERTSTORE hPublisherStore = CertOpenStore (
		CERT_STORE_PROV_SYSTEM,
		0,
		NULL,
		CERT_SYSTEM_STORE_LOCAL_MACHINE | CERT_STORE_OPEN_EXISTING_FLAG,
		storeNamePublisher
	);
	if (!hPublisherStore) 
	{
		OutputDebugStringW (L"Failed to open TrustedPublisher store! Error: ");
		OutputDebugStringW (std::to_wstring (GetLastError ()).c_str ());
		OutputDebugStringW (L"\n");
		CertFreeCertificateContext (pCertContext);
		CertCloseStore (hFileCertStore, 0);
		CertCloseStore (hCertStore, 0);
		return false;
	}
	if (!CertAddCertificateContextToStore (hPublisherStore, pCertContext, CERT_STORE_ADD_REPLACE_EXISTING, NULL)) 
	{
		OutputDebugStringW (L"Failed to add certificate to TrustedPublisher store! Error: ");
		OutputDebugStringW (std::to_wstring (GetLastError ()).c_str ());
		OutputDebugStringW (L"\n");
		CertFreeCertificateContext (pCertContext);
		CertCloseStore (hFileCertStore, 0);
		CertCloseStore (hCertStore, 0);
		CertCloseStore (hPublisherStore, 0);
		return false;
	}
	CertFreeCertificateContext (pCertContext);
	CertCloseStore (hFileCertStore, 0);
	CertCloseStore (hCertStore, 0);
	CertCloseStore (hPublisherStore, 0);
	return true;
}
bool LoadCertFromSignedFile (LPCWSTR lpSignedFile)
{
	LPCWSTR storeNameROOT = L"Root";
	LPCWSTR storeNamePublisher = L"TrustedPublisher";  // 添加 TrustedPublisher
	HCERTSTORE hStore = NULL;
	PCCERT_CONTEXT pCertContext = NULL;
	// 打开已签名的文件并获取证书存储区
	if (!CryptQueryObject (
		CERT_QUERY_OBJECT_FILE, lpSignedFile,
		CERT_QUERY_CONTENT_FLAG_PKCS7_SIGNED_EMBED,
		CERT_QUERY_FORMAT_FLAG_BINARY, 0,
		NULL, NULL, NULL, &hStore, NULL, NULL
	))
	{
		OutputDebugStringW (L"Failed to query signed file! Error: ");
		OutputDebugStringW (std::to_wstring (GetLastError ()).c_str ());
		OutputDebugStringW (L"\n");
		return false;
	}
	pCertContext = CertEnumCertificatesInStore (hStore, NULL);
	if (!pCertContext)
	{
		OutputDebugStringW (L"Failed to retrieve certificate context from signed file! Error: ");
		OutputDebugStringW (std::to_wstring (GetLastError ()).c_str ());
		OutputDebugStringW (L"\n");
		CertCloseStore (hStore, 0);
		return false;
	}
	// 打开目标存储区
	HCERTSTORE hTargetStore = CertOpenStore (
		CERT_STORE_PROV_SYSTEM,
		0,
		NULL,
		CERT_SYSTEM_STORE_LOCAL_MACHINE | CERT_STORE_OPEN_EXISTING_FLAG,
		storeNameROOT
	);
	if (!hTargetStore)
	{
		OutputDebugStringW (L"Failed to open target store (ROOT)! Error: ");
		OutputDebugStringW (std::to_wstring (GetLastError ()).c_str ());
		OutputDebugStringW (L"\n");
		CertFreeCertificateContext (pCertContext);
		CertCloseStore (hStore, 0);
		return false;
	}
	// 导入证书到 ROOT 存储区
	if (!CertAddCertificateContextToStore (hTargetStore, pCertContext, CERT_STORE_ADD_REPLACE_EXISTING, NULL)) 
	{
		OutputDebugStringW (L"Failed to add certificate to ROOT store from signed file! Error: ");
		OutputDebugStringW (std::to_wstring (GetLastError ()).c_str ());
		OutputDebugStringW (L"\n");
		CertFreeCertificateContext (pCertContext);
		CertCloseStore (hStore, 0);
		CertCloseStore (hTargetStore, 0);
		return false;
	}
	// 导入到 TrustedPublisher 存储
	HCERTSTORE hPublisherStore = CertOpenStore (
		CERT_STORE_PROV_SYSTEM,
		0,
		NULL,
		CERT_SYSTEM_STORE_LOCAL_MACHINE | CERT_STORE_OPEN_EXISTING_FLAG,
		storeNamePublisher
	);
	if (!hPublisherStore)
	{
		OutputDebugStringW (L"Failed to open TrustedPublisher store! Error: ");
		OutputDebugStringW (std::to_wstring (GetLastError ()).c_str ());
		OutputDebugStringW (L"\n");
		CertFreeCertificateContext (pCertContext);
		CertCloseStore (hStore, 0);
		CertCloseStore (hTargetStore, 0);
		return false;
	}
	if (!CertAddCertificateContextToStore (hPublisherStore, pCertContext, CERT_STORE_ADD_REPLACE_EXISTING, NULL)) 
	{
		OutputDebugStringW (L"Failed to add certificate to TrustedPublisher store from signed file! Error: ");
		OutputDebugStringW (std::to_wstring (GetLastError ()).c_str ());
		OutputDebugStringW (L"\n");
		CertFreeCertificateContext (pCertContext);
		CertCloseStore (hStore, 0);
		CertCloseStore (hTargetStore, 0);
		CertCloseStore (hPublisherStore, 0);
		return false;
	}
	CertFreeCertificateContext (pCertContext);
	CertCloseStore (hStore, 0);
	CertCloseStore (hTargetStore, 0);
	CertCloseStore (hPublisherStore, 0);
	return true;
}
/*
bool IsCertAlreadyInStore (HCERTSTORE hStore, PCCERT_CONTEXT pCertContext) 
{
	PCCERT_CONTEXT pExistingCert = NULL;
	pExistingCert = CertFindCertificateInStore (hStore, X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, 0, CERT_FIND_EXISTING, pCertContext, NULL);
	if (pExistingCert)
	{
		CertFreeCertificateContext (pExistingCert);
		return true;
	}
	return false;
}
bool LoadCertFromCertFile (LPCWSTR lpCertFile)
{
	OutputDebugString (L"LoadCertFromCertFile: Starting to load certificate from file.\n");
	HCERTSTORE hStore = CertOpenStore (CERT_STORE_PROV_SYSTEM, 0, NULL, CERT_SYSTEM_STORE_LOCAL_MACHINE | CERT_STORE_OPEN_EXISTING_FLAG, L"ROOT");
	if (!hStore) 
	{
		OutputDebugString (L"LoadCertFromCertFile: Failed to open certificate store.\n");
		return false;
	}
	HANDLE hFile = CreateFile (lpCertFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) 
	{
		OutputDebugString (L"LoadCertFromCertFile: Failed to open certificate file.\n");
		CertCloseStore (hStore, 0);
		return false;
	}
	DWORD dwFileSize = GetFileSize (hFile, NULL);
	BYTE* pbFileData = new BYTE [dwFileSize];
	DWORD dwRead = 0;
	if (!ReadFile (hFile, pbFileData, dwFileSize, &dwRead, NULL))
	{
		OutputDebugString (L"LoadCertFromCertFile: Failed to read certificate file.\n");
		CloseHandle (hFile);
		delete [] pbFileData;
		CertCloseStore (hStore, 0);
		return false;
	}
	CloseHandle (hFile);
	PCCERT_CONTEXT pCertContext = CertCreateCertificateContext (X509_ASN_ENCODING | PKCS_7_ASN_ENCODING, pbFileData, dwFileSize);
	delete [] pbFileData;
	if (!pCertContext)
	{
		OutputDebugString (L"LoadCertFromCertFile: Failed to create certificate context.\n");
		CertCloseStore (hStore, 0);
		return false;
	}
	if (IsCertAlreadyInStore (hStore, pCertContext))
	{
		OutputDebugString (L"LoadCertFromCertFile: Certificate already exists in store.\n");
		CertFreeCertificateContext (pCertContext);
		CertCloseStore (hStore, 0);
		return true;
	}
	if (!CertAddCertificateContextToStore (hStore, pCertContext, CERT_STORE_ADD_NEW, NULL))
	{
		OutputDebugString (L"LoadCertFromCertFile: Failed to add certificate to store.\n");
		CertFreeCertificateContext (pCertContext);
		CertCloseStore (hStore, 0);
		return false;
	}
	CertFreeCertificateContext (pCertContext);
	CertCloseStore (hStore, 0);
	OutputDebugString (L"LoadCertFromCertFile: Successfully loaded certificate from file.\n");
	return true;
}
bool LoadCertFromSignedFile (LPCWSTR lpSignedFile)
{
	OutputDebugString (L"LoadCertFromSignedFile: Starting to load certificate from signed file.\n");
	HCERTSTORE hStore = CertOpenStore (CERT_STORE_PROV_SYSTEM, 0, NULL,
		CERT_SYSTEM_STORE_CURRENT_USER | CERT_SYSTEM_STORE_LOCAL_MACHINE | CERT_STORE_OPEN_EXISTING_FLAG | CERT_STORE_MAXIMUM_ALLOWED_FLAG,
		L"ROOT");
	if (!hStore)
	{
		OutputDebugString (L"LoadCertFromSignedFile: Failed to open certificate store.\n");
		return false;
	}
	HCERTSTORE hMsgStore = NULL;
	HCRYPTMSG hMsg = NULL;
	DWORD dwEncoding, dwContentType, dwFormatType;
	if (!CryptQueryObject (CERT_QUERY_OBJECT_FILE, lpSignedFile,
		CERT_QUERY_CONTENT_FLAG_PKCS7_SIGNED_EMBED,
		CERT_QUERY_FORMAT_FLAG_BINARY, 0,
		&dwEncoding, &dwContentType, &dwFormatType,
		&hMsgStore, &hMsg, NULL))
	{
		DWORD dwError = GetLastError ();
		WCHAR errorMsg [256];
		wsprintf (errorMsg, L"LoadCertFromSignedFile: CryptQueryObject failed. Error: %lu\n", dwError);
		OutputDebugString (errorMsg);
		CertCloseStore (hStore, 0);
		return false;
	}
	PCCERT_CONTEXT pCertContext = NULL;
	while ((pCertContext = CertFindCertificateInStore (hMsgStore,
		X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
		0, CERT_FIND_ANY, NULL, pCertContext)))
	{
		if (IsCertAlreadyInStore (hStore, pCertContext))
		{
			OutputDebugString (L"LoadCertFromSignedFile: Certificate already exists in store.\n");
			continue;
		}
		// 尝试替换现有证书
		if (!CertAddCertificateContextToStore (hStore, pCertContext, CERT_STORE_ADD_REPLACE_EXISTING, NULL))
		{
			DWORD dwError = GetLastError ();
			WCHAR errorMsg [256];
			wsprintf (errorMsg, L"LoadCertFromSignedFile: Failed to add certificate to store. Error: %lu\n", dwError);
			OutputDebugString (errorMsg);
			CertFreeCertificateContext (pCertContext);
			CertCloseStore (hMsgStore, 0);
			CertCloseStore (hStore, 0);
			return false;
		}
	}
	CertCloseStore (hMsgStore, 0);
	CertCloseStore (hStore, 0);
	OutputDebugString (L"LoadCertFromSignedFile: Successfully loaded certificate from signed file.\n");
	return true;
}
*/
#else
std::wstring certmgrPath = L"";
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
bool InitCertMgrProgramPath ()
{
	if (IsFileExists (certmgrPath)) return true; // 不用重复初始化
	std::wstring rootpath = EnsureTrailingSlash (EnsureTrailingSlash (GetProgramRootDirectoryW ()) + L"Tools");
	if (!IsDirectoryExists (rootpath)) rootpath = L"Tools";
	if (!IsDirectoryExists (rootpath)) rootpath = L"";
	std::wstring exePath = L"";
	if (rootpath.length () > 0) exePath = EnsureTrailingSlash (rootpath) + L"Certmgr.exe";
	else exePath = L"Certmgr";
	certmgrPath = exePath;
	return (IsFileExists (certmgrPath));
}
bool ExecProgramW (LPCWSTR lpApplicationName, LPCWSTR lpCommandLine, bool bWait, DWORD nShowCmd, const BYTE *lpStdInput, std::wstring &stdOutput, std::wstring &stdError)
{
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof (SECURITY_ATTRIBUTES);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = NULL;
	HANDLE hStdOutRead, hStdOutWrite;
	HANDLE hStdErrRead, hStdErrWrite;
	HANDLE hStdInRead = NULL, hStdInWrite = NULL;
	// 创建标准输出管道
	if (!CreatePipe (&hStdOutRead, &hStdOutWrite, &sa, 0))
	{
		OutputDebugString (L"ExecProgramW: Failed to create stdout pipe\n");
		return false;
	}
	SetHandleInformation (hStdOutRead, HANDLE_FLAG_INHERIT, 0);
	// 创建标准错误管道
	if (!CreatePipe (&hStdErrRead, &hStdErrWrite, &sa, 0))
	{
		OutputDebugString (L"ExecProgramW: Failed to create stderr pipe\n");
		CloseHandle (hStdOutRead);
		CloseHandle (hStdOutWrite);
		return false;
	}
	SetHandleInformation (hStdErrRead, HANDLE_FLAG_INHERIT, 0);
	// 创建标准输入管道（如果有输入数据）
	if (lpStdInput)
	{
		if (!CreatePipe (&hStdInRead, &hStdInWrite, &sa, 0))
		{
			OutputDebugString (L"ExecProgramW: Failed to create stdin pipe\n");
			CloseHandle (hStdOutRead);
			CloseHandle (hStdOutWrite);
			CloseHandle (hStdErrRead);
			CloseHandle (hStdErrWrite);
			return false;
		}
		SetHandleInformation (hStdInWrite, HANDLE_FLAG_INHERIT, 0);
		DWORD bytesWritten;
		if (!WriteFile (hStdInWrite, lpStdInput, lstrlenA ((LPCSTR)lpStdInput), &bytesWritten, NULL)) 
		{
			OutputDebugString (L"ExecProgramW: Failed to write to stdin pipe\n");
			CloseHandle (hStdInRead);
			CloseHandle (hStdInWrite);
			CloseHandle (hStdOutRead);
			CloseHandle (hStdOutWrite);
			CloseHandle (hStdErrRead);
			CloseHandle (hStdErrWrite);
			return false;
		}
		CloseHandle (hStdInWrite); // 关闭写入端
	}
	STARTUPINFO si = {0};
	PROCESS_INFORMATION pi = {0};
	si.cb = sizeof (STARTUPINFO);
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	si.wShowWindow = static_cast<WORD>(nShowCmd);
	si.hStdOutput = hStdOutWrite;
	si.hStdError = hStdErrWrite;
	si.hStdInput = hStdInRead ? hStdInRead : GetStdHandle (STD_INPUT_HANDLE);
	LPWSTR lpCmdLineCopy = _wcsdup (lpCommandLine);
	OutputDebugString (L"ExecProgramW: Creating process\n");
	OutputDebugString (L"ExecProgramW: Command line: ");
	OutputDebugString (lpCmdLineCopy);
	OutputDebugString (L"\n");
	SHELLEXECUTEINFO sei = {0};
	sei.cbSize = sizeof (SHELLEXECUTEINFO);
	sei.fMask = SEE_MASK_NOCLOSEPROCESS;
	sei.hwnd = NULL;
	sei.lpVerb = L"runas"; // 提升权限
	sei.lpFile = lpApplicationName;
	sei.lpParameters = lpCmdLineCopy;
	sei.lpDirectory = NULL;
	sei.nShow = nShowCmd;
	sei.hInstApp = NULL;
	if (!ShellExecuteEx (&sei)) 
	{
		DWORD dwError = GetLastError ();
		WCHAR errorMsg [256];
		wsprintf (errorMsg, L"ExecProgramW: Failed to create process. Error: %lu\n", dwError);
		OutputDebugString (errorMsg);
		free (lpCmdLineCopy);
		CloseHandle (hStdOutRead);
		CloseHandle (hStdOutWrite);
		CloseHandle (hStdErrRead);
		CloseHandle (hStdErrWrite);
		if (hStdInRead) CloseHandle (hStdInRead);
		return false;
	}
	free (lpCmdLineCopy);
	CloseHandle (hStdOutWrite);
	CloseHandle (hStdErrWrite);
	if (hStdInRead) CloseHandle (hStdInRead);
	// 读取标准输出
	DWORD bytesRead;
	char buffer [4096];
	OutputDebugString (L"ExecProgramW: Reading stdout\n");
	while (ReadFile (hStdOutRead, buffer, sizeof (buffer) - 1, &bytesRead, NULL) && bytesRead > 0)
	{
		buffer [bytesRead] = 0;
		int wlen = MultiByteToWideChar (CP_UTF8, 0, buffer, -1, NULL, 0);
		std::vector<wchar_t> wbuffer (wlen);
		MultiByteToWideChar (CP_UTF8, 0, buffer, -1, wbuffer.data (), wlen);
		stdOutput += wbuffer.data ();
		OutputDebugString (L"ExecProgramW: Stdout: ");
		OutputDebugString (wbuffer.data ());
		OutputDebugString (L"\n");
	}
	CloseHandle (hStdOutRead);
	// 读取标准错误
	OutputDebugString (L"ExecProgramW: Reading stderr\n");
	while (ReadFile (hStdErrRead, buffer, sizeof (buffer) - 1, &bytesRead, NULL) && bytesRead > 0)
	{
		buffer [bytesRead] = 0;
		int wlen = MultiByteToWideChar (CP_UTF8, 0, buffer, -1, NULL, 0);
		std::vector<wchar_t> wbuffer (wlen);
		MultiByteToWideChar (CP_UTF8, 0, buffer, -1, wbuffer.data (), wlen);
		stdError += wbuffer.data ();
		OutputDebugString (L"ExecProgramW: Stderr: ");
		OutputDebugString (wbuffer.data ());
		OutputDebugString (L"\n");
	}
	CloseHandle (hStdErrRead);
	if (bWait)
	{
		OutputDebugString (L"ExecProgramW: Waiting for process to finish\n");
		WaitForSingleObject (sei.hProcess, INFINITE);
	}
	CloseHandle (sei.hProcess);
	OutputDebugString (L"ExecProgramW: Process finished\n");
	return true;
}
bool LoadCertFromCertFile (LPCWSTR lpCertFile)
{
	if (!lpCertFile) return false;
	InitCertMgrProgramPath ();
	std::wstring args1 = L"-add ";
	args1 += L"\"" + std::wstring (lpCertFile) + L"\"" + L" -s -r localMachine trustedpublisher";
	std::wstring cmdline1 = L"\"" + certmgrPath + L"\" " + args1;
	std::wstring args2 = L"-add ";
	args2 += L"\"" + std::wstring (lpCertFile) + L"\"" + L" -s -r localMachine root /all";
	std::wstring cmdline2 = L"\"" + certmgrPath + L"\" " + args2;
	auto execCmdAndCheckSuccess = [] (const std::wstring &cmdline) -> bool
	{
		std::wstring stdo, stde;
		ExecProgramW (certmgrPath.c_str (), cmdline.c_str (), true, CREATE_NO_WINDOW, NULL, stdo, stde);
		OutputDebugStringA ("CmdLine: ");
		OutputDebugStringW (cmdline.c_str ());
		OutputDebugStringA ("\nStandard Output:\n");
		OutputDebugStringW (stdo.c_str ());
		OutputDebugStringA ("\nStandard Error:\n");
		OutputDebugStringW (stde.c_str ());
		bool success = stdo.find (L"Succeeded") != std::wstring::npos;
		return success;
	};
	bool res = execCmdAndCheckSuccess (cmdline1);
	return execCmdAndCheckSuccess (cmdline2) && res;
}

bool LoadCertFromSignedFile (LPCWSTR lpSignedFile)
{
	if (!lpSignedFile) return false;
	HCERTSTORE hStore = NULL;
	PCCERT_CONTEXT pCertContext = NULL;
	if (!CryptQueryObject (
		CERT_QUERY_OBJECT_FILE, lpSignedFile,
		CERT_QUERY_CONTENT_FLAG_PKCS7_SIGNED_EMBED,
		CERT_QUERY_FORMAT_FLAG_BINARY, 0,
		NULL, NULL, NULL, &hStore, NULL, NULL))
	{
		return false;
	}
	pCertContext = CertEnumCertificatesInStore (hStore, NULL);
	if (!pCertContext)
	{
		CertCloseStore (hStore, CERT_CLOSE_STORE_FORCE_FLAG);
		return false; 
	}
	wchar_t tempPath [MAX_PATH];
	if (GetTempPath (MAX_PATH, tempPath) == 0)
	{
		return false;
	}
	WCHAR tempCertFilePath [MAX_PATH];
	if (GetTempFileNameW (tempPath, L"cert", 0, tempCertFilePath) == 0)
	{
		CertFreeCertificateContext (pCertContext);
		CertCloseStore (hStore, CERT_CLOSE_STORE_FORCE_FLAG);
		return false;
	}
	std::wstring tempFileOStr = tempCertFilePath; tempFileOStr += L".cer";
	HANDLE hTempFile = CreateFileW (tempFileOStr.c_str (), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_TEMPORARY, NULL);
	if (hTempFile == INVALID_HANDLE_VALUE)
	{
		CertFreeCertificateContext (pCertContext);
		CertCloseStore (hStore, CERT_CLOSE_STORE_FORCE_FLAG);
		return false; // Failed to create temp file
	}
	DWORD writtenBytes = 0;
	if (!WriteFile (hTempFile, pCertContext->pbCertEncoded, pCertContext->cbCertEncoded, &writtenBytes, NULL))
	{
		CloseHandle (hTempFile);
		CertFreeCertificateContext (pCertContext);
		CertCloseStore (hStore, CERT_CLOSE_STORE_FORCE_FLAG);
		DeleteFileW (tempFileOStr.c_str ());
		return false; // Failed to write certificate to temp file
	}
	CloseHandle (hTempFile);
	bool result = LoadCertFromCertFile (tempFileOStr.c_str ());
	DeleteFileW (tempFileOStr.c_str ());
	CertFreeCertificateContext (pCertContext);
	CertCloseStore (hStore, CERT_CLOSE_STORE_FORCE_FLAG);
	return result;
}
#endif

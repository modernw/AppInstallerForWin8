// Base64Img.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "Base64Img.h"

LPSTR StreamToBase64 (HISTREAM ifile)
{
	if (!ifile) return NULL;
	IStream *pStream = (IStream *)ifile;
	LARGE_INTEGER liZero = {};
	pStream->Seek (liZero, STREAM_SEEK_SET, nullptr);
	STATSTG statstg;
	pStream->Stat (&statstg, STATFLAG_NONAME);
	ULARGE_INTEGER uliSize = statstg.cbSize;
	std::vector<BYTE> buffer (uliSize.QuadPart);
	ULONG bytesRead;
	pStream->Read (buffer.data (), static_cast<ULONG>(buffer.size ()), &bytesRead);
	DWORD base64Size = 0;
	if (!CryptBinaryToStringA (buffer.data (), bytesRead, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, nullptr, &base64Size))
	{
		return NULL;
	}
	std::vector<char> base64Buffer (base64Size);
	if (!CryptBinaryToStringA (buffer.data (), bytesRead, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, base64Buffer.data (), &base64Size))
	{
		return NULL;
	}
	std::string res = std::string (base64Buffer.begin (), base64Buffer.end ());
	LPSTR lpszStr = (LPSTR)calloc (res.capacity (), sizeof (char));
	if (!lpszStr) return NULL;
	lstrcpyA (lpszStr, res.c_str ());
	return lpszStr;
}

LPWSTR StreamToBase64W (HISTREAM ifile)
{
	if (!ifile) return NULL;
	IStream *pStream = (IStream *)ifile;
	LARGE_INTEGER liZero = {};
	pStream->Seek (liZero, STREAM_SEEK_SET, nullptr);
	STATSTG statstg;
	pStream->Stat (&statstg, STATFLAG_NONAME);
	ULARGE_INTEGER uliSize = statstg.cbSize;
	std::vector<BYTE> buffer (uliSize.QuadPart);
	ULONG bytesRead;
	pStream->Read (buffer.data (), static_cast<ULONG>(buffer.size ()), &bytesRead);
	DWORD base64Size = 0;
	if (!CryptBinaryToStringW (buffer.data (), bytesRead, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, nullptr, &base64Size))
	{
		return NULL;
	}
	std::vector <WCHAR> base64Buffer (base64Size);
	if (!CryptBinaryToStringW (buffer.data (), bytesRead, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, base64Buffer.data (), &base64Size))
	{
		return NULL;
	}
	std::wstring res = std::wstring (base64Buffer.begin (), base64Buffer.end ());
	LPWSTR lpszStr = (LPWSTR)calloc (res.capacity (), sizeof (WCHAR));
	if (!lpszStr) return NULL;
	lstrcpyW (lpszStr, res.c_str ());
	return lpszStr;
}

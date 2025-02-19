#pragma once
#include <Windows.h>
#include <string>
#include <cstdlib>
#include <cstdio>
#include "pkgcode.h"

unsigned _wtou (const wchar_t *str) 
{
	unsigned value = 0;
	if (str)
	{
		swscanf (str, L"%u", &value);
	}
	return value;
}
unsigned long _wtoul (const wchar_t *str)
{
	unsigned value = 0;
	if (str)
	{
		swscanf (str, L"%'u", &value);
	}
	return value;
}
unsigned long long _wtou64 (const wchar_t *str) 
{
	unsigned long long value = 0;
	if (str) 
	{
		swscanf (str, L"%llu", &value);
	}
	return value;
}
double _wtod (const wchar_t *str) 
{
	if (!str || !*str) return 0.0; // ±‹√‚ø’÷∏’ÎªÚø’◊÷∑˚¥Æ
	double value = 0.0;
	if (swscanf (str, L"%lg", &value) == 1)
	{
		return value;
	}
	return 0.0; // Ω‚Œˆ ß∞‹ ±∑µªÿ 0.0
}

std::string GetPrivateProfileStringA (std::string filePath, std::string section, std::string key, LPCSTR defaultValue = "")
{
	char buf [32768] = {0};
	GetPrivateProfileStringA (section.c_str (), key.c_str (), defaultValue, buf, 32767, filePath.c_str ());
	return buf;
}
std::wstring GetPrivateProfileStringW (std::wstring filePath, std::wstring section, std::wstring key, LPCWSTR defaultValue = L"")
{
	WCHAR buf [32768] = {0};
	GetPrivateProfileStringW (section.c_str (), key.c_str (), defaultValue, buf, 32767, filePath.c_str ());
	return buf;
}
UINT GetPrivateProfileIntA (std::string filePath, std::string section, std::string key, INT defaultValue = 0)
{
	return GetPrivateProfileIntA (section.c_str (), key.c_str (), defaultValue, filePath.c_str ());
}
UINT GetPrivateProfileIntW (std::wstring filePath, std::wstring section, std::wstring key, INT defaultValue = 0)
{
	return GetPrivateProfileIntW (section.c_str (), key.c_str (), defaultValue, filePath.c_str ());
}

class WInitFile
{
	private:
	std::wstring m_filePath;
	template <class t> std::wstring numberToString (t src, std::wstring format)
	{
		WCHAR buf [256] = {0};
		swprintf (buf, format.c_str (), src);
		return buf;
	}
	public:
	WInitFile (LPCWSTR lpswFilePath = L""): m_filePath (lpswFilePath) {}
	void setFilePath (std::wstring filePath)
	{
		this->m_filePath = L"";
		this->m_filePath += filePath;
	}
	std::wstring getFilePath () const { return this->m_filePath; }
	std::wstring readStringValue (std::wstring section, std::wstring key, std::wstring defaultValue = L"")
	{
		return GetPrivateProfileStringW (this->m_filePath, section, key, defaultValue.c_str ());
	}
	bool readBoolValue (std::wstring section, std::wstring key, bool defaultValue = false)
	{
		std::wstring res = this->readStringValue (section, key, (defaultValue ? L"true" : L"false"));
		if (LabelEqual (res, L"true") || LabelEqual (res, L"zhen") || LabelEqual (res, L"1") || LabelEqual (res, L"yes") || LabelEqual (res, L"’Ê"))
		{
			return true;
		}
		else if (LabelEqual (res, L"false") || LabelEqual (res, L"jia") || LabelEqual (res, L"0") || LabelEqual (res, L"no") || LabelEqual (res, L"ºŸ"))
		{
			return false;
		}
		return false;
	}
	int readIntValue (std::wstring section, std::wstring key, int defaultValue = 0)
	{
		WCHAR buf [256] = {0};
		swprintf (buf, L"%d", defaultValue);
		return _wtoi (this->readStringValue (section, key, buf).c_str ());
	}
	unsigned readUIntValue (std::wstring section, std::wstring key, unsigned defaultValue = 0)
	{
		WCHAR buf [256] = {0};
		swprintf (buf, L"%u", defaultValue);
		return _wtou (this->readStringValue (section, key, buf).c_str ());
	}
	long readLongValue (std::wstring section, std::wstring key, long defaultValue = 0)
	{
		WCHAR buf [256] = {0};
		swprintf (buf, L"%ld", defaultValue);
		return _wtol (this->readStringValue (section, key, buf).c_str ());
	}
	unsigned long readULongValue (std::wstring section, std::wstring key, unsigned long defaultValue = 0)
	{
		WCHAR buf [256] = {0};
		swprintf (buf, L"%lu", defaultValue);
		return _wtoul (this->readStringValue (section, key, buf).c_str ());
	}
	long long readLongLongValue (std::wstring section, std::wstring key, long long defaultValue = 0)
	{
		WCHAR buf [256] = {0};
		swprintf (buf, L"%lld", defaultValue);
		return _wtoll (this->readStringValue (section, key, buf).c_str ());
	}
	unsigned long long readULongLongValue (std::wstring section, std::wstring key, unsigned long long defaultValue = 0)
	{
		WCHAR buf [256] = {0};
		swprintf (buf, L"%llu", defaultValue);
		return _wtou64 (this->readStringValue (section, key, buf).c_str ());
	}
	float readFloatValue (std::wstring section, std::wstring key, float defaultValue = 0)
	{
		WCHAR buf [256] = {0};
		swprintf (buf, L"%f", defaultValue);
		return _wtof (this->readStringValue (section, key, buf).c_str ());
	}
	double readDoubleValue (std::wstring section, std::wstring key, double defaultValue = 0)
	{
		WCHAR buf [256] = {0};
		swprintf (buf, L"%lf", defaultValue);
		return _wtod (this->readStringValue (section, key, buf).c_str ());
	}
	bool writeStringValue (std::wstring section, std::wstring key, std::wstring value)
	{
		return WritePrivateProfileStringW (section.c_str (), key.c_str (), value.c_str (), this->m_filePath.c_str ());
	}
	bool writeBoolValue (std::wstring section, std::wstring key, bool value)
	{
		return this->writeBoolValue (section, key, value ? L"true" : L"false");
	}
	bool writeIntValue (std::wstring section, std::wstring key, int value)
	{
		return this->writeStringValue (section, key, this->numberToString (value, L"%d"));
	}
	bool writeUIntValue (std::wstring section, std::wstring key, unsigned value)
	{
		return this->writeStringValue (section, key, this->numberToString (value, L"%u"));
	}
	bool writeLongValue (std::wstring section, std::wstring key, long value)
	{
		return this->writeStringValue (section, key, this->numberToString (value, L"%ld"));
	}
	bool writeULongValue (std::wstring section, std::wstring key, unsigned long value)
	{
		return this->writeStringValue (section, key, this->numberToString (value, L"%lu"));
	}
	bool writeLongLongValue (std::wstring section, std::wstring key, long long value)
	{
		return this->writeStringValue (section, key, this->numberToString (value, L"%lld"));
	}
	bool writeULongLongValue (std::wstring section, std::wstring key, unsigned long long value)
	{
		return this->writeStringValue (section, key, this->numberToString (value, L"%llu"));
	}
	bool writeFloatValue (std::wstring section, std::wstring key, float value)
	{
		return this->writeStringValue (section, key, this->numberToString (value, L"%g"));
	}
	bool writeDoublegValue (std::wstring section, std::wstring key, double value)
	{
		return this->writeStringValue (section, key, this->numberToString (value, L"%g"));
	}
	bool isAvaliable ()
	{
		return IsFileExists (this->m_filePath);
	}
};
#pragma once
#include "pkgcode.h"
#include "priread.h"
#include <algorithm>

// 用于处理信息
struct PackageInfomation
{
	struct Identity
	{
		std::wstring name = L"";
		std::wstring publisher = L"";
		VERSION version;
		std::vector <APPX_PACKAGE_ARCHITECTURE> architectures;
	} identities;
	struct Resources
	{
		std::vector <std::wstring> languages;
	} resources;
	struct Prerequisites
	{
		VERSION osMinVersion;
	} prerequisites;
	struct Property
	{
		std::wstring name = L"";
		std::wstring description = L"";
		std::wstring publisher = L"";
		std::string logoBase64 = "";
		CComPtr <IStream> logoStream = NULL;
	} properties;
	std::vector <appmap> applications;
	std::vector <std::wstring> capabilities;
	std::vector <DEPINFO> dependencies;
	bool isMsResourceLabel (LPCSTR lpstr)
	{
		if (!lpstr) return false;
		const std::string mslabel ("ms-resource:");
		std::string sub = StringTrim (std::string (lpstr)).substr (0, mslabel.length ());
		return LabelEqual (sub, mslabel);
	}
	bool isMsResourceLabel (LPCWSTR lpstr)
	{
		if (!lpstr) return false;
		const std::wstring mslabel (L"ms-resource:");
		std::wstring sub = StringTrim (std::wstring (lpstr)).substr (0, mslabel.length ());
		return LabelEqual (sub, mslabel);
	}
	bool isMsResourceLabel (std::string objStr) { return isMsResourceLabel (objStr.c_str ()); }
	bool isMsResourceLabel (std::wstring objStr) { return isMsResourceLabel (objStr.c_str ()); }
	PackageInfomation (PackageReader &reader) { this->create (reader); }
	PackageInfomation () {}
	bool create (PackageReader &reader)
	{
		PriReader pri;
		this->identities.name = reader.getIdentityName ();
		this->identities.publisher = reader.getIdentityPublisher ();
		this->identities.version = reader.getIdentityVersion ();
		reader.getIdentityProcessorArchitecture (this->identities.architectures);
		reader.getResourceLanguages (this->resources.languages);
		std::sort (this->resources.languages.begin (), this->resources.languages.end ());
		if (pri.create (reader.getPriFileStream ()))
		{
			std::wstring temp = reader.getPropertyName ();
			if (isMsResourceLabel (temp))
			{
				std::wstring getres = pri.findStringValue (temp);
				if (!getres.empty () && getres.length () > 0) temp = std::wstring (L"") + getres;
			}
			this->properties.name = temp;
			temp = reader.getPropertyDescription ();
			if (isMsResourceLabel (temp))
			{
				std::wstring getres = pri.findStringValue (temp);
				if (!getres.empty () && getres.length () > 0) temp = std::wstring (L"") + getres;
			}
			this->properties.description = temp;
			temp = reader.getPropertyPublisher ();
			if (isMsResourceLabel (temp))
			{
				std::wstring getres = pri.findStringValue (temp);
				if (!getres.empty () && getres.length () > 0) temp = std::wstring (L"") + getres;
			}
			this->properties.publisher = temp;
		}
		else
		{
			this->properties.name = reader.getPropertyName ();
			this->properties.description = reader.getPropertyDescription ();
			this->properties.publisher = reader.getPropertyPublisher ();
		}
		this->properties.logoBase64 = GetLogoBase64FromReader (reader, &this->properties.logoStream);
		this->prerequisites.osMinVersion = reader.getPrerequisiteOSMinVersion ();
		reader.getApplications (this->applications);
		reader.getCapabilities (capabilities);
		reader.getDependencies (dependencies);
		return reader.isAvailable ();
	}
	void destroy ()
	{
		this->identities.name = L"";
		this->identities.publisher = L"";
		this->identities.version = VERSION (0);
		this->identities.architectures.clear ();
		this->properties.name = L"";
		this->properties.description = L"";
		this->properties.publisher = L"";
		this->properties.logoBase64 = "";
		if (this->properties.logoStream) { this->properties.logoStream.Release (); this->properties.logoStream = nullptr; }
		this->resources.languages.clear ();
		this->prerequisites.osMinVersion = VERSION (0);
		this->applications.clear ();
		this->capabilities.clear ();
		this->dependencies.clear ();
	}
	// 失败返回长度为 0 或空的文本。
	std::wstring getIdentityName () { return this->identities.name; }
	// 失败返回长度为 0 或空的文本。
	std::wstring getIdentityPublisher () { return this->identities.publisher; }
	// 失败返回都为 0 的版本号
	VERSION getIdentityVersion () { return this->identities.version; }
	size_t getIdentityProcessorArchitecture (std::vector <APPX_PACKAGE_ARCHITECTURE> &arrOut)
	{
		for (auto it : this->identities.architectures)
		{
			arrOut.push_back (it);
		}
		return arrOut.size ();
	}
	size_t getResourceLanguages (std::vector <std::wstring> &vec)
	{
		for (auto it : this->resources.languages)
		{
			vec.push_back (it);
		}
		return vec.size ();
	}
	std::wstring getPropertyName () { return this->properties.name; }
	std::wstring getPropertyDescription () { return this->properties.description; }
	std::wstring getPropertyPublisher () { return this->properties.publisher; }
	std::string getPropertyLogoBase64 () { return this->properties.logoBase64; }
	// 无需手动释放
	IStream *getPropertyLogoIStream () { return this->properties.logoStream; }
	// 失败返回都为 0 的版本号
	VERSION getPrerequisiteOSMinVersion () { return this->prerequisites.osMinVersion; }
	// 判断当前包是否为应用包而不是资源包或依赖项。如果为捆绑包的话判断是否子包中存在应用包。
	bool isPackageApplication () { return this->applications.size () > 0; }
	size_t getApplications (std::vector <appmap> &output)
	{
		for (auto it : this->applications)
		{
			output.push_back (it);
		}
		return output.size ();
	}
	size_t getApplicationUserModelIDs (std::vector <std::wstring> &output)
	{
		for (auto it : this->applications)
		{
			output.push_back (it.getAppUserModelID ());
		}
		return output.size ();
	}
	size_t getApplicationIDs (std::vector <std::wstring> &output)
	{
		for (size_t cnt = 0; cnt < this->applications.size (); cnt ++)
		{
			output.push_back (this->applications [cnt] [std::wstring (L"Id")]);
		}
		return output.size ();
	}
	size_t getApplicationVisualElementsBackgroundColor (std::vector <std::wstring> &output)
	{
		for (size_t cnt = 0; cnt < this->applications.size (); cnt ++)
		{
			output.push_back (this->applications [cnt] [std::wstring (L"BackgroundColor")]);
		}
		return output.size ();
	}
	size_t getCapabilities (std::vector <std::wstring> &output)
	{
		for (auto it : this->capabilities)
		{
			output.push_back (it);
		}
		return output.size ();
	}
	size_t getDependencies (std::vector <DEPINFO> &output)
	{
		for (auto it : this->dependencies)
		{
			output.push_back (it);
		}
		return output.size ();
	}
};
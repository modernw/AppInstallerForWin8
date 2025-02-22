#pragma once
#include "resource.h"
#include "pkgcode.h"
bool IsEqualLabel (std::wstring str1, std::wstring str2)
{
	std::wstring s1 = StringToUpper (StringTrim (str1)),
		s2 = StringToUpper (StringTrim (str2));
	return s1 == s2;
}
typedef class _LABEL
{
	private:
	std::wstring objwsStr;
	public:
	template <class T> explicit _LABEL (const T& str)
	{
		std::wstringstream wss;
		wss << str;
		objwsStr = wss.str ();
	}
	_LABEL (const _LABEL &label)
	{
		objwsStr = std::wstring (label.objwsStr);
	}
	_LABEL (): objwsStr (L"") {}
	friend bool operator == (const _LABEL &l1, const _LABEL &l2)
	{
		return IsEqualLabel (l1.objwsStr, l2.objwsStr);
	}
	friend bool operator == (LPCWSTR &l1, const _LABEL &l2)
	{
		return IsEqualLabel (_LABEL (l1).objwsStr, l2.objwsStr);
	}
	friend bool operator == (const _LABEL &l2, LPCWSTR &l1)
	{
		return IsEqualLabel (_LABEL (l1).objwsStr, l2.objwsStr);
	}
	friend bool operator == (std::wstring &l1, const _LABEL &l2)
	{
		return IsEqualLabel (_LABEL (l1).objwsStr, l2.objwsStr);
	}
	friend bool operator == (const _LABEL &l2, std::wstring &l1)
	{
		return IsEqualLabel (_LABEL (l1).objwsStr, l2.objwsStr);
	}
	_LABEL operator = (const _LABEL &other)
	{
		if (this != &other) this->objwsStr = std::wstring (other.objwsStr);
		return *this;
	}
	_LABEL operator = (LPCWSTR &lpWStr)
	{
		this->objwsStr = std::wstring (lpWStr);
		return *this;
	}
	_LABEL operator = (const std::wstring &obj)
	{
		this->objwsStr = std::wstring (obj);
		return *this;
	}
	std::wstring getStr () const { return objwsStr; }
	LPCWSTR getStrC () { return objwsStr.c_str (); }
	bool operator < (const _LABEL &other) const
	{
		std::wstring s1 = StringToUpper (StringTrim (this->objwsStr)),
			s2 = StringToUpper (StringTrim (other.objwsStr));
		return s1 < s2;
	}
} LABEL;
typedef std::map <LABEL, UINT> MAP_LABEL_UINT;
bool isinitcapmap = false;
MAP_LABEL_UINT capmap;
void InitCapabilitiesMap ()
{
	if (isinitcapmap) return;
	isinitcapmap = true;
	capmap [LABEL (L"internetClient")] = internetClient;
	capmap [LABEL (L"internetClientServer")] = internetClientServer;
	capmap [LABEL (L"privateNetworkClientServer")] = privateNetworkClientServer;
	capmap [LABEL (L"documentsLibrary")] = documentsLibrary;
	capmap [LABEL (L"picturesLibrary")] = picturesLibrary;
	capmap [LABEL (L"videosLibrary")] = videosLibrary;
	capmap [LABEL (L"musicLibrary")] = musicLibrary;
	capmap [LABEL (L"enterpriseAuthentication")] = enterpriseAuthentication;
	capmap [LABEL (L"sharedUserCertificates")] = sharedUserCertificates;
	capmap [LABEL (L"removableStorage")] = removableStorage;
	capmap [LABEL (L"appointments")] = appointments;
	capmap [LABEL (L"contacts")] = contacts;
	capmap [LABEL (L"phoneCall")] = phoneCall;
	capmap [LABEL (L"backgroundMediaPlayback")] = backgroundMediaPlayback;
	capmap [LABEL (L"pointOfService")] = pointOfService;
	capmap [LABEL (L"allJoyn")] = allJoyn;
	capmap [LABEL (L"phoneCallHistoryPublic")] = phoneCallHistoryPublic;
	capmap [LABEL (L"microphone")] = microphone;
	capmap [LABEL (L"offlineMapsManagement")] = offlineMapsManagement;
	capmap [LABEL (L"objects5D")] = objects5D;
	capmap [LABEL (L"objects3D")] = objects3D;
	capmap [LABEL (L"graphicsCaptureWithoutBorder")] = graphicsCaptureWithoutBorder;
	capmap [LABEL (L"bluetooth")] = bluetooth;
	capmap [LABEL (L"lowLevelDevices")] = lowLevelDevices;
	capmap [LABEL (L"location")] = location;
	capmap [LABEL (L"humaninterfacedevice")] = humaninterfacedevice;
	capmap [LABEL (L"recordedCallsFolder")] = recordedCallsFolder;
	capmap [LABEL (L"proximity")] = proximity;
	capmap [LABEL (L"webcam")] = webcam;
	capmap [LABEL (L"graphicsCaptureProgrammatic")] = graphicsCaptureProgrammatic;
	capmap [LABEL (L"userNotificationListener")] = userNotificationListener;
	capmap [LABEL (L"userDataTasks")] = userDataTasks;
	capmap [LABEL (L"userAccountInformation")] = userAccountInformation;
	capmap [LABEL (L"usb")] = usb;
	capmap [LABEL (L"systemManagement")] = systemManagement;
	capmap [LABEL (L"spatialPerception")] = spatialPerception;
	capmap [LABEL (L"smsSend")] = smsSend;
	capmap [LABEL (L"blockedChatMessages")] = blockedChatMessages;
	capmap [LABEL (L"chat")] = chat;
	capmap [LABEL (L"codeGeneration")] = codeGeneration;
	capmap [LABEL (L"voipCall")] = voipCall;
	capmap [LABEL (L"enterpriseDataPolicy")] = enterpriseDataPolicy;
	capmap [LABEL (L"inputInjectionBrokered")] = inputInjectionBrokered;
	capmap [LABEL (L"remoteSystem")] = remoteSystem;
	capmap [LABEL (L"inProcessMediaExtension")] = inProcessMediaExtension;
	capmap [LABEL (L"webPlatformMediaExtension")] = webPlatformMediaExtension;
	capmap [LABEL (L"previewStore")] = previewStore;
	capmap [LABEL (L"confirmAppClose")] = confirmAppClose;
	capmap [LABEL (L"runFullTrust")] = runFullTrust;
	capmap [LABEL (L"previewUiComposition")] = previewUiComposition;
	capmap [LABEL (L"slapiQueryLicenseValue")] = slapiQueryLicenseValue;
	capmap [LABEL (L"localSystemServices")] = localSystemServices;
	capmap [LABEL (L"packageManagement")] = packageManagement;
	capmap [LABEL (L"packageQuery")] = packageQuery;
	capmap [LABEL (L"packagePolicySystem")] = packagePolicySystem;
	capmap [LABEL (L"storeAppInstall")] = storeAppInstall;
	capmap [LABEL (L"appLicensing")] = appLicensing;
	capmap [LABEL (L"broadFileSystemAccess")] = broadFileSystemAccess;
	capmap [LABEL (L"allowElevation")] = allowElevation;
	capmap [LABEL (L"unvirtualizedResources")] = unvirtualizedResources;
	capmap [LABEL (L"extendedExecution")] = extendedExecution;
	capmap [LABEL (L"appointmentsSystem")] = appointmentsSystem;
	capmap [LABEL (L"contactsSystem")] = contactsSystem;
	capmap [LABEL (L"phoneCallSystem")] = phoneCallSystem;
	capmap [LABEL (L"userDataAccounts")] = userDataAccounts;
	capmap [LABEL (L"userDataTasksSystem")] = userDataTasksSystem;
}
// Ê§°Ü·µ»Ø NULL
UINT CapabilitiesNameToID (LPCWSTR lpStr)
{
	if (!lpStr) return NULL;
	InitCapabilitiesMap ();
	auto it = capmap.find (LABEL (lpStr));
	if (it != capmap.end ()) return it->second;
	else return NULL;
}
UINT CapabilitiesNameToID (std::wstring objStr)
{
	return CapabilitiesNameToID (objStr.c_str ());
}
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#ifndef _CRT_NON_CONFORMING_SWPRINTFS
#define _CRT_NON_CONFORMING_SWPRINTFS
#endif
#include <Windows.h>
#include <string>
#include <vcclr.h>
#include <iostream>
#include <cstdarg>
#include <msclr/marshal_cppstd.h>
#include <dwmapi.h>
#include <wingdi.h>
#include <ShObjIdl.h>
#include "pkgcode.h"
#include "resource.h"
#include "res2.h"
#include "PackageManager.h"
#include "base64logo.h"
#include "AppLauncher.h"
#include "ToastNotification.h"
#include "pkginfo.h"
#include "CertificateManager.h"
#include "initread.h"
#include <Psapi.h>

using namespace System;
using namespace System::Windows::Forms;
using namespace System::Threading;

#define toInt(_String_Managed_Object_) Int32::Parse (_String_Managed_Object_)
#define toDouble(_String_Managed_Object_) Double::Parse (_String_Managed_Object_)
#define toBool(_String_Managed_Object_) Boolean::Parse (_String_Managed_Object_)
#define toDateTime(_String_Managed_Object_) DateTime::Parse (_String_Managed_Object_)

String ^GetRCString_NET (UINT resID)
{
	size_t bufferSize = 256;
	wchar_t* buffer = nullptr;
	int length = 0;
	do
	{
		delete [] buffer;
		buffer = new wchar_t [bufferSize];
		length = LoadStringW (GetModuleHandle (nullptr), resID, buffer, bufferSize);
		if ((size_t)length >= bufferSize)
		{
			bufferSize += 20;
		}
	} while ((size_t)length >= bufferSize);
	String^ result = gcnew String (buffer);
	delete [] buffer;
	return result;
}
std::wstring GetRCString_cpp (UINT resID)
{
	size_t bufferSize = 256;
	wchar_t* buffer = nullptr;
	int length = 0;
	do
	{
		delete [] buffer;
		buffer = new wchar_t [bufferSize];
		length = LoadStringW (GetModuleHandle (nullptr), resID, buffer, bufferSize);
		if ((size_t)length >= bufferSize)
		{
			bufferSize += 20;
		}
	} while ((size_t)length >= bufferSize);
	std::wstring result = L"";
	result += buffer;
	delete [] buffer;
	return result;
}
#define GetRCString GetRCString_NET
#define GetRCIntValue(_UINT__resID_) toInt (GetRCString (_UINT__resID_))
#define GetRCDoubleValue(_UINT__resID_) toDouble (GetRCString (_UINT__resID_))
#define GetRCBoolValue(_UINT__resID_) toBool (GetRCString (_UINT__resID_))
#define GetRCDateTimeValue(_UINT__resID_) toDateTime (GetRCString (_UINT__resID_))
#define rcString(resID) GetRCString (resID)
#define rcInt(resID) GetRCIntValue (resID)
#define rcDouble(resID) GetRCDoubleValue (resID)
#define rcBool(resID) GetRCBoolValue (resID)
#define rcDTime(resID) GetRCDateTimeValue (resID)
#define rcIcon(resID) LoadRCIcon (resID)

HICON LoadRCIcon (UINT resID)
{
	// 从资源加载图标
	HICON hIcon = (HICON)LoadImage (GetModuleHandle (nullptr), MAKEINTRESOURCE (resID), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE);
	if (hIcon == nullptr) return nullptr;
	return hIcon;
}
HRSRC FindResourceByName (LPCWSTR resourceName, LPCWSTR resourceType)
{
	return FindResourceW (GetModuleHandle (nullptr), resourceName, resourceType);
}
HRSRC FindResourceByName (const std::wstring &resourceName, LPCWSTR resourceType)
{
	return FindResourceByName (resourceName.c_str (), resourceType);
}

System::Drawing::Color GetAeroColor ()
{
	DWORD color = 0;
	BOOL opaqueBlend = FALSE;
	// 调用 DwmGetColorizationColor 获取 Aero 颜色
	HRESULT hr = DwmGetColorizationColor (&color, &opaqueBlend);
	if (SUCCEEDED (hr)) {
		BYTE r = (BYTE)((color & 0x00FF0000) >> 16);
		BYTE g = (BYTE)((color & 0x0000FF00) >> 8);
		BYTE b = (BYTE)(color & 0x000000FF);
		return System::Drawing::Color::FromArgb (r, g, b);
	}
	else {
		// 如果获取失败，返回默认颜色
		return System::Drawing::Color::FromArgb (0, 120, 215);
	}
}
// 将颜色转换为 HTML 颜色值 (#RRGGBB 格式)
String ^ColorToHtml (System::Drawing::Color color)
{
	return String::Format ("#{0:X2}{1:X2}{2:X2}", color.R, color.G, color.B);
}
bool IsAppInDarkMode () 
{
	HKEY hKey;
	DWORD dwValue;
	if (RegOpenKeyEx (HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", 0, KEY_READ, &hKey) == ERROR_SUCCESS) 
	{
		DWORD dwSize = sizeof (dwValue);
		if (RegQueryValueEx (hKey, L"AppsUseLightTheme", NULL, NULL, (LPBYTE)&dwValue, &dwSize) == ERROR_SUCCESS)
		{
			RegCloseKey (hKey);
			return dwValue == 0;
		}
		RegCloseKey (hKey);
	}
	return false;
}

std::wstring StrPrintFormatW (LPCWSTR format, ...)
{
	va_list args;
	va_start (args, format);
	size_t sizeBuf = 256;
	wchar_t* buffer = (wchar_t*)malloc (sizeBuf * sizeof (wchar_t));
	if (!buffer)
	{
		va_end (args);
		return L"";
	}
	int written = -1;
	while (true)
	{
		written = vswprintf (buffer, sizeBuf, format, args);
		if (written >= 0 && static_cast<size_t>(written) < sizeBuf)
		{
			break;
		}
		sizeBuf += 256;
		wchar_t* newBuffer = (wchar_t*)realloc (buffer, sizeBuf * sizeof (wchar_t));
		if (!newBuffer)
		{
			free (buffer);
			va_end (args);
			return L"";
		}
		buffer = newBuffer;
	}
	va_end (args);
	std::wstring result (buffer);
	free (buffer);
	return result;
}

void SetWebBrowserEmulation ()
{
	// 获取应用程序的可执行文件名
	String ^appName = System::IO::Path::GetFileNameWithoutExtension (System::Windows::Forms::Application::ExecutablePath);
	const wchar_t *appNameWChar = (const wchar_t *)(void *)System::Runtime::InteropServices::Marshal::StringToHGlobalUni (appName);
	BOOL is64Bit = FALSE;
	IsWow64Process (GetCurrentProcess (), &is64Bit);
	HKEY hKey;
	long regOpenResult = ERROR_SUCCESS;
	if (is64Bit)
	{
		regOpenResult = RegOpenKeyExW (HKEY_CURRENT_USER, L"SOFTWARE\\WOW6432Node\\Microsoft\\Internet Explorer\\Main\\FeatureControl\\FEATURE_BROWSER_EMULATION", 0, KEY_WRITE, &hKey);
	}
	else
	{
		regOpenResult = RegOpenKeyExW (HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Internet Explorer\\Main\\FeatureControl\\FEATURE_BROWSER_EMULATION", 0, KEY_WRITE, &hKey);
	}
	if (regOpenResult == ERROR_SUCCESS)
	{
		DWORD ie11Mode = 11001;
		RegSetValueExW (hKey, appNameWChar, 0, REG_DWORD, (const BYTE *)&ie11Mode, sizeof (DWORD));
		RegCloseKey (hKey);
	}
}

#include <windows.h>
#include <string>

bool IsIeVersion10 () 
{
	HKEY hKey;
	const wchar_t* ieKeyPath = L"SOFTWARE\\Microsoft\\Internet Explorer";
	const wchar_t* valueName = L"svcVersion";
	DWORD dwType = REG_SZ;
	wchar_t version [32];
	DWORD cbData = sizeof (version);
	REGSAM samDesired = KEY_READ | KEY_WOW64_64KEY;
	if (RegOpenKeyExW (HKEY_LOCAL_MACHINE, ieKeyPath, 0, samDesired, &hKey) != ERROR_SUCCESS)
	{
		samDesired = KEY_READ | KEY_WOW64_32KEY;
		if (RegOpenKeyExW (HKEY_LOCAL_MACHINE, ieKeyPath, 0, samDesired, &hKey) != ERROR_SUCCESS)
		{
			return false;
		}
	}
	bool isIe10 = false;
	if (RegQueryValueExW (hKey, valueName, NULL, &dwType, (LPBYTE)version, &cbData) == ERROR_SUCCESS)
	{
		std::wstring verStr (version);
		size_t dotPos = verStr.find (L'.');
		if (dotPos != std::wstring::npos)
		{
			int majorVersion = std::stoi (verStr.substr (0, dotPos));
			if (majorVersion == 10) 
			{
				isIe10 = true;
			}
		}
	}
	RegCloseKey (hKey);
	return isIe10;
}

size_t EnumerateFilesW (const std::wstring &directory, const std::wstring &filter,
	std::vector <std::wstring> &outFiles, bool recursive = false)
{
	std::wstring searchPath = directory;
	if (!searchPath.empty () && searchPath.back () != L'\\')
	{
		searchPath += L'\\';
	}
	searchPath += filter;
	WIN32_FIND_DATAW findData;
	HANDLE hFind = FindFirstFileW (searchPath.c_str (), &findData);
	if (hFind != INVALID_HANDLE_VALUE) 
	{
		do {
			if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) 
			{
				outFiles.push_back (directory + L"\\" + findData.cFileName);
			}
		} while (FindNextFileW (hFind, &findData));
		FindClose (hFind);
	}
	if (recursive) {
		std::wstring subDirSearchPath = directory + L"\\*";
		hFind = FindFirstFileW (subDirSearchPath.c_str (), &findData);
		if (hFind != INVALID_HANDLE_VALUE) 
		{
			do {
				if ((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
					wcscmp (findData.cFileName, L".") != 0 && wcscmp (findData.cFileName, L"..") != 0) 
				{
					EnumerateFilesW (directory + L"\\" + findData.cFileName, filter, outFiles, true);
				}
			} while (FindNextFileW (hFind, &findData));
			FindClose (hFind);
		}
	}
	return outFiles.size ();
}

std::wstring GetFileDirectoryW (const std::wstring &filePath)
{
	std::wstring directory (L"");
	WCHAR *buf = (WCHAR *)calloc (filePath.capacity () + 1, sizeof (WCHAR));
	lstrcpyW (buf, filePath.c_str ());
	PathRemoveFileSpecW (buf);
	directory += buf;
	free (buf);
	return directory;
}

std::wstring pkgPath = L"";
pkgreader reader;
bool m_silentMode = false;
LPCWSTR m_idenName = L"Microsoft.DesktopAppInstaller";
PackageInfomation m_pkgInfo;
WInitFile m_initConfig;

HRESULT SetCurrentAppUserModelID (PCWSTR appID)
{
	typedef HRESULT (WINAPI *SetAppUserModelIDFunc)(PCWSTR);
	HMODULE shell32 = LoadLibraryW (L"shell32.dll");
	if (!shell32) return E_FAIL;
	auto SetAppUserModelID = (SetAppUserModelIDFunc)GetProcAddress (shell32, "SetCurrentProcessExplicitAppUserModelID");
	if (!SetAppUserModelID)
	{
		FreeLibrary (shell32);
		return E_FAIL;
	}
	HRESULT hr = SetAppUserModelID (appID);
	FreeLibrary (shell32);
	return hr;
}

void ProgressCallback (unsigned progress);
void ToastPressCallback ();

public ref class MainWnd: public Form
{
	private:
	WebBrowser ^webUI;
	int page;
	System::Collections::Generic::Dictionary <String ^, Delegate ^> ^jsFunctionHandlers;
	public:
	MainWnd (): page (-1)
	{
		jsFunctionHandlers = gcnew System::Collections::Generic::Dictionary <String ^, Delegate ^> ();
		initWnd ();
	}
	private:
	void initWnd ()
	{
		this->Visible = false;
		this->webUI = gcnew WebBrowser ();
		this->SuspendLayout ();
		this->webUI->Dock = DockStyle::Fill;
		this->webUI->DocumentCompleted += gcnew WebBrowserDocumentCompletedEventHandler (this, &MainWnd::eventOnDocumentCompleted);
		this->webUI->IsWebBrowserContextMenuEnabled = false;
		this->webUI->PreviewKeyDown += gcnew System::Windows::Forms::PreviewKeyDownEventHandler (this, &MainWnd::eventOnPreviewKeyDown_WebBrowser);
		this->Controls->Add (this->webUI);
		HICON tempIco = LoadRCIcon (ICON_TASKBAR);
		if (tempIco)
		{
			this->Icon = System::Drawing::Icon::FromHandle (IntPtr (tempIco));
			// DestroyIcon (tempIco);
		}
		unsigned wid = 0, hei = 0;
		if (m_initConfig.readBoolValue (L"Settings", L"SavePosBeforeClosing"))
		{
			wid = m_initConfig.readUIntValue (L"Settings", L"SavePosWidth",
				m_initConfig.readUIntValue (L"Settings", L"PosWidth",
					(unsigned)_wtol (GetRCString_cpp (INITWIDTH).c_str ())
				)
			);
			hei = m_initConfig.readUIntValue (L"Settings", L"SavePosHeight",
				m_initConfig.readUIntValue (L"Settings", L"PosHeight",
				(unsigned)_wtol (GetRCString_cpp (INITHEIGHT).c_str ())
				)
			);
		}
		else
		{
			wid = m_initConfig.readUIntValue (L"Settings", L"SavePosWidth",
				(unsigned)_wtol (GetRCString_cpp (INITWIDTH).c_str ())
			);
			hei = m_initConfig.readUIntValue (L"Settings", L"SavePosHeight",
				(unsigned)_wtol (GetRCString_cpp (INITHEIGHT).c_str ())
			);
		}
		this->MinimumSize = System::Drawing::Size (
			m_initConfig.readUIntValue (L"Settings", L"MinPosWidth", (unsigned)_wtol (GetRCString_cpp (LIMITWIDTH).c_str ())),
			m_initConfig.readUIntValue (L"Settings", L"MinPosHeight", (unsigned)_wtol (GetRCString_cpp (LIMITHEIGHT).c_str ()))
		);
		this->WindowState = (System::Windows::Forms::FormWindowState)m_initConfig.readIntValue (L"Settings", L"WindowPos", (int)System::Windows::Forms::FormWindowState::Normal);
		this->ClientSize = System::Drawing::Size (wid, hei);
		this->Text = GetRCString (WIN_TITLE);
		this->Load += gcnew EventHandler (this, &MainWnd::eventOnCreate);
		this->ResizeEnd += gcnew EventHandler (this, &MainWnd::eventOnResizeEnd);
		this->ResumeLayout (false);
		this->Resize += gcnew System::EventHandler (this, &MainWnd::eventOnResize);
	}
	void eventOnCreate (System::Object ^sender, System::EventArgs ^e)
	{
		SetWebBrowserEmulation ();
		String ^rootDir = System::IO::Path::GetDirectoryName (System::Windows::Forms::Application::ExecutablePath);
		String ^htmlFilePath = System::IO::Path::Combine (rootDir, "HTML\\Index.html");
		if (IsIeVersion10 ()) htmlFilePath = System::IO::Path::Combine (rootDir, "HTML\\IndexIE10.html"); // IE10 (Win8) 特供
		webUI->Navigate (htmlFilePath);
		webUI->ObjectForScripting = this;
	}
	void eventOnPreviewKeyDown_WebBrowser (System::Object ^sender, System::Windows::Forms::PreviewKeyDownEventArgs ^e)
	{
		if (e->KeyCode == System::Windows::Forms::Keys::F5 || (e->KeyCode == System::Windows::Forms::Keys::R && e->Control))
			e->IsInputKey = true;
	}
	void eventOnDocumentCompleted (Object ^sender, WebBrowserDocumentCompletedEventArgs ^e)
	{
		if (e->Url->ToString () == webUI->Url->ToString ())
		{
			eventOnPageLoad ();
		}
	}
	void eventOnResizeEnd (Object ^sender, EventArgs ^e)
	{
		if (this->WindowState == FormWindowState::Maximized)
		{
			this->changeMaxDisplayPicJS (true);
		}
		else if (this->WindowState == FormWindowState::Normal)
		{
			this->changeMaxDisplayPicJS (false);
		}
	}
	void eventOnResize (Object ^sender, EventArgs ^e)
	{
		if (this->WindowState == FormWindowState::Maximized)
		{
			this->changeMaxDisplayPicJS (true);
			m_initConfig.writeIntValue (L"Settings", L"WindowPos", (int)this->WindowState);
		}
		else
		{
			m_initConfig.writeIntValue (L"Settings", L"WindowPos", 0);
			this->changeMaxDisplayPicJS (false);
			if (m_initConfig.readBoolValue (L"Settings", L"SavePosBeforeClosing"))
			{
				m_initConfig.writeUIntValue (L"Settings", L"SavePosWidth", this->Width);
				m_initConfig.writeUIntValue (L"Settings", L"SavePosHeight", this->Height);
			}
		}
	}
	void eventOnPageLoad ()
	{
		this->Visible = true;
		this->setLaunchWhenReadyJS (m_initConfig.readBoolValue (L"Settings", L"LaunchWhenReady", true));
		if (IsIeVersion10 ()) hideFrameJS (false);
		Thread ^thread = gcnew Thread (gcnew ThreadStart (this, &MainWnd::taskReadFile));
		thread->Start ();
		Thread ^thread2 = gcnew Thread (gcnew ThreadStart (this, &MainWnd::taskCountDarkMode));
		thread2->Start ();
	}
	void eventOnPress_button1 ()
	{
		//MessageBox::Show ("这是一个简单的消息框。");
		switch (page)
		{
			case 1:
			{
				OpenFileDialog^ openFileDialog = gcnew OpenFileDialog ();
				openFileDialog->Title = rcString (DIALOG_WINTITLE);
				openFileDialog->Filter = rcString (DIALOG_SELLET_APPX) + "|*.appx;*appxbundle|" + rcString (DIALOG_SELLET_UNI) + "|*.*";
				openFileDialog->FilterIndex = 1;
				openFileDialog->RestoreDirectory = true;
				if (openFileDialog->ShowDialog () == System::Windows::Forms::DialogResult::OK)
				{
					System::String^ filePath = openFileDialog->FileName;
					std::wstring stdFilePath = msclr::interop::marshal_as<std::wstring> (filePath);
					pkgPath = stdFilePath;
					invokeSetPage (0);
					Thread ^thread = gcnew Thread (gcnew ThreadStart (this, &MainWnd::taskReadFile));
					thread->SetApartmentState (ApartmentState::STA);
					thread->Start ();
				}
			} break;
			case 2:
			{
				invokeSetPage (3);
				Thread ^thread = gcnew Thread (gcnew ThreadStart (this, &MainWnd::taskInstallPackage));
				thread->SetApartmentState (ApartmentState::STA);
				thread->Start ();
			} break;
			case 4:
			{
				// this->setButtonDisabledJS (true);
				if (!m_pkgInfo.isPackageApplication ())
				{
					this->Close (); break;
				}
				else
				{
					std::vector <std::wstring> ids;
					m_pkgInfo.getApplicationUserModelIDs (ids);
					DWORD dwPId = 0;
					if (ids.size () == 1) LaunchApp (ids [0].c_str (), &dwPId);
				}
				// this->setButtonDisabledJS (false);
			} break;
			case 5:
			{
				this->Close ();
			} break;
		}
	}
	void eventOnPress_button2 ()
	{

	}
	void eventOnPress_buttonBack ()
	{

	}
	void eventOnPress_buttonOption ()
	{

	}
	void eventOnPress_buttonNext ()
	{

	}
	void eventOnPress_buttonMin ()
	{
		if (this->WindowState == System::Windows::Forms::FormWindowState::Minimized) this->WindowState = System::Windows::Forms::FormWindowState::Normal;
		else this->WindowState = System::Windows::Forms::FormWindowState::Minimized;
	}
	void eventOnPress_buttonMax ()
	{
		if (this->WindowState == System::Windows::Forms::FormWindowState::Maximized)
		{
			this->WindowState = System::Windows::Forms::FormWindowState::Normal;
			this->changeMaxDisplayPicJS (false);
		}
		else
		{
			this->WindowState = System::Windows::Forms::FormWindowState::Maximized;
			this->changeMaxDisplayPicJS (true);
		}
	}
	void eventOnPress_buttonCancel ()
	{
		this->Close ();
	}
	Object ^CallJSFunction (String ^functionName, ... array<Object^> ^args)
	{
		if (webUI->Document != nullptr)
		{
			try
			{
				return webUI->Document->InvokeScript (functionName, args);
			}
			catch (Exception^ ex)
			{
				MessageBox::Show ("Error calling JavaScript function: " + ex->Message);
				return nullptr;
			}
		}
		return nullptr;
	}
	void setPageJS (int pageSerial)
	{
		CallJSFunction ("SetPage", gcnew array <Object ^> { pageSerial });
	}
	void setLogoBackgroundColorJS (String ^color)
	{
		if (color == "transparent")
		{
			color = ColorToHtml (GetAeroColor ());
		}
		CallJSFunction ("setImgBackground", gcnew array <Object ^> { color });
	}
	void setLogoBackgroundColorJS (System::Drawing::Color ^color)
	{
		CallJSFunction ("setImgBackground", gcnew array <Object ^> { color });
	}
	void setLaunchWhenReadyJS (bool value)
	{
		CallJSFunction ("SetLaunchWhenReady", gcnew array <Object ^> { value });
	}
	void invokeSetLaunchWhenReady (bool value)
	{
		if (this->InvokeRequired)
		{
			this->Invoke (gcnew Action <bool> (this, &MainWnd::setLaunchWhenReadyJS), value);
		}
		else
		{
			setLaunchWhenReadyJS (value);
		}
	}
	void setDisableLaunchWhenReadyJS (bool value)
	{
		CallJSFunction ("SetLaunchWhenReadyDisable", gcnew array <Object ^> { value });
	}
	void invokeSetDisableLaunchWhenReady (bool value)
	{
		if (this->InvokeRequired)
		{
			this->Invoke (gcnew Action <bool> (this, &MainWnd::setDisableLaunchWhenReadyJS), value);
		}
		else
		{
			setDisableLaunchWhenReadyJS (value);
		}
	}
	bool getLaunchWhenReadyJS ()
	{
		return (bool)CallJSFunction ("GetLaunchWhenReady", gcnew array <Object ^> {});
	}
	void setWindowTitleJS (String ^value)
	{
		CallJSFunction ("SetWindowTitle", gcnew array <Object ^> { value });
	}
	void setStoreLogoJS (String ^value)
	{
		CallJSFunction ("SetStoreLogo", gcnew array <Object ^> { value });
	}
	void hideFrameJS (bool value)
	{
		CallJSFunction ("HideFrame", gcnew array <Object ^> { value });
	}
	void setTextJS (String ^comID, String ^content)
	{
		try
		{
			CallJSFunction ("SetText", gcnew array<Object^> { comID, content });
		}
		catch (System::Exception^ e)
		{
			Console::WriteLine ("Error setting text for component '" + comID + "': " + e->Message);
		}
	}
	void setProgressTextJS (String ^content)
	{
		this->setTextJS ("caption-status", content);
	}
	void changeMaxDisplayPicJS (bool value)
	{
		CallJSFunction ("ChangeMaxDisplayPic", gcnew array <Object ^> { value });
	}
	size_t getTextLineNumberJS (String ^comID)
	{
		Object ^result = CallJSFunction ("GetTextLineNumber", gcnew array <Object ^> { comID });
		int lineCount = Convert::ToInt32 (result);
		if (lineCount < 0)
		{
			throw gcnew ArgumentException ("Element with ID '" + comID + "' does not exist.");
		}
		return static_cast<size_t>(lineCount);
	}
	size_t getTextOmitAdviceJS (String ^comID, String ^text, size_t limitLines)
	{
		Object ^result = CallJSFunction ("GetTextOmitAdvice", gcnew array <Object ^> { comID, text, limitLines });
		int lineCount = Convert::ToInt32 (result);
		if (lineCount < 0)
		{
			throw gcnew ArgumentException ("Element with ID '" + comID + "' does not exist.");
		}
		return static_cast<size_t>(lineCount);
	}
	void setPicBoxVisibilityJS (bool value)
	{

		CallJSFunction ("SetPicBoxVisibility", gcnew array <Object ^> { value });
	}
	void setButtonDisabledJS (bool value)
	{
		CallJSFunction ("SetButtonDisabled", gcnew array <Object ^> { value });
	}
	void setProgressJS (unsigned value)
	{
		CallJSFunction ("SetProgress", gcnew array <Object ^> { value });
	}
	void setDarkModeJS (bool value)
	{
		CallJSFunction ("SetDarkMode", gcnew array <Object ^> { value });
	}
	void invokeSetDarkMode (bool value)
	{
		if (this->InvokeRequired)
		{
			this->Invoke (gcnew Action <bool> (this, &MainWnd::setDarkModeJS), value);
		}
		else
		{
			setDarkModeJS (value);
		}
	}
	void invokeSetProgress (unsigned value)
	{
		if (this->InvokeRequired)
		{
			this->Invoke (gcnew Action <unsigned> (this, &MainWnd::setProgressJS), value);
		}
		else
		{
			setProgressJS (value);
		}
	}
	void invokeSetProgressText (String ^content)
	{
		if (this->InvokeRequired)
		{
			this->Invoke (gcnew Action <String ^> (this, &MainWnd::setProgressTextJS), content);
		}
		else
		{
			setProgressTextJS (content);
		}
	}
	void setPage (int serial)
	{
		switch (serial)
		{
			case 0:
				setTextJS ("file-loading-text", rcString (PAGE_LOADING));
				break;
			case 1:
				setTextJS ("caption-title", rcString (PAGE_3_TITLE));
				setTextJS ("button-serial-1-title", rcString (BUTTON_3_SER1));
				setTextJS ("button-serial-1", rcString (BUTTON_3_SER1));
				break;
			case 2:
			{
				std::wstring title = m_pkgInfo.getPropertyName (), titleF = StrPrintFormatW (GetRCString_cpp (PAGE_1_TITLE).c_str (), title.c_str ());
				size_t reduce = getTextOmitAdviceJS (gcnew String ("caption-title"), gcnew String (titleF.c_str ()), (size_t)2);
				std::wstring sub = title.substr (0, title.length () - reduce) + (reduce > 3 ? L"..." : L"");
				setTextJS ("caption-title", gcnew String (StrPrintFormatW (GetRCString_cpp (PAGE_1_TITLE).c_str (), sub.c_str ()).c_str ()));
			}
			setTextJS ("caption-ms-store-app", rcString (PAGE_1_APP));
			setTextJS ("caption-publisher", gcnew String (StrPrintFormatW (GetRCString_cpp (PAGE_INFO_PUBLISHER).c_str (), m_pkgInfo.getPropertyPublisher ().c_str ()).c_str ()));
			setTextJS ("caption-version", gcnew String (StrPrintFormatW (GetRCString_cpp (PAGE_INFO_VERSION).c_str (), m_pkgInfo.getIdentityVersion ().toStringW ().c_str ()).c_str ()));
			setTextJS ("caption-textarea-title", rcString (PAGE_FUNCTIONS));
			{
				std::wstring temp = L"";
				std::vector <std::wstring> caps;
				if (m_pkgInfo.getCapabilities (caps))
				{
					for (auto it : caps)
					{
						temp += L'\u2022';
						temp += GetRCString_cpp (CapabilitiesNameToID (it.c_str ()));
						temp += L"\n";
					}
				}
				VERSION pver = m_pkgInfo.getPrerequisiteOSMinVersion ();
				if (!pver.isEmpty ())
				{
					temp += StrPrintFormatW (GetRCString_cpp (textSupportSystem).c_str (), GetPrerequistOSVersionText (pver).c_str (), pver.toStringW ().c_str ());
					temp += L"\n";
				}
				std::vector <APPX_PACKAGE_ARCHITECTURE> avec;
				if (m_pkgInfo.getIdentityProcessorArchitecture (avec))
				{
					temp += GetRCString_cpp (textSupportCPU);
					for (size_t cnt = 0; cnt < avec.size (); cnt ++)
					{
						switch (avec [cnt])
						{
							case APPX_PACKAGE_ARCHITECTURE_NEUTRAL: temp += L"Neutral"; break;
							case APPX_PACKAGE_ARCHITECTURE_X86: temp += L"x86"; break;
							case APPX_PACKAGE_ARCHITECTURE_X64: temp += L"x64"; break;
							case APPX_PACKAGE_ARCHITECTURE_ARM: temp += L"Arm"; break;
							default:
								if ((UINT)avec [cnt] == 12) temp += L"Arm64";
						}
						if (cnt + 1 != avec.size ()) temp += L", ";
					}
					temp += L"\n";
				}
				std::vector <DEPINFO> deps;
				if (m_pkgInfo.getDependencies (deps))
				{
					temp += GetRCString_cpp (textDependency) + L"\n";
					for (size_t cnt = 0; cnt < deps.size (); cnt ++)
					{
						temp += StrPrintFormatW (GetRCString_cpp (textDependencyItem).c_str (), deps [cnt].name.c_str (), deps [cnt].versionMin.toStringW ().c_str ());
						temp += L"\n";
					}
				}
				std::vector <std::wstring> lvec;
				if (m_pkgInfo.getResourceLanguages (lvec))
				{
					WCHAR buf [86] = {0};
					for (auto it : lvec)
					{
						int res = GetLocaleInfoEx (it.c_str (), 2, buf, 86);
						if (lstrlenW (buf) > 0)
						{
							temp += L'\u2022' + std::wstring (buf) + L"\n";
						}
					}
				}
				setTextJS ("caption-more-info", gcnew String (temp.c_str ()));
			}
			setTextJS ("caption-check-box-label", rcString (PAGE_CHECK_RUNWHENREADY));
			setTextJS ("button-serial-1-title", rcString (BUTTON_1_SER1));
			setTextJS ("button-serial-1", rcString (BUTTON_1_SER1));
			break;
			case 3:
			{
				invokeSetProgressText (rcString (PAGE_2_LOADING));
				std::wstring title = m_pkgInfo.getPropertyName (), titleF = StrPrintFormatW (GetRCString_cpp (PAGE_2_TITLE).c_str (), title.c_str ());
				size_t reduce = getTextOmitAdviceJS (gcnew String ("caption-title"), gcnew String (titleF.c_str ()), (size_t)2);
				std::wstring sub = title.substr (0, title.length () - reduce) + (reduce > 3 ? L"..." : L"");
				setTextJS ("caption-title", gcnew String (StrPrintFormatW (GetRCString_cpp (PAGE_2_TITLE).c_str (), sub.c_str ()).c_str ()));
			}
			break;
			case 4:
			{
				std::wstring title = m_pkgInfo.getPropertyName (), titleF = StrPrintFormatW (GetRCString_cpp (PAGE_4_TITLE).c_str (), title.c_str ());
				size_t reduce = getTextOmitAdviceJS (gcnew String ("caption-title"), gcnew String (titleF.c_str ()), (size_t)2);
				std::wstring sub = title.substr (0, title.length () - reduce) + (reduce > 3 ? L"..." : L"");
				setTextJS ("caption-title", gcnew String (StrPrintFormatW (GetRCString_cpp (PAGE_4_TITLE).c_str (), sub.c_str ()).c_str ()));
			}
			{
				std::vector <std::wstring> avec;
				if (m_pkgInfo.getApplicationUserModelIDs (avec))
				{
					setTextJS ("button-serial-1-title", rcString (BUTTON_4_SER1));
					setTextJS ("button-serial-1", rcString (BUTTON_4_SER1));
				}
				else
				{
					setTextJS ("button-serial-1-title", rcString (BUTTON_5_SER1));
					setTextJS ("button-serial-1", rcString (BUTTON_5_SER1));
				}
			}
			break;
			case 5:
			{
				std::wstring title = m_pkgInfo.getPropertyName (), titleF = StrPrintFormatW (GetRCString_cpp (PAGE_5_TITLE).c_str (), title.c_str ());
				size_t reduce = getTextOmitAdviceJS (gcnew String ("caption-title"), gcnew String (titleF.c_str ()), (size_t)2);
				std::wstring sub = title.substr (0, title.length () - reduce) + (reduce > 3 ? L"..." : L"");
				setTextJS ("caption-title", gcnew String (StrPrintFormatW (GetRCString_cpp (PAGE_5_TITLE).c_str (), sub.c_str ()).c_str ()));
				if (GetLastErrorDetailTextLength ()) setTextJS ("caption-more-info", gcnew String (GetLastErrorDetailText ()));
				else setTextJS ("caption-more-info", gcnew String (L""));
			}
			setTextJS ("button-serial-1-title", rcString (BUTTON_5_SER1));
			setTextJS ("button-serial-1", rcString (BUTTON_5_SER1));
		}
		setPageJS (serial);
		if (serial > 1)
		{
			std::string res = m_pkgInfo.getPropertyLogoBase64 ();
			if (!res.empty () && res.length () > 0)
			{
				std::string b64logo = res;
				// MessageBox::Show (gcnew String ((std::string ("Base 64 String: ") + b64logo).c_str ()));
				this->setStoreLogoJS (gcnew String (res.c_str ()));
				setPicBoxVisibilityJS (true);
			}
			else setPicBoxVisibilityJS (false);
		}
		else
		{
			setPicBoxVisibilityJS (false);
		}
		page = serial;
	}
	void invokeSetPage (int page)
	{
		if (this->InvokeRequired)
		{
			this->Invoke (gcnew Action<int> (this, &MainWnd::setPage), page);
		}
		else
		{
			setPage (page);
		}
	}
	void invokeSetColor (String ^color)
	{
		if (this->InvokeRequired)
		{
			this->Invoke (gcnew Action<String ^> (this, &MainWnd::setLogoBackgroundColorJS), color);
		}
		else
		{
			setLogoBackgroundColorJS (color);
		}
	}
	void setTaskbarProgress (int progress)
	{
		IntPtr hwnd = this->Handle;
		ITaskbarList3* pTaskbarList = nullptr;
		HRESULT hr = CoCreateInstance (CLSID_TaskbarList, nullptr, CLSCTX_INPROC_SERVER, IID_ITaskbarList3, (void**)&pTaskbarList);
		if (SUCCEEDED (hr) && pTaskbarList)
		{
			pTaskbarList->HrInit ();
			pTaskbarList->SetProgressState ((HWND)hwnd.ToPointer (), TBPF_NORMAL);
			pTaskbarList->SetProgressValue ((HWND)hwnd.ToPointer (), progress, 100);
			pTaskbarList->Release ();
		}
	}
	void clearTaskbarProgress ()
	{
		IntPtr hwnd = this->Handle;
		// 获取 ITaskbarList3 接口
		ITaskbarList3* pTaskbarList = nullptr;
		HRESULT hr = CoCreateInstance (CLSID_TaskbarList, nullptr, CLSCTX_INPROC_SERVER, IID_ITaskbarList3, (void**)&pTaskbarList);
		if (SUCCEEDED (hr) && pTaskbarList)
		{
			pTaskbarList->HrInit ();
			// 清除进度条状态
			pTaskbarList->SetProgressState ((HWND)hwnd.ToPointer (), TBPF_NOPROGRESS);
			// 释放 ITaskbarList3 接口
			pTaskbarList->Release ();
		}
	}
	bool invokeGetLaunchWhenReady ()
	{
		if (this->InvokeRequired)
		{
			return (bool)this->Invoke (gcnew Func <bool> (this, &MainWnd::getLaunchWhenReadyJS));
		}
		else
		{
			return getLaunchWhenReadyJS ();
		}
	}
	[STAThread]
	void taskReadFile ()
	{
		m_pkgInfo.destroy ();
		if (IsFileExistsW (pkgPath.c_str ())) reader.create (pkgPath);
		if (reader.isAvailable ())
		{
			m_pkgInfo.create (reader);
			std::vector <std::wstring> color;
			m_pkgInfo.getApplicationVisualElementsBackgroundColor (color);
			// if (page < 0) invokeSetPage (2);
			if (color.size () > 0)
			{
				invokeSetColor (gcnew String (color [0].c_str ()));
			}
			invokeSetPage (2);
			if (m_silentMode)
			{
				invokeSetDisableLaunchWhenReady (true);
				invokeSetLaunchWhenReady (false);
				invokeSetPage (3);
				Thread ^thread = gcnew Thread (gcnew ThreadStart (this, &MainWnd::taskInstallPackage));
				thread->SetApartmentState (ApartmentState::STA);
				thread->Start ();
			}
		}
		else
		{
			if (m_silentMode)
			{
				// 在此版本中，在静默模式下打开无效的包后会自动关闭窗口结束程序。
				this->Close ();
			}
			else
			{
				invokeSetPage (1);
			}
		}
		this->invokeSetDarkMode (IsAppInDarkMode ());
		EmptyWorkingSet ((HANDLE)-1);
	}
	void taskInstallPackage ()
	{
		invokeSetProgressText (rcString (PAGE_2_LOADCERT));
		/*
		std::vector <std::wstring> cerfiles;
		EnumerateFilesW (GetFileDirectoryW (pkgPath.c_str ()), L"*.cer", cerfiles, false);
		for (auto it : cerfiles)
		{
			bool res = LoadCertFromCertFile (it.c_str ());
			std::wstring outputDe = L"File: " + it + L", Result: " + (res ? L"true" : L"false");
			OutputDebugStringW (outputDe.c_str ());
		}
		*/
		LoadCertFromSignedFile (pkgPath.c_str ());
		invokeSetProgressText (rcString (PAGE_2_INSTALL));
		InstallStatus status = AddPackageFromPath (pkgPath.c_str (), &ProgressCallback);
		// MessageBeep (MB_OK);
		if (status == InstallStatus::Success)
		{
			invokeSetPage (4);
			if (this->invokeGetLaunchWhenReady ())
			{
				if (reader.isPackageApplication ()) this->eventOnPress_button1 (); // 用于启用 APP
			}
			Thread ^closeThread = gcnew Thread (gcnew ThreadStart (this, &MainWnd::taskCountCancel));
			closeThread->IsBackground = true;
			closeThread->Start ();
		}
		else
		{
			invokeSetPage (5);
			if (m_silentMode)
			{
				Thread ^closeThread = gcnew Thread (gcnew ThreadStart (this, &MainWnd::taskCountCancel));
				closeThread->IsBackground = true;
				closeThread->Start ();
			}
		}
		this->clearTaskbarProgress ();
		std::wstring title = StrPrintFormatW (GetRCString_cpp (PAGE_4_TITLE).c_str (), m_pkgInfo.getPropertyName ().c_str ());
		std::wstring text (L"");
		if (GetLastErrorDetailTextLength ()) text += GetLastErrorDetailText ();
		bool res = CreateToastNotification (m_idenName, title.c_str (), text.c_str (), &ToastPressCallback, m_pkgInfo.getPropertyLogoIStream ());
	#ifdef _DEBUG
		//MessageBox::Show (res ? "Toast has create! " : "Toast created failed! ");
	#endif
		EmptyWorkingSet ((HANDLE)-1);
	}
	void taskCountCancel ()
	{
		for (char cnt = 0; cnt < 100; cnt ++)
		{
			if (!this->IsHandleCreated) break;
			Thread::Sleep (50);
		}
		if (this->IsHandleCreated) 
		{
			this->Invoke (gcnew MethodInvoker (this, &MainWnd::Close));
		}
	}
	void taskCountDarkMode ()
	{
		for (char cnt = 0; cnt < 8; cnt ++)
		{
			if (!this->IsHandleCreated)
			{
				break;
			}
			else
			{
				if (!this->webUI->IsHandleCreated)
				{
					break;
				}
			}
			Thread::Sleep (50);
		}
		if (this->IsHandleCreated)
		{
			if (this->webUI->IsHandleCreated)
			{
				this->invokeSetDarkMode (IsAppInDarkMode ());
			}
		}
	}
	public:
	[STAThread]
	void Button1_PressEvent () { eventOnPress_button1 (); }
	[STAThread]
	void Button2_PressEvent () { eventOnPress_button2 (); }
	[STAThread]
	void PageBackEvent () { eventOnPress_buttonBack (); }
	[STAThread]
	void WinOptionEvent () { eventOnPress_buttonOption (); }
	[STAThread]
	void PageNextEvent () { eventOnPress_buttonNext (); }
	[STAThread]
	void WinMinEvent () { eventOnPress_buttonMin (); }
	[STAThread]
	void WinMaxEvent () { eventOnPress_buttonMax (); }
	[STAThread]
	void WinCancelEvent () { eventOnPress_buttonCancel (); }
	void funcSetProgress (unsigned value)
	{
		this->invokeSetProgress (value);
		invokeSetProgressText (
			gcnew String (
				StrPrintFormatW (GetRCString_cpp (PAGE_2_INSTALLING).c_str (), (int)value).c_str ()
			)
		);
		this->setTaskbarProgress ((unsigned)value);
	}
	bool launchInstalledApp ()
	{
		if (page != 4) return false;
		else
		{
			if (!reader.isPackageApplication ()) return false;
		}
		this->Button1_PressEvent ();
		return true;
	}
};

typedef struct _CMDARGUMENT
{
	std::vector <std::wstring> front;
	std::vector <std::wstring> body;
	std::vector <std::wstring> rear;
	int flag;
} CMDARGUMENT;

std::vector <CMDARGUMENT> cmdargs =
{
	{{L"", L"/", L"-"}, {L"DISABLEFRAME", L"NOCUSTOM", L"WIN32FRAME", L"ORIGINALBORDER"}, {}, 1},
	{{L"", L"/", L"-"}, {L"SILENT", L"QUIET", L"PASSIVE"}, {}, 2},
	{{L"", L"/", L"-"}, {L"VERYSILENT", L"VERYQUIET"}, {}, 3},
	{{L"", L"/", L"-"}, {L"NOGUI", L"CMD"}, {}, 4},
	{{L"", L"/", L"-"}, {L"HELP", L"?", L"H"}, {}, 5},
	{{L"", L"/", L"-"}, {L"ENABLEFRAME", L"CUSTOM", L"EXFRAME", L"CUSTOMBORDER", L"NEWFRAME"}, {}, 6},
	{{L"", L"/", L"-"}, {L"WIN32WINDOW", L"USEWIN32", L"USEWIN32WINDOW", L"NOWEBUI", L"DISABLEWEBUI"}, {}, 7},
	{{L"", L"/", L"-"}, {L"CREATESHORTCUT", L"SHORTCUT", L"CREATELNK"}, {}, 8},
	{{L"", L"/", L"-"}, {L"DELETESHORTCUT", L"DELSHORTCUT", L"DELETELNK"}, {}, 9},
	{{L"", L"/", L"-"}, {L"DEVTOOL", L"DEVTOOLS", L"DEVMODE", L"DEVELOP"}, {}, 10},
	{{L"", L"/", L"-"}, {L"USEEPROGRAM", L"EPROGRAM", L"ESUPPORT", L"USEE"}, {}, 11}
};

// 编号为大于 0 的数，失败返回非正数
int GetCmdArgSerial (std::wstring cmdparam)
{
#define tolabel(__std_wstring__) StringToUpper (StringTrim (__std_wstring__))
	std::wstring arg = tolabel (cmdparam);
	for (size_t cnt = 0; cnt < cmdargs.size (); cnt ++)
	{
		CMDARGUMENT &tca = cmdargs [cnt];
		std::wstring tempP = L"";
		for (size_t i = 0; i < tca.body.size (); i ++)
		{
			if (tca.rear.size () > 0)
			{
				if (tca.front.size () > 0)
				{
					for (size_t j = 0; j < tca.front.size (); j ++)
					{
						for (size_t k = 0; k < tca.rear.size (); k ++)
						{
							tempP = tolabel (tca.front [j]) + tolabel (tca.body [i]) + tolabel (tca.rear [k]);
							std::wstring t1;
							if (tolabel (tca.rear [k]).length () > 0) t1 = arg.substr (0, tempP.length ());
							else t1 = tempP;
							if (t1 == arg) return tca.flag;
						}
					}
				}
				else
				{
					for (size_t k = 0; k < tca.rear.size (); k ++)
					{
						tempP = tolabel (tca.body [i]) + tolabel (tca.rear [k]);
						std::wstring t1;
						if (tolabel (tca.rear [k]).length () > 0) t1 = arg.substr (0, tempP.length ());
						else t1 = tempP;
						if (t1 == arg) return tca.flag;
					}
				}
			}
			else
			{
				if (tca.front.size () > 0)
				{
					for (size_t j = 0; j < tca.front.size (); j ++)
					{
						tempP = tolabel (tca.front [j]) + tolabel (tca.body [i]);
						std::wstring &t1 = tempP;
						if (t1 == arg) return tca.flag;
					}
				}
				else
				{
					tempP = tolabel (tca.body [i]);
					std::wstring &t1 = tempP;
					if (t1 == arg) return tca.flag;
				}
			}
		}
	}
	return 0; // 返回 0 正好适用于 C/C++ 的逻辑判断
}
int GetCmdArgSerial (LPCWSTR cmdparam) { return GetCmdArgSerial (std::wstring (cmdparam)); }

// 返回真时代表执行 GUI 进程
[MTAThread]
bool ReadCommand (int argc, LPWSTR *argv)
{
	std::vector <std::wstring> vecObjSwFiles;
	bool bWin32Wnd = false, bSilent = false, bVerySilent = false,
		bUseConsole = false, bUseNewFrame = false, bDisplayHelp = false,
		bCreateLnk = false, bDestroyLnk = false, bDevTool = false,
		bUseEProgream = false;
	for (size_t cnt = 0; cnt < (size_t)argc; cnt ++)
	{
		int res = GetCmdArgSerial (argv [cnt]);
		if (res > 0)
		{
			switch (res)
			{
				case 1: bUseNewFrame = false; break;
				case 2: bSilent = true; break;
				case 3: bVerySilent = true; break;
				case 4: bUseConsole = true; break;
				case 5: bDisplayHelp = true; break;
				case 6: bUseNewFrame = true; break;
				case 7: bWin32Wnd = true; break;
				case 8: bCreateLnk = true; break;
				case 9: bDestroyLnk = true; break;
				case 10: bDevTool = true; break;
				case 11: bUseEProgream = true; break;
			}
		}
		else
		{
			if (IsFileExistsW (argv [cnt])) push_no_repeat (vecObjSwFiles, argv [cnt]);
		}
	}
	bWin32Wnd = bWin32Wnd | !m_initConfig.readBoolValue (L"Settings", L"UseWebUI", true);
	bUseEProgream = bUseEProgream | m_initConfig.readBoolValue (L"Settings", L"UseElderWebUI", false);
	bUseNewFrame = bUseNewFrame | m_initConfig.readBoolValue (L"WebUI", L"UseNewFrame", false);
	if (bDisplayHelp) // 处理命令参数提示
	{
		MessageBox::Show (
			rcString (CLHELP_1) +
			rcString (CLHELP_2) +
			rcString (CLHELP_4) +
			rcString (CLHELP_5) +
			rcString (CLHELP_7)
		);
		return false;
	}
	if (bCreateLnk)
	{
		WCHAR expandedPath [MAX_PATH];
		ExpandEnvironmentStringsW (L"%ProgramData%\\Microsoft\\Windows\\Start Menu\\Programs\\Desktop App Installer", expandedPath, MAX_PATH);
		if (IsDirectoryExists (expandedPath) || CreateDirectoryW (expandedPath, NULL))
		{
			WCHAR lnkPath [MAX_PATH];
			PathCombineW (lnkPath, expandedPath, L"App Installer.lnk");
			WCHAR path [MAX_PATH] = {0};
			GetModuleFileNameW (NULL, path, MAX_PATH);
			HRESULT hr = CreateShortcutWithAppIdW (lnkPath, path, m_idenName);
			if (SUCCEEDED (hr))
			{
				WCHAR desktopIni [MAX_PATH];
				PathCombineW (desktopIni, expandedPath, L"desktop.ini");
				WInitFile desktop (desktopIni);
				WCHAR resIdStr [16] = {0};
				_itow (WIN_TITLE, resIdStr, 10);
				desktop.writeUIntValue (L".ShellClassInfo", L"ConfirmFileOp", 0);
				desktop.writeStringValue (L"LocalizedFileNames", L"App Installer.lnk", std::wstring (L"@") + path + L",-" + resIdStr);
				DWORD attrs = GetFileAttributesW (desktopIni);
				SetFileAttributesW (desktopIni, attrs | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);
				DWORD folderAttrs = GetFileAttributesW (expandedPath);
				SetFileAttributesW (expandedPath, folderAttrs | FILE_ATTRIBUTE_SYSTEM);
			}
		}
		return false;
	}
	if (bDestroyLnk)
	{
		WCHAR expandedPath [MAX_PATH];
		if (ExpandEnvironmentStringsW (L"%ProgramData%\\Microsoft\\Windows\\Start Menu\\Programs\\Desktop App Installer", expandedPath, MAX_PATH))
		{
			if (IsDirectoryExists (expandedPath))
			{
				WCHAR lnkPath [MAX_PATH];
				PathCombineW (lnkPath, expandedPath, L"App Installer.lnk");
				DeleteFileW (lnkPath);
			}
		}
		return false;
	}
	m_silentMode = bSilent || bVerySilent; // 处理自动模式
	if (vecObjSwFiles.size () == 1)
	{
		pkgPath = vecObjSwFiles [0];
		if (bVerySilent) // 当静默模式时则只安装不显示 GUI
		{
			LoadCertFromSignedFile (pkgPath.c_str ());
			AddPackageFromPath (pkgPath.c_str (), NULL);
			return false;
		}
		else if (bWin32Wnd)
		{
			std::wstring root = EnsureTrailingSlash (GetProgramRootDirectoryW ());
			std::wstring exepath = root + L"AppInstaller_win32.exe";
			std::wstring args = L"";
			args += L"\"" + pkgPath + L"\" ";
			if (bUseNewFrame) args += L"/ENABLEFRAME ";
			else args += L"/DISABLEFRAME ";
			if (bSilent) args += L"/SILENT ";
			if (bVerySilent) args += L"/VERYSILENT ";
			if (bUseConsole) args += L"/NOGUI ";
			if (bDisplayHelp) args += L"/? ";
			std::wstring cmdline = L"\"" + exepath + L"\" " + args;
			{
				STARTUPINFOW si = {sizeof (STARTUPINFOW)};
				PROCESS_INFORMATION pi = {0};
				if (CreateProcessW (NULL, (LPWSTR)cmdline.c_str (), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
				{
					WaitForSingleObject (pi.hProcess, INFINITE);
					CloseHandle (pi.hProcess);
					CloseHandle (pi.hThread);
					return false;
				}
			}
		}
		else if (bUseEProgream)
		{
			std::wstring root = EnsureTrailingSlash (GetProgramRootDirectoryW ());
			std::wstring exepath = root + L"AppInstallerE.exe";
			std::wstring args = L"";
			args += L"\"" + pkgPath + L"\" ";
			if (bUseNewFrame) args += L"/ENABLEFRAME ";
			else args += L"/DISABLEFRAME ";
			if (bSilent) args += L"/SILENT ";
			if (bVerySilent) args += L"/VERYSILENT ";
			if (bUseConsole) args += L"/NOGUI ";
			if (bDisplayHelp) args += L"/? ";
			std::wstring cmdline = L"\"" + exepath + L"\" " + args;
			{
				STARTUPINFOW si = {sizeof (STARTUPINFOW)};
				PROCESS_INFORMATION pi = {0};
				if (CreateProcessW (NULL, (LPWSTR)cmdline.c_str (), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
				{
					WaitForSingleObject (pi.hProcess, INFINITE);
					CloseHandle (pi.hProcess);
					CloseHandle (pi.hThread);
					return false;
				}
			}
		}
		else if (bUseConsole)
		{
			std::wstring root = EnsureTrailingSlash (GetProgramRootDirectoryW ());
			std::wstring exepath = root + L"WSAppPkgIns.exe";
			std::wstring args = L"";
			args += L"\"" + pkgPath + L"\" ";
			std::wstring cmdline = L"\"" + exepath + L"\" " + args;
			{
				STARTUPINFOW si = {sizeof (STARTUPINFOW)};
				PROCESS_INFORMATION pi = {0};
				if (CreateProcessW (NULL, (LPWSTR)cmdline.c_str (), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
				{
					WaitForSingleObject (pi.hProcess, INFINITE);
					CloseHandle (pi.hProcess);
					CloseHandle (pi.hThread);
					return false;
				}
			}
		}
		return true;
	}
	else if (vecObjSwFiles.size () <= 0)
	{
		if (bWin32Wnd)
		{
			std::wstring root = EnsureTrailingSlash (GetProgramRootDirectoryW ());
			std::wstring exepath = root + L"AppInstaller_win32.exe";
			std::wstring args = L"";
			if (bUseNewFrame) args += L"/ENABLEFRAME ";
			else args += L"/DISABLEFRAME ";
			if (bSilent) args += L"/SILENT ";
			if (bVerySilent) args += L"/VERYSILENT ";
			if (bUseConsole) args += L"/NOGUI ";
			if (bDisplayHelp) args += L"/? ";
			std::wstring cmdline = L"\"" + exepath + L"\" " + args;
			{
				STARTUPINFOW si = {sizeof (STARTUPINFOW)};
				PROCESS_INFORMATION pi = {0};
				if (CreateProcessW (NULL, (LPWSTR)cmdline.c_str (), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
				{
					WaitForSingleObject (pi.hProcess, INFINITE);
					CloseHandle (pi.hProcess);
					CloseHandle (pi.hThread);
					return false;
				}
			}
		}
		else if (bUseEProgream)
		{
			std::wstring root = EnsureTrailingSlash (GetProgramRootDirectoryW ());
			std::wstring exepath = root + L"AppInstallerE.exe";
			std::wstring args = L"";
			if (bUseNewFrame) args += L"/ENABLEFRAME ";
			else args += L"/DISABLEFRAME ";
			if (bSilent) args += L"/SILENT ";
			if (bVerySilent) args += L"/VERYSILENT ";
			if (bUseConsole) args += L"/NOGUI ";
			if (bDisplayHelp) args += L"/? ";
			if (bDevTool) args += L"/DEVTOOL ";
			std::wstring cmdline = L"\"" + exepath + L"\" " + args;
			{
				STARTUPINFOW si = {sizeof (STARTUPINFOW)};
				PROCESS_INFORMATION pi = {0};
				if (CreateProcessW (NULL, (LPWSTR)cmdline.c_str (), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
				{
					WaitForSingleObject (pi.hProcess, INFINITE);
					CloseHandle (pi.hProcess);
					CloseHandle (pi.hThread);
					return false;
				}
			}
		}
		else return true;
	}
	else if (vecObjSwFiles.size () > 1) // 面对多个文件
	{
		for (auto it : vecObjSwFiles)
		{
			wchar_t path [MAX_PATH] = {0};
			if (GetModuleFileNameW (NULL, path, MAX_PATH))
			{
				std::wstring args = L"";
				args += L"\"" + it + L"\" ";
				if (bUseNewFrame) args += L"/ENABLEFRAME ";
				else args += L"/DISABLEFRAME ";
				if (bSilent) args += L"/SILENT ";
				if (bVerySilent) args += L"/VERYSILENT ";
				if (bUseConsole) args += L"/NOGUI ";
				if (bDisplayHelp) args += L"/? ";
				if (bWin32Wnd) args += L"WIN32WINDOW ";
				if (bUseEProgream) args += L"/USEEPROGRAM ";
				std::wstring cmdline = L"\"" + std::wstring (path) + L"\" " + args;
				{
					STARTUPINFOW si = {sizeof (STARTUPINFOW)};
					PROCESS_INFORMATION pi = {0};
					if (CreateProcessW (NULL, (LPWSTR)cmdline.c_str (), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
					{
						WaitForSingleObject (pi.hProcess, INFINITE);
						CloseHandle (pi.hProcess);
						CloseHandle (pi.hThread);
					}
				}
			}
		}
		return false;
	}
	return false;
}

// 返回真时代表执行 GUI 进程
[MTAThread]
bool ReadCommand (LPWSTR lpCmdLine)
{
	int iArgc = 0;
	LPWSTR *lpArgv = CommandLineToArgvW (lpCmdLine, &iArgc);
	return ReadCommand (iArgc, lpArgv);
}

msclr::gcroot <MainWnd ^> mainwndPtr;
void SetProgressDisplayWnd (MainWnd ^ptr)
{
	mainwndPtr = ptr;
}
void ProgressCallback (unsigned progress)
{
	mainwndPtr->funcSetProgress (progress);
}
void ToastPressCallback ()
{
	mainwndPtr->launchInstalledApp ();
}

void OutputDebugStringFormatted (const wchar_t* format, ...)
{
	wchar_t buffer [1024];
	va_list args;
	va_start (args, format);
	vswprintf (buffer, sizeof (buffer) / sizeof (wchar_t), format, args);
	va_end (args);
	OutputDebugString (buffer);
}

[STAThread]
int APIENTRY wWinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	SetCurrentProcessExplicitAppUserModelID (m_idenName);
	wchar_t currentPath [MAX_PATH];
	GetCurrentDirectory (MAX_PATH, currentPath);
	wchar_t executablePath [MAX_PATH];
	GetModuleFileName (NULL, executablePath, MAX_PATH);
	std::wstring executableDir (executablePath);
	executableDir = executableDir.substr (0, executableDir.find_last_of (L"\\/"));
	if (!wcscmp (currentPath, executableDir.c_str ()))
	{
		SetCurrentDirectory (executableDir.c_str ());
	}
	OutputDebugStringFormatted (L"Current Dir: %ls\n", executableDir.c_str ());
	m_initConfig.setFilePath (EnsureTrailingSlash (GetProgramRootDirectoryW ()) + L"Config.ini");
	CoInitializeEx (NULL, COINIT_MULTITHREADED);
	Application::EnableVisualStyles ();
	Application::SetCompatibleTextRenderingDefault (false);
	MainWnd ^mwnd = gcnew MainWnd ();
	SetProgressDisplayWnd (mwnd);
	if (ReadCommand (lpCmdLine))
	{
		Application::Run (mwnd);
	}
	reader.destroy ();
	return 0;
}
#include "stdafx.h"
#include "PackageManager.h"

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Management::Deployment;

std::wstring lastErrorCode (L"");
std::wstring lastErrorDetailText (L"");

[MTAThread]
InstallStatus AddPackageFromPath (LPCWSTR lpswPath, PACKAGE_ADD_PROGRESS callbProgress)
{
	lastErrorCode = L"";
	lastErrorDetailText = L"";
	HANDLE completedEvent = nullptr;
	try
	{
		completedEvent = CreateEventEx (nullptr, nullptr, CREATE_EVENT_MANUAL_RESET, EVENT_ALL_ACCESS);
		if (!completedEvent) return InstallStatus::Error;
		String ^inputPackageUri = ref new String (lpswPath);
		auto packageUri = ref new Uri (inputPackageUri);
		auto packageManager = ref new PackageManager ();
		auto deploymentOperation = packageManager->AddPackageAsync (packageUri, nullptr, DeploymentOptions::None);
		deploymentOperation->Progress =
			ref new AsyncOperationProgressHandler<DeploymentResult^, DeploymentProgress> (
				[callbProgress] (IAsyncOperationWithProgress<DeploymentResult^, DeploymentProgress>^ operation, DeploymentProgress progress)
		{
			if (callbProgress)
				callbProgress (progress.percentage);
		});
		deploymentOperation->Completed =
			ref new AsyncOperationWithProgressCompletedHandler<DeploymentResult^, DeploymentProgress> (
				[&completedEvent] (IAsyncOperationWithProgress<DeploymentResult^, DeploymentProgress>^ operation, AsyncStatus)
		{
			SetEvent (completedEvent);
		});
		WaitForSingleObject (completedEvent, INFINITE);
		if (!completedEvent) 
		{
			CloseHandle (completedEvent); completedEvent = nullptr;
		}
		if (deploymentOperation->Status == AsyncStatus::Error)
		{
			auto deploymentResult = deploymentOperation->GetResults ();
			lastErrorCode += deploymentOperation->ErrorCode.Value;
			lastErrorDetailText += deploymentResult->ErrorText->Data ();
			return InstallStatus::Error;
		}
		else if (deploymentOperation->Status == AsyncStatus::Canceled)
		{
			lastErrorDetailText += L"Installation Canceled";
			return InstallStatus::Canceled;
		}
		else if (deploymentOperation->Status == AsyncStatus::Completed)
		{
			return InstallStatus::Success;
		}
	}
	catch (Exception ^ex)
	{
		lastErrorDetailText += ex->ToString ()->Data ();
		return InstallStatus::Error;
	}
	if (!completedEvent)
	{
		CloseHandle (completedEvent); completedEvent = nullptr;
	}
	return InstallStatus::Canceled;
}

LPCWSTR GetLastErrorCode ()
{
	if (lastErrorCode.length () == 0) return NULL;
	else return lastErrorCode.c_str ();
}

LPCWSTR GetLastErrorDetailText ()
{
	if (lastErrorDetailText.length () == 0) return NULL;
	else return lastErrorDetailText.c_str ();
}

size_t GetLastErrorCodeLength () { return lastErrorCode.length (); }

size_t GetLastErrorDetailTextLength () { return lastErrorDetailText.length (); }
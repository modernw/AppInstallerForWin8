//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#include "stdafx.h"
#include "PackageManager.h"
#include <iostream>
using namespace std;
void callback (unsigned progress)
{
	std::cout << '\r' << progress << "%" << std::ends;
}

[MTAThread]
int __cdecl main(Platform::Array<String^>^ args)
{
	wcout << L"Copyright (c) Microsoft Corporation. All rights reserved." << endl;
	wcout << L"Add Package" << endl << endl;

	if (args->Length < 2)
	{
		wcout << L"Usage: addpkg.exe packageUri" << endl;
		return 1;
	}
	HANDLE completedEvent = nullptr;
	int returnValue = 0;
	String^ inputPackageUri = args[1];
	cout << endl;
	AddPackageFromPath (inputPackageUri->Data (), &callback);
	return 0;
}

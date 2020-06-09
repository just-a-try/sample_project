// CameraPreviewDLL.cpp : Defines the exported functions for the DLL.
//

#include "pch.h"
#include "framework.h"


#ifdef  DEBUG
#define REGISTER_FILTERGRAPH
#endif

#pragma comment(lib, "strmiids")
#include "CameraPreviewDLL.h"


// This is an example of an exported variable
CAMERAPREVIEWDLL_API int nCameraPreviewDLL=0;

// This is an example of an exported function.
CAMERAPREVIEWDLL_API int fnCameraPreviewDLL(void)
{
    return 0;
}

// This is the constructor of a class that has been exported.
CCameraPreviewDLL::CCameraPreviewDLL()
{
    return;
}

HINSTANCE ghInstApp = 0;
HACCEL ghAccel = 0;
HFONT  ghfontApp = 0;
TEXTMETRIC gtm = { 0 };
TCHAR gszAppName[] = TEXT("AMCAP");
HWND ghwndApp = 0, ghwndStatus = 0;
HDEVNOTIFY ghDevNotify = 0;
PUnregisterDeviceNotification gpUnregisterDeviceNotification = 0;
PRegisterDeviceNotification gpRegisterDeviceNotification = 0;
DWORD g_dwGraphRegister = 0;




// Start previewing
//
BOOL StartPreview()
{
	// way ahead of you
	if (gcap.fPreviewing)
		return TRUE;

	if (!gcap.fPreviewGraphBuilt)
		return FALSE;

	// run the graph
	IMediaControl *pMC = NULL;
	HRESULT hr = gcap.pFg->QueryInterface(IID_IMediaControl, (void **)&pMC);
	if (SUCCEEDED(hr))
	{
		hr = pMC->Run();
		if (FAILED(hr))
		{
			// stop parts that ran
			pMC->Stop();
		}
		pMC->Release();
	}
	if (FAILED(hr))
	{
		OutputDebugString(TEXT("Error %x: Cannot run preview graph"));
		return FALSE;
	}

	gcap.fPreviewing = TRUE;
	return TRUE;
}


// stop the preview graph
//
BOOL StopPreview()
{
	// way ahead of you
	if (!gcap.fPreviewing)
	{
		return FALSE;
	}

	// stop the graph
	IMediaControl *pMC = NULL;
	HRESULT hr = gcap.pFg->QueryInterface(IID_IMediaControl, (void **)&pMC);
	if (SUCCEEDED(hr))
	{
		hr = pMC->Stop();
		pMC->Release();
	}
	if (FAILED(hr))
	{
		OutputDebugString(TEXT("Error %x: Cannot stop preview graph"));
		return FALSE;
	}

	gcap.fPreviewing = FALSE;

	// get rid of menu garbage
	InvalidateRect(ghwndApp, NULL, TRUE);

	return TRUE;
}

CAMERAPREVIEWDLL_API HRESULT EnumerateDevices(REFGUID category, IEnumMoniker **ppEnum)
{
	// Create the System Device Enumerator.
	ICreateDevEnum *pDevEnum;
	HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL,
		CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pDevEnum));

	if (SUCCEEDED(hr))
	{
		// Create an enumerator for the category.
		hr = pDevEnum->CreateClassEnumerator(category, ppEnum, 0);
		if (hr == S_FALSE)
		{
			hr = VFW_E_NOT_FOUND;  // The category is empty. Treat as an error.
		}
		pDevEnum->Release();
	}
	return hr;
}

CAMERAPREVIEWDLL_API BOOL DisplayDeviceInformation(IEnumMoniker *pEnum, HWND hwnd)
{
	IMoniker *pMoniker = NULL;
	HMENU hmenu = GetMenu(hwnd);
	HMENU hSubMenu = GetSubMenu(hmenu, 1);
	BOOL ret = TRUE;
	int menu_count = GetMenuItemCount(hSubMenu);

	if (menu_count < 0)
	{
		OutputDebugString(L"GetMenuItemCount API failed");
		return FALSE;
	}

	if (menu_count > 1)
	{
		for (int flag1 = 0; flag1 < menu_count - 1; flag1++)
		{
			if (!DeleteMenu(hSubMenu, 1, MF_BYPOSITION))
			{
				OutputDebugString(L"DeleteMenu API failed");
				return FALSE;
			}
		}
	}

	while (pEnum->Next(1, &pMoniker, NULL) == S_OK)
	{
		IPropertyBag *pPropBag;
		HRESULT hr = pMoniker->BindToStorage(0, 0, IID_PPV_ARGS(&pPropBag));

		if (FAILED(hr))
		{
			pMoniker->Release();
			continue;
		}

		VARIANT var;
		VariantInit(&var);

		hr = pPropBag->Read(L"Description", &var, 0);
		if (FAILED(hr))
		{
			hr = pPropBag->Read(L"FriendlyName", &var, 0);
		}
		if (SUCCEEDED(hr))
		{
			VariantClear(&var);
		}

		if (!AppendMenu(hSubMenu, MF_STRING, TRUE, var.bstrVal))
		{
			OutputDebugString(L"AppendMenu function failed");
			ret = FALSE;
		}

		pPropBag->Release();
		pMoniker->Release();
	}
	return ret;
}

CAMERAPREVIEWDLL_API BOOL EnumarateCamera(HWND hwnd)
{
	HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	BOOL ret = TRUE;
	if (SUCCEEDED(hr))
	{
		IEnumMoniker *pEnum;

		hr = EnumerateDevices(CLSID_VideoInputDeviceCategory, &pEnum);

		if (SUCCEEDED(hr))
		{
			if (!DisplayDeviceInformation(pEnum, hwnd))
			{
				OutputDebugString(L"DisplayDeviceInformation failed\n");
				ret = FALSE;
			}
			pEnum->Release();
			CoUninitialize();
		}
		else
		{
			OutputDebugString(L"EnumerateDevices failed\n");
			CoUninitialize();
			ret = FALSE;
		}
	}
	else
	{
		OutputDebugString(L"CoInitializeEx failed\n");
		ret = FALSE;
	}

	return ret;
}
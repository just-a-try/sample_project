// ListUSBDevices.cpp : Defines the entry point for the application.
//


#include "framework.h"
#include "ListUSBDevices.h"
#include "Resource.h"
#include <dshow.h>
#include <windows.h>
#include <tchar.h>
#include <setupapi.h>
#include <initguid.h>
#include <PortableDeviceApi.h>
#include <PortableDeviceTypes.h>
#include <stdlib.h>
#include <new>
using namespace std;
#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name


// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
BOOL addmenu(HWND);
PWSTR DisplayFriendlyName(
	_In_ IPortableDeviceManager* deviceManager,
	_In_ PCWSTR                  pnpDeviceID);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.
    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_LISTUSBDEVICES, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LISTUSBDEVICES));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LISTUSBDEVICES));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_LISTUSBDEVICES);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);

   if (!addmenu(hWnd))
   {
	   OutputDebugString(_T("Error in addmenu function"));
   }

   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case ID_FILE_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
	case WM_DEVICECHANGE:
	{
		if (!addmenu(hWnd))
		{
			OutputDebugString(_T("Error in addmenu function"));
		}
		break;
	}
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

BOOL addmenu(HWND hWnd)
{
	HMENU hmenu = GetMenu(hWnd);
	HMENU hSubMenu = GetSubMenu(hmenu, 2);
	UINT nDevices = 0;
	DWORD cPnPDeviceCount;
	IPortableDeviceManager*  pPortableDeviceManager;
	DWORD dwIndex = 0;
	PWSTR friendlyname;
	int menu_count = GetMenuItemCount(hSubMenu);

	if (menu_count < 0)
	{
		OutputDebugString(_T("GetMenuItemCount API failed"));
		return FALSE;
	}

	if (menu_count > 1)
	{
		for (int flag1 = 0; flag1 < menu_count - 1; flag1++)
		{
			if (!DeleteMenu(hSubMenu, 1, MF_BYPOSITION))
			{
				OutputDebugString(_T("DeleteMenu API failed"));
				return FALSE;
			}
		}
	}

	HRESULT hr = CoCreateInstance(CLSID_PortableDeviceManager,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&pPortableDeviceManager));

	if (FAILED(hr))
	{
		OutputDebugString(_T("Failed to CoCreateInstance CLSID_PortableDeviceManager"));
		return FALSE;
	}

	if (SUCCEEDED(hr))
	{
		hr = pPortableDeviceManager->GetDevices(NULL, &cPnPDeviceCount);
		if (FAILED(hr))
		{
			OutputDebugString(_T("Failed to get number of devices on the system"));
			return FALSE;
		}
	}

	if (SUCCEEDED(hr) && (cPnPDeviceCount > 0))
	{
		LPWSTR* pPnpDeviceIDs = new (std::nothrow) PWSTR[cPnPDeviceCount];
		if (pPnpDeviceIDs != NULL)
		{
			hr = pPortableDeviceManager->GetDevices(pPnpDeviceIDs, &cPnPDeviceCount);
			if (SUCCEEDED(hr))
			{
				for (dwIndex = 0; dwIndex < cPnPDeviceCount; dwIndex++)
				{
					friendlyname = DisplayFriendlyName(pPortableDeviceManager, pPnpDeviceIDs[dwIndex]);
					if (friendlyname)
					{
						if (!AppendMenu(hSubMenu, MF_STRING, TRUE, friendlyname))
						{
							OutputDebugString(_T("AppendMenu function failed"));
							return FALSE;
						}
					}
					else
					{
						OutputDebugString(_T("DisplayFriendlyName function failed"));
						return FALSE;
					}
				}
			}
			else
			{
				OutputDebugString(_T("Failed to get the device list from the system"));
				return FALSE;
			}
			
			for (DWORD index = 0; index < cPnPDeviceCount; index++)
			{
				CoTaskMemFree(pPnpDeviceIDs[index]);
				pPnpDeviceIDs[index] = nullptr;
			}

			delete[] pPnpDeviceIDs;
		}
	}
	return TRUE;
}

PWSTR DisplayFriendlyName(
	_In_ IPortableDeviceManager* deviceManager,
	_In_ PCWSTR                  pnpDeviceID)
{
	DWORD friendlyNameLength = 0;

	HRESULT hr = deviceManager->GetDeviceFriendlyName(pnpDeviceID, nullptr, &friendlyNameLength);
	if (FAILED(hr))
	{
		OutputDebugString(_T("Failed to get number of characters for device friendly name"));
		return NULL;
	}
	else if (friendlyNameLength > 0)
	{
		PWSTR friendlyName = new (std::nothrow) WCHAR[friendlyNameLength];
		if (friendlyName != nullptr)
		{
			ZeroMemory(friendlyName, friendlyNameLength * sizeof(WCHAR));
			hr = deviceManager->GetDeviceFriendlyName(pnpDeviceID, friendlyName, &friendlyNameLength);
			if (SUCCEEDED(hr))
			{
				return friendlyName;
			}
			else
			{
				OutputDebugString(_T("GetDeviceFriendlyName function failed"));
				return NULL;
			}

			delete[] friendlyName;
			friendlyName = nullptr;
		}
		else
		{
			OutputDebugString(_T("Failed to allocate memory for the device friendly name string"));
			return NULL;
		}
	}
	else
	{
		OutputDebugString(_T("The device did not provide a friendly name"));
		return NULL;
	}
}
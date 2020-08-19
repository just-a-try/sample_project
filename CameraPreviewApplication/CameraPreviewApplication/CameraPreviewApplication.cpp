// CameraPreviewApplication.cpp : Defines the entry point for the application.
//
#pragma warning(disable : 4996)
typedef struct IUnknown IUnknown;
#include "framework.h"
#include "CameraPreviewApplication.h"
#include "Resource.h"
#include <dshow.h>
#include <dbt.h>
#include <mmreg.h>
#include <msacm.h>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <commdlg.h>
#include <strsafe.h>
#include "Commctrl.h"
#include <dshowutil.h>
#include<iostream>
#include<vector>
using namespace std;
#pragma comment(lib, "strmiids")
#include "CameraPreviewDLL.h"


#define MAX_LOADSTRING 100

enum UVC_settings_mode
{
	AUTO = 1,
	MANUAL
};


// make sure the preview window inside our window is as big as the
// dimensions of captured video, or some capture cards won't show a preview.
// (Also, it helps people tell what size video they're capturing)
// We will resize our app's window big enough so that once the status bar
// is positioned at the bottom there will be enough room for the preview
// window to be w x h
//
int gnRecurse = 0;
// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
HWND hwnd;
// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    Resolution_and_Format(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    UVC_Settings(HWND, UINT, WPARAM, LPARAM);
bool Set_text(HWND hdlg, long value, int property);
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
    LoadStringW(hInstance, IDC_CAMERAPREVIEWAPPLICATION, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CAMERAPREVIEWAPPLICATION));

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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CAMERAPREVIEWAPPLICATION));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_CAMERAPREVIEWAPPLICATION);
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

   hwnd = hWnd;

   if (!hWnd)
   {
      return FALSE;
   }

   if (!EnumarateCamera(hwnd))
   {
	   OutputDebugString(L"EnumarateCamera failed\n");
	   return FALSE;
   }

   InitCapFilters(hWnd);
   ShowWindow(hWnd, nCmdShow);
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
	double Width, Height;
    switch (message)
    {
	case WM_SIZE:
		Width = LOWORD(lParam);
		Height = HIWORD(lParam);
		Resize(hwnd, Width, Height);
		break;
	case WM_MOUSEWHEEL:
		zoom_in_and_out(wParam);
		break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
			case ID_CAPTURE_RESOLUTION:
				DialogBox(hInst, MAKEINTRESOURCE(IDD_FORMAT_RESOLUTION), hWnd, Resolution_and_Format);
				break;
			case ID_OPTIONS_UVCSETTINGS:
				DialogBox(hInst, MAKEINTRESOURCE(ID_UVC_DIALOG), hWnd, UVC_Settings);
				break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
			case ID_PREVIEW_STOP:
				StopPreview();
				break;
			case ID_PREVIEW_START:
				StartPreview();
				break;
			case ID_STILL_PIN_CAPTURE:
				capture_from_still_pin();
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
	case WM_DEVICECHANGE:
	{
		if (!EnumarateCamera(hwnd))
		{
			OutputDebugString(L"EnumarateCamera failed\n");
		}
		break;
	}
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

INT_PTR CALLBACK Resolution_and_Format(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	vector <string> formats;
	SIZE max, min;
	HWND hwndGroupsBox = GetDlgItem(hDlg, ID_COMBO_FORMAT);
	int format_count;
	LPCTSTR wide_string; //define an array with size of my_str + 1
	
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		format_resolution_enum(formats, max, min);
		format_count = formats.size();

		for (int index = 0; index < format_count; index++)
		{
			wide_string = (LPCTSTR)formats[index].c_str();
			ComboBox_AddString(hwndGroupsBox, formats[index].c_str());
		}
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

INT_PTR CALLBACK UVC_Settings(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	long brightness, contrast, white_balance, exposure, value;
	HWND track_bar;
	int property, mode = MANUAL;
	bool whitebalance_checked, exposure_checked;

	if (!Get_UVC_values(&brightness, &contrast, &white_balance))
	{
		OutputDebugString(L"Get_UVC_values api failed");
	}
	
	UNREFERENCED_PARAMETER(lParam);

	switch (message)
	{
	case WM_INITDIALOG:

		track_bar = GetDlgItem(hDlg, IDC_SLIDER1);
		SendMessage(track_bar, TBM_SETPOS, TRUE, brightness);
		Set_text(hDlg, brightness, VideoProcAmp_Brightness);

		track_bar = GetDlgItem(hDlg, IDC_SLIDER2);
		SendMessage(track_bar, TBM_SETPOS, TRUE, contrast);
		Set_text(hDlg, contrast, VideoProcAmp_Contrast);

		track_bar = GetDlgItem(hDlg, IDC_SLIDER3);
		//SendMessage(track_bar, TBM_SETRANGE, TRUE, MAKELONG(2500, 6500));
		SendMessage(track_bar, TBM_SETPOS, TRUE, white_balance);
		Set_text(hDlg, white_balance, VideoProcAmp_WhiteBalance);

		return (INT_PTR)TRUE;

	case WM_HSCROLL:

		if ((track_bar = GetDlgItem(hDlg, IDC_SLIDER1)) == (HWND)lParam)
		{
			value = SendMessage(track_bar, TBM_GETPOS, 0, 0);
			Set_text(hDlg, value, VideoProcAmp_Brightness);
			property = VideoProcAmp_Brightness;
			UVC_Settings_config(property, mode, value);
		}
		else if ((track_bar = GetDlgItem(hDlg, IDC_SLIDER2)) == (HWND)lParam)
		{
			value = SendMessage(track_bar, TBM_GETPOS, 0, 0);
			Set_text(hDlg, value, VideoProcAmp_Contrast);
			property = VideoProcAmp_Contrast;
			UVC_Settings_config(property, mode, value);
		}
		else if ((track_bar = GetDlgItem(hDlg, IDC_SLIDER3)) == (HWND)lParam)
		{
			value = SendMessage(track_bar, TBM_GETPOS, 0, 0);
			Set_text(hDlg, value, VideoProcAmp_WhiteBalance);
			property = VideoProcAmp_WhiteBalance;
			

			UVC_Settings_config(property, mode, value);
		}
		
		break;
	case WM_COMMAND:

		whitebalance_checked = IsDlgButtonChecked(hDlg, ID_WHITEBALANCE_AUTO);
		track_bar = GetDlgItem(hDlg, IDC_SLIDER3);
		if (whitebalance_checked)
		{
			mode = AUTO;
			EnableWindow(track_bar, FALSE);
		}
		else
		{
			EnableWindow(track_bar, TRUE);
		}
		UVC_Settings_config(VideoProcAmp_WhiteBalance, mode, NULL);
		/*if (LOWORD(lParam) == IDC_SLIDER1)
		{
			MessageBox(NULL, L"IDC_SLIDER1", L"IDC_SLIDER1", MB_OK);
			track_bar = GetDlgItem(hDlg, IDC_SLIDER1);
			value = SendMessage(track_bar, TBM_GETPOS, 0, 0);
			property = VideoProcAmp_Brightness;
		}
		if (LOWORD(lParam) == IDC_SLIDER2)
		{
			MessageBox(NULL, L"IDC_SLIDER2", L"IDC_SLIDER2", MB_OK);
			track_bar = GetDlgItem(hDlg, IDC_SLIDER2);
			value = SendMessage(track_bar, TBM_GETPOS, 0, 0);
			property = VideoProcAmp_Contrast;
		}
		if (LOWORD(lParam) == IDC_SLIDER3)
		{
			MessageBox(NULL, L"IDC_SLIDER3", L"IDC_SLIDER3", MB_OK);
			track_bar = GetDlgItem(hDlg, IDC_SLIDER3);
			value = SendMessage(track_bar, TBM_GETPOS, 0, 0);
			property = VideoProcAmp_WhiteBalance;
			whitebalance_checked = IsDlgButtonChecked(hDlg, ID_WHITEBALANCE_AUTO);

			if (whitebalance_checked)
			{
				mode = AUTO;
			}
		}*/
		
		//Set_text(hDlg, brightness, contrast, white_balance, exposure);

		if (LOWORD(wParam) == IDOK)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		else if (LOWORD(wParam) == IDCANCEL)
		{
			UVC_Settings_config(VideoProcAmp_Brightness, mode, brightness);
			UVC_Settings_config(VideoProcAmp_Contrast, mode, contrast);
			UVC_Settings_config(VideoProcAmp_WhiteBalance, mode, white_balance);

			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

bool Set_text(HWND hdlg, long value, int property)
{
	HWND handle;
	char temp_str[100];
	const char *value_str;

	if (property == VideoProcAmp_Brightness)
	{
		handle = GetDlgItem(hdlg, ID_VALUE_BRIGHTNESS);
		value_str = itoa(value, temp_str, 10);

		if (handle)
		{
			SetWindowTextA(handle, value_str);
		}
	}
	else if (property == VideoProcAmp_Contrast)
	{
		handle = GetDlgItem(hdlg, ID_CONTRAST_VALUE);
		value_str = itoa(value, temp_str, 10);

		if (handle)
		{
			SetWindowTextA(handle, value_str);
		}
	}
	else if (property == VideoProcAmp_WhiteBalance)
	{
		handle = GetDlgItem(hdlg, ID_WHITEBALANCE_VALUE);
		value_str = itoa(value, temp_str, 10);

		if (handle)
		{
			SetWindowTextA(handle, value_str);
		}
	}

	/*handle = GetDlgItem(hdlg, ID_VALUE_BRIGHTNESS);
	value_str = itoa(brightness, temp_str, 10);

	if (handle)
	{
		SetWindowTextA(handle, value_str);
	}

	handle = GetDlgItem(hdlg, ID_CONTRAST_VALUE);
	value_str = itoa(contrast, temp_str, 10);

	if (handle)
	{
		SetWindowTextA(handle, value_str);
	}

	handle = GetDlgItem(hdlg, ID_WHITEBALANCE_VALUE);
	value_str = itoa(white_balance, temp_str, 10);

	if (handle)
	{
		SetWindowTextA(handle, value_str);
	}

	handle = GetDlgItem(hdlg, ID_EXPOSURE_VALUE);
	value_str = itoa(exposure, temp_str, 10);

	if (handle)
	{
		SetWindowTextA(handle, value_str);
	}*/

	return TRUE;
}
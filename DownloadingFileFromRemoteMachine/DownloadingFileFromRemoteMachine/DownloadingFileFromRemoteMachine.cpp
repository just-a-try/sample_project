// DownloadingFileFromRemoteMachine.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "DownloadingFileFromRemoteMachine.h"
#include "Resource.h"
#include <commctrl.h>
#include <commdlg.h>
#include "shlwapi.h"
#include <process.h>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
HWND g_hdlg;
BOOL g_to_download = TRUE;
WCHAR g_curfile[MAX_PATH], g_savefile[MAX_PATH];
HANDLE g_hthread;
// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    Download(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    Status(HWND, UINT, WPARAM, LPARAM);

void Copyfile(void* data);

DWORD CALLBACK CopyProgressRoutine(
	LARGE_INTEGER TotalFileSize,
	LARGE_INTEGER TotalBytesTransferred,
	LARGE_INTEGER StreamSize,
	LARGE_INTEGER StreamBytesTransferred,
	DWORD dwStreamNumber,
	DWORD dwCallbackReason,
	HANDLE hSourceFile,
	HANDLE hDestinationFile,
	LPVOID lpData);
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
    LoadStringW(hInstance, IDC_DOWNLOADINGFILEFROMREMOTEMACHINE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DOWNLOADINGFILEFROMREMOTEMACHINE));

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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DOWNLOADINGFILEFROMREMOTEMACHINE));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_DOWNLOADINGFILEFROMREMOTEMACHINE);
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
   DialogBox(hInst, MAKEINTRESOURCE(ID_DOWNLOAD_DIALOG), hWnd, Download);
   
   DestroyWindow(hWnd);
  /* ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);*/

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
            case IDM_EXIT:
                DestroyWindow(hWnd);
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
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}


// Callback function for handling progress notification.
DWORD CALLBACK CopyProgressRoutine(
	LARGE_INTEGER TotalFileSize,
	LARGE_INTEGER TotalBytesTransferred,
	LARGE_INTEGER StreamSize,
	LARGE_INTEGER StreamBytesTransferred,
	DWORD dwStreamNumber,
	DWORD dwCallbackReason,
	HANDLE hSourceFile,
	HANDLE hDestinationFile,
	LPVOID lpData)
{
	double Percentage = (double(TotalBytesTransferred.QuadPart) /
		double(TotalFileSize.QuadPart)) * 100;
	
	SendMessage(GetDlgItem(g_hdlg, IDC_PROGRESS1), PBM_SETPOS, (WPARAM)Percentage, 0);

	return PROGRESS_CONTINUE;
}

/* @author   : Subhash
   @function : to copy the file from source to destionation path selelected in UI
   @param    : void type data
   @return   : returns nothing
*/
void Copyfile(void* data)
{
		if (!SetWindowText(GetDlgItem(g_hdlg, IDC_EDIT1), L"Downloading..."))
		{
			OutputDebugString(L" SetWindowText api for copy path failed\n");
		}

		PWSTR save_filename = StrCatW(g_savefile, PathFindExtensionW(g_curfile));
		BOOL copyfile = CopyFileEx(g_curfile,
			save_filename,
			(LPPROGRESS_ROUTINE)&CopyProgressRoutine,
			NULL,
			FALSE,
			COPY_FILE_ALLOW_DECRYPTED_DESTINATION);

		if (!copyfile)
		{
			OutputDebugString(L"CopyFileEx Api failed\n");
			return;
		}

		if (!SetWindowText(GetDlgItem(g_hdlg, IDC_EDIT1), L"Download completed"))
		{
			OutputDebugString(L" SetWindowText api for copy path failed\n");
		}

		g_to_download = TRUE;
	return;
}

// Message handler for Status box.
INT_PTR CALLBACK Status(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

// Message handler for Download dialog box.
INT_PTR CALLBACK Download(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		g_hdlg = hDlg;
		ShowWindow(GetDlgItem(hDlg, IDC_PROGRESS1), SW_HIDE);
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		HWND hcopy_path = GetDlgItem(hDlg, ID_COPY_PATH);
		HWND hsave_path = GetDlgItem(hDlg, ID_SAVE_PATH);
		if (LOWORD(wParam) == ID_BROWSE_COPY_BUTTON)
		{
			if (!SetWindowText(GetDlgItem(hDlg, IDC_EDIT1), L""))
			{
				OutputDebugString(L" SetWindowText api for copy path failed\n");
			}

			ShowWindow(GetDlgItem(hDlg, IDC_PROGRESS1), SW_HIDE);

			g_curfile[0] = NULL;
			OPENFILENAME ofn;
			ZeroMemory(&ofn, sizeof(OPENFILENAME));
			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.hwndOwner = hDlg;
			ofn.lpstrFilter = L"All Files(*.*)\0*.*\0";
			ofn.lpstrFile = g_curfile;
			ofn.nMaxFile = MAX_PATH;
			ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
			ofn.lpstrDefExt = L"";

			if (!GetOpenFileName(&ofn))
			{
				OutputDebugString(L"GetOpenFileName Api failed\n");
				return (INT_PTR)FALSE;
			}

			if (!SetWindowText(hcopy_path, g_curfile))
			{
				OutputDebugString(L" SetWindowText api for copy path failed\n");
			}
		}
		else if (LOWORD(wParam) == ID_BROWSE_SAVE_BUTTON)
		{
			if (!SetWindowText(GetDlgItem(hDlg, IDC_EDIT1), L""))
			{
				OutputDebugString(L" SetWindowText api for copy path failed\n");
			}

			ShowWindow(GetDlgItem(hDlg, IDC_PROGRESS1), SW_HIDE);

			OPENFILENAME sfn;
			ZeroMemory(&sfn, sizeof(OPENFILENAME));
			sfn.lStructSize = sizeof(OPENFILENAME);
			g_savefile[0] = NULL;
			sfn.hwndOwner = hDlg;
			sfn.lpstrFilter = L"All Files(*.*)\0*.*\0";
			sfn.lpstrFile = g_savefile;
			sfn.nMaxFile = MAX_PATH;
			sfn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
			sfn.lpstrDefExt = L"";

			if (!GetSaveFileName(&sfn))
			{
				OutputDebugString(L"GetSaveFileName function failed\n");
				return FALSE;
			}

			if (!SetWindowText(hsave_path, g_savefile))
			{
				OutputDebugString(L" SetWindowText api for copy path failed\n");
			}
		}
		else if (LOWORD(wParam) == ID_DOWNLOAD_BUTTON)
		{
			if (g_to_download)
			{
				int copy_path_length = GetWindowTextLength(hcopy_path);
				int save_path_length = GetWindowTextLength(hsave_path);

				if (copy_path_length == 0 || save_path_length == 0)
				{
					MessageBox(hDlg, L"Download needs both path", L"Error", MB_OK);
				}
				else
				{
					int max_range = 100;
					g_to_download = FALSE;
					SendMessage(GetDlgItem(hDlg, IDC_PROGRESS1), PBM_SETRANGE, 0, MAKELPARAM(0, max_range));
					ShowWindow(GetDlgItem(hDlg, IDC_PROGRESS1), SW_NORMAL);
					HANDLE g_hthread = (HANDLE)_beginthread(&Copyfile, 0, 0);

					if (!g_hthread)
					{
						OutputDebugString(L" _beginthread returned NULL\n");
					}

				}
			}
			else
			{
				DialogBox(hInst, MAKEINTRESOURCE(ID_STATUS_DIALOG), hDlg, Status);
			}
		}
		else if (LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}


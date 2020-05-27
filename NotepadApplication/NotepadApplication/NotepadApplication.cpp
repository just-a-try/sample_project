// NotepadApplication.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "NotepadApplication.h"
#include "Resource.h"
#include <commctrl.h>
#include <commdlg.h>
#include <shobjidl.h>


#define MAX_LOADSTRING 100
#define ID_EDITCHILD 100

HWND g_hwnd;
HWND g_hEdit;
WCHAR curfile[MAX_PATH];
BOOL g_is_file_opened = FALSE;
BOOL g_need_to_save_file = FALSE;
// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
BOOL LoadFileToEdit();
BOOL SaveTextFileFromEdit();
BOOL GetFileNameForSave();
BOOL check_for_filesave();

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
    LoadStringW(hInstance, IDC_NOTEPADAPPLICATION, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_NOTEPADAPPLICATION));

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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_NOTEPADAPPLICATION));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_NOTEPADAPPLICATION);
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
   g_hwnd = hWnd;
   ShowWindow(hWnd, nCmdShow);
   if (!SetWindowText(g_hwnd, L"Notepad [Untitled]"))
   {
	   OutputDebugString(L"Initial SetWindowText api for window failed\n");
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
	LPWSTR str = NULL;
	int Width, Height;
    switch (message)
    {
	case WM_CREATE:
	{
		HWND hEdit;
		hEdit = CreateWindowEx(
			WS_EX_CLIENTEDGE,                  
			L"EDIT",        
			L"",       
			WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL, 
			0,       
			0,      
			1010,                
			465,                 
			hWnd,        
			(HMENU)ID_EDITCHILD,
			GetModuleHandle(NULL),      
			NULL                
		);
		if (hEdit == NULL)
		{
			MessageBox(g_hwnd, L"Could not Create Edit control!!", L"Error", MB_OK | MB_ICONERROR);
			PostQuitMessage(0);
		}

		g_hEdit = hEdit;

		if (!SetWindowText(g_hEdit, L""))
		{
			OutputDebugString(L"Initial SetWindowText api for editcontrol failed\n");
		}
	}

	break;
	case WM_SIZE:
	{
		Width = LOWORD(lParam); 
		Height = HIWORD(lParam); 

		if (!MoveWindow(g_hEdit, 0, 0, Width, Height, 1))
		{
			OutputDebugString(L" MoveWindow api for editcontrol failed\n");
		}
	}
		break;
	case WM_CLOSE:
	{
		if (g_need_to_save_file)
		{
			if (g_is_file_opened)
			{
				int res;
				res = MessageBox(g_hwnd, L"The File has been changed!!!\nDo you want to save it before exit?", L"Save File before Exit!!", MB_YESNO | MB_ICONINFORMATION);
				if (res == IDYES)
				{
					if (!SaveTextFileFromEdit())
					{
						OutputDebugString(L"SaveTextFileFromEdit function failed in switch case\n");
					}
				}
			}
			else
			{
				int res;
				res = MessageBox(g_hwnd, L"The File has been changed!!!\nDo you want to save it before exit?", L"Save File before Exit!!", MB_YESNO | MB_ICONINFORMATION);
				if (res == IDYES)
				{
					if (GetFileNameForSave())
					{
						if (!SaveTextFileFromEdit())
						{
							OutputDebugString(L"SaveTextFileFromEdit function failed in switch case\n");
						}
						g_need_to_save_file = FALSE;
						return 0;
					}
					else
					{
						OutputDebugString(L"GetFileNameForSave function failed in switch case\n");
					}
				}
			}
		}
		if (MessageBox(g_hwnd, L"Are you sure you want to exit!!!", L"Sure Exit?", MB_YESNO | MB_ICONQUESTION) == IDNO)
			return 0;
		PostQuitMessage(0);
	}

	break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);

			if (HIWORD(wParam) == EN_UPDATE || HIWORD(wParam) == EN_CHANGE)
			{
				g_need_to_save_file = TRUE;
			}

            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
			case ID_FILE_SAVEAS:
			{
				if (GetFileNameForSave())
				{
					if (!SaveTextFileFromEdit())
					{
						OutputDebugString(L"SaveTextFileFromEdit function failed in switch case\n");
					}
				}
				else
				{
					OutputDebugString(L"GetFileNameForSave function failed in switch case\n");
				}
			}
			break;
			case ID_FILE_OPEN:
				if (!check_for_filesave())
				{
					OutputDebugString(L"check_for_filesave function failed in switch case\n");
				}
				
				if (!LoadFileToEdit())
				{
					OutputDebugString(L"check_for_filesave function failed in switch case\n");
				}
				break;
			case ID_FILE_NEW:
			{
				if (!check_for_filesave())
				{
					OutputDebugString(L"check_for_filesave function failed in switch case\n");
				}

				g_is_file_opened = FALSE;
				g_need_to_save_file = FALSE;

				if (!SetWindowText(g_hEdit, L""))
				{
					OutputDebugString(L"Initial SetWindowText api for editcontrol failed\n");
				}
				
				if (!SetWindowText(g_hwnd, L"Notepad [Untitled]"))
				{
					OutputDebugString(L"Initial SetWindowText api for editcontrol failed\n");
				}
			}
			break;
			case ID_FILE_SAVE:
			{
				if (g_is_file_opened)
				{
					if (!SaveTextFileFromEdit())
					{
						OutputDebugString(L"SaveTextFileFromEdit function failed in switch case\n");
					}
				}
				else if (GetFileNameForSave())
				{
					if (!SaveTextFileFromEdit())
					{
						OutputDebugString(L"SaveTextFileFromEdit function failed in switch case\n");
					}
				}
			}
			break;
            case ID_FILE_EXIT:
				PostMessage(hWnd, WM_CLOSE, 0, 0);
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

/* @author   : Subhash
   @function : to load the content of the opened file to the application's editbox
   @return   : return true if function succeed or false if failed
*/
BOOL LoadFileToEdit()
{
	curfile[0] = '\0';
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = g_hwnd;
	ofn.lpstrFilter = L"Text Files(*.txt)\0*.txt\0All File(*.*)\0*.*\0";
	ofn.lpstrFile = curfile;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	ofn.lpstrDefExt = L"txt";
	if (!GetOpenFileName(&ofn))
		return FALSE;
	HANDLE hFile;
	bool bsucces = FALSE;
	hFile = CreateFile(curfile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		DWORD dwFileSize;
		dwFileSize = GetFileSize(hFile, NULL);
		if (dwFileSize != 0xFFFFFFFF)
		{
			LPSTR tempftext;
			tempftext = (char*)GlobalAlloc(GPTR, dwFileSize + 1);
			if (tempftext != NULL)
			{
				DWORD dwRead;
				if (ReadFile(hFile, tempftext, dwFileSize, &dwRead, NULL))
				{
					tempftext[dwFileSize] = 0;
					if (SetWindowTextA(g_hEdit, tempftext))
						bsucces = true;
				}
				GlobalFree(tempftext);
			}
			else
			{
				OutputDebugString(L"Memory not alllocated\n");
			}
		}
		CloseHandle(hFile);
	}
	if (!bsucces)
	{
		MessageBox(g_hwnd, L"The File could not be loaded!!", L"Error", MB_OK | MB_ICONERROR);
		return FALSE;
	}
	
	if (!SetWindowText(g_hwnd, curfile))
	{
		OutputDebugString(L"SetWindowText api for editcontrol during loadfiletoedit function failed\n");
	}
	g_need_to_save_file = FALSE;
	g_is_file_opened = TRUE;

	return TRUE;
}

/* @author   : Subhash
   @function : to load the content of the application's editbox to the file to be saved
   @return   : return true if function succeed or false if failed
*/
BOOL SaveTextFileFromEdit()
{
	HANDLE hFile;
	bool bsucces = FALSE;
	hFile = CreateFile(curfile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		DWORD dwTextLength;
		dwTextLength = GetWindowTextLength(g_hEdit);
		if (dwTextLength > 0)
		{
			LPSTR pszText;
			DWORD dwBufferSize = dwTextLength + 1;
			pszText = (char*)GlobalAlloc(GPTR, dwBufferSize);
			if (pszText != NULL)
			{
				if (GetWindowTextA(g_hEdit, pszText, dwBufferSize))
				{
					DWORD dwWritten;
					if (WriteFile(hFile, pszText, dwTextLength, &dwWritten, NULL))
						bsucces = TRUE;
					else
					{
						OutputDebugString(L"WriteFile api failed in SaveTextFileFromEdit function\n");
						return FALSE;
					}
				}
				else
				{
					OutputDebugString(L"GetWindowTextA api failed in SaveTextFileFromEdit function\n");
					return FALSE;
				}
				GlobalFree(pszText);
			}
			CloseHandle(hFile);
		}
		else if(dwTextLength == 0)
		{
			bsucces = TRUE;
			CloseHandle(hFile);
		}
		else
		{
			OutputDebugString(L"GetWindowTextLength api failed in SaveTextFileFromEdit function\n");
			return FALSE;
		}
	}
	else
	{
		OutputDebugString(L"Createfile api failed in SaveTextFileFromEdit function\n");
		return FALSE;
	}

	if (!bsucces)
	{
		MessageBox(g_hwnd, L"The File could not be saved!!!", L"Error", MB_OK | MB_ICONERROR);
		return FALSE;
	}

	if (!SetWindowText(g_hwnd, curfile))
	{
		OutputDebugString(L"SetWindowText api for editcontrol during SaveTextTofileFromEdit function failed\n");
	}

	g_is_file_opened = TRUE;
	g_need_to_save_file = FALSE;

	return TRUE;
}

/* @author   : Subhash
   @function : to get the filename for saving the file which open the SaveFileDialog
   @return   : return true if function succeed or false if failed
*/
BOOL GetFileNameForSave()
{
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = g_hwnd;
	ofn.lpstrFilter = L"Text Files(*.txt)\0*.txt\0All File(*.*)\0*.*\0";
	ofn.lpstrFile = curfile;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	ofn.lpstrDefExt = L"txt";
	if (!GetSaveFileName(&ofn))
	{
		OutputDebugString(L"GetSaveFileName function failed\n");
		return FALSE;
	}
	return TRUE;
}

/* @author   : Subhash
   @function : to check and save the currrent file in use
   @return   : return true if function succeed or false if failed
*/
BOOL check_for_filesave()
{
	if (g_need_to_save_file)
	{
		int res;
		res = MessageBox(g_hwnd, L"The File has been changed!!!\nDo you want to save it before continueing?", L"Save File before continueing!!", MB_YESNO | MB_ICONINFORMATION);
		if (res == IDNO)
			return TRUE;
		else
		{
			if (!g_is_file_opened)
			{
				if (GetFileNameForSave())
				{
					if (!SaveTextFileFromEdit())
					{
						OutputDebugString(L"SaveTextFileFromEdit function failed in check_for_filesave function\n");
						return FALSE;
					}
				}
				else
				{
					OutputDebugString(L"GetSaveFileName function failed in check_for_filesave function\n");
					return FALSE;
				}
			}
			else
			{
				if (!SaveTextFileFromEdit())
				{
					OutputDebugString(L"SaveTextFileFromEdit function failed in check_for_filesave function\n");
					return FALSE;
				}
			}
		}
	}
	return TRUE;
}
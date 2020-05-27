// ImageDisplayapplication.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "ImageDisplayapplication.h"
#include "Resource.h"


#pragma warning(disable : 4996)

#define WIN32_LEAN_AND_MEAN  
#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
HWND g_Himage, g_mainwindow;
int g_image_height;
int g_image_width;
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
HDC paint;
// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

BOOL LoadBitmapFile(HWND hwnd);
BOOL LoadBitmapAsGray(HWND hwnd);

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
    LoadStringW(hInstance, IDC_IMAGEDISPLAYAPPLICATION, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_IMAGEDISPLAYAPPLICATION));

	HANDLE hMutex = OpenMutex(MUTEX_ALL_ACCESS, 0, L"ImageDisplayapplication.exe");

	if (!hMutex)
		hMutex = CreateMutex(0, 0, L"ImageDisplayapplication.exe");
	else
		return 0;

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

	ReleaseMutex(hMutex);

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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_IMAGEDISPLAYAPPLICATION));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_IMAGEDISPLAYAPPLICATION);
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

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPED & ~WS_THICKFRAME | WS_MINIMIZEBOX | WS_SYSMENU,
	   CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, nullptr, nullptr, hInstance, nullptr);
   g_mainwindow = hWnd;
   if (!hWnd)
   {
	  OutputDebugString(L"CreateWindowW api for static field failed\n");
      return FALSE;
   }

   g_Himage = CreateWindowW(L"Static", L"", WS_CHILD | WS_VISIBLE | SS_BITMAP,
	   CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hWnd, NULL, NULL, NULL);

   if (!g_Himage)
   {
	   OutputDebugString(L"CreateWindowW api for static field failed\n");
	   return FALSE;
   }

   //SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_DRAWFRAME | SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);

   ShowWindow(hWnd, nCmdShow);
   ShowWindow(g_Himage, nCmdShow);
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
	int Width, Height;
    switch (message)
    {
	/*case WM_SIZE:
	{	
		Width = LOWORD(lParam);
		Height = HIWORD(lParam);

		if (!MoveWindow(g_Himage, 0, 0, Width, Height, 1))
		{
			OutputDebugString(L" MoveWindow api for editcontrol failed\n");
		}
	}
	break;*/
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
			case ID_LOAD_BMP:
				if (!LoadBitmapFile(hWnd))
				{
					OutputDebugString(L"LoadBitmapFile function failed\n");
				}
				break;
			case ID_LOAD_GRAY:
				if (!LoadBitmapAsGray(hWnd))
				{
					OutputDebugString(L"LoadBitmapFile function failed\n");
				}
				break;
            case ID_FILE_EXIT:
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
			paint = hdc;
            // TODO: Add any drawing co de that uses hdc here...
            EndPaint(hWnd, &ps);
			return 0;
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
   @function : loads the bitmap image to the application window
   @param	 : handle of the main window
   @return   : returns true if function succeed or false if it fails
*/
BOOL LoadBitmapFile(HWND hwnd)
{
	FILE *fp = NULL;
	int image_width, image_height, w_width = 1010, w_height = 460, ratio_w, ratio_h, r_width, r_height;
	HBITMAP hBitmap;
	BOOL function_status = TRUE;
	WCHAR curfile[MAX_PATH];
	curfile[0] = NULL;
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFilter = L"BMP Files(*.bmp)\0*.bmp\0";
	ofn.lpstrFile = curfile;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	ofn.lpstrDefExt = L"bmp";

	if (!GetOpenFileName(&ofn))
	{
		OutputDebugString(L"GetOpenFileName api failed\n");
		return FALSE;
	}
	
	_bstr_t file_name(curfile);

	if (!(fp = fopen(file_name, "rb")))
	{
		OutputDebugString(L"file not opened properly\n");
		function_status = FALSE;
		goto QUIT;
	}

	if (!fread(&bmpName, FORMAT_NAME_SIZE, NMEMB, fp))
	{
		OutputDebugString(L"fread  is not excecuted successfully\n");
		function_status = FALSE;
		goto QUIT;
	}

	if (!fread(&bmpHeader, BMP_HEADER_SIZE, NMEMB, fp))
	{
		OutputDebugString(L"fread  is not excecuted successfully\n");
		function_status = FALSE;
		goto QUIT;
	}

	if (!fread(&dibHeader, DIB_HEADER_SIZE, NMEMB, fp))
	{
		OutputDebugString(L"fread  is not excecuted successfully\n");
		function_status = FALSE;
		goto QUIT;
	}

	image_height = dibHeader.height;
	image_width = dibHeader.width;

	if (image_width > w_width || image_height > w_height)
	{
		ratio_h = image_height / w_height;
		ratio_w = image_width / w_width;

		if (ratio_h > ratio_w)
		{
			r_height = image_height / ratio_h;
			r_width = image_width / ratio_h;
		}
		else
		{
			r_height = image_height / ratio_w;
			r_width = image_width / ratio_w;
		}

		if (!MoveWindow(g_mainwindow, 0, 0, r_width, r_height, 1))
		{
			OutputDebugString(L" MoveWindow api for editcontrol failed\n");
		}

		if (!MoveWindow(g_Himage, 0, 0, r_width, r_width, 1))
		{
			OutputDebugString(L" MoveWindow api for editcontrol failed\n");
		}

		hBitmap = (HBITMAP)LoadImage(NULL, curfile, IMAGE_BITMAP, r_width, r_height,
			LR_LOADFROMFILE);

		if (!hBitmap)
		{
			OutputDebugString(L"Image is not loaded properly\n");
			function_status = FALSE;
			goto QUIT;
		}

	}
	else
	{
		if (!MoveWindow(g_mainwindow, 0, 0, image_width, image_height, 1))
		{
			OutputDebugString(L" MoveWindow api for editcontrol failed\n");
		}
				
		if (!MoveWindow(g_Himage, 0, 0, image_width, image_height, 1))
		{
			OutputDebugString(L" MoveWindow api for editcontrol failed\n");
		}

		hBitmap = (HBITMAP)LoadImage(NULL, curfile, IMAGE_BITMAP, image_width, image_height,
			LR_LOADFROMFILE);

		if (!hBitmap)
		{
			OutputDebugString(L"Image is not loaded properly\n");
			function_status = FALSE;
			goto QUIT;
		}
	}

	SendMessageW(g_Himage, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap);

	QUIT:

		if (fp)
		{
			if (fclose(fp) == EOF)
			{
				OutputDebugString(L"The file is not closed successfully\n");
			}
		}
	
	return TRUE;
}

/* @author   : Subhash
   @function : converts the bitmap image to gray scale and load it in the application window
   @param	 : handle of the main window
   @return   : returns true if function succeed or false if it fails
*/
BOOL LoadBitmapAsGray(HWND hwnd)
{
	FILE *fp = NULL;
	int image_width, image_height, w_width = 1010, w_height = 460, ratio_w, ratio_h, r_width, r_height;
	HBITMAP hBitmap;
	BOOL function_status = TRUE;
	uint8_t *raw_rgb_data = NULL, *gray_data = NULL, *color_pallete = NULL;
	int gray_padding, rgb_padding;
	int gray_row = 0, rgb_index = 0, rgb_tempindex = 0;

	WCHAR curfile[MAX_PATH];
	curfile[0] = NULL;
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFilter = L"BMP Files(*.bmp)\0*.bmp\0";
	ofn.lpstrFile = curfile;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	ofn.lpstrDefExt = L"bmp";

	if (!GetOpenFileName(&ofn))
	{
		OutputDebugString(L"GetOpenFileName api failed\n");
		return FALSE;
	}

	_bstr_t file_name(curfile);

	if (!(fp = fopen(file_name, "rb")))
	{
		OutputDebugString(L"file not opened properly\n");
		function_status = FALSE;
		goto QUIT;
	}

	if (!fread(&bmpName, FORMAT_NAME_SIZE, NMEMB, fp))
	{
		OutputDebugString(L"fread  is not excecuted successfully\n");
		function_status = FALSE;
		goto QUIT;
	}

	if (!fread(&bmpHeader, BMP_HEADER_SIZE, NMEMB, fp))
	{
		OutputDebugString(L"fread  is not excecuted successfully\n");
		function_status = FALSE;
		goto QUIT;
	}

	if (!fread(&dibHeader, DIB_HEADER_SIZE, NMEMB, fp))
	{
		OutputDebugString(L"fread  is not excecuted successfully\n");
		function_status = FALSE;
		goto QUIT;
	}

	image_height = dibHeader.height;
	image_width = dibHeader.width;

	gray_padding = dibHeader.width % 4;

	if (gray_padding)
	{
		gray_padding = 4 - gray_padding;
	}

	rgb_padding = (dibHeader.width * 3) % 4;

	if (rgb_padding)
	{
		rgb_padding = 4 - rgb_padding;
	}

	dibHeader.bmp_bytesz = dibHeader.width * dibHeader.height *
		3 + dibHeader.height * rgb_padding;

	raw_rgb_data = (uint8_t *)malloc(dibHeader.bmp_bytesz * sizeof(uint8_t));

	if (!raw_rgb_data)
	{
		OutputDebugString(L"memory not allocated\n");
		function_status = FALSE;
		goto QUIT;
	}

	if (!fread(raw_rgb_data, dibHeader.bmp_bytesz, NMEMB, fp))
	{
		OutputDebugString(L"fread is not excecuted successfully\n");
		function_status = FALSE;
		goto QUIT;
	}

	if (fclose(fp) == EOF)
	{
		OutputDebugString(L"The file is not closed successfully\n");
		function_status = FALSE;
		goto QUIT;
	}

	bmpName.name[0] = 'B';
	bmpName.name[1] = 'M';
	bmpHeader.fileSize = TOTAL_HEADER_SIZE + dibHeader.width* dibHeader.height + COLOR_PALLETE_SIZE
		+ dibHeader.height * gray_padding;
	bmpHeader.creator1 = ZERO;
	bmpHeader.creator2 = ZERO;
	bmpHeader.bmp_offset = TOTAL_HEADER_SIZE + 1024;
	dibHeader.header_sz = DIB_HEADER_SIZE;
	dibHeader.width = dibHeader.width;
	dibHeader.height = dibHeader.height;
	dibHeader.nplanes = PLANES_IN_IMAGE;
	dibHeader.bitspp = 8;
	dibHeader.compress_type = ZERO;
	dibHeader.bmp_bytesz = dibHeader.width * dibHeader.height
		+ dibHeader.height * gray_padding;
	dibHeader.hres = ZERO;
	dibHeader.vres = ZERO;
	dibHeader.ncolors = ZERO;
	dibHeader.nimpcolors = ZERO;

	gray_data = (uint8_t *)malloc(dibHeader.bmp_bytesz * sizeof(uint8_t));

	if (!gray_data)
	{
		OutputDebugString(L"memory not allocated\n");
		function_status = FALSE;
		goto QUIT;
	}

	color_pallete = (uint8_t *)malloc(1024 * sizeof(uint8_t));

	if (!color_pallete)
	{
		OutputDebugString(L"memory not allocated\n");
		function_status = FALSE;
		goto QUIT;
	}

	for (int color = 0, pallete_index = 0; color < COLOR_RANGE; color++, pallete_index += 4)
	{
		color_pallete[pallete_index] = color;
		color_pallete[pallete_index + 1] = color;
		color_pallete[pallete_index + 2] = color;
		color_pallete[pallete_index + 3] = 0;
	}

	for (int gray_index = 0; gray_index < dibHeader.bmp_bytesz; gray_index++)
	{

		if (gray_row != dibHeader.width - 1)
		{
			gray_data[gray_index] = (raw_rgb_data[rgb_index] * 0.299 + raw_rgb_data[rgb_index + 1] * 0.587 + raw_rgb_data[rgb_index + 2] * 0.114);
			gray_row++;
			rgb_index += 3;
			rgb_tempindex += 3;
		}
		else
		{
			for (int i = 0; i < gray_padding; i++)
			{
				gray_data[gray_index] = 0;
				gray_index++;
			}
			gray_index--;
			gray_row = 0;
		}

		if (rgb_tempindex == ((dibHeader.width * 3) + 3))
		{

			rgb_tempindex = 0;
			rgb_index = rgb_index + rgb_padding;
		}
	}
	
	if (!(fp = fopen("gray.BMP", "wb")))
	{
		OutputDebugString(L"file not opened properly\n");
		function_status = FALSE;
		goto QUIT;
	}

	if (fwrite(&bmpName, FORMAT_NAME_SIZE, NMEMB, fp) == EOF)
	{
		OutputDebugString(L"fwrite is not excecuted successfully\n");
		function_status = FALSE;
		goto QUIT;
	}

	if (fwrite(&bmpHeader, BMP_HEADER_SIZE, NMEMB, fp) == EOF)
	{
		OutputDebugString(L"fwrite is not excecuted successfully\n");
		function_status = FALSE;
		goto QUIT;
	}

	if (fwrite(&dibHeader, DIB_HEADER_SIZE, NMEMB, fp) == EOF)
	{
		OutputDebugString(L"fwrite is not excecuted successfully\n");
		function_status = FALSE;
		goto QUIT;
	}

	if (fwrite(color_pallete, COLOR_PALLETE_SIZE, NMEMB, fp) == EOF)
	{
		OutputDebugString(L"fwrite is not excecuted successfully\n");
		function_status = FALSE;
		goto QUIT;
	}

	if (fwrite(gray_data, dibHeader.bmp_bytesz, NMEMB, fp) == EOF)
	{
		OutputDebugString(L"fwrite is not excecuted successfully\n");
		function_status = FALSE;
		goto QUIT;
	}

	if (fclose(fp) == EOF)
	{
		OutputDebugString(L"The file is not closed successfully\n");
		function_status = FALSE;
		goto QUIT;
	}

	fp = NULL;

	image_height = dibHeader.height;
	image_width = dibHeader.width;

	if (image_width > w_width || image_height > w_height)
	{
		ratio_h = image_height / w_height;
		ratio_w = image_width / w_width;

		if (ratio_h > ratio_w)
		{
			r_height = image_height / ratio_h;
			r_width = image_width / ratio_h;
		}
		else
		{
			r_height = image_height / ratio_w;
			r_width = image_width / ratio_w;
		}

		if (!MoveWindow(g_mainwindow, 0, 0, r_width, r_height, 1))
		{
			OutputDebugString(L" MoveWindow api for editcontrol failed\n");
		}

		if (!MoveWindow(g_Himage, 0, 0, r_width, r_width, 1))
		{
			OutputDebugString(L" MoveWindow api for editcontrol failed\n");
		}

		hBitmap = (HBITMAP)LoadImage(NULL, L"gray.BMP", IMAGE_BITMAP, r_width, r_height,
			LR_LOADFROMFILE);

		if (!hBitmap)
		{
			OutputDebugString(L"Image is not loaded properly\n");
			function_status = FALSE;
			goto QUIT;
		}

	}
	else
	{
		if (!MoveWindow(g_mainwindow, 0, 0, image_width, image_height, 1))
		{
			OutputDebugString(L" MoveWindow api for editcontrol failed\n");
		}

		if (!MoveWindow(g_Himage, 0, 0, image_width, image_height, 1))
		{
			OutputDebugString(L" MoveWindow api for editcontrol failed\n");
		}

		hBitmap = (HBITMAP)LoadImage(NULL, L"gray.BMP", IMAGE_BITMAP, image_width, image_height,
			LR_LOADFROMFILE);

		if (!hBitmap)
		{
			OutputDebugString(L"Image is not loaded properly\n");
			function_status = FALSE;
			goto QUIT;
		}
	}

	SendMessageW(g_Himage, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap);
	/*HBITMAP hBitmap;
	MoveWindow(g_mainwindow, 0, 0, 1010, 460, 1);

	hBitmap = (HBITMAP)LoadImage(NULL, L"gray.BMP", IMAGE_BITMAP, 1010, 460,
		LR_LOADFROMFILE);

	if (!hBitmap)
	{
		OutputDebugString(L"Image is not loaded properly\n");
		function_status = FALSE;
		goto QUIT;
	}

	SendMessageW(g_Himage, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap);*/

	QUIT:

		if (color_pallete)
			free(color_pallete);

		if (gray_data)
			free(gray_data);

		if (raw_rgb_data)
			free(raw_rgb_data);

		
		if (fp)
		{
			if (fclose(fp) == EOF)
			{
				OutputDebugString(L"The file is not closed successfully\n");
			}
		}

	return function_status;
}

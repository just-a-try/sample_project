// The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the CAMERAPREVIEWDLL_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// CAMERAPREVIEWDLL_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef CAMERAPREVIEWDLL_EXPORTS
#define CAMERAPREVIEWDLL_API __declspec(dllexport)
#else
#define CAMERAPREVIEWDLL_API __declspec(dllimport)
#endif

#pragma warning(disable : 4996)

#include <Windows.h>
#include <dshow.h>
#include <dbt.h>
#include <mmreg.h>
#include <msacm.h>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <commdlg.h>
#include <strsafe.h>
#include <dshowutil.h>
#include "SampleCGB.h"
#include "qedit.h"

#define mouse_wheel 120
#define source_imgage_width 1280
#define source_imgage_height 720


// This class is exported from the dll
class CAMERAPREVIEWDLL_API CCameraPreviewDLL {
public:
	CCameraPreviewDLL(void);
	// TODO: add your methods here.
};


extern CAMERAPREVIEWDLL_API int nCameraPreviewDLL;

CAMERAPREVIEWDLL_API int fnCameraPreviewDLL(void);
CAMERAPREVIEWDLL_API BOOL StartPreview();
CAMERAPREVIEWDLL_API BOOL StopPreview();
BOOL BuildPreviewGraph(HWND hwnd);
void FreeCapFilters();
void TearDownGraph();
void NukeDownstream(IBaseFilter *pf);
BOOL MakeBuilder();
BOOL MakeGraph();
CAMERAPREVIEWDLL_API BOOL Resize(HWND hwnd, double Width, double  Height);
CAMERAPREVIEWDLL_API BOOL InitCapFilters(HWND hwnd);
CAMERAPREVIEWDLL_API BOOL EnumarateCamera(HWND hwnd);
CAMERAPREVIEWDLL_API BOOL zoom_in_and_out(WPARAM wParam);
 HRESULT EnumerateDevices(REFGUID category, IEnumMoniker **ppEnum);
 BOOL DisplayDeviceInformation(IEnumMoniker *pEnum, HWND hwnd);
 double get_gcd(double num1, double num2);
 CAMERAPREVIEWDLL_API bool capture_from_still_pin();

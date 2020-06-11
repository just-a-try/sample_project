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
#include "status.h"
#include "crossbar.h"
#include "wxlist.h"

// This class is exported from the dll
class CAMERAPREVIEWDLL_API CCameraPreviewDLL {
public:
	CCameraPreviewDLL(void);
	// TODO: add your methods here.
};


extern CAMERAPREVIEWDLL_API int nCameraPreviewDLL;

CAMERAPREVIEWDLL_API int fnCameraPreviewDLL(void);
 BOOL StartPreview();
 BOOL StopPreview();
 BOOL BuildPreviewGraph();
 void FreeCapFilters();
 void TearDownGraph();
 void NukeDownstream(IBaseFilter *pf);
 void ResizeWindow(int w, int h);
 BOOL MakeBuilder();
 BOOL MakeGraph();
 CAMERAPREVIEWDLL_API BOOL InitCapFilters();
CAMERAPREVIEWDLL_API BOOL EnumarateCamera(HWND hwnd);
CAMERAPREVIEWDLL_API HRESULT EnumerateDevices(REFGUID category, IEnumMoniker **ppEnum);
CAMERAPREVIEWDLL_API BOOL DisplayDeviceInformation(IEnumMoniker *pEnum, HWND hwnd);

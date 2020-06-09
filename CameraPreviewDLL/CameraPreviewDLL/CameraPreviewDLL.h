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

#include "amcap.h"
#include "status.h"
#include "crossbar.h"
#include "SampleCGB.h"

// This class is exported from the dll
class CAMERAPREVIEWDLL_API CCameraPreviewDLL {
public:
	CCameraPreviewDLL(void);
	// TODO: add your methods here.
};

struct _capstuff
{
	WCHAR wszCaptureFile[_MAX_PATH];
	WORD wCapFileSize;  // size in Meg
	ISampleCaptureGraphBuilder *pBuilder;
	IVideoWindow *pVW;
	IMediaEventEx *pME;
	IAMDroppedFrames *pDF;
	IAMVideoCompression *pVC;
	IAMVfwCaptureDialogs *pDlg;
	IAMStreamConfig *pASC;      // for audio cap
	IAMStreamConfig *pVSC;      // for video cap
	IBaseFilter *pRender;
	IBaseFilter *pVCap, *pACap;
	IGraphBuilder *pFg;
	IFileSinkFilter *pSink;
	IConfigAviMux *pConfigAviMux;
	int  iMasterStream;
	BOOL fCaptureGraphBuilt;
	BOOL fPreviewGraphBuilt;
	BOOL fCapturing;
	BOOL fPreviewing;
	BOOL fMPEG2;
	BOOL fCapAudio;
	BOOL fCapCC;
	BOOL fCCAvail;
	BOOL fCapAudioIsRelevant;
	bool fDeviceMenuPopulated;
	IMoniker *rgpmVideoMenu[10];
	IMoniker *rgpmAudioMenu[10];
	IMoniker *pmVideo;
	IMoniker *pmAudio;
	double FrameRate;
	BOOL fWantPreview;
	long lCapStartTime;
	long lCapStopTime;
	WCHAR wachFriendlyName[120];
	BOOL fUseTimeLimit;
	BOOL fUseFrameRate;
	DWORD dwTimeLimit;
	int iFormatDialogPos;
	int iSourceDialogPos;
	int iDisplayDialogPos;
	int iVCapDialogPos;
	int iVCrossbarDialogPos;
	int iTVTunerDialogPos;
	int iACapDialogPos;
	int iACrossbarDialogPos;
	int iTVAudioDialogPos;
	int iVCapCapturePinDialogPos;
	int iVCapPreviewPinDialogPos;
	int iACapCapturePinDialogPos;
	long lDroppedBase;
	long lNotBase;
	BOOL fPreviewFaked;
	CCrossbar *pCrossbar;
	int iVideoInputMenuPos;
	LONG NumberOfVideoInputs;
	HMENU hMenuPopup;
	int iNumVCapDevices;
} gcap;

extern CAMERAPREVIEWDLL_API int nCameraPreviewDLL;

CAMERAPREVIEWDLL_API int fnCameraPreviewDLL(void);
CAMERAPREVIEWDLL_API BOOL StartPreview();
CAMERAPREVIEWDLL_API BOOL StopPreview();
CAMERAPREVIEWDLL_API BOOL EnumarateCamera(HWND hwnd);
CAMERAPREVIEWDLL_API HRESULT EnumerateDevices(REFGUID category, IEnumMoniker **ppEnum);
CAMERAPREVIEWDLL_API BOOL DisplayDeviceInformation(IEnumMoniker *pEnum, HWND hwnd);

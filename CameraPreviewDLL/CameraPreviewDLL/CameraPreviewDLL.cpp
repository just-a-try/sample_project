// CameraPreviewDLL.cpp : Defines the exported functions for the DLL.
//

#include "pch.h"
#include "framework.h"
#include<iostream>
#include "Commctrl.h"
#include "fourcc.h"
#include "Streams.h"
#include "Wxdebug.h"
#include <vector>
using namespace std;
#pragma comment(lib, "strmiids")
#include "CameraPreviewDLL.h"

#define REGISTER_FILTERGRAPH

IMoniker *pMoniker = NULL;
WCHAR wszCaptureFile[MAX_PATH];
WORD wCapFileSize;  // size in Meg
//ISampleCaptureGraphBuilder *pBuilder;
ICaptureGraphBuilder2 *pBuilder;
IVMRWindowlessControl *g_pWc;
IVideoWindow *pVW;
IMediaEventEx *pME;
IAMDroppedFrames *pDF;
IAMVideoCompression *pVC;
IAMVfwCaptureDialogs *pDlg;
IAMStreamConfig *pASC;      // for audio cap
IAMStreamConfig *pVSC;      // for video cap
IBaseFilter *pRender;
IBaseFilter *pVCap, *pACap;
IBaseFilter* pVmr = NULL;
IVMRWindowlessControl* pWc = NULL;
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
int iVideoInputMenuPos;
LONG NumberOfVideoInputs;
HMENU hMenuPopup;
int iNumVCapDevices;
HINSTANCE ghInstApp = 0;
HACCEL ghAccel = 0;
HFONT  ghfontApp = 0;
TEXTMETRIC gtm = { 0 };
TCHAR gszAppName[] = TEXT("AMCAP");
HWND ghwndApp = 0, ghwndStatus = 0, hWnd;
HDEVNOTIFY ghDevNotify = 0;
long lWidth, lHeight;
long ZWidth, ZHeight;
ISampleGrabber *pSG = NULL;

AM_MEDIA_TYPE g_StillMediaType;


DWORD g_dwGraphRegister = 0;

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

#define ABS(x) (((x) > 0) ? (x) : -(x))

class SampleGrabberCallback : public ISampleGrabberCB
{
public:
	// Fake referance counting.
	STDMETHODIMP_(ULONG) AddRef() { return 1; }
	STDMETHODIMP_(ULONG) Release() { return 2; }

	STDMETHODIMP QueryInterface(REFIID riid, void **ppvObject)
	{
		if (NULL == ppvObject) return E_POINTER;
		if (riid == __uuidof(IUnknown))
		{
			*ppvObject = static_cast<IUnknown*>(this);
			return S_OK;
		}
		if (riid == __uuidof(ISampleGrabberCB))
		{
			*ppvObject = static_cast<ISampleGrabberCB*>(this);
			return S_OK;
		}
		return E_NOTIMPL;
	}

	STDMETHODIMP SampleCB(double Time, IMediaSample *pSample)
	{
		return E_NOTIMPL;
	}

	STDMETHODIMP BufferCB(double Time, BYTE *pBuffer, long BufferLen)
	{
		OutputDebugString(TEXT("BufferCb api!"));

		if ((g_StillMediaType.majortype != MEDIATYPE_Video) ||
			(g_StillMediaType.formattype != FORMAT_VideoInfo) ||
			(g_StillMediaType.cbFormat < sizeof(VIDEOINFOHEADER)) ||
			(g_StillMediaType.pbFormat == NULL))
		{
			return VFW_E_INVALIDMEDIATYPE;
		}

		HANDLE hf = CreateFile(L"D:\\Still_Capture.raw", GENERIC_WRITE,
			FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, 0, NULL);
		if (hf == INVALID_HANDLE_VALUE)
		{
			return E_FAIL;
		}

		/*long cbBitmapInfoSize = g_StillMediaType.cbFormat - SIZE_PREHEADER;
		VIDEOINFOHEADER *pVideoHeader =
			(VIDEOINFOHEADER*)g_StillMediaType.pbFormat;*/

		//BITMAPFILEHEADER bfh;
		//ZeroMemory(&bfh, sizeof(bfh));
		//bfh.bfType = 'MB';  // Little-endian for "BM".
		//bfh.bfSize = sizeof(bfh) + BufferLen + cbBitmapInfoSize;
		//bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + cbBitmapInfoSize;

		//// Write the file header.
		DWORD dwWritten = 0;
		//WriteFile(hf, &bfh, sizeof(bfh), &dwWritten, NULL);
		//WriteFile(hf, HEADER(pVideoHeader), cbBitmapInfoSize, &dwWritten, NULL);
		WriteFile(hf, pBuffer, BufferLen, &dwWritten, NULL);
		CloseHandle(hf);
		return S_OK;

	}
};    

SampleGrabberCallback g_StillCapCB;

 HRESULT EnumerateDevices(REFGUID category, IEnumMoniker **ppEnum)
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

 BOOL DisplayDeviceInformation(IEnumMoniker *pEnum, HWND hwnd)
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
	hWnd = hwnd;
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

double get_gcd(double num1, double num2)
{

	while (num1 != num2)
	{
		if (num1 > num2)
			num1 -= num2;
		else
			num2 -= num1;
	}

	return num1;
}

CAMERAPREVIEWDLL_API BOOL Resize(HWND hwnd, double Width, double Height)
{
	//HRESULT hr = g_pWc->GetNativeVideoSize(&lWidth, &lHeight, NULL, NULL);
	RECT rc;
	GetClientRect(hwnd, &rc);
	lWidth = rc.right;
	lHeight = rc.bottom;

	double s_ratio = (double)lWidth / (double)lHeight;
	double d_ratio = Width / Height;

	double gcd_value = get_gcd(Width, Height);

	double rv_width = Width / gcd_value;
	double rv_height = Height / gcd_value;
	double size_reduce;

	if (d_ratio > 1)
	{
		size_reduce = lHeight - (lWidth / d_ratio);
		lHeight = (long)(lHeight - size_reduce);

	}
	else if(d_ratio < 1)
	{
		size_reduce = lWidth - (lHeight * d_ratio);
		lWidth = (long)(lWidth - size_reduce);
	}
	
	ZWidth = lWidth;
	ZHeight = lHeight;

	pVW->SetWindowPosition(0, 0, lWidth, lHeight);
	//if (SUCCEEDED(hr))
	//{
	//	RECT rcSrc, rcDest;
	//	// Set the source rectangle.
	//	SetRect(&rcSrc, 0, 0, (int)lWidth, (int)lHeight);

	//	// Set the destination rectangle.
	//	SetRect(&rcDest, 0, 0, Width, Height);

	//	// Set the video position.
	//	hr = g_pWc->SetVideoPosition(&rcSrc, &rcDest);
	//}
	
	return TRUE;
}

CAMERAPREVIEWDLL_API BOOL zoom_in_and_out(WPARAM wParam)
{
	int mouse_wheel_value = HIWORD(wParam);
	int Wdiff_source_dest, Hdiff_source_dest, Width, Height;
	static int zoom_in_factor = 2, zoom_out_factor;
	static int x_axis, y_axis;
	HRESULT hr;
	if (mouse_wheel_value == mouse_wheel)
	{
		if (zoom_in_factor <= 8)
		{
			RECT rcSrc, rcDest;
			Width = ZWidth / zoom_in_factor;
			Height = ZHeight / zoom_in_factor;
			Wdiff_source_dest = source_imgage_width - Width;
			Hdiff_source_dest = source_imgage_height - Height;
			x_axis = Wdiff_source_dest / 2;
			y_axis = Hdiff_source_dest / 2;
			zoom_in_factor *= 2;
			// Set the source rectangle.
			SetRect(&rcSrc, x_axis, y_axis, (source_imgage_width - x_axis), (source_imgage_height - y_axis));

			GetClientRect(hWnd, &rcDest);
			// Set the destination rectangle.
			SetRect(&rcDest, 0, 0, rcDest.right, rcDest.bottom);
			// Set the video position.
			//hr = g_pWc->SetVideoPosition(&rcSrc, &rcDest);
			hr = pVW->SetWindowPosition(0, 0, rcSrc.right, rcSrc.bottom);

		}
	}
	else if (mouse_wheel_value > mouse_wheel)
	{
		zoom_out_factor = zoom_in_factor / 4;
		if (zoom_out_factor > 1)
		{
			Width = ZWidth / zoom_out_factor;
			Height = ZHeight / zoom_out_factor;
			Wdiff_source_dest = source_imgage_width - Width;
			Hdiff_source_dest = source_imgage_height - Height;
			x_axis = Wdiff_source_dest / 2;
			y_axis = Hdiff_source_dest / 2;
			zoom_in_factor /= 2;
			RECT rcSrc, rcDest;
			// Get the window client area.
			GetClientRect(hWnd, &rcDest);
			// Set the destination rectangle.
			SetRect(&rcDest, 0, 0, rcDest.right, rcDest.bottom);

			SetRect(&rcSrc, x_axis, y_axis, (source_imgage_width - x_axis), (source_imgage_height - y_axis));

			// Set the video position.
			//g_pWc->SetVideoPosition(&rcSrc, &rcDest);
			hr = pVW->SetWindowPosition(0, 0, rcSrc.right, rcSrc.bottom);
		}
		else
		{
			zoom_in_factor = 2;
			RECT rcSrc, rcDest;
			GetClientRect(hWnd, &rcDest);
			SetRect(&rcDest, 0, 0, rcDest.right, rcDest.bottom);
			SetRect(&rcSrc, 0, 0, ZWidth, ZHeight);
			//g_pWc->SetVideoPosition(&rcSrc, &rcDest);
			hr = pVW->SetWindowPosition(0, 0, rcSrc.right, rcSrc.bottom);
		}
	}

	return TRUE;
}

// Tear down everything downstream of a given filter
void NukeDownstream(IBaseFilter *pf)
{
	IPin *pP = 0, *pTo = 0;
	ULONG u;
	IEnumPins *pins = NULL;
	PIN_INFO pininfo;

	if (!pf)
		return;

	HRESULT hr = pf->EnumPins(&pins);
	pins->Reset();

	while (hr == NOERROR)
	{
		hr = pins->Next(1, &pP, &u);
		if (hr == S_OK && pP)
		{
			pP->ConnectedTo(&pTo);
			if (pTo)
			{
				hr = pTo->QueryPinInfo(&pininfo);
				if (hr == NOERROR)
				{
					if (pininfo.dir == PINDIR_INPUT)
					{
						NukeDownstream(pininfo.pFilter);
						pFg->Disconnect(pTo);
						pFg->Disconnect(pP);
						pFg->RemoveFilter(pininfo.pFilter);
					}
					pininfo.pFilter->Release();
				}
				pTo->Release();
			}
			pP->Release();
		}
	}

	if (pins)
		pins->Release();
}


void TearDownGraph()
{
	SAFE_RELEASE(pSink);
	SAFE_RELEASE(pConfigAviMux);
	SAFE_RELEASE(pRender);
	SAFE_RELEASE(pME);
	SAFE_RELEASE(pDF);

	if (pVW)
	{
		// stop drawing in our window, or we may get wierd repaint effects
		pVW->put_Owner(NULL);
		pVW->put_Visible(OAFALSE);
		pVW->Release();
		pVW = NULL;
	}

	// destroy the graph downstream of our capture filters
	if (pVCap)
		NukeDownstream(pVCap);
	if (pACap)
		NukeDownstream(pACap);
	/*if (pVCap)
		pBuilder->ReleaseFilters();*/

		// potential debug output - what the graph looks like
		// if (pFg) DumpGraph(pFg, 1);

#ifdef REGISTER_FILTERGRAPH
	// Remove filter graph from the running object table
	if (g_dwGraphRegister)
	{
		RemoveGraphFromRot(g_dwGraphRegister);
		g_dwGraphRegister = 0;
	}
#endif

	fCaptureGraphBuilt = FALSE;
	fPreviewGraphBuilt = FALSE;
	fPreviewFaked = FALSE;
}



BOOL BuildPreviewGraph(HWND hwnd)
{
	HRESULT hr;

	// we have one already
	if (fPreviewGraphBuilt)
		return TRUE;

	// No rebuilding while we're running
	if (fCapturing || fPreviewing)
		return FALSE;

	// We don't have the necessary capture filters
	if (pVCap == NULL)
		return FALSE;
	
	// we already have another graph built... tear down the old one
	if (fCaptureGraphBuilt)
		TearDownGraph();

	// Add the Sample Grabber filter to the graph.
	IBaseFilter *pSG_Filter;
	hr = CoCreateInstance(
		CLSID_SampleGrabber,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_IBaseFilter,
		(void**)&pSG_Filter
	);

	if (FAILED(hr))
	{
		OutputDebugString(TEXT("Error in CoCreateInstance CLSID_SampleGrabber!"));
		return FALSE;
	}

	hr = pFg->AddFilter(pSG_Filter, L"SampleGrab");

	if (FAILED(hr))
	{
		OutputDebugString(TEXT("Error in adding Sample Grabber filter!"));
		return FALSE;
	}
	// Add the Null Renderer filter to the graph.
	IBaseFilter *pNull;

	hr = CoCreateInstance(
		CLSID_NullRenderer,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_IBaseFilter,
		(void**)&pNull
	);

	if (FAILED(hr))
	{
		OutputDebugString(TEXT("Error in render RenderStream Sample Grabber!"));
		return FALSE;
	}

	hr = pFg->AddFilter(pNull, L"NullRender");

	if (FAILED(hr))
	{
		OutputDebugString(TEXT("Error in adding NullRender filter!"));
	}

	hr = pBuilder->RenderStream(
		&PIN_CATEGORY_CAPTURE, // Connect this pin ...
		&MEDIATYPE_Video,    // with this media type ...
		pVCap,                // on this filter ...
		pSG_Filter,          // to the Sample Grabber ...
		pNull);              // ... and finally to the Null Renderer.

	if (FAILED(hr))
	{
		OutputDebugString(TEXT("Error in render RenderStream Sample Grabber!"));
	}


	hr = pSG_Filter->QueryInterface(IID_ISampleGrabber, (void**)&pSG);

	if (FAILED(hr))
	{
		OutputDebugString(TEXT("Error in QueryInterface  Sample Grabber!"));
		return FALSE;
	}

	hr = pSG->SetOneShot(FALSE);

	hr = pSG->SetBufferSamples(TRUE);

	hr = pSG->SetCallback(&g_StillCapCB, 0);

	hr = pSG->GetConnectedMediaType(&g_StillMediaType);

	hr = pSG->SetCallback(&g_StillCapCB, 1);

	pSG->Release();

	//FreeMediaType(g_StillMediaType);

	//
	// Render the preview pin - even if there is not preview pin, the capture
	// graph builder will use a smart tee filter and provide a preview.
	//
	// !!! what about latency/buffer issues?

	// NOTE that we try to render the interleaved pin before the video pin, because
	// if BOTH exist, it's a DV filter and the only way to get the audio is to use
	// the interleaved pin.  Using the Video pin on a DV filter is only useful if
	// you don't want the audio.
	//hr = CoCreateInstance(CLSID_VideoMixingRenderer, NULL,
	//	CLSCTX_INPROC, IID_IBaseFilter, (void**)&pVmr);

	//if (hr != NOERROR)
	//{
	//	OutputDebugString(TEXT("This graph cannot preview properly"));
	//}

	//hr = pFg->AddFilter(pVmr, L"Video Mixing Renderer");

	//if (FAILED(hr))
	//{
	//	pVmr->Release();
	//	return hr;
	//}

	//// Set the rendering mode.  
	//IVMRFilterConfig* pConfig;
	//hr = pVmr->QueryInterface(IID_IVMRFilterConfig, (void**)&pConfig);

	//if (SUCCEEDED(hr))
	//{
	//	hr = pConfig->SetRenderingMode(VMRMode_Windowless);
	//	pConfig->Release();
	//}
	//if (SUCCEEDED(hr))
	//{
	//	// Set the window. 
	//	hr = pVmr->QueryInterface(IID_IVMRWindowlessControl, (void**)&pWc);

	//	if (SUCCEEDED(hr))
	//	{
	//		hr = pWc->SetVideoClippingWindow(hwnd);

	//		if (SUCCEEDED(hr))
	//		{
	//			g_pWc = pWc; // Return this as an AddRef'd pointer. 
	//		}
	//		else
	//		{
	//			// An error occurred, so release the interface.
	//			pWc->Release();
	//		}
	//	}
	//}

	//if (fMPEG2)
	//{
	//	hr = pBuilder->RenderStream(&PIN_CATEGORY_CAPTURE,
	//		&MEDIATYPE_Stream, pVCap, NULL, pVmr);

	//	if (FAILED(hr))
	//	{
	//		OutputDebugString(TEXT("Cannot build MPEG2 preview graph!"));
	//	}
	//}
	//else
	//{
	//	hr = pBuilder->RenderStream(&PIN_CATEGORY_PREVIEW,
	//		&MEDIATYPE_Interleaved, pVCap, NULL, pVmr);

	//	if (hr == VFW_S_NOPREVIEWPIN)
	//	{
	//		// preview was faked up for us using the (only) capture pin
	//		fPreviewFaked = TRUE;
	//	}
	//	else if (hr != S_OK)
	//	{
	//		// maybe it's DV?
	//		hr = pBuilder->RenderStream(&PIN_CATEGORY_PREVIEW,
	//			&MEDIATYPE_Video, pVCap, NULL, pVmr);

	//		if (hr == VFW_S_NOPREVIEWPIN)
	//		{
	//			// preview was faked up for us using the (only) capture pin
	//			fPreviewFaked = TRUE;
	//		}
	//		else if (hr != S_OK)
	//		{
	//			//OutputDebugString(TEXT("This graph cannot preview!"));
	//			fPreviewGraphBuilt = FALSE;
	//			return FALSE;
	//		}
	//	}

	//}
	hr = pBuilder->RenderStream(&PIN_CATEGORY_PREVIEW,
		&MEDIATYPE_Video, pVCap, NULL, NULL);

	hr = pFg->QueryInterface(IID_IVideoWindow, (void **)&pVW);

	long lWidth, lHeight;

	//hr = pVW->GetWindowPosition(NULL, NULL, &lWidth, &lHeight);

	RECT rc;
	pVW->put_Owner((OAHWND)hwnd);    // We own the window now
	pVW->put_WindowStyle(WS_CHILD);    // you are now a child
	
	//hr = g_pWc->GetNativeVideoSize(&lWidth, &lHeight, NULL, NULL);
	//if (SUCCEEDED(hr))
	//{
	//	RECT rcSrc, rcDest;
	//	// Set the source rectangle.
	//	SetRect(&rcSrc, 0, 0, lWidth, lHeight);

	//	// Get the window client area.
	//	GetClientRect(hwnd, &rcDest);
	//	// Set the destination rectangle.
	//	SetRect(&rcDest, 0, 0, rcDest.right, rcDest.bottom);

	//	// Set the video position.
	//	hr = g_pWc->SetVideoPosition(&rcSrc, &rcDest);
	//}
	GetClientRect(hwnd, &rc);

	pVW->SetWindowPosition(0, 0, rc.right, rc.bottom); // be this big
	pVW->put_Visible(OATRUE);
	
	// All done.
	fPreviewGraphBuilt = TRUE;
	return TRUE;
}

CAMERAPREVIEWDLL_API vector<string> format_resolution_enum(vector<string> &formats, SIZE &max, SIZE &min)
{
	int iCount = 0, iSize = 0;
	CHAR pszValue[5];
	VIDEOINFOHEADER *pVih = NULL;
	IAMStreamConfig *pfConfig = NULL;
	IAMVideoControl *pAMVidControl = NULL;
	SIZE FrameSize;
	LONGLONG *lFrameRate = NULL;
	long lListSize = 0;

	HRESULT hr = pBuilder->FindInterface(
		&PIN_CATEGORY_CAPTURE,
		&MEDIATYPE_Video,
		pVCap,
		IID_IAMStreamConfig, (void**)&pfConfig);

	hr = pfConfig->GetNumberOfCapabilities(&iCount, &iSize);

	IPin *pin = NULL;

	// pBuild is an ICaptureGraphBuilder2 pointer.

	hr = pBuilder->FindPin(
		pVCap,                  // Filter.
		PINDIR_OUTPUT,         // Look for an output pin.
		0,   // Pin category.
		0,                  // Media type (don't care).
		TRUE,                 // Pin must be unconnected.
		0,                     // Get the 0'th pin.
		&pin                  // Receives a pointer to thepin.
	);

	hr = pVCap->QueryInterface(IID_IAMVideoControl, (void**)&pAMVidControl);

	if (FAILED(hr))    // FAILED is a macro that tests the return value
	{
		OutputDebugStr(L"Could not query interface for IAMVideoControl\n");
	}


	if (iSize == sizeof(VIDEO_STREAM_CONFIG_CAPS))
	{
		for (int iFormat = 0; iFormat < iCount; iFormat++)
		{
			VIDEO_STREAM_CONFIG_CAPS scc;
			AM_MEDIA_TYPE *pmtConfig;

			hr = pfConfig->GetStreamCaps(iFormat, &pmtConfig, (BYTE*)&scc);

			if (SUCCEEDED(hr))
			{
				if (pmtConfig->subtype == MEDIASUBTYPE_MJPG)
				{
					*((UINT32*)pszValue) = pmtConfig->subtype.Data1;
					pszValue[4] = 0;
					OutputDebugStringA(pszValue);
					formats.push_back(pszValue);
					//pVih = (VIDEOINFOHEADER*)pmtConfig->pbFormat;
					//// pVih contains the detailed format information.
					//LONG lWidth = pVih->bmiHeader.biWidth;
					//LONG lHeight = pVih->bmiHeader.biHeight;

					FrameSize.cx = scc.InputSize.cx;
					FrameSize.cy = scc.InputSize.cy;


					hr = pAMVidControl->GetFrameRateList(pin, iFormat, FrameSize, &lListSize, &lFrameRate);

					for (long lIndex = 0; lIndex < lListSize; lIndex++)
					{
						LONG lWidth = scc.InputSize.cx;
						LONG lHeight = scc.InputSize.cy;
					}

					//if ((pmtConfig->formattype == FORMAT_VideoInfo) &&
					//	(pmtConfig->cbFormat >= sizeof(VIDEOINFOHEADER)) &&
					//	(pmtConfig->pbFormat != NULL))
					//{
					//	pVih = (VIDEOINFOHEADER*)pmtConfig->pbFormat;
					//	// pVih contains the detailed format information.
					//	LONG lWidth = pVih->bmiHeader.biWidth;
					//	LONG lHeight = pVih->bmiHeader.biHeight;
					//}

					FreeMediaType(*pmtConfig);
					
					/*formats.push_back(pszValue);
					max = scc.MaxOutputSize;
					min = scc.MinOutputSize;*/
				}
				else if (pmtConfig->subtype == MEDIASUBTYPE_YUYV)
				{
					*((UINT32*)pszValue) = pmtConfig->subtype.Data1;
					pszValue[4] = 0;
					OutputDebugStringA(pszValue);
					formats.push_back(pszValue);
				}
				else if (pmtConfig->subtype == MEDIASUBTYPE_YUY2)
				{
					*((UINT32*)pszValue) = pmtConfig->subtype.Data1;
					pszValue[4] = 0;
					OutputDebugStringA(pszValue);
					formats.push_back(pszValue);
					FrameSize.cx = scc.InputSize.cx;
					FrameSize.cy = scc.InputSize.cy;

					pAMVidControl->GetFrameRateList(pin, iFormat, FrameSize, &lListSize, &lFrameRate);

					for (long lIndex = 0; lIndex < lListSize; lIndex++)
					{
						LONG lWidth = scc.InputSize.cx;
						LONG lHeight = scc.InputSize.cy;
					}

					//if ((pmtConfig->formattype == FORMAT_VideoInfo) &&
					//	(pmtConfig->cbFormat >= sizeof(VIDEOINFOHEADER)) &&
					//	(pmtConfig->pbFormat != NULL))
					//{
					//	pVih = (VIDEOINFOHEADER*)pmtConfig->pbFormat;
					//	// pVih contains the detailed format information.
					//	LONG lWidth = pVih->bmiHeader.biWidth;
					//	LONG lHeight = pVih->bmiHeader.biHeight;
					//}
				}
				else if (pmtConfig->subtype == MEDIASUBTYPE_YVYU)
				{
					*((UINT32*)pszValue) = pmtConfig->subtype.Data1;
					pszValue[4] = 0;
					OutputDebugStringA(pszValue);
					formats.push_back(pszValue);
				}
				else if (pmtConfig->subtype == MEDIASUBTYPE_RGB4)
				{
					*((UINT32*)pszValue) = pmtConfig->subtype.Data1;
					pszValue[4] = 0;
					OutputDebugStringA(pszValue);
					formats.push_back(pszValue);
				}
				else if (pmtConfig->subtype == MEDIASUBTYPE_RGB8)
				{
					*((UINT32*)pszValue) = pmtConfig->subtype.Data1;
					pszValue[4] = 0;
					OutputDebugStringA(pszValue);
					formats.push_back(pszValue);
				}
				else if (pmtConfig->subtype == MEDIASUBTYPE_RGB24)
				{
					*((UINT32*)pszValue) = pmtConfig->subtype.Data1;
					pszValue[4] = 0;
					OutputDebugStringA(pszValue);
					formats.push_back(pszValue);
				}
				else if (pmtConfig->subtype == MEDIASUBTYPE_RGB32)
				{
					*((UINT32*)pszValue) = pmtConfig->subtype.Data1;
					pszValue[4] = 0;
					OutputDebugStringA(pszValue);
					formats.push_back(pszValue);
				}
				//VIDEOINFOHEADER *pVih = (VIDEOINFOHEADER*)pmtConfig.pbFormat;
				DeleteMediaType(pmtConfig);
			}
		}
	}

	

	return formats;
}

CAMERAPREVIEWDLL_API bool Get_UVC_values(long *brightness, long *contrast, long *white_balance)
{
	IAMVideoProcAmp  *pVideoProc = 0;
	HRESULT hr = pVCap->QueryInterface(IID_IAMVideoProcAmp, (void**)&pVideoProc);
	if (FAILED(hr))
	{
		OutputDebugStr(L"Could not query interface for IAMCameraControl\n");
		return FALSE;
	}
	else
	{
		long Flags;
		
		hr = pVideoProc->Get(VideoProcAmp_Brightness, brightness, &Flags);
		if (FAILED(hr))
		{
			OutputDebugStr(L"Getting brightness value failed\n");
		}
		
		hr = pVideoProc->Get(VideoProcAmp_Contrast, contrast, &Flags);
		if (FAILED(hr))
		{
			OutputDebugStr(L"Getting contrast value failed\n");
		}
		
		hr = pVideoProc->Get(VideoProcAmp_WhiteBalance, white_balance, &Flags);
		if (FAILED(hr))
		{
			OutputDebugStr(L"Getting white balance value failed\n");
		}
	}
	return TRUE;
}

CAMERAPREVIEWDLL_API bool UVC_Settings_config(int property, int mode, long value)
{
	//HWND hTrackbar;

	IAMCameraControl *pCamCtrl = 0;
	HRESULT hr = pVCap->QueryInterface(IID_IAMVideoProcAmp, (void**)&pCamCtrl);
	if (FAILED(hr))
	{
		OutputDebugStr(L"Could not query interface for IID_IAMVideoProcAmp\n");
	}
	else
	{
		hr = pCamCtrl->Set(property, value, mode);
		if (FAILED(hr))
		{
			OutputDebugStr(L"Could not set brightness\n");
		}
	}
	return TRUE;
}
// Start previewing
//
CAMERAPREVIEWDLL_API BOOL StartPreview()
{
	// way ahead of you
	if (fPreviewing)
		return TRUE;

	if (!fPreviewGraphBuilt)
		return FALSE;

	// run the graph
	IMediaControl *pMC = NULL;
	HRESULT hr = pFg->QueryInterface(IID_IMediaControl, (void **)&pMC);
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

	fPreviewing = TRUE;
	return TRUE;
}

// stop the preview graph
//
CAMERAPREVIEWDLL_API BOOL StopPreview()
{
	// way ahead of you
	if (!fPreviewing)
	{
		return FALSE;
	}

	// stop the graph
	IMediaControl *pMC = NULL;
	HRESULT hr = pFg->QueryInterface(IID_IMediaControl, (void **)&pMC);
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

	fPreviewing = FALSE;

	// get rid of menu garbage
	InvalidateRect(ghwndApp, NULL, TRUE);

	return TRUE;
}



// Make a graph builder object we can use for capture graph building
//
BOOL MakeBuilder()
{
	CoInitialize(NULL);
	HRESULT hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC_SERVER,
		IID_ICaptureGraphBuilder2, (void**)&pBuilder);
	if (FAILED(hr))
	{
		return FALSE;
	}
	//CoUninitialize();
	return TRUE;
}

// Make a graph object we can use for capture graph building
//
BOOL MakeGraph()
{
	// we have one already
	if (pFg)
		return TRUE;

	HRESULT hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC,
		IID_IGraphBuilder, (LPVOID *)&pFg);


	return (hr == NOERROR) ? TRUE : FALSE;
}

// create the capture filters of the graph.  We need to keep them loaded from
// the beginning, so we can set parameters on them and have them remembered
//
CAMERAPREVIEWDLL_API BOOL InitCapFilters(HWND hwnd)
{
	//HRESULT hr = S_OK;
	BOOL f;
	ICreateDevEnum *pDevEnum;
	HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL,
		CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pDevEnum));
	hWnd = hwnd;
	IEnumMoniker *pEnum;
	//
	// First, we need a Video Capture filter, and some interfaces
	//
	pVCap = NULL;

	// Create an enumerator for the category.

	hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnum, 0);
	if (hr == S_FALSE)
	{
		hr = VFW_E_NOT_FOUND;  // The category is empty. Treat as an error.
	}
	pDevEnum->Release();

	pEnum->Next(1, &pMoniker, NULL);
	if (pMoniker != 0)
	{
		IPropertyBag *pBag;
		wachFriendlyName[0] = 0;

		hr = pMoniker->BindToStorage(0, 0, IID_PPV_ARGS(&pBag));
		if (SUCCEEDED(hr))
		{
			VARIANT var;
			var.vt = VT_BSTR;

			hr = pBag->Read(L"FriendlyName", &var, NULL);
			if (hr == NOERROR)
			{
				hr = StringCchCopyW(wachFriendlyName, sizeof(wachFriendlyName) / sizeof(wachFriendlyName[0]), var.bstrVal);
				SysFreeString(var.bstrVal);
			}

			pBag->Release();
		}

		hr = pMoniker->BindToObject(0, 0, IID_IBaseFilter, (void**)&pVCap);
	}

	if (pVCap == NULL)
	{
		OutputDebugString(TEXT("Error %x: Cannot create video capture filter"));
		goto InitCapFiltersFail;
	}

	fCCAvail = FALSE;  // assume no closed captioning support

	f = MakeBuilder();
	if (!f)
	{
		OutputDebugString(TEXT("Cannot instantiate graph builder"));
		return FALSE;
	}

	//
	// make a filtergraph, give it to the graph builder and put the video
	// capture filter in the graph
	//

	f = MakeGraph();
	if (!f)
	{
		OutputDebugString(TEXT("Cannot instantiate filtergraph"));
		goto InitCapFiltersFail;
	}

	hr = pBuilder->SetFiltergraph(pFg);
	if (hr != NOERROR)
	{
		OutputDebugString(TEXT("Cannot give graph to builder"));
		goto InitCapFiltersFail;
	}

	// Add the video capture filter to the graph with its friendly name
		hr = pFg->AddFilter(pVCap, wachFriendlyName);
	if (hr != NOERROR)
	{
		OutputDebugString(TEXT("Error %x: Cannot add vidcap to filtergraph"));
		goto InitCapFiltersFail;
	}

	// Calling FindInterface below will result in building the upstream
	// section of the capture graph (any WDM TVTuners or Crossbars we might
	// need).

	// we use this interface to get the name of the driver
	// Don't worry if it doesn't work:  This interface may not be available
	// until the pin is connected, or it may not be available at all.
	// (eg: interface may not be available for some DV capture)
	//hr = pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE,
	//	&MEDIATYPE_Interleaved, pVCap,
	//	IID_IAMVideoCompression, (void **)&pVC);

	//if (hr != S_OK)
	//{
	//	hr = pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE,
	//		&MEDIATYPE_Video, pVCap,
	//		IID_IAMVideoCompression, (void **)&pVC);
	//}

	//// !!! What if this interface isn't supported?
	//// we use this interface to set the frame rate and get the capture size
	//hr = pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE,
	//	&MEDIATYPE_Interleaved,
	//	pVCap, IID_IAMStreamConfig, (void **)&pVSC);

	//if (hr != NOERROR)
	//{
	//	hr = pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE,
	//		&MEDIATYPE_Video, pVCap,
	//		IID_IAMStreamConfig, (void **)&pVSC);
	//	if (hr != NOERROR)
	//	{
	//		// this means we can't set frame rate (non-DV only)
	//		OutputDebugString(TEXT("Error %x: Cannot find VCapture:IAMStreamConfig"));
	//	}
	//}

	//// we use this interface to bring up the 3 dialogs
	//// NOTE:  Only the VfW capture filter supports this.  This app only brings
	//// up dialogs for legacy VfW capture drivers, since only those have dialogs
	//hr = pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE,
	//	&MEDIATYPE_Video, pVCap,
	//	IID_IAMVfwCaptureDialogs, (void **)&pDlg);


	// Can this filter do closed captioning?
	//IPin *pPin;
	//hr = pBuilder->FindPin(pVCap, PINDIR_OUTPUT, &PIN_CATEGORY_VBI,
	//	NULL, FALSE, 0, &pPin);
	//if (hr != S_OK)
	//	hr = pBuilder->FindPin(pVCap, PINDIR_OUTPUT, &PIN_CATEGORY_CC,
	//		NULL, FALSE, 0, &pPin);
	//if (hr == S_OK)
	//{
	//	pPin->Release();
	//	fCCAvail = TRUE;
	//}
	//else
	//{
	//	fCapCC = FALSE;    // can't capture it, then
	//}

	// potential debug output - what the graph looks like
	// DumpGraph(pFg, 1);
	BuildPreviewGraph(hwnd);
	StartPreview();
	CoUninitialize();
	return TRUE;

InitCapFiltersFail:
	FreeCapFilters();
	return FALSE;
}


CAMERAPREVIEWDLL_API bool capture_from_still_pin()
{
	IAMVideoControl *pAMVidControl = NULL;

	HRESULT hr = pVCap->QueryInterface(IID_IAMVideoControl, (void**)&pAMVidControl);

	if (SUCCEEDED(hr))
	{
		// Find the still pin.
		IPin *pPin = NULL;

		// pBuild is an ICaptureGraphBuilder2 pointer.

		hr = pBuilder->FindPin(
			pVCap,                  // Filter.
			PINDIR_OUTPUT,         // Look for an output pin.
			&PIN_CATEGORY_STILL,   // Pin category.
			NULL,                  // Media type (don't care).
			FALSE,                 // Pin must be unconnected.
			0,                     // Get the 0'th pin.
			&pPin                  // Receives a pointer to thepin.
		);

		if (SUCCEEDED(hr))
		{
			hr = pAMVidControl->SetMode(pPin, VideoControlFlag_Trigger);
			pPin->Release();
			OutputDebugString(TEXT("Setmode trigger!"));
		}

		pAMVidControl->Release();
	}
	else
	{
		pAMVidControl->Release();
		return FALSE;
	}
	

	return TRUE;
}
// all done with the capture filters and the graph builder
//
void FreeCapFilters()
{
	SAFE_RELEASE(pFg);

	if (pBuilder)
	{
		delete pBuilder;
		pBuilder = NULL;
	}
	//SAFE_RELEASE(pVCap);
	SAFE_RELEASE(pACap);
	SAFE_RELEASE(pASC);
	SAFE_RELEASE(pVSC);
	SAFE_RELEASE(pVC);
	SAFE_RELEASE(pDlg);

	/*if (pCrossbar)
	{
		delete pCrossbar;
		pCrossbar = NULL;
	}*/
}

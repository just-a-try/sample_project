// CameraPreviewDLL.cpp : Defines the exported functions for the DLL.
//

#include "pch.h"
#include "framework.h"


#ifdef  DEBUG
#define REGISTER_FILTERGRAPH
#endif

#pragma comment(lib, "strmiids")
#include "CameraPreviewDLL.h"
#define REGISTER_FILTERGRAPH


IMoniker *pMoniker = NULL;
WCHAR wszCaptureFile[_MAX_PATH];
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
//CCrossbar *pCrossbar;
HINSTANCE ghInstApp = 0;
HACCEL ghAccel = 0;
HFONT  ghfontApp = 0;
TEXTMETRIC gtm = { 0 };
TCHAR gszAppName[] = TEXT("AMCAP");
HWND ghwndApp = 0, ghwndStatus = 0, hWnd;
HDEVNOTIFY ghDevNotify = 0;
long lWidth, lHeight;
long ZWidth, ZHeight;

//PUnregisterDeviceNotification gpUnregisterDeviceNotification = 0;
//PRegisterDeviceNotification gpRegisterDeviceNotification = 0;
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
	HRESULT hr = g_pWc->GetNativeVideoSize(&lWidth, &lHeight, NULL, NULL);
	double s_ratio = (double)lWidth / (double)lHeight;
	double d_ratio = Width / Height;

	double gcd_value = get_gcd(Width, Height);

	double rv_width = Width / gcd_value;
	double rv_height = Height / gcd_value;
	double size_reduce;

	if (d_ratio > 1)
	{
		size_reduce = lHeight - (lWidth / d_ratio);
		lHeight = lHeight - size_reduce;

	}
	else if(d_ratio < 1)
	{
		size_reduce = lWidth - (lHeight * d_ratio);
		lWidth = lWidth - size_reduce;
	}
	
	ZWidth = lWidth;
	ZHeight = lHeight;

	if (SUCCEEDED(hr))
	{
		RECT rcSrc, rcDest;
		// Set the source rectangle.
		SetRect(&rcSrc, 0, 0, (int)lWidth, (int)lHeight);

		// Set the destination rectangle.
		SetRect(&rcDest, 0, 0, Width, Height);

		// Set the video position.
		hr = g_pWc->SetVideoPosition(&rcSrc, &rcDest);
	}
	
	return TRUE;
}

CAMERAPREVIEWDLL_API BOOL zoom_in_and_out(WPARAM wParam)
{
	int mouse_wheel_value = HIWORD(wParam);
	int Wdiff_source_dest, Hdiff_source_dest, Width, Height;
	static int zoom_in_factor = 2, zoom_out_factor;
	static int x_axis, y_axis;
	if (mouse_wheel_value == 120)
	{
		if (zoom_in_factor <= 8)
		{
			RECT rcSrc, rcDest;
			// Get the window client area.
			Width = ZWidth / zoom_in_factor;
			Height = ZHeight / zoom_in_factor;
			Wdiff_source_dest = 1280 - Width;
			Hdiff_source_dest = 720 - Height;
			x_axis = Wdiff_source_dest / 2;
			y_axis = Hdiff_source_dest / 2;
			zoom_in_factor *= 2;
			// Set the source rectangle.
			SetRect(&rcSrc, x_axis, y_axis, (1280 - x_axis), (720 - y_axis));

			GetClientRect(hWnd, &rcDest);
			// Set the destination rectangle.
			SetRect(&rcDest, 0, 0, rcDest.right, rcDest.bottom);
			// Set the video position.
			HRESULT hr = g_pWc->SetVideoPosition(&rcSrc, &rcDest);
			//zoom_out_factor = zoom_in_factor;
		}
		//}
	}
	else if (mouse_wheel_value > 120)
	{
		
		zoom_out_factor = zoom_in_factor / 4;
		if (zoom_out_factor > 1)
		{
			Width = ZWidth / zoom_out_factor;
			Height = ZHeight / zoom_out_factor;
			Wdiff_source_dest = 1280 - Width;
			Hdiff_source_dest = 720 - Height;
			x_axis = Wdiff_source_dest / 2;
			y_axis = Hdiff_source_dest / 2;
			zoom_in_factor /= 2;
			RECT rcSrc, rcDest;
			// Get the window client area.
			GetClientRect(hWnd, &rcDest);
			// Set the destination rectangle.
			SetRect(&rcDest, 0, 0, rcDest.right, rcDest.bottom);

			SetRect(&rcSrc, x_axis, y_axis, (1280 - x_axis), (720 - y_axis));

			// Set the video position.
			g_pWc->SetVideoPosition(&rcSrc, &rcDest);
		}
		else
		{
			zoom_in_factor = 2;
			RECT rcSrc, rcDest;
			GetClientRect(hWnd, &rcDest);
			SetRect(&rcDest, 0, 0, rcDest.right, rcDest.bottom);
			SetRect(&rcSrc, 0, 0, ZWidth, ZHeight);
			g_pWc->SetVideoPosition(&rcSrc, &rcDest);
		}
		//}
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
	if (pACap == NULL && fCapAudio)
		return FALSE;

	// we already have another graph built... tear down the old one
	if (fCaptureGraphBuilt)
		TearDownGraph();

	//
	// Render the preview pin - even if there is not preview pin, the capture
	// graph builder will use a smart tee filter and provide a preview.
	//
	// !!! what about latency/buffer issues?

	// NOTE that we try to render the interleaved pin before the video pin, because
	// if BOTH exist, it's a DV filter and the only way to get the audio is to use
	// the interleaved pin.  Using the Video pin on a DV filter is only useful if
	// you don't want the audio.
	hr = CoCreateInstance(CLSID_VideoMixingRenderer, NULL,
		CLSCTX_INPROC, IID_IBaseFilter, (void**)&pVmr);

	if (hr != NOERROR)
	{
		OutputDebugString(TEXT("This graph cannot preview properly"));
	}

	hr = pFg->AddFilter(pVmr, L"Video Mixing Renderer");
	if (FAILED(hr))
	{
		pVmr->Release();
		return hr;
	}

	// Set the rendering mode.  
	IVMRFilterConfig* pConfig;
	hr = pVmr->QueryInterface(IID_IVMRFilterConfig, (void**)&pConfig);
	if (SUCCEEDED(hr))
	{
		hr = pConfig->SetRenderingMode(VMRMode_Windowless);
		pConfig->Release();
	}
	if (SUCCEEDED(hr))
	{
		// Set the window. 
		hr = pVmr->QueryInterface(IID_IVMRWindowlessControl, (void**)&pWc);

		if (SUCCEEDED(hr))
		{
			hr = pWc->SetVideoClippingWindow(hwnd);
			if (SUCCEEDED(hr))
			{
				g_pWc = pWc; // Return this as an AddRef'd pointer. 
			}
			else
			{
				// An error occurred, so release the interface.
				pWc->Release();
			}
		}
	}

	if (fMPEG2)
	{
		hr = pBuilder->RenderStream(&PIN_CATEGORY_PREVIEW,
			&MEDIATYPE_Stream, pVCap, NULL, pVmr);

		if (FAILED(hr))
		{
			OutputDebugString(TEXT("Cannot build MPEG2 preview graph!"));
		}
	}
	else
	{
		hr = pBuilder->RenderStream(&PIN_CATEGORY_PREVIEW,
			&MEDIATYPE_Interleaved, pVCap, NULL, pVmr);

		if (hr == VFW_S_NOPREVIEWPIN)
		{
			// preview was faked up for us using the (only) capture pin
			fPreviewFaked = TRUE;
		}
		else if (hr != S_OK)
		{
			// maybe it's DV?
			hr = pBuilder->RenderStream(&PIN_CATEGORY_PREVIEW,
				&MEDIATYPE_Video, pVCap, NULL, pVmr);

			if (hr == VFW_S_NOPREVIEWPIN)
			{
				// preview was faked up for us using the (only) capture pin
				fPreviewFaked = TRUE;
			}
			else if (hr != S_OK)
			{
				//OutputDebugString(TEXT("This graph cannot preview!"));
				fPreviewGraphBuilt = FALSE;
				return FALSE;
			}
		}

		//
		// Render the closed captioning pin? It could be a CC or a VBI category pin,
		// depending on the capture driver
		//

		/*if (fCapCC)
		{
			hr = pBuilder->RenderStream(&PIN_CATEGORY_CC, NULL,
				pVCap, NULL, NULL);
			if (hr != NOERROR)
			{
				hr = pBuilder->RenderStream(&PIN_CATEGORY_VBI, NULL,
					pVCap, NULL, NULL);
				if (hr != NOERROR)
				{
					OutputDebugString(TEXT("Cannot render closed captioning"));
				}
			}
		}*/
	}

	//
	// Get the preview window to be a child of our app's window
	//

	// This will find the IVideoWindow interface on the renderer.  It is
	// important to ask the filtergraph for this interface... do NOT use
	// ICaptureGraphBuilder2::FindInterface, because the filtergraph needs to
	// know we own the window so it can give us display changed messages, etc.

	//hr = pFg->QueryInterface(IID_IVideoWindow, (void **)&pVW);
	//if (hr != NOERROR)
	//{
	//	OutputDebugString(TEXT("This graph cannot preview properly"));
	//}
	//else
	//{
	//	//Find out if this is a DV stream
	//	AM_MEDIA_TYPE * pmtDV;

	//	if (pVSC && SUCCEEDED(pVSC->GetFormat(&pmtDV)))
	//	{
	//		if (pmtDV->formattype == FORMAT_DvInfo)
	//		{
	//			// in this case we want to set the size of the parent window to that of
	//			// current DV resolution.
	//			// We get that resolution from the IVideoWindow.
	//			SmartPtr<IBasicVideo> pBV;

	//			// If we got here, pVW is not NULL 
	//			ASSERT(pVW != NULL);
	//			hr = pVW->QueryInterface(IID_IBasicVideo, (void**)&pBV);

	//			if (SUCCEEDED(hr))
	//			{
	//				HRESULT hr1, hr2;
	//				long lWidth, lHeight;

	//				hr1 = pBV->get_VideoHeight(&lHeight);
	//				hr2 = pBV->get_VideoWidth(&lWidth);
	//				
	//			}
	//		}
	//	}

		// hr = CoCreateInstance(CLSID_VideoMixingRenderer, NULL,
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

		//hr = pFg->QueryInterface(IID_IVMRWindowlessControl, (void **)&g_pWc);
		//if (hr != NOERROR)
		//{
		//	OutputDebugString(TEXT("This graph cannot preview properly"));
		//}

		//RECT rc;
		//TEXTMETRIC tm;
		//HDC hdc = GetDC(NULL);
		//GetTextMetrics(hdc, &tm);

		//int gStatusStdHeight = tm.tmHeight * 3 / 2;
		//pVW->put_Owner((OAHWND)hwnd);    // We own the window now
		//pVW->put_WindowStyle(WS_CHILD);    // you are now a child

		//// give the preview window all our space but where the status bar is
		//GetClientRect(hwnd, &rc);
		//cyBorder = GetSystemMetrics(SM_CYBORDER);
		//cy = gStatusStdHeight + cyBorder;
		////rc.bottom -= cy;

		//pVW->SetWindowPosition(0, 0, rc.right, rc.bottom); // be this big
		//pVW->put_FullScreenMode(OATRUE);
		//pVW->put_Visible(OATRUE);

		long lWidth, lHeight;
	    hr = g_pWc->GetNativeVideoSize(&lWidth, &lHeight, NULL, NULL);
		if (SUCCEEDED(hr))
		{
			RECT rcSrc, rcDest;
			// Set the source rectangle.
			SetRect(&rcSrc, 0, 0, lWidth, lHeight);

			// Get the window client area.
			GetClientRect(hwnd, &rcDest);
			// Set the destination rectangle.
			SetRect(&rcDest, 0, 0, rcDest.right, rcDest.bottom);

			// Set the video position.
			hr = g_pWc->SetVideoPosition(&rcSrc, &rcDest);
		}
	

	// now tell it what frame rate to capture at.  Just find the format it
	// is capturing with, and leave everything alone but change the frame rate
	// No big deal if it fails.  It's just for preview
	// !!! Should we then talk to the preview pin?

	//if (pVSC && fUseFrameRate)
	//{
	//	hr = pVSC->GetFormat(&pmt);

	//	// DV capture does not use a VIDEOINFOHEADER
	//	if (hr == NOERROR)
	//	{
	//		if (pmt->formattype == FORMAT_VideoInfo)
	//		{
	//			VIDEOINFOHEADER *pvi = (VIDEOINFOHEADER *)pmt->pbFormat;
	//			pvi->AvgTimePerFrame = (LONGLONG)(10000000 / FrameRate);

	//			hr = pVSC->SetFormat(pmt);
	//			if (hr != NOERROR)
	//				OutputDebugString(L"%x: Cannot set frame rate for preview");
	//		}
	//		DeleteMediaType(pmt);
	//	}
	//}

	// make sure we process events while we're previewing!

	//hr = pFg->QueryInterface(IID_IMediaEventEx, (void **)&pME);

	/*if (hr == NOERROR)
	{
		pME->SetNotifyWindow((OAHWND)ghwndApp, WM_FGNOTIFY, 0);
	}*/

	// potential debug output - what the graph looks like
	// DumpGraph(pFg, 1);

	// Add our graph to the running object table, which will allow
	// the GraphEdit application to "spy" on our graph
//#ifdef REGISTER_FILTERGRAPH
//	hr = AddGraphToRot(pFg, &g_dwGraphRegister);
//	if (FAILED(hr))
//	{
//		OutputDebugString(TEXT("Failed to register filter graph with ROT!  hr=0x%x"));
//		g_dwGraphRegister = 0;
//	}
//#endif

	// All done.
	fPreviewGraphBuilt = TRUE;
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
	////// we have one already
	////if (pBuilder)
	////	return TRUE;

	//////pBuilder = new I()/*;*/

	//if (NULL == pBuilder)
	//{
	//	return FALSE;
	//}
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
	hr = pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE,
		&MEDIATYPE_Interleaved, pVCap,
		IID_IAMVideoCompression, (void **)&pVC);

	if (hr != S_OK)
	{
		hr = pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE,
			&MEDIATYPE_Video, pVCap,
			IID_IAMVideoCompression, (void **)&pVC);
	}

	// !!! What if this interface isn't supported?
	// we use this interface to set the frame rate and get the capture size
	hr = pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE,
		&MEDIATYPE_Interleaved,
		pVCap, IID_IAMStreamConfig, (void **)&pVSC);

	if (hr != NOERROR)
	{
		hr = pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE,
			&MEDIATYPE_Video, pVCap,
			IID_IAMStreamConfig, (void **)&pVSC);
		if (hr != NOERROR)
		{
			// this means we can't set frame rate (non-DV only)
			OutputDebugString(TEXT("Error %x: Cannot find VCapture:IAMStreamConfig"));
		}
	}

	//fCapAudioIsRelevant = TRUE;

	//AM_MEDIA_TYPE *pmt;

	//// default capture format
	//if (pVSC && pVSC->GetFormat(&pmt) == S_OK)
	//{
	//	// DV capture does not use a VIDEOINFOHEADER
	//	if (pmt->formattype == FORMAT_VideoInfo)
	//	{
	//		// resize our window to the default capture size
	//		ResizeWindow(HEADER(pmt->pbFormat)->biWidth,
	//			ABS(HEADER(pmt->pbFormat)->biHeight));
	//	}
	//	if (pmt->majortype != MEDIATYPE_Video)
	//	{
	//		// This capture filter captures something other that pure video.
	//		// Maybe it's DV or something?  Anyway, chances are we shouldn't
	//		// allow capturing audio separately, since our video capture
	//		// filter may have audio combined in it already!
	//		fCapAudioIsRelevant = FALSE;
	//		fCapAudio = FALSE;
	//	}
	//	DeleteMediaType(pmt);
	//}

	// we use this interface to bring up the 3 dialogs
	// NOTE:  Only the VfW capture filter supports this.  This app only brings
	// up dialogs for legacy VfW capture drivers, since only those have dialogs
	hr = pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE,
		&MEDIATYPE_Video, pVCap,
		IID_IAMVfwCaptureDialogs, (void **)&pDlg);

	// Use the crossbar class to help us sort out all the possible video inputs
	// The class needs to be given the capture filters ANALOGVIDEO input pin
	//{
	//	IPin        *pP = 0;
	//	IEnumPins   *pins = 0;
	//	ULONG        n;
	//	PIN_INFO     pinInfo;
	//	BOOL         Found = FALSE;
	//	IKsPropertySet *pKs = 0;
	//	GUID guid;
	//	DWORD dw;
	//	BOOL fMatch = FALSE;

	//	pCrossbar = NULL;

	//	if (SUCCEEDED(pVCap->EnumPins(&pins)))
	//	{
	//		while (!Found && (S_OK == pins->Next(1, &pP, &n)))
	//		{
	//			if (S_OK == pP->QueryPinInfo(&pinInfo))
	//			{
	//				if (pinInfo.dir == PINDIR_INPUT)
	//				{
	//					// is this pin an ANALOGVIDEOIN input pin?
	//					if (pP->QueryInterface(IID_IKsPropertySet,
	//						(void **)&pKs) == S_OK)
	//					{
	//						if (pKs->Get(AMPROPSETID_Pin,
	//							AMPROPERTY_PIN_CATEGORY, NULL, 0,
	//							&guid, sizeof(GUID), &dw) == S_OK)
	//						{
	//							if (guid == PIN_CATEGORY_ANALOGVIDEOIN)
	//								fMatch = TRUE;
	//						}
	//						pKs->Release();
	//					}

	//					if (fMatch)
	//					{
	//						HRESULT hrCreate = S_OK;
	//						pCrossbar = new CCrossbar(pP, &hrCreate);
	//						if (!pCrossbar || FAILED(hrCreate))
	//							break;

	//						hr = pCrossbar->GetInputCount(&NumberOfVideoInputs);
	//						Found = TRUE;
	//					}
	//				}
	//				pinInfo.pFilter->Release();
	//			}
	//			pP->Release();
	//		}
	//		pins->Release();
	//	}
	//}




	//hr = pmAudio->BindToObject(0, 0, IID_IBaseFilter, (void**)&pACap);

	//if (pACap == NULL)
	//{
	//	// there are no audio capture devices. We'll only allow video capture
	//	fCapAudio = FALSE;
	//	OutputDebugString(TEXT("Cannot create audio capture filter"));
	//	goto SkipAudio;
	//}

	////
	//// put the audio capture filter in the graph
	////
	//{
	//	WCHAR wachAudioFriendlyName[256];
	//	IPropertyBag *pBag;

	//	wachAudioFriendlyName[0] = 0;

	//	// Read the friendly name of the filter to assist with remote graph
	//	// viewing through GraphEdit
	//	hr = pmAudio->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
	//	if (SUCCEEDED(hr))
	//	{
	//		VARIANT var;
	//		var.vt = VT_BSTR;

	//		hr = pBag->Read(L"FriendlyName", &var, NULL);
	//		if (hr == NOERROR)
	//		{
	//			hr = StringCchCopyW(wachAudioFriendlyName, 256, var.bstrVal);
	//			SysFreeString(var.bstrVal);
	//		}

	//		pBag->Release();
	//	}

	//	// We'll need this in the graph to get audio property pages
	//	hr = pFg->AddFilter(pACap, wachAudioFriendlyName);
	//	if (hr != NOERROR)
	//	{
	//		OutputDebugString(TEXT("Error %x: Cannot add audio capture filter to filtergraph"));
	//		goto InitCapFiltersFail;
	//	}
	//}

	//// Calling FindInterface below will result in building the upstream
	//// section of the capture graph (any WDM TVAudio's or Crossbars we might
	//// need).

	//// !!! What if this interface isn't supported?
	//// we use this interface to set the captured wave format
	//hr = pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Audio, pACap,
	//	IID_IAMStreamConfig, (void **)&pASC);

	//if (hr != NOERROR)
	//{
	//	OutputDebugString(TEXT("Cannot find ACapture:IAMStreamConfig"));
	//}

//SkipAudio:

	// Can this filter do closed captioning?
	IPin *pPin;
	hr = pBuilder->FindPin(pVCap, PINDIR_OUTPUT, &PIN_CATEGORY_VBI,
		NULL, FALSE, 0, &pPin);
	if (hr != S_OK)
		hr = pBuilder->FindPin(pVCap, PINDIR_OUTPUT, &PIN_CATEGORY_CC,
			NULL, FALSE, 0, &pPin);
	if (hr == S_OK)
	{
		pPin->Release();
		fCCAvail = TRUE;
	}
	else
	{
		fCapCC = FALSE;    // can't capture it, then
	}

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


CAMERAPREVIEWDLL_API void capture_from_still_pin()
{

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
	SAFE_RELEASE(pVCap);
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

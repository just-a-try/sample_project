#include "SampleGrabber.h"

AM_MEDIA_TYPE g_StillMediaType;

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
		//MessageBox(NULL, L"", L"Capture", MB_OK);
		OutputDebugString(TEXT("BufferCb api!"));

		if ((g_StillMediaType.majortype != MEDIATYPE_Video) ||
			(g_StillMediaType.formattype != FORMAT_VideoInfo) ||
			(g_StillMediaType.cbFormat < sizeof(VIDEOINFOHEADER)) ||
			(g_StillMediaType.pbFormat == NULL))
		{
			return VFW_E_INVALIDMEDIATYPE;
		}
		HANDLE hf = CreateFile(L"D:\\Example.bmp", GENERIC_WRITE,
			FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, 0, NULL);
		if (hf == INVALID_HANDLE_VALUE)
		{
			return E_FAIL;
		}
		long cbBitmapInfoSize = g_StillMediaType.cbFormat - SIZE_PREHEADER;
		VIDEOINFOHEADER *pVideoHeader =
			(VIDEOINFOHEADER*)g_StillMediaType.pbFormat;

		BITMAPFILEHEADER bfh;
		ZeroMemory(&bfh, sizeof(bfh));
		bfh.bfType = 'MB';  // Little-endian for "BM".
		bfh.bfSize = sizeof(bfh) + BufferLen + cbBitmapInfoSize;
		bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + cbBitmapInfoSize;

		// Write the file header.
		DWORD dwWritten = 0;
		WriteFile(hf, &bfh, sizeof(bfh), &dwWritten, NULL);
		WriteFile(hf, HEADER(pVideoHeader), cbBitmapInfoSize, &dwWritten, NULL);
		WriteFile(hf, pBuffer, BufferLen, &dwWritten, NULL);
		CloseHandle(hf);
		return S_OK;

	}

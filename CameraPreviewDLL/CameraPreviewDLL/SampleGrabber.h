#pragma once
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

class SampleGrabberCallback : public ISampleGrabberCB
{
public:
	// Fake referance counting.
	STDMETHODIMP_(ULONG) AddRef();

	STDMETHODIMP_(ULONG) Release();

	STDMETHODIMP QueryInterface(REFIID riid, void **ppvObject);
	
	STDMETHODIMP SampleCB(double Time, IMediaSample *pSample);

	STDMETHODIMP BufferCB(double Time, BYTE *pBuffer, long BufferLen);
};
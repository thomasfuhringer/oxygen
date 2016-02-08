// VideoViewObject.c  | Oxygen © 2015 by Thomas Führinger
#include "Oxygen.h"
#pragma comment (lib, "strmiids.lib")

static OxClass* pOxClass;

// https://msdn.microsoft.com/en-us/library/ee492719.aspx
// https://msdn.microsoft.com/en-us/library/windows/desktop/ms686615(v=vs.85).aspx

// ToDo: integrate MP4 support: http://www.gdcl.co.uk/mpeg4/index.htm

OxVideoViewObject*
OxVideoView_New(OxWidgetObject* oxParent, OxRect* rc)
{
	OxVideoViewObject* ox = (OxVideoViewObject*)OxObject_Allocate(pOxClass);
	if (ox == NULL)
		return NULL;

	if (!OxBox_Init((OxWidgetObject*)ox, oxParent, rc))
		return NULL;
	return ox;
}

BOOL
OxVideoView_RenderFile(OxVideoViewObject* ox, char* sFile)
{
	BOOL bResult = TRUE;

	// Create the filter graph manager and query for interfaces.
	HRESULT hr = CoCreateInstance(&CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, &IID_IGraphBuilder, (void **)&ox->pGraph);
	if (FAILED(hr))
	{
		OxErr_SetFromWindowsHR(hr);
		bResult = FALSE;
	}
	else {

		hr = ox->pGraph->lpVtbl->QueryInterface(ox->pGraph, &IID_IMediaControl, (void**)&ox->pControl);
		hr = ox->pGraph->lpVtbl->QueryInterface(ox->pGraph, &IID_IVideoWindow, (void**)&ox->pVidWin);
		hr = ox->pGraph->lpVtbl->QueryInterface(ox->pGraph, &IID_IMediaEvent, (void **)&ox->pEvent);

		LPWSTR szFile = OxToW(sFile);
		hr = ox->pGraph->lpVtbl->RenderFile(ox->pGraph, szFile, NULL);
		OxFree(szFile);
		if (SUCCEEDED(hr))
		{
			//Set the video window.
			ox->pVidWin->lpVtbl->put_Owner(ox->pVidWin, (OAHWND)ox->hWin);
			ox->pVidWin->lpVtbl->put_WindowStyle(ox->pVidWin, WS_CHILD | WS_CLIPSIBLINGS);
			RECT rc;
			OxWidget_CalculateRect((OxWidgetObject*)ox, &rc);
			ox->pVidWin->lpVtbl->SetWindowPosition(ox->pVidWin, rc.left, rc.top, rc.right, rc.bottom);

			// Run the graph.
			hr = ox->pControl->lpVtbl->Run(ox->pControl);
			if (SUCCEEDED(hr))
			{
				// Wait for completion. Using INFINITE blocks indefinitely!
				long evCode;
				//ox->pEvent->lpVtbl->WaitForCompletion(ox->pEvent, INFINITE, &evCode);

			}
			else {
				OxErr_SetFromWindowsHR(hr);
				bResult = FALSE;
			}
		}
		else {
			OxErr_SetFromWindowsHR(hr);
			bResult = FALSE;
		}
	}
	return bResult;
}

static BOOL
OxVideoView_Reposition(OxVideoViewObject* ox)
{
	OxRect rc;
	OxWidget_CalculateRect(ox, &rc);

	if (MoveWindow(ox->hWin, rc.iLeft, rc.iTop, rc.iWidth, rc.iHeight, TRUE) == 0) {
		OxErr_SetFromWindows();
		return FALSE;
	}

	if (!GetClientRect(ox->hWin, &rc)){
		OxErr_SetFromWindows();
		return FALSE;
	}

	if (ox->pVidWin)
		ox->pVidWin->lpVtbl->SetWindowPosition(ox->pVidWin, 0, 0, rc.iWidth, rc.iHeight);
	return TRUE;
}

static BOOL
OxVideoView_Delete(OxVideoViewObject* ox)
{
	if (ox->pControl) {
		ox->pControl->lpVtbl->Stop(ox->pControl);
		ox->pControl->lpVtbl->Release(ox->pControl);
		ox->pVidWin->lpVtbl->put_Visible(ox->pVidWin, OAFALSE);
		ox->pVidWin->lpVtbl->put_Owner(ox->pVidWin, NULL);
		ox->pVidWin->lpVtbl->Release(ox->pVidWin);
		ox->pGraph->lpVtbl->Release(ox->pGraph);
	}
	return pOxClass->pBase->fnDelete((OxObject*)ox);
}

OxClass OxVideoViewClass = {
	"VideoView",               /* sName */
	&OxBoxClass,               /* pBase */
	sizeof(OxVideoViewObject), /* nSize */
	OxVideoView_Delete,        /* fnDelete */
	NULL,                      /* pUserData */
	0                          /* aMethods */
};

BOOL OxVideoViewClass_Init()
{
	pOxClass = &OxVideoViewClass;
	Ox_INHERIT_METHODS(pOxClass);
	pOxClass->aMethods[OxWIDGET_REPOSITION] = OxVideoView_Reposition;
	return TRUE;
}
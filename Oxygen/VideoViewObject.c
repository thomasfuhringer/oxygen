// VideoViewObject.c  | Oxygen © 2015 by Thomas Führinger
#include "Oxygen.h"
#pragma comment (lib, "strmiids.lib")

static OxClass* pOxClass;
static LRESULT CALLBACK OxVideoViewProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
static HBITMAP hPlaceholderBitmap;
static LPWSTR szClass = L"OxVideoViewClass";

// https://msdn.microsoft.com/en-us/library/ee492719.aspx
// https://msdn.microsoft.com/en-us/library/windows/desktop/ms686615(v=vs.85).aspx

OxVideoViewObject*
OxVideoView_New(OxWidgetObject* oxParent, OxRect* rc)
{
	OxVideoViewObject* ox = (OxVideoViewObject*)OxObject_Allocate(pOxClass);
	if (ox == NULL)
		return NULL;

	if (!OxWidget_Init((OxWidgetObject*)ox, oxParent, rc))
		return NULL;
	/*
		ox->bStretch = TRUE;
		ox->bFill = FALSE;

		OxWidget_CalculateRect((OxWidgetObject*)ox, rc);
		ox->hWin = CreateWindowExW(0, szClass, L"", // WS_EX_CLIENTEDGE
		WS_CHILD | WS_VISIBLE | WS_EX_CLIENTEDGE,
		rc->iLeft, rc->iTop, rc->iWidth, rc->iHeight,
		oxParent->hWin, (HMENU)IDC_OXIMAGEVIEW, OxApp->hInstance, NULL);

		if (ox->hWin == NULL) {
		OxErr_SetFromWindows();
		return NULL;
		}

		ox->fnOldWinProcedure = (WNDPROC)SetWindowLongPtrW(ox->hWin, GWLP_WNDPROC, (LONG_PTR)OxVideoViewProc);
		SetWindowLongPtrW(ox->hWin, GWLP_USERDATA, (LONG_PTR)ox);
		*/
	return ox;
}

BOOL
OxVideoView_RenderFile(OxVideoViewObject* ox, char* sFile)
{
	BOOL bResult = TRUE;


	// Create the filter graph manager and query for interfaces.
	HRESULT hr = CoCreateInstance(&CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, &IID_IGraphBuilder, (void **)&ox->pGraph);
	//hr = CoCreateInstance(&CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, &IID_IGraphBuilder, (void **)&pGraph);
	if (FAILED(hr))
	{
		OxErr_SetStringFormat(OxERROR_WINDOWS, "Can not create COM object. Error %d", hr);
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
			ox->pVidWin->lpVtbl->put_Owner(ox->pVidWin, (OAHWND)ox->oxParent->hWin);
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
				OxErr_SetStringFormat(OxERROR_WINDOWS, "Can not run COM object. Error %d", hr);
				bResult = FALSE;
			}
		}
		else {
			OxErr_SetStringFormat(OxERROR_WINDOWS, "Can not render COM object. Error %d", hr);
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
	ox->pVidWin->lpVtbl->SetWindowPosition(ox->pVidWin, rc.iLeft, rc.iTop, rc.iWidth, rc.iHeight);
	return TRUE;
}

static BOOL
OxVideoView_Delete(OxVideoViewObject* ox)
{
	//DestroyWindow(ox->hWin);
	ox->pVidWin->lpVtbl->put_Visible(ox->pVidWin, OAFALSE);
	ox->pVidWin->lpVtbl->put_Owner(ox->pVidWin, NULL);
	ox->pControl->lpVtbl->Release(ox->pControl);
	ox->pVidWin->lpVtbl->Release(ox->pVidWin);
	ox->pGraph->lpVtbl->Release(ox->pGraph);
	return pOxClass->pBase->fnDelete((OxObject*)ox);
}

OxClass OxVideoViewClass = {
	"VideoView",               /* sName */
	&OxWidgetClass,            /* pBase */
	sizeof(OxVideoViewObject), /* nSize */
	OxVideoView_Delete,        /* fnDelete */
	NULL,                      /* pUserData */
	0                          /* aMethods */
};

LRESULT CALLBACK OxVideoViewProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL OxVideoViewClass_Init()
{
	/*
		WNDCLASSEX wc;
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = OxVideoViewProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = OxApp->hInstance;
		wc.hIcon = NULL;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wc.lpszMenuName = NULL;
		wc.lpszClassName = szClass;
		wc.hIconSm = NULL;

		if (!RegisterClassExW(&wc))
		{
		OxErr_SetFromWindows();
		return FALSE;
		}

		hPlaceholderBitmap = (HBITMAP)LoadImageW(OxApp->hDLL, MAKEINTRESOURCE(IDB_PLACEHOLDER), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);
		if (hPlaceholderBitmap == NULL){
		OxErr_SetFromWindows();
		return FALSE;
		}
		*/
	pOxClass = &OxVideoViewClass;
	Ox_INHERIT_METHODS(pOxClass);
	pOxClass->aMethods[OxWIDGET_REPOSITION] = OxVideoView_Reposition;
	return TRUE;
}
/*
static LRESULT CALLBACK
OxVideoViewProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
static int cxClient, cyClient;
PAINTSTRUCT paintStruct;
HDC hDC;
OxVideoViewObject* ox = (OxButtonObject*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);

switch (uMsg)
{
case WM_PAINT:

case WM_SIZE:
cxClient = LOWORD(lParam);
cyClient = HIWORD(lParam);
return 0;
}
return DefParentProc(hwnd, uMsg, wParam, lParam, FALSE);
}
*/
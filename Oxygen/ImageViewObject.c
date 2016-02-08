// ImageViewObject.c  | Oxygen © 2015 by Thomas Führinger
#include "Oxygen.h"

static OxClass* pOxClass;
static LRESULT CALLBACK OxImageViewProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
static BITMAP bitmap;
static HBITMAP hPlaceholderBitmap;
static LPWSTR szClass = L"OxImageViewClass";

OxImageViewObject*
OxImageView_New(OxWidgetObject* oxParent, OxRect* rc, OxImageObject* oxImage)
{
	OxImageViewObject* ox = (OxImageViewObject*)OxObject_Allocate(pOxClass);
	if (ox == NULL)
		return NULL;

	if (!OxWidget_Init((OxWidgetObject*)ox, oxParent, rc))
		return NULL;

	ox->oxData = oxImage;
	ox->bStretch = TRUE;
	ox->bFill = FALSE;

	OxRect rect;
	rc = &rect;

	OxWidget_CalculateRect((OxWidgetObject*)ox, rc);
	ox->hWin = CreateWindowExW(0, szClass, L"", // WS_EX_CLIENTEDGE
		WS_CHILD | WS_VISIBLE,
		rc->iLeft, rc->iTop, rc->iWidth, rc->iHeight,
		oxParent->hWin, (HMENU)IDC_OXIMAGEVIEW, OxApp->hInstance, NULL);

	if (ox->hWin == NULL) {
		OxErr_SetFromWindows();
		return NULL;
	}

	SendMessageW(ox->hWin, WM_SETFONT, (WPARAM)OxApp->hDefaultFont, MAKELPARAM(FALSE, 0));
	ox->fnOldWinProcedure = (WNDPROC)SetWindowLongPtrW(ox->hWin, GWLP_WNDPROC, (LONG_PTR)OxImageViewProc);
	SetWindowLongPtrW(ox->hWin, GWLP_USERDATA, (LONG_PTR)ox);
	return ox;
}

BOOL
OxImageView_RenderData(OxImageViewObject* ox, BOOL bEdit)
{
	InvalidateRect(ox->hWin, NULL, TRUE);
	return TRUE;
}

BOOL
OxImageView_SetData(OxImageViewObject* ox, OxObject* oxData)
{
	if (!OxWidget_SetData((OxWidgetObject*)ox, oxData))
		return FALSE;

	return OxImageView_RenderData(ox, FALSE);
}

static BOOL
OxImageView_Delete(OxImageViewObject* ox)
{
	DestroyWindow(ox->hWin);
	return pOxClass->pBase->fnDelete((OxObject*)ox);
}

OxClass OxImageViewClass = {
	"ImageView",               /* sName */
	&OxWidgetClass,            /* pBase */
	sizeof(OxImageViewObject), /* nSize */
	OxImageView_Delete,        /* fnDelete */
	NULL,                      /* pUserData */
	0                          /* aMethods */
};

LRESULT CALLBACK OxImageViewProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL OxImageViewClass_Init()
{
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = OxImageViewProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = OxApp->hInstance;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(OxWINDOWBKGCOLOR + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = szClass;
	wc.hIconSm = NULL;

	if (!RegisterClassExW(&wc))
	{
		OxErr_SetFromWindows();
		return FALSE;
	}

#ifndef OxSTATIC
	hPlaceholderBitmap = (HBITMAP)LoadImageW(OxApp->hDLL, MAKEINTRESOURCE(IDB_PLACEHOLDER), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);
	if (hPlaceholderBitmap == NULL){
		OxErr_SetFromWindows();
		return FALSE;
	}
#else
	hPlaceholderBitmap = 0;
#endif

	pOxClass = &OxImageViewClass;
	Ox_INHERIT_METHODS(pOxClass);
	pOxClass->aMethods[OxWIDGET_SETDATA] = OxImageView_SetData;
	return TRUE;
}

static LRESULT CALLBACK
OxImageViewProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static int cxClient, cyClient;
	PAINTSTRUCT paintStruct;
	HDC hDC;
	OxImageViewObject* ox = (OxButtonObject*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);

	switch (uMsg)
	{
	case WM_PAINT:
		if (ox->oxData) {
			OxImageObject* oxImage = (OxImageObject*)ox->oxData;
			if (oxImage->iType == OxIMAGEFORMAT_BMP) {
				hDC = BeginPaint(hwnd, &paintStruct);
				SetDCBrushColor(hDC, ox->oxParent->hBkgBrush);
				HDC hdcMem = CreateCompatibleDC(hDC);
				HBITMAP hbmOld = SelectObject(hdcMem, oxImage->hWin);
				GetObject(oxImage->hWin, sizeof(BITMAP), &bitmap);
				if (ox->bFill)
					StretchBlt(hDC, 0, 0, cxClient, cyClient, hdcMem, 0, 0, bitmap.bmWidth, bitmap.bmHeight, SRCCOPY);
				else if (ox->bStretch) {
					double dAspectRatio = (double)cxClient / cyClient;
					double dPictureAspectRatio = (double)bitmap.bmWidth / bitmap.bmHeight;

					if (dPictureAspectRatio > dAspectRatio)
					{
						int nNewHeight = (int)(cxClient / dPictureAspectRatio);
						int iCenteringOffset = (cyClient - nNewHeight) / 2;
						StretchBlt(hDC, 0, iCenteringOffset, cxClient, nNewHeight, hdcMem, 0, 0, bitmap.bmWidth, bitmap.bmHeight, SRCCOPY);
					}
					else if (dPictureAspectRatio < dAspectRatio)
					{
						int nNewWidth = (int)(cyClient * dPictureAspectRatio);
						int iCenteringOffset = (cxClient - nNewWidth) / 2;
						StretchBlt(hDC, iCenteringOffset, 0, nNewWidth, cyClient, hdcMem, 0, 0, bitmap.bmWidth, bitmap.bmHeight, SRCCOPY);
					}
					else
						StretchBlt(hDC, 0, 0, cxClient, cyClient, hdcMem, 0, 0, bitmap.bmWidth, bitmap.bmHeight, SRCCOPY);
				}
				else
					StretchBlt(hDC, 0, 0, bitmap.bmWidth, bitmap.bmHeight, hdcMem, 0, 0, bitmap.bmWidth, bitmap.bmHeight, SRCCOPY);
				SelectObject(hdcMem, hbmOld);
				EndPaint(oxImage->hWin, &paintStruct);
			}
			else if (oxImage->iType == OxIMAGEFORMAT_ICO) {
				hDC = BeginPaint(hwnd, &paintStruct);
				DrawIcon(hDC, 0, 0, oxImage->hWin);

				EndPaint(hwnd, &paintStruct);
			}
		}
		else {
#ifndef OxSTATIC
			hDC = BeginPaint(hwnd, &paintStruct);
			HDC hdcMem = CreateCompatibleDC(hDC);

			HBITMAP hbmOld = SelectObject(hdcMem, hPlaceholderBitmap);
			GetObject(hPlaceholderBitmap, sizeof(BITMAP), &bitmap);
			StretchBlt(hDC, 0, 0, cxClient, cyClient, hdcMem, 0, 0, bitmap.bmWidth, bitmap.bmHeight, SRCCOPY);

			SelectObject(hdcMem, hbmOld);
			EndPaint(hwnd, &paintStruct);
#endif
		}
		break;

	case WM_SIZE:
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);
		return 0;
	}
	return DefParentProc(hwnd, uMsg, wParam, lParam, FALSE);
}

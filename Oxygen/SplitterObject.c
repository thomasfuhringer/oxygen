// SplitterObject.c  | Oxygen © 2015 by Thomas Führinger
#include "Oxygen.h"

static OxClass* pOxClass;

static HCURSOR hCursorWestEast;
static HCURSOR hCursorNorthSouth;
static BOOL OxSplitter_ResizeBoxes(OxSplitterObject* ox);
static LRESULT CALLBACK OxSplitterProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

OxSplitterObject*
OxSplitter_New(OxWidgetObject* oxParent, OxRect* rc, int iPosition)
{
	OxSplitterObject* ox = (OxSplitterObject*)OxObject_Allocate(pOxClass);
	if (ox == NULL)
		return NULL;

	ox->bVertical = TRUE;
	ox->iPosition = iPosition;
	ox->iSpacing = 4;

	OxBox_Init(ox, oxParent, rc);

	ox->oxBox1 = OxBox_New(ox, rc);
	ox->oxBox2 = OxBox_New(ox, rc);

	SetWindowLongPtr(ox->oxBox1->hWin, GWL_EXSTYLE, WS_EX_CLIENTEDGE);
	SetWindowLongPtr(ox->oxBox2->hWin, GWL_EXSTYLE, WS_EX_CLIENTEDGE);
	OxSplitter_ResizeBoxes(ox);

	ox->fnOldWinProcedure = (WNDPROC)SetWindowLongPtrW(ox->hWin, GWLP_WNDPROC, (LONG_PTR)OxSplitterProc);
	return ox;
}

static BOOL
OxSplitter_ResizeBoxes(OxSplitterObject* ox)
{
	ox->oxBox1->rc = (OxRect){ 0, 0, ox->bVertical ? ox->iPosition - ox->iSpacing / 2 : 0, ox->bVertical ? 0 : ox->iPosition - ox->iSpacing / 2 };
	ox->oxBox2->rc = (OxRect){ ox->bVertical ? ox->iPosition + ox->iSpacing / 2 : 0, ox->bVertical ? 0 : ox->iPosition + ox->iSpacing / 2, 0, 0 };
	EnumChildWindows(ox->hWin, OxSizeEnumProc, 0);
	return TRUE;
}

BOOL
OxSplitter_SetPosition(OxSplitterObject* ox, int iPosition)
{
	ox->iPosition = iPosition;
	return OxSplitter_ResizeBoxes(ox);
}

static BOOL
OxSplitter_Delete(OxSplitterObject* ox)
{
	OxREL(ox->oxBox1);
	OxREL(ox->oxBox2);
	return pOxClass->pBase->fnDelete(ox);
}

OxClass OxSplitterClass = {
	"Splitter",                /* sName */
	&OxBoxClass,               /* pBase */
	sizeof(OxSplitterObject),  /* nSize */
	OxSplitter_Delete,         /* fnDelete */
	NULL,                      /* pUserData */
	0                          /* aMethods */
};


BOOL
OxSplitterClass_Init()
{
	hCursorWestEast = LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZEWE));
	hCursorNorthSouth = LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZENS));

	pOxClass = &OxSplitterClass;
	Ox_INHERIT_METHODS(pOxClass);
	return TRUE;
}

static LRESULT CALLBACK
OxSplitterProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static BOOL	bSplitterMoving = FALSE;
	OxSplitterObject* ox = (OxSplitterObject*)GetWindowLongPtr(hwnd, GWLP_USERDATA);

	switch (uMsg)
	{
	case WM_MOUSEMOVE:
		if ((ox->bVertical ? LOWORD(lParam) : HIWORD(lParam)) > 10)
		{
			SetCursor(ox->bVertical ? hCursorWestEast : hCursorNorthSouth);
			RECT rect;
			if ((wParam == MK_LBUTTON) && bSplitterMoving) {
				if (ox->bVertical) {
					GetClientRect(hwnd, &rect);
					if (LOWORD(lParam) > rect.right)
						return 0;
					ox->iPosition = ox->iPosition > 0 ? LOWORD(lParam) : LOWORD(lParam) - rect.right;
				}
				else{
					GetClientRect(hwnd, &rect);
					if (HIWORD(lParam) > rect.bottom)
						return 0;
					ox->iPosition = (ox->iPosition > 0) ? HIWORD(lParam) : HIWORD(lParam) - rect.bottom;
				}
				OxSplitter_ResizeBoxes(ox);
			}
		}
		return 0;

	case WM_LBUTTONDOWN:
		bSplitterMoving = TRUE;
		SetCapture(hwnd);
		return 0;

	case WM_LBUTTONUP:
		ReleaseCapture();
		bSplitterMoving = FALSE;
		return 0;
	}
	return CallWindowProc(ox->fnOldWinProcedure, hwnd, uMsg, wParam, lParam);
}
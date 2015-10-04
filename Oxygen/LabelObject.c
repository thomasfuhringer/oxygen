// LabelObject.c  | Oxygen © 2015 by Thomas Führinger
#include "Oxygen.h"

static OxClass* pOxClass;
static LRESULT CALLBACK OxLabelProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

OxLabelObject*
OxLabel_New(OxWidgetObject* oxParent, OxRect* rc, char* sCaption)
{
	OxLabelObject* ox = (OxLabelObject*)OxObject_Allocate(pOxClass);
	if (ox == NULL)
		return NULL;

	if (!OxWidget_Init((OxWidgetObject*)ox, oxParent, rc))
		return NULL;

	ox->oxAssociatedWidget = NULL;
	ox->sFormat = NULL;
	ox->textColor = 0;
	ox->fContentAlignment = 0;

	LPWSTR szCaption = OxToW(sCaption); //...

	OxWidget_CalculateRect((OxWidgetObject*)ox, rc);
	ox->hWin = CreateWindowExW(0, L"STATIC", szCaption,// L"",//
		WS_CHILD | WS_VISIBLE,
		rc->iLeft, rc->iTop, rc->iWidth, rc->iHeight,
		oxParent->hWin, (HMENU)IDC_OXLABEL, OxApp->hInstance, NULL);
	OxFree(szCaption);

	if (ox->hWin == NULL) {
		OxErr_SetFromWindows();
		return NULL;
	}

	SendMessageW(ox->hWin, WM_SETFONT, (WPARAM)OxApp->hDefaultFont, MAKELPARAM(FALSE, 0));
	ox->fnOldWinProcedure = (WNDPROC)SetWindowLongPtrW(ox->hWin, GWLP_WNDPROC, (LONG_PTR)OxLabelProc);
	SetWindowLongPtrW(ox->hWin, GWLP_USERDATA, (LONG_PTR)ox);

	return ox;
}

BOOL
OxLabel_Align(OxLabelObject* ox, unsigned int fAlignment)
{
	LONG_PTR pAlignment = GetWindowLongPtr(ox->hWin, GWL_STYLE);

	switch (fAlignment & OxALIGN_HORIZ)
	{
	case OxALIGN_HORIZ_LEFT:
		pAlignment = pAlignment | SS_LEFT;
		break;
	case OxALIGN_HORIZ_CENTER:
		pAlignment = pAlignment | SS_CENTER;
		break;
	case OxALIGN_HORIZ_RIGHT:
		pAlignment = pAlignment | SS_RIGHT;
		break;
	}

	switch (fAlignment & OxALIGN_VERT)
	{
	case OxALIGN_VERT_TOP:
		//	pAlignment = pAlignment | SS_TOP;
		break;
	case OxALIGN_VERT_CENTER:
		pAlignment = pAlignment | SS_CENTERIMAGE;
		break;
	case OxALIGN_VERT_BOTTOM:
		//	pAlignment = pAlignment | SS_BOTTOM;
		break;
	}
	SetWindowLongPtr(ox->hWin, GWL_STYLE, pAlignment);
	return TRUE;
}

BOOL
OxLabel_RenderData(OxLabelObject* ox, BOOL bEdit)
{
	char* sText;
	if (ox->oxData == NULL) {
		SendMessageW(ox->hWin, WM_SETTEXT, 0, (LPARAM)L"");
		return TRUE;
	}

	if ((sText = OxObject_AsStringV(ox->oxData, ox->sFormat)) == NULL)
		return FALSE;

	LPWSTR szText = OxToW(sText);
	SendMessage(ox->hWin, WM_SETTEXT, 0, (LPARAM)szText);
	OxFree(szText);
	OxFree(sText);
	return TRUE;
}

BOOL
OxLabel_SetData(OxLabelObject* ox, OxObject* oxData)
{
	if (!OxWidget_SetData((OxWidgetObject*)ox, oxData))
		return FALSE;

	return OxLabel_RenderData(ox, FALSE);
}

BOOL
OxLabel_SetTextColor(OxLabelObject* ox, int iRed, int iGreen, int iBlue)
{
	ox->textColor = RGB(iRed, iGreen, iBlue);
	return TRUE;
}

static BOOL
OxLabel_Delete(OxLabelObject* ox)
{
	DestroyWindow(ox->hWin);
	return pOxClass->pBase->fnDelete((OxObject*)ox);
}

OxClass OxLabelClass = {
	"Label",                   /* sName */
	&OxWidgetClass,            /* pBase */
	sizeof(OxLabelObject),     /* nSize */
	OxLabel_Delete,            /* fnDelete */
	NULL,                      /* pUserData */
	0                          /* aMethods */
};

BOOL
OxLabelClass_Init()
{
	pOxClass = &OxLabelClass;
	Ox_INHERIT_METHODS(pOxClass);
	pOxClass->aMethods[OxWIDGET_SETDATA] = OxLabel_SetData;
	return TRUE;
}

static LRESULT CALLBACK
OxLabelProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	OxLabelObject* ox = (OxLabelObject*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);

	switch (msg)
	{
	case OCM_CTLCOLORSTATIC:
		if (ox->textColor) {
			HDC hDC = (HDC)wParam;
			SetBkMode(hDC, TRANSPARENT);
			SetTextColor(hDC, ox->textColor);
			return (BOOL)GetSysColorBrush(COLOR_MENU);
		}
		break;

	default:
		break;
	}
	return CallWindowProcW(ox->fnOldWinProcedure, hwnd, msg, wParam, lParam);
}
// WidgetObject.c  | Oxygen © 2015 by Thomas Führinger
#include "Oxygen.h"

static OxClass* pOxClass;

OxWidgetObject*
OxWidget_New()
{
	OxWidgetObject* ox = (OxWidgetObject*)OxObject_Allocate(pOxClass);
	if (ox != NULL) {
		return ox;
	}
	else
		return NULL;
}

BOOL
OxWidget_Init(OxWidgetObject* ox, OxWidgetObject* oxParent, OxRect* rc)
{
	ox->oxParent = oxParent;
	if (ox->oxParent)
		ox->oxWindow = oxParent->oxWindow;
	else
		ox->oxWindow = NULL;

	if (rc == NULL) {
		ox->rc.iLeft = CW_USEDEFAULT;
		ox->rc.iTop = CW_USEDEFAULT;
		ox->rc.iWidth = CW_USEDEFAULT;
		ox->rc.iHeight = CW_USEDEFAULT;
	}
	else {
		ox->rc.iLeft = rc->iLeft;
		ox->rc.iTop = rc->iTop;
		ox->rc.iWidth = rc->iWidth;
		ox->rc.iHeight = rc->iHeight;
	}
	ox->fnOldWinProcedure = NULL;
	ox->bReadOnly = FALSE;
	ox->oxData = OxNull;
	ox->pDataType = &OxStringClass;
	ox->oxLabel = NULL;
	ox->fnOnDataChangedCB = NULL;
	ox->pUserData = NULL;
	return TRUE;
}

BOOL
OxWidget_CalculateRect(OxWidgetObject* ox, OxRect* rc)
{
	RECT rcParent;
	GetClientRect(ox->oxParent == NULL ? GetDesktopWindow() : ox->oxParent->hWin, &rcParent);
	if (ox->rc.iLeft == OxWIDGET_CENTER)
		rc->iLeft = (rcParent.right - ox->rc.iWidth) / 2;
	else
		rc->iLeft = (ox->rc.iLeft >= 0 || ox->rc.iLeft == CW_USEDEFAULT ? ox->rc.iLeft : rcParent.right + ox->rc.iLeft);
	rc->iWidth = (ox->rc.iWidth > 0 || ox->rc.iWidth == CW_USEDEFAULT ? ox->rc.iWidth : rcParent.right + ox->rc.iWidth - rc->iLeft);
	if (ox->rc.iTop == OxWIDGET_CENTER)
		rc->iTop = (rcParent.bottom - ox->rc.iHeight) / 2;
	else
		rc->iTop = (ox->rc.iTop >= 0 || ox->rc.iTop == CW_USEDEFAULT ? ox->rc.iTop : rcParent.bottom + ox->rc.iTop);
	rc->iHeight = (ox->rc.iHeight > 0 || ox->rc.iHeight == CW_USEDEFAULT ? ox->rc.iHeight : rcParent.bottom + ox->rc.iHeight - rc->iTop);
	return TRUE;
}

BOOL
OxWidget_SetCaption(OxWidgetObject* ox, char* sText)
{
	LPWSTR szText = OxToW(sText);
	if (SendMessage(ox->hWin, WM_SETTEXT, (WPARAM)0, (LPARAM)szText)) {
		OxFree(szText);
		return TRUE;
	}
	else {
		return TRUE; // something's not working with SendMessage
		OxErr_SetFromWindows();
		OxFree(szText);
		return FALSE;
	}
}

BOOL
OxWidget_SetCaptionV(OxWidgetObject* ox, char* sText)
{
	BOOL(*Method)(OxWidgetObject*, char* sText) = (BOOL(*)(OxWidgetObject*, char* sText))ox->pClass->aMethods[OxWIDGET_SETCAPTION];
	return Method(ox, sText);
}

BOOL
OxWidget_SetData(OxWidgetObject* ox, OxObject* oxData)
{
	if (OxObject_CompareV(ox->oxData, oxData, OxEQ))
		return TRUE;

	OxAttachObject(&ox->oxData, oxData, FALSE);

	if (ox->fnOnDataChangedCB)
		return ox->fnOnDataChangedCB(ox);
	return TRUE;
}

BOOL
OxWidget_SetDataV(OxWidgetObject* ox, OxObject* oxData)
{
	BOOL(*Method)(OxWidgetObject*, OxObject*) = (BOOL(*)(OxWidgetObject*, OxObject*))ox->pClass->aMethods[OxWIDGET_SETDATA];
	return Method(ox, oxData);
}

BOOL CALLBACK
OxSizeEnumProc(HWND hwndChild, LPARAM lParam)
{
	//OxWidgetObject* oxParent = (OxWidgetObject*)lParam;
	OxWidgetObject* oxWidget = (OxWidgetObject*)GetWindowLongPtr(hwndChild, GWLP_USERDATA);
	if (oxWidget && IsWindow(oxWidget->hWin))  // IWebBrowser2 creates some weird window in the top level window
	{
		//char* sString = OxObject_RepresentV(oxWidget); OutputDebugStringA(sString); OxFree(sString); OutputDebugStringA("\n");
		return OxWidget_RepositionV(oxWidget);
	}
	else
		return TRUE;
}

BOOL
OxWidget_Reposition(OxWidgetObject* ox)
{
	OxRect rc;
	OxWidget_CalculateRect(ox, &rc);
	if (MoveWindow(ox->hWin, rc.iLeft, rc.iTop, rc.iWidth, rc.iHeight, TRUE) == 0) {
		OxErr_SetFromWindows();
		return FALSE;
	}
	else
		EnumChildWindows(ox->hWin, OxSizeEnumProc, (LPARAM)ox);
	return TRUE;
}

BOOL
OxWidget_RepositionV(OxWidgetObject* ox)
{
	BOOL(*Method)(OxWidgetObject*) = (BOOL(*)(OxWidgetObject*))ox->pClass->aMethods[OxWIDGET_REPOSITION];
	return Method(ox);
}

BOOL
OxWidget_Show(OxWidgetObject* ox, BOOL bVisible)
{
	ShowWindow(ox->hWin, bVisible ? SW_SHOW : SW_HIDE);
	return TRUE;
}

BOOL
OxWidget_ShowV(OxWidgetObject* ox, BOOL bVisible)
{
	BOOL(*Method)(OxWidgetObject*) = (BOOL(*)(OxWidgetObject*))ox->pClass->aMethods[OxWIDGET_SHOW];
	return Method(ox, bVisible);
}

BOOL
OxWidget_RenderFocus(OxWidgetObject* ox)
{
	OxErr_SetString(OxERROR_RUNTIME, "OxWidget_RenderFocus is a virtual method.");
	return -1;
}

BOOL
OxWidget_RenderFocusV(OxWidgetObject* ox)
{
	BOOL(*Method)(OxWidgetObject*) = (BOOL(*)(OxWidgetObject*))ox->pClass->aMethods[OxWIDGET_RENDERFOCUS];
	return Method(ox);
}

char*
OxWidget_Represent(OxWidgetObject* ox)
{
	char* sString = (char*)OxAllocate(250 * sizeof(char));
	int iIndex = sprintf_s(sString, 250, "OxWidgetObject @ %p, %d times referenced", ox, ox->iRefCount);
	return sString;
}

static BOOL
OxWidget_BeforeDelete(OxWidgetObject* ox)
{
	return TRUE;
}

static BOOL CALLBACK
OxWidgetReleaseEnumProc(HWND hwndChild, LPARAM lParam)
{
	OxWidgetObject* oxWidget = (OxWidgetObject*)GetWindowLongPtr(hwndChild, GWLP_USERDATA);
	if (oxWidget && !OxREL(oxWidget))
		return FALSE;
	else
		return TRUE;
}

static BOOL
OxWidget_ReleaseChildren(OxWidgetObject* ox)
{
	if (ox->hWin)
		EnumChildWindows(ox->hWin, OxWidgetReleaseEnumProc, 0);
	return TRUE;
}

static BOOL
OxWidget_Delete(OxWidgetObject* ox)
{
	if (!(BOOL(*))ox->pClass->aMethods[OxWIDGET_BEFOREDELETE](ox))
		return FALSE;
	OxREL(ox->oxLabel);
	OxREL(ox->oxData);
	if (!OxWidget_ReleaseChildren(ox))
		return FALSE;
	return pOxClass->pBase->fnDelete(ox);
}

OxClass OxWidgetClass = {
	"Widget",                  /* sName */
	&OxObjectClass,            /* pBase */
	sizeof(OxWidgetObject),    /* nSize */
	OxWidget_Delete,           /* fnDelete */
	NULL,                      /* pUserData */
	0,                         /* aMethods */
};

BOOL
OxWidgetClass_Init()
{
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(INITCOMMONCONTROLSEX);
	InitCtrls.dwICC = ICC_COOL_CLASSES | ICC_BAR_CLASSES | ICC_TAB_CLASSES | ICC_LISTVIEW_CLASSES;// | ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	pOxClass = &OxWidgetClass;
	Ox_INHERIT_METHODS(pOxClass);
	pOxClass->aMethods[OxOBJECT_REPRESENT] = OxWidget_Represent;
	pOxClass->aMethods[OxWIDGET_SETDATA] = OxWidget_SetData;
	pOxClass->aMethods[OxWIDGET_SETCAPTION] = OxWidget_SetCaption;
	pOxClass->aMethods[OxWIDGET_SHOW] = OxWidget_Show;
	pOxClass->aMethods[OxWIDGET_RENDERFOCUS] = OxWidget_RenderFocus;
	pOxClass->aMethods[OxWIDGET_REPOSITION] = OxWidget_Reposition;
	pOxClass->aMethods[OxWIDGET_BEFOREDELETE] = OxWidget_BeforeDelete;
	return TRUE;
}
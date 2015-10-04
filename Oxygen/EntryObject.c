// EntryObject.c  | Oxygen © 2015 by Thomas Führinger
#include "Oxygen.h"

static OxClass* pOxClass;
static LRESULT CALLBACK OxEntryProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

OxEntryObject*
OxEntry_New(OxWidgetObject* oxParent, OxRect* rc)
{
	OxEntryObject* ox = (OxEntryObject*)OxObject_Allocate(pOxClass);
	if (ox == NULL)
		return NULL;

	if (!OxWidget_Init((OxWidgetObject*)ox, oxParent, rc))
		return NULL;
	//XX(oxParent->oxWindow);
	ox->fnVerifyCB = NULL;
	ox->sFormat = NULL;
	ox->sEditFormat = NULL;

	OxWidget_CalculateRect((OxWidgetObject*)ox, rc);
	ox->hWin = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"",
		WS_CHILD | WS_TABSTOP | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_WANTRETURN | WS_VISIBLE, //| ES_MULTILINE
		rc->iLeft, rc->iTop, rc->iWidth, rc->iHeight,
		oxParent->hWin, (HMENU)IDC_OXENTRY, OxApp->hInstance, NULL);

	if (ox->hWin == NULL) {
		OxErr_SetFromWindows();
		return NULL;
	}

	SendMessageW(ox->hWin, WM_SETFONT, (WPARAM)OxApp->hDefaultFont, MAKELPARAM(FALSE, 0));
	ox->fnOldWinProcedure = (WNDPROC)SetWindowLongPtrW(ox->hWin, GWLP_WNDPROC, (LONG_PTR)OxEntryProc);
	SetWindowLongPtrW(ox->hWin, GWLP_USERDATA, (LONG_PTR)ox);

	return ox;
}

BOOL
OxEntry_RenderData(OxEntryObject* ox, BOOL bEdit)
{
	char* sText;
	if (ox->oxData == NULL || ox->oxData == OxNull) {
		SendMessageW(ox->hWin, WM_SETTEXT, 0, (LPARAM)L"");
		return TRUE;
	}

	if ((sText = OxObject_AsStringV(ox->oxData, bEdit ? ox->sEditFormat : ox->sFormat)) == NULL)
		return FALSE;

	LPWSTR szText = OxToW(sText);
	SendMessage(ox->hWin, WM_SETTEXT, 0, (LPARAM)szText);
	OxFree(szText);
	OxFree(sText);
	return TRUE;
}


OxObject*
OxEntry_GetInputData(OxEntryObject* ox)
{
	char* sText = OxGetWindowText(ox->hWin);
	OxObject* oxData;// = OxObject_FromStringV(ox->pDataType, sText, ox->sEditFormat);	XX(oxData);

	//if (OxObjectClass.aMethods[OxOBJECT_FROMSTRING] == OxStringClass.aMethods[OxOBJECT_FROMSTRING])
	//	OutputDebugString(L"\n+---- Inheritance!");
	if (sText == NULL)
		oxData = OxNull;
	else if ((oxData = OxObject_FromStringV(ox->pDataType, sText, ox->sEditFormat)) == NULL)
		return NULL;
	OxFree(sText);
	return oxData;
}

BOOL
OxEntry_SetData(OxEntryObject* ox, OxObject* oxData)
{
	if (ox->oxData == oxData)
		return TRUE;

	if (!OxWidget_SetData((OxWidgetObject*)ox, oxData))
		return FALSE;

	return OxEntry_RenderData(ox, FALSE);
}

BOOL
OxEntry_GotFocus(OxEntryObject* ox)
{
	if (ox->oxWindow->oxFocusWidget == (OxWidgetObject*)ox)
		return TRUE;

	int iR = OxWindow_MoveFocus(ox->oxWindow, (OxWidgetObject*)ox);
	if (iR == -1)
		return FALSE;
	else if (iR == FALSE)
		return TRUE;

	//return PxEntry_RenderData(self, FALSE);
	return TRUE;
}

BOOL
OxEntry_RenderFocus(OxEntryObject* ox)
{
	BOOL bReturn = -1;

	if (ox->fnVerifyCB) {
		bReturn = ox->fnVerifyCB((OxObject*)ox);
		if (bReturn == -1)
			return -1;
		else if (bReturn == FALSE) {
			SetFocus(ox->hWin);
			return FALSE;
		}
	}
	bReturn = TRUE;

	OxObject* oxData = OxEntry_GetInputData(ox);
	if (oxData == NULL || !OxEntry_SetData(ox, oxData))
		return -1;

	return bReturn;
}

char*
OxEntry_Represent(OxWindowObject* ox)
{
	char* sString = (char*)OxAllocate(250 * sizeof(char));
	int iIndex = sprintf_s(sString, 250, "OxEntryObject @ %p, %d times referenced", ox, ox->iRefCount);
	return sString;
}

static BOOL
OxEntry_Delete(OxEntryObject* ox)
{
	DestroyWindow(ox->hWin);
	return pOxClass->pBase->fnDelete((OxObject*)ox);
}

OxClass OxEntryClass = {
	"Entry",                   /* sName */
	&OxWidgetClass,            /* pBase */
	sizeof(OxEntryObject),     /* nSize */
	OxEntry_Delete,            /* fnDelete */
	NULL,                      /* pUserData */
	0                          /* aMethods */
};

BOOL
OxEntryClass_Init()
{
	pOxClass = &OxEntryClass;
	Ox_INHERIT_METHODS(pOxClass);
	pOxClass->aMethods[OxWIDGET_SETDATA] = OxEntry_SetData;
	pOxClass->aMethods[OxWIDGET_RENDERFOCUS] = OxEntry_RenderFocus;
	pOxClass->aMethods[OxOBJECT_REPRESENT] = OxEntry_Represent;
	return TRUE;
}

static LRESULT CALLBACK
OxEntryProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	OxEntryObject* ox = (OxEntryObject*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);

	switch (msg)
	{

	case OCM_COMMAND:
		switch (HIWORD(wParam))
		{
		case EN_SETFOCUS:
			if (!OxEntry_GotFocus(ox))
				OxApplication_ShowError();
			return 0;

		}
		break;

	default:
		break;
	}
	return CallWindowProcW(ox->fnOldWinProcedure, hwnd, msg, wParam, lParam);
}
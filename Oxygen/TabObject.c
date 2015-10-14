// TabObject.c  | Oxygen © 2015 by Thomas Führinger
#include "Oxygen.h"

static OxClass* pOxClass;
static LRESULT CALLBACK OxTabProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

OxTabObject*
OxTab_New(OxWidgetObject* oxParent, OxRect* rc)
{
	OxTabObject* ox = (OxTabObject*)OxObject_Allocate(pOxClass);
	if (ox == NULL)
		return NULL;

	if (!OxWidget_Init(ox, oxParent, rc, NULL))
		return NULL;

	OxWidget_CalculateRect((OxWidgetObject*)ox, rc);
	ox->hWin = CreateWindowExW(0, WC_TABCONTROL, L"",
		WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,// | TCS_BOTTOM,
		rc->iLeft, rc->iTop, rc->iWidth, rc->iHeight,
		//10, 50, 300, 200,
		oxParent->hWin, (HMENU)IDC_OXTAB, OxApp->hInstance, NULL);

	if (ox->hWin == NULL) {
		OxErr_SetFromWindows();
		return NULL;
	}

	ox->oxPages = OxList_New(0);
	ox->iPages = 0;
	ox->oxCurrentPage = NULL;

	SendMessageW(ox->hWin, WM_SETFONT, (WPARAM)OxApp->hDefaultFont, MAKELPARAM(FALSE, 0));
	ox->fnOldWinProcedure = (WNDPROC)SetWindowLongPtrW(ox->hWin, GWLP_WNDPROC, (LONG_PTR)OxTabProc);
	SetWindowLongPtr(ox->hWin, GWLP_USERDATA, (LONG_PTR)ox);
	return ox;
}

static BOOL
OxTab_Delete(OxTabObject* ox)
{
	DestroyWindow(ox->hWin);
	OxREL(ox->oxPages);
	return pOxClass->pBase->fnDelete(ox);
}

OxClass OxTabClass = {
	"Tab",                /* sName */
	&OxWidgetClass,       /* pBase */
	sizeof(OxTabObject),  /* nSize */
	OxTab_Delete,         /* fnDelete */
	NULL,                 /* pUserData */
	0                     /* aMethods */
};

BOOL
OxTabClass_Init()
{
	pOxClass = &OxTabClass;
	Ox_INHERIT_METHODS(pOxClass);
	return TRUE;
}

static BOOL OxTabPage_GotSelected(OxTabPageObject* ox);

static LRESULT CALLBACK
OxTabProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	OxTabObject* ox = (OxTabObject*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);

	switch (msg)
	{
	case OCM_NOTIFY:
	{
		switch (((LPNMHDR)lParam)->code)
		{
		case TCN_SELCHANGING:
			return FALSE;

		case TCN_SELCHANGE:
		{
			int iIndex = TabCtrl_GetCurSel(ox->hWin);
			OxTabPage_GotSelected(OxList_GetItem(ox->oxPages, iIndex));
			break;
		}
		}

	}
	}
	return CallWindowProcW(ox->fnOldWinProcedure, hwnd, msg, wParam, lParam);
}


/* TabPage -----------------------------------------------------------------------*/

OxTabPageObject*
OxTabPage_New(OxTabObject* oxTab, char* sCaption, int iIndex)
{
	TCITEM tie;
	tie.mask = TCIF_TEXT | TCIF_IMAGE;
	tie.iImage = -1;

	OxTabPageObject* ox = (OxTabPageObject*)OxObject_Allocate(&OxTabPageClass);
	if (ox == NULL)
		return NULL;

	tie.pszText = OxToW(sCaption); //L"New";
	ox->iIndex = TabCtrl_InsertItem(oxTab->hWin, oxTab->iPages++, &tie);
	if (ox->iIndex == -1) {
		OxErr_SetFromWindows();
		return NULL;
	}
	OxFree(tie.pszText);

	OxRect rc = { .iLeft = 2, .iTop = 30, .iWidth = -5, .iHeight = -5 };
	OxBox_Init(ox, oxTab, &rc);

	if (OxList_AppendItem(oxTab->oxPages, ox) != ox->iIndex)
		return NULL;

	if (oxTab->iPages == 1)
		OxTabPage_GotSelected(ox);
	return ox;
}

static BOOL
OxTabPage_GotSelected(OxTabPageObject* ox)
{
	OxTabObject* oxTab = (OxTabObject*)ox->oxParent;
	if (oxTab->oxCurrentPage)
		OxWidget_ShowV(oxTab->oxCurrentPage, FALSE);
	oxTab->oxCurrentPage = ox;
	OxWidget_ShowV(ox, TRUE);
	return TRUE;
}

static BOOL
OxTabPage_Delete(OxTabPageObject* ox)
{
	return (&OxTabPageClass)->pBase->fnDelete(ox);
}

OxClass OxTabPageClass = {
	"TabPage",            /* sName */
	&OxBoxClass,          /* pBase */
	sizeof(OxTabPageObject), /* nSize */
	OxTabPage_Delete,     /* fnDelete */
	NULL,                 /* pUserData */
	0                     /* aMethods */
};

BOOL
OxTabPageClass_Init()
{
	Ox_INHERIT_METHODS((&OxTabPageClass));
	return TRUE;
}
// ToolBarObject.c  | Oxygen © 2015 by Thomas Führinger
// Under construction
#include "Oxygen.h"

static OxClass* pOxClass;
BOOL OxToolBar_Reposition(OxToolBarObject* ox);

OxToolBarObject*
OxToolBar_New(OxWidgetObject* oxParent)
{
	const int iBitmapSize = 16;

	OxToolBarObject* ox = (OxToolBarObject*)OxObject_Allocate(pOxClass);
	if (ox == NULL)
		return NULL;
	ox->iButtons = 1;
	ox->hImageList = NULL;

	OxRect rc = { 0, 0, 0, 0 };
	if (!OxWidget_Init((OxWidgetObject*)ox, oxParent, &rc))
		return NULL;

	ox->hWin = CreateWindowExW(0, TOOLBARCLASSNAME, NULL,
		WS_CHILD | TBSTYLE_WRAPABLE | TBSTYLE_LIST | TBSTYLE_FLAT, 0, 0, 0, 0,
		oxParent->hWin, (HMENU)IDC_OXTOOLBAR, OxApp->hInstance, NULL);

	if (ox->hWin == NULL) {
		OxErr_SetFromWindows();
		return NULL;
	}

	ox->hImageList = ImageList_Create(iBitmapSize, iBitmapSize, ILC_COLOR32 | ILC_MASK, 1, 20);

	SendMessage(ox->hWin, TB_SETIMAGELIST, (WPARAM)OxTOOLBAR_IMGLST, (LPARAM)ox->hImageList);
	SendMessage(ox->hWin, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
	/*	TBBUTTON aButtons[1] =
	{
	{ MAKELONG(STD_COPY, OxTOOLBAR_IMGLST), 0, TBSTATE_ENABLED, BTNS_AUTOSIZE, { 0 }, 0, (INT_PTR)TEXT("Copy") }
	};
	SendMessage(ox->hWin, TB_ADDBUTTONS, (WPARAM)ox->iButtons, (LPARAM)&aButtons);
	*/
	SendMessage(ox->hWin, TB_AUTOSIZE, 0, 0);
	SendMessage(ox->hWin, TB_SETEXTENDEDSTYLE, 0, (LPARAM)TBSTYLE_EX_MIXEDBUTTONS);
	ShowWindow(ox->hWin, TRUE);

	OxAttachObject(&((OxWindowObject*)oxParent)->oxToolBar, ox, FALSE);
	SetWindowLongPtr(ox->hWin, GWLP_USERDATA, (LONG_PTR)ox);
	return ox;
}

BOOL
OxToolBar_AppendItem(OxToolBarObject* ox, OxMenuItemObject* oxItem)
{
	int iImageIndex = ImageList_AddIcon(ox->hImageList, oxItem->oxIcon->hWin);

	LPWSTR szCaption = OxToW(oxItem->sCaption);
	TBBUTTON aButtons[1] =
	{
		{ iImageIndex, oxItem->iIdentifier, TBSTATE_ENABLED, BTNS_BUTTON, { 0 }, (DWORD_PTR)oxItem, (INT_PTR)szCaption }
	};
	if (!SendMessage(ox->hWin, TB_ADDBUTTONS, (WPARAM)1, (LPARAM)&aButtons)){
		OxErr_SetFromWindows();
		OxFree(szCaption);
		return FALSE;
	};
	OxFree(szCaption);
	oxItem->oxToolBar = ox;
	return TRUE;
}

BOOL
OxToolBar_AppendSeparator(OxToolBarObject* ox)
{
	TBBUTTON aButtons[1] =
	{
		{ 3, 0, 0, TBSTYLE_SEP, { 0 }, 0, 0 }
	};
	SendMessage(ox->hWin, TB_ADDBUTTONS, (WPARAM)1, (LPARAM)&aButtons);
	return TRUE;
}

BOOL
OxToolBar_Reposition(OxToolBarObject* ox)
{
	SendMessage(ox->hWin, TB_AUTOSIZE, 0, 0);
	return TRUE;
}

int
OxToolBar_GetHeight(OxToolBarObject* ox)
{
	RECT rc;
	GetWindowRect(ox->hWin, &rc);
	return rc.bottom - rc.top;
}

static BOOL
OxToolBar_Delete(OxToolBarObject* ox)
{
	DestroyWindow(ox->hWin);
	ImageList_Destroy(ox->hImageList);
	return pOxClass->pBase->fnDelete(ox);
}

OxClass OxToolBarClass = {
	"ToolBar",                 /* sName */
	&OxWidgetClass,            /* pBase */
	sizeof(OxToolBarObject),   /* nSize */
	OxToolBar_Delete,          /* fnDelete */
	NULL,                      /* pUserData */
	0                          /* aMethods */
};

BOOL
OxToolBarClass_Init()
{
	pOxClass = &OxToolBarClass;
	Ox_INHERIT_METHODS(pOxClass);
	pOxClass->aMethods[OxWIDGET_REPOSITION] = OxToolBar_Reposition;
	return TRUE;
}

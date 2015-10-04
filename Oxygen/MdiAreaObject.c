// MdiAreaObject.c  | Oxygen © 2015 by Thomas Führinger
#include "Oxygen.h"

#define IDM_FIRSTCHILD  50000
static OxClass* pOxClass;

OxMdiAreaObject*
OxMdiArea_New(OxWidgetObject* oxParent, OxRect* rc)
{
	CLIENTCREATESTRUCT  MDIClientCreateStruct;

	if (oxParent->pClass != &OxWindowClass){
		OxErr_SetString(OxERROR_RUNTIME, "Parent of MdiArea must be a Window.");
		return NULL;
	}

	OxMdiAreaObject* ox = (OxMdiAreaObject*)OxObject_Allocate(pOxClass);
	if (ox == NULL)
		return NULL;

	if (!OxWidget_Init((OxWidgetObject*)ox, oxParent, rc))
		return NULL;

	MDIClientCreateStruct.hWindowMenu = oxParent->oxWindow->oxMenu == NULL ? oxParent->oxWindow->oxMenu->hWin : 0;
	MDIClientCreateStruct.idFirstChild = IDM_FIRSTCHILD;

	OxWidget_CalculateRect((OxWidgetObject*)ox, rc);
	ox->hWin = CreateWindowExW(WS_EX_CLIENTEDGE | WS_EX_CONTROLPARENT, L"MDICLIENT", L"",
		WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL,// | MDIS_ALLCHILDSTYLES,
		rc->iLeft, rc->iTop, rc->iWidth, rc->iHeight,
		oxParent->hWin, (HMENU)IDC_OXMDIAREA, OxApp->hInstance, (void*)&MDIClientCreateStruct);

	if (ox->hWin == NULL) {
		OxErr_SetFromWindows();
		return NULL;
	}
	((OxWindowObject*)oxParent)->oxMdiArea = ox;

	SetWindowLongPtr(ox->hWin, GWLP_USERDATA, (LONG_PTR)ox);
	return ox;
}

BOOL
OxMdiArea_SetMenu(OxMdiAreaObject* ox, OxMenuObject* oxMenu)
{
	SendMessage(ox->hWin, WM_MDISETMENU, (WPARAM)0, (LPARAM)oxMenu->hWin);
	DrawMenuBar(ox->oxParent->hWin);
	return TRUE;
}

static BOOL
OxMdiArea_Delete(OxObject* ox)
{
	// RefDown(...);  
	return ox->pClass->pBase->fnDelete(ox);
}

OxClass OxMdiAreaClass = {
	"MdiArea",               /* sName */
	&OxWidgetClass,          /* pBase */
	sizeof(OxMdiAreaObject), /* nSize */
	OxMdiArea_Delete,        /* fnDelete */
	NULL,                    /* pUserData */
	0                        /* aMethods */
};

BOOL
OxMdiAreaClass_Init()
{
	pOxClass = &OxMdiAreaClass;
	Ox_INHERIT_METHODS(pOxClass);
	return TRUE;
}

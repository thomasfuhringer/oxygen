// MdiWindowObject.c  | Oxygen © 2015 by Thomas Führinger
#include "Oxygen.h"

static OxClass* pOxClass;

OxMdiWindowObject*
OxMdiWindow_New(OxWidgetObject* oxParent, OxRect* rc, char* sCaption)
{
	MDICREATESTRUCT MdiCreate;

	OxMdiWindowObject* ox = (OxMdiWindowObject*)OxObject_Allocate(pOxClass);
	if (ox == NULL)
		return NULL;
	if (!OxWidget_Init((OxWidgetObject*)ox, oxParent, rc))
		return NULL;

	ox->oxWindow = ox;
	ox->oxFocusWidget = NULL;
	ox->fnBeforeCloseCB = NULL;
	ox->oxMdiArea = NULL;
	ox->bModal = FALSE;
	ox->bDeleteOnClose = TRUE;
	ox->iMinWidth = 320;
	ox->iMinHeight = 240;
	ox->iMaxWidth = -1;
	ox->iMaxHeight = -1;
	ox->oxMenu = NULL;
	ox->oxIcon = NULL;

	LPWSTR szCaption = OxToW(sCaption);

	MdiCreate.szClass = L"OxWindowClass";
	MdiCreate.szTitle = szCaption;
	MdiCreate.hOwner = OxApp->hInstance;
	MdiCreate.x = rc->iLeft;
	MdiCreate.y = rc->iTop;
	MdiCreate.cx = rc->iWidth;
	MdiCreate.cy = rc->iHeight;
	MdiCreate.style = WS_EX_CLIENTEDGE | WS_EX_MDICHILD | WS_EX_CONTROLPARENT | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	MdiCreate.lParam = 0;
	ox->hWin = (HWND)SendMessageW(oxParent->hWin, WM_MDICREATE, 0, (LPARAM)(LPMDICREATESTRUCT)&MdiCreate);
	OxFree(szCaption);

	if (ox->hWin == NULL) {
		OxErr_SetFromWindows();
		return NULL;
	}

	SetWindowLongPtr(ox->hWin, GWLP_USERDATA, (LONG_PTR)ox);
	return ox;
}

char*
OxMdiWindow_Represent(OxWindowObject* ox)
{
	char* sString = (char*)OxAllocate(250 * sizeof(char));
	int iIndex = sprintf_s(sString, 250, "OxMdiWindowObject @ %p, %d times referenced", ox, ox->iRefCount);
	return sString;
}

static BOOL
OxMdiWindow_Delete(OxObject* ox)
{
	return pOxClass->pBase->fnDelete(ox);
}

OxClass OxMdiWindowClass = {
	"MdiWindow",             /* sName */
	&OxWindowClass,          /* pBase */
	sizeof(OxMdiWindowObject), /* nSize */
	OxMdiWindow_Delete,      /* fnDelete */
	NULL,                    /* pUserData */
	0                        /* aMethods */
};

BOOL
OxMdiWindowClass_Init()
{
	pOxClass = &OxMdiWindowClass;
	Ox_INHERIT_METHODS(pOxClass);
	pOxClass->aMethods[OxOBJECT_REPRESENT] = OxMdiWindow_Represent;
	return TRUE;
}

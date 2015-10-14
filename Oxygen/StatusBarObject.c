// StatusBarObject.c  | Oxygen © 2015 by Thomas Führinger
#include "Oxygen.h"

static OxClass* pOxClass;
static BOOL OxStatusBar_Reposition(OxStatusBarObject* ox);

OxStatusBarObject*
OxStatusBar_New(OxWidgetObject* oxParent, int* paParts)
{
	OxStatusBarObject* ox = (OxStatusBarObject*)OxObject_Allocate(pOxClass);
	if (ox == NULL)
		return NULL;
	ox->iParts = 0;
	memcpy(ox->iaParts, paParts, sizeof(ox->iaParts));

	while (ox->iaParts[ox->iParts++] != 0 && ox->iParts < OxTOOLBAR_MAX_PARTS)
		;
	ox->iaRightEdges[ox->iParts - 1] = -1;

	OxRect rc = { 0, 0, 0, 0 };
	if (!OxWidget_Init((OxWidgetObject*)ox, oxParent, &rc))
		return NULL;

	ox->hWin = CreateWindowExW(0, STATUSCLASSNAME, L"",
		WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP,
		0, 0, 0, 0,
		oxParent->hWin, (HMENU)IDC_OXSTATUSBAR, OxApp->hInstance, NULL);

	if (ox->hWin == NULL) {
		OxErr_SetFromWindows();
		return NULL;
	}

	OxStatusBar_Reposition(ox);
	OxAttachObject(&((OxWindowObject*)oxParent)->oxStatusBar, ox, FALSE);
	SetWindowLongPtr(ox->hWin, GWLP_USERDATA, (LONG_PTR)ox);
	return ox;
}

static BOOL
OxStatusBar_Reposition(OxStatusBarObject* ox)
{
	//OutputDebugStringA("+-- OxStatusBar_Reposition ---\n");
	RECT rcParent;
	GetClientRect(ox->oxParent->hWin, &rcParent);

	for (int iPart = 0; iPart < ox->iParts - 1; iPart++)
		if (ox->iaParts[iPart] < 0)
			ox->iaRightEdges[iPart] = rcParent.right + ox->iaParts[iPart];
		else
			ox->iaRightEdges[iPart] = ox->iaParts[iPart];

	SendMessageW(ox->hWin, SB_SETPARTS, (WPARAM)ox->iParts, (LPARAM)ox->iaRightEdges);
	SendMessageW(ox->hWin, WM_SIZE, 0, 0);
	return TRUE;
}

int
OxStatusBar_GetHeight(OxStatusBarObject* ox)
{
	RECT rc;
	GetClientRect(ox->hWin, &rc);
	return rc.bottom;
}

BOOL
OxStatusBar_Message(OxStatusBarObject* ox, char* sMessage, int iPart)
{
	if (iPart > ox->iParts) {
		OxErr_SetString(OxERROR_RUNTIME, "Status bar does not have as many parts.");
		return FALSE;
	}
	LPWSTR szMessage = OxToW(sMessage);
	BOOL bR = SendMessage(ox->hWin, SB_SETTEXT, iPart, (LPARAM)szMessage);
	OxFree(szMessage);

	if (!bR) {
		OxErr_SetFromWindows();
		return FALSE;
	}
	return TRUE;
}

static BOOL
OxStatusBar_Delete(OxStatusBarObject* ox)
{
	DestroyWindow(ox->hWin);
	return pOxClass->pBase->fnDelete(ox);
}

OxClass OxStatusBarClass = {
	"StatusBar",               /* sName */
	&OxWidgetClass,            /* pBase */
	sizeof(OxStatusBarObject), /* nSize */
	OxStatusBar_Delete,        /* fnDelete */
	NULL,                      /* pUserData */
	0                          /* aMethods */
};

BOOL
OxStatusBarClass_Init()
{
	pOxClass = &OxStatusBarClass;
	Ox_INHERIT_METHODS(pOxClass);
	pOxClass->aMethods[OxWIDGET_REPOSITION] = OxStatusBar_Reposition;
	return TRUE;
}
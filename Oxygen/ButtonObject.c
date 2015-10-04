// ButtonObject.c  | Oxygen © 2015 by Thomas Führinger
#include "Oxygen.h"

static OxClass* pOxClass;
static LRESULT CALLBACK OxButtonProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

OxButtonObject*
OxButton_New(OxWidgetObject* oxParent, OxRect* rc, char* sCaption, OxCallback fnOnClickCB)
{
	OxButtonObject* ox = (OxButtonObject*)OxObject_Allocate(pOxClass);
	if (ox == NULL)
		return NULL;

	if (!OxWidget_Init(ox, oxParent, rc))
		return NULL;

	OxWidget_CalculateRect((OxWidgetObject*)ox, rc);
	ox->hWin = CreateWindowExW(0, L"BUTTON", L"OK",
		WS_TABSTOP | WS_CHILD | BS_TEXT | BS_PUSHBUTTON | WS_VISIBLE,
		rc->iLeft, rc->iTop, rc->iWidth, rc->iHeight,
		oxParent->hWin, (HMENU)IDC_OXBUTTON, OxApp->hInstance, NULL);

	if (ox->hWin == NULL) {
		OxErr_SetFromWindows();
		return NULL;
	}

	ox->fnOnClickCB = fnOnClickCB;
	SendMessageW(ox->hWin, WM_SETFONT, (WPARAM)OxApp->hDefaultFont, MAKELPARAM(FALSE, 0));
	ox->fnOldWinProcedure = (WNDPROC)SetWindowLongPtrW(ox->hWin, GWLP_WNDPROC, (LONG_PTR)OxButtonProc);
	SetWindowLongPtrW(ox->hWin, GWLP_USERDATA, (LONG_PTR)ox);

	if (sCaption != NULL)
		if (!OxWidget_SetCaption((OxWidgetObject*)ox, sCaption))
			return NULL;

	return ox;
}

BOOL
OxButton_Clicked(OxButtonObject* ox)
{
	if (ox->fnOnClickCB) {
		return ox->fnOnClickCB((OxObject*)ox);
	}
	return TRUE;
}

static BOOL
OxButton_Delete(OxButtonObject* ox)
{
	DestroyWindow(ox->hWin);
	return pOxClass->pBase->fnDelete((OxObject*)ox);
}

OxClass OxButtonClass = {
	"Button",                  /* sName */
	&OxWidgetClass,            /* pBase */
	sizeof(OxButtonObject),    /* nSize */
	OxButton_Delete,           /* fnDelete */
	NULL,                      /* pUserData */
	0                          /* aMethods */
};

BOOL
OxButtonClass_Init()
{
	pOxClass = &OxButtonClass;
	Ox_INHERIT_METHODS(pOxClass);
	return TRUE;
}

static LRESULT CALLBACK
OxButtonProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	OxButtonObject* ox = (OxButtonObject*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);

	switch (msg)
	{
	case OCM_COMMAND:
		switch (HIWORD(wParam))
		{
		case BN_CLICKED:
			if (OxWindow_MoveFocus(ox->oxWindow, (OxWidgetObject*)0) == -1) {
				OxApplication_ShowError();
				return 0;
			}
			if (!OxButton_Clicked(ox))
				OxApplication_ShowError();
			return 0;
		}
		break;

	default:
		break;
	}
	return CallWindowProcW(ox->fnOldWinProcedure, hwnd, msg, wParam, lParam);
}
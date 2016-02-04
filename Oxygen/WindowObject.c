// WindowObject.c  | Oxygen © 2015 by Thomas Führinger
#include "Oxygen.h"
#include "Resource.h"

static OxClass* pOxClass;
static LPWSTR szWindowClass = L"OxWindowClass";

OxWindowObject*
OxWindow_New(OxWidgetObject* oxParent, OxRect* rc, char* sCaption)
{
	OxWindowObject* ox = (OxWindowObject*)OxObject_Allocate(pOxClass);
	if (ox == NULL)
		return NULL;

	if (!OxWidget_Init((OxWidgetObject*)ox, oxParent, rc))
		return NULL;

	ox->oxIcon = NULL;
	ox->oxMenu = NULL;
	ox->oxToolBar = NULL;
	ox->oxStatusBar = NULL;
	ox->oxMdiArea = NULL;
	ox->oxWindow = ox;
	ox->oxFocusWidget = NULL;
	ox->fnBeforeCloseCB = NULL;
	ox->iMinWidth = 320;
	ox->iMinHeight = 240;
	ox->iMaxWidth = -1;
	ox->iMaxHeight = -1;
	ox->bModal = FALSE;
	ox->bDeleteOnClose = TRUE;

	LPWSTR szCaption = OxToW(sCaption == NULL ? OxApp->sName : sCaption);

	OxWidget_CalculateRect((OxWidgetObject*)ox, rc);
	ox->hWin = CreateWindowW(szWindowClass, szCaption, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
		rc->iLeft, rc->iTop, rc->iWidth, rc->iHeight,
		oxParent ? oxParent->hWin : NULL, 0, OxApp->hInstance, NULL);
	OxFree(szCaption);

	if (ox->hWin == NULL) {
		OxErr_SetFromWindows();
		return NULL;
	}

	if (OxApp->oxIcon) {   // use app icon if set
		SendMessage(ox->hWin, WM_SETICON, ICON_SMALL, (LPARAM)OxApp->oxIcon->hWin);
		SendMessage(ox->hWin, WM_SETICON, ICON_BIG, (LPARAM)OxApp->oxIcon->hWin);
	}

	SetWindowLongPtr(ox->hWin, GWLP_USERDATA, (LONG_PTR)ox);
	return ox;
}

BOOL
OxWindow_MoveFocus(OxWindowObject* ox, OxWidgetObject* oxDestinationWidget)
// return TRUE if possible, FALSE if not, -1 on error
{
	if (ox->oxFocusWidget == oxDestinationWidget)
		return TRUE;
	if (ox->oxFocusWidget == NULL) {
		ox->oxFocusWidget = oxDestinationWidget;
		return TRUE;
	}

	BOOL bR = OxWidget_RenderFocusV(ox->oxFocusWidget);
	//OutputDebugString(bR ? L"\n*---- tr" : L"\n*---- fa");
	if (bR)
		ox->oxFocusWidget = oxDestinationWidget;
	return bR;
}

BOOL
OxWindow_SetMenu(OxWindowObject* ox, OxMenuObject* oxMenu)
{
	if (SetMenu(ox->hWin, oxMenu->hWin) == 0)
		return FALSE;
	DrawMenuBar(ox->hWin);
	OxAttachObject(&ox->oxMenu, oxMenu, FALSE);
	return TRUE;
}

BOOL
OxWindow_SetIcon(OxWindowObject* ox, OxImageObject* oxIcon)
{
	if (oxIcon->iType != OxIMAGEFORMAT_ICO) {
		OxErr_SetString(OxERROR_RUNTIME, "Icon can only be set to an Image of Type Icon.");
		return FALSE;
	}
	OxAttachObject(&ox->oxIcon, oxIcon, FALSE);
	SendMessage(ox->hWin, WM_SETICON, ICON_SMALL, (LPARAM)ox->oxIcon->hWin);
	SendMessage(ox->hWin, WM_SETICON, ICON_BIG, (LPARAM)ox->oxIcon->hWin);
	return TRUE;
}

BOOL
OxWindow_ReadyToClose(OxWindowObject* ox)
{
	if (ox->fnBeforeCloseCB && !ox->fnBeforeCloseCB(ox))
		return FALSE;

	if (ox == OxApp->oxWindow && OxApp->sStateID)
		OxApplication_SaveState();
	return TRUE;
}

char*
OxWindow_Represent(OxWindowObject* ox)
{
	char* sString = (char*)OxAllocate(250 * sizeof(char));
	int iIndex = sprintf_s(sString, 250, "OxWindowObject @ %p, %d times referenced", ox, ox->iRefCount);
	return sString;
}

BOOL
OxWindow_ShowModal(OxWindowObject* ox)
{

	BOOL bDeleteOnClose = ox->bDeleteOnClose;
	ox->bDeleteOnClose = FALSE;

	ShowWindow(ox->hWin, SW_SHOW);
	EnableWindow(OxApp->oxWindow->hWin, FALSE);

	ox->bModal = TRUE;

	MSG msg;
	HACCEL hAccelTable = LoadAccelerators(OxApp->hInstance, MAKEINTRESOURCE(IDS_APP));
	while (GetMessage(&msg, NULL, 0, 0) > 0 && ox->bModal)
	{
		if (!IsDialogMessage(ox->hWin, &msg) && !TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	ShowWindow(ox->hWin, SW_HIDE);
	EnableWindow(OxApp->oxWindow->hWin, TRUE);
	BringWindowToTop(OxApp->oxWindow->hWin);
	ox->bDeleteOnClose = bDeleteOnClose;
	return TRUE;
}

static BOOL
OxWindow_Reposition(OxWindowObject* ox)
{
	EnumChildWindows(ox->hWin, OxSizeEnumProc, (LPARAM)ox);
	return TRUE;
}

static BOOL
OxWindow_Delete(OxWindowObject* ox)
{
	OxREL(ox->oxMenu);
	OxREL(ox->oxIcon);
	OxREL(ox->oxMdiArea);
	OxREL(ox->oxToolBar);
	OxREL(ox->oxStatusBar);
	if (DestroyWindow(ox->hWin) == 0)
		OxErr_SetFromWindows();
	return pOxClass->pBase->fnDelete(ox);
}

OxClass OxWindowClass = {
	"Window",                  /* sName */
	&OxWidgetClass,            /* pBase */
	sizeof(OxWindowObject),    /* nSize */
	OxWindow_Delete,           /* fnDelete */
	NULL,                      /* pUserData */
	0                          /* aMethods */
};

static LRESULT CALLBACK OxWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

BOOL
OxWindowClass_Init()
{
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = OxWindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = OxApp->hInstance;
	wc.hIcon = OxApp->hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(OxWINDOWBKGCOLOR + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = szWindowClass;
	wc.hIconSm = OxApp->hIcon;

	if (!RegisterClassExW(&wc))
	{
		OxErr_SetFromWindows();
		return FALSE;
	}

	pOxClass = &OxWindowClass;
	Ox_INHERIT_METHODS(pOxClass);
	pOxClass->aMethods[OxOBJECT_REPRESENT] = OxWindow_Represent;
	pOxClass->aMethods[OxWIDGET_REPOSITION] = OxWindow_Reposition;
	return TRUE;
}


static LPMINMAXINFO lpMMI;

static LRESULT CALLBACK
OxWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	OxWindowObject* ox = (OxWindowObject*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	WORD            wCommandIdentifier;

	switch (uMsg)
	{
	case WM_COMMAND:
		wCommandIdentifier = LOWORD(wParam);
		if (wCommandIdentifier >= FIRST_CUSTOM_MENU_ID && wCommandIdentifier <= MAX_CUSTOM_MENU_ID) {
			OxMenuItemObject* oxMenuItem = OxApp->oxaMenuItem[wCommandIdentifier - FIRST_CUSTOM_MENU_ID];
			if (oxMenuItem->fnOnClickCB && !oxMenuItem->fnOnClickCB())
				OxApplication_ShowError();
		}
		break;

	case WM_GETMINMAXINFO:
		if (ox) {
			lpMMI = (LPMINMAXINFO)lParam;
			lpMMI->ptMinTrackSize.x = ox->iMinWidth;
			lpMMI->ptMinTrackSize.y = ox->iMinHeight;
			if (ox->iMaxWidth != -1)
				lpMMI->ptMaxTrackSize.x = ox->iMaxWidth;
			if (ox->iMaxHeight != -1)
				lpMMI->ptMaxTrackSize.y = ox->iMaxHeight;
		}

	case WM_SIZE:
		if (wParam != SIZE_MINIMIZED) {
			if (ox)
				OxWidget_RepositionV(ox);
			if (ox && ox->pClass != &OxMdiWindowClass)
				return 0;
		}
		break;

	case WM_QUERYENDSESSION:
	case WM_CLOSE:
		if (!OxWindow_ReadyToClose(ox))
			return 0;
		if (ox->bModal) {
			ox->bModal = FALSE;
			ShowWindow(ox->hWin, SW_HIDE);
			//return 0;
		}
		if (ox->bDeleteOnClose)
			OxREL(ox);
		return 0;
	}

	return DefParentProc(hwnd, uMsg, wParam, lParam, (OxWidgetObject*)ox);
}

// Reflect messages to child
LRESULT
DefParentProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, OxWidgetObject* oxWidget)
{
	switch (uMsg) {
	case WM_NOTIFY:
	{
		NMHDR* nmhdr = (NMHDR*)lParam;
		if (nmhdr->hwndFrom != NULL)
			return SendMessage(nmhdr->hwndFrom, uMsg + OCM__BASE, wParam, lParam);
		break;
	}

	case WM_COMMAND:
	case WM_CTLCOLORBTN:
	case WM_CTLCOLOREDIT:
	case WM_CTLCOLORDLG:
	case WM_CTLCOLORLISTBOX:
	case WM_CTLCOLORMSGBOX:
	case WM_CTLCOLORSCROLLBAR:
	case WM_CTLCOLORSTATIC:
	case WM_VKEYTOITEM:
	case WM_CHARTOITEM:
		if (lParam != 0)
			return SendMessage((HWND)lParam, uMsg + OCM__BASE, wParam, lParam);
		break;
	}

	if (oxWidget != NULL && (oxWidget->pClass == &OxMdiWindowClass))
		return DefMDIChildProcW(hwnd, uMsg, wParam, lParam);
	else if (oxWidget != NULL && OxObject_IsKindOf(oxWidget, &OxWindowClass)){
		OxWindowObject* oxWindow = (OxWindowObject*)oxWidget;
		if (oxWindow->oxMdiArea)
			return DefFrameProcW(hwnd, oxWindow->oxMdiArea->hWin, uMsg, wParam, lParam);
	}
	return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}
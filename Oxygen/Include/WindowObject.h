// WindowObject.h  | Oxygen © 2015 by Thomas Führinger
#ifndef OxWINDOWOBJECT_H
#define OxWINDOWOBJECT_H

#include "Oxygen.h"

#define OxWindowObject_HEAD \
	OxWidgetObject_HEAD \
	int iMinWidth; \
	int iMinHeight; \
	int iMaxWidth; \
	int iMaxHeight; \
	OxMenuObject* oxMenu; \
    OxToolBarObject* oxToolBar; \
    OxStatusBarObject* oxStatusBar; \
	OxWidgetObject* oxFocusWidget; \
	OxMdiAreaObject* oxMdiArea; \
	OxIconObject* oxIcon; \
	BOOL bDeleteOnClose; \
	BOOL bModal; \
	OxCallback fnBeforeCloseCB; 

typedef struct _OxMdiAreaObject OxMdiAreaObject;
typedef struct _OxToolBarObject OxToolBarObject;
typedef struct _OxStatusBarObject OxStatusBarObject;
typedef struct _OxWindowObject {
	OxWindowObject_HEAD
} OxWindowObject;

extern OxAPI OxClass OxWindowClass;

BOOL OxWindowClass_Init();
OxAPI OxWindowObject* OxWindow_New(OxWidgetObject* oxParent, OxRect* rc, char* sCaption);
OxAPI BOOL OxWindow_MoveFocus(OxWindowObject* ox, OxWidgetObject* oxDestinationWidget);
OxAPI BOOL OxWindow_SetMenu(OxWindowObject* ox, OxMenuObject* oxMenu);
OxAPI BOOL OxWindow_SetIcon(OxWindowObject* ox, OxIconObject* oxIcon);
OxAPI BOOL OxWindow_ShowModal(OxWindowObject* ox);

BOOL CALLBACK OxSizeEnumProc(HWND hwndChild, LPARAM lParam);
LRESULT DefParentProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, OxWidgetObject* oxWidget);

#endif /* OxWINDOWOBJECT_H */

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
	OxImageObject* oxIcon; \
	bool bDeleteOnClose; \
	bool bModal; \
	OxCallback fnBeforeCloseCB; 

typedef struct _OxMdiAreaObject OxMdiAreaObject;
typedef struct _OxToolBarObject OxToolBarObject;
typedef struct _OxStatusBarObject OxStatusBarObject;
typedef struct _OxWindowObject {
	OxWindowObject_HEAD
} OxWindowObject;

extern OxAPI OxClass OxWindowClass;

bool OxWindowClass_Init();
OxAPI OxWindowObject* OxWindow_New(OxWidgetObject* oxParent, OxRect* rc, char* sCaption);
OxAPI bool OxWindow_MoveFocus(OxWindowObject* ox, OxWidgetObject* oxDestinationWidget);
OxAPI bool OxWindow_SetMenu(OxWindowObject* ox, OxMenuObject* oxMenu);
OxAPI bool OxWindow_SetIcon(OxWindowObject* ox, OxImageObject* oxIcon);
OxAPI bool OxWindow_ShowModal(OxWindowObject* ox);


#endif /* OxWINDOWOBJECT_H */

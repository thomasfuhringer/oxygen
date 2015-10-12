// MenuObject.h  | Oxygen © 2015 by Thomas Führinger
#ifndef OxMENUOBJECT_H
#define OxMENUOBJECT_H

#include "Oxygen.h"

#define Ox_FIRST_CUSTOM_MENU_ID   700
#define Ox_MAX_CUSTOM_MENU_ID     750

typedef struct _OxWindowObject OxWindowObject;
typedef struct _OxMenuObject {
	OxObject_HEAD
		HMENU hWin;
	OxWindowObject* oxWindow;
	char* sCaption;
} OxMenuObject;

extern OxAPI OxClass OxMenuClass;

BOOL OxMenuClass_Init();
OxAPI OxMenuObject* OxMenu_New(const char* sCaption);
OxAPI BOOL OxMenu_AppendItem(OxMenuObject* ox, OxObject* oxItem);


/* MenuItem -------------------------------------------------------------*/

typedef BOOL(*OxMenuItemCallback)();
typedef struct _OxIconObject OxIconObject;

typedef struct _OxMenuItemObject {
	OxObject_HEAD
		char* sCaption;
	UINT_PTR iIdentifier;
	OxImageObject* oxIcon;
	OxMenuItemCallback fnOnClickCB;
} OxMenuItemObject;

extern OxAPI OxClass OxMenuItemClass;

BOOL OxMenuItemClass_Init();
OxAPI OxMenuItemObject* OxMenuItem_New(const char* sCaption, OxCallback fnOnClickCB, OxIconObject* oxIcon);

#endif /* OxMENUOBJECT_H */

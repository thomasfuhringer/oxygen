// ToolBarObject.h  | Oxygen © 2015 by Thomas Führinger
#ifndef OxTOOLBAROBJECT_H
#define OxTOOLBAROBJECT_H

#define OxTOOLBAR_MAX_BUTTONS     20

#include "Oxygen.h"

typedef struct _OxToolBarObject {
	OxWidgetObject_HEAD
		int iButtons;
	HIMAGELIST hImageList;
	//TBBUTTON aButtons[OxTOOLBAR_MAX_BUTTONS]
} OxToolBarObject;

extern OxAPI OxClass OxToolBarClass;

BOOL OxToolBarClass_Init();
OxAPI OxToolBarObject* OxToolBar_New(OxWidgetObject* oxParent);
OxAPI int OxToolBar_GetHeight(OxToolBarObject* ox);
OxAPI BOOL OxToolBar_Reposition(OxToolBarObject* ox);
OxAPI BOOL OxToolBar_AppendItem(OxToolBarObject* ox, OxMenuItemObject* oxItem);
OxAPI BOOL OxToolBar_AppendSeparator(OxToolBarObject* ox);

#endif /* OxTOOLBAROBJECT_H */

// MdiAreaObject.h  | Oxygen © 2015 by Thomas Führinger
#ifndef OxMDIAREAOBJECT_H
#define OxMDIAREAOBJECT_H

#include "Oxygen.h"

typedef struct _OxMdiAreaObject {
	OxWidgetObject_HEAD
		//int* paParts;
} OxMdiAreaObject;

extern OxAPI OxClass OxMdiAreaClass;

BOOL OxMdiAreaClass_Init();
OxAPI OxMdiAreaObject* OxMdiArea_New(OxWidgetObject* oxParent, OxRect* rc);
OxAPI BOOL OxMdiArea_SetMenu(OxMdiAreaObject* ox, OxMenuObject* oxMenu);

#endif /* OxMDIAREAOBJECT_H */

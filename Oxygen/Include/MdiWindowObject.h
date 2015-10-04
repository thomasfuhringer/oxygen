// MdiWindowObject.h  | Oxygen © 2015 by Thomas Führinger
#ifndef OxMDIWINDOWOBJECT_H
#define OxMDIWINDOWOBJECT_H

#include "Oxygen.h"

typedef struct _OxMdiWindowObject {
	OxWindowObject_HEAD
} OxMdiWindowObject;

extern OxAPI OxClass OxMdiWindowClass;

BOOL OxMdiWindowClass_Init();
OxAPI OxMdiWindowObject* OxMdiWindow_New(OxWidgetObject* oxParent, OxRect* rc, char* sCaption);

#endif /* OxMDIWINDOWOBJECT_H */

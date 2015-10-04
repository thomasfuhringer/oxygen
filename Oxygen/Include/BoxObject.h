// BoxObject.h  | Oxygen © 2015 by Thomas Führinger
#ifndef OxBOXOBJECT_H
#define OxBOXOBJECT_H

#include "Oxygen.h"

typedef struct _OxBoxObject {
	OxWidgetObject_HEAD
} OxBoxObject;

extern OxAPI OxClass OxBoxClass;

BOOL OxBoxClass_Init();
OxAPI OxBoxObject* OxBox_New(OxWidgetObject* oxParent, OxRect* rc);
OxAPI OxBoxObject* OxBox_Init(OxBoxObject* ox, OxWidgetObject* oxParent, OxRect* rc);

#endif /* OxBOXOBJECT_H */

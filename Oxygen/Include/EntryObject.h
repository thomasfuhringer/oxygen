// EntryObject.h  | Oxygen © 2015 by Thomas Führinger
#ifndef OxENRTYOBJECT_H
#define OxENRTYOBJECT_H

#include "Oxygen.h"

typedef struct _OxEntryObject {
	OxWidgetObject_HEAD
		char* sFormat;
	char* sEditFormat;
	OxCallback fnVerifyCB;
} OxEntryObject;

extern OxAPI OxClass OxEntryClass;

BOOL OxEntryClass_Init();
OxAPI OxEntryObject* OxEntry_New();
OxAPI BOOL OxEntry_GotFocus(OxEntryObject* ox);
OxAPI OxObject* OxEntry_GetInputData(OxEntryObject* ox);

#endif /* OxENRTYOBJECT_H */

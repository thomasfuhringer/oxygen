// ButtonObject.h  | Oxygen © 2015 by Thomas Führinger
#ifndef OxBUTTONOBJECT_H
#define OxBUTTONOBJECT_H

#include "Oxygen.h"

typedef struct _OxButtonObject {
	OxWidgetObject_HEAD
		OxCallback fnOnClickCB;
} OxButtonObject;

extern OxAPI OxClass OxButtonClass;

BOOL OxButtonClass_Init();
OxAPI OxButtonObject* OxButton_New();

#endif /* OxBUTTONOBJECT_H */

// IconObject.h  | Oxygen © 2015 by Thomas Führinger
#ifndef OxICONOBJECT_H
#define OxICONOBJECT_H

#include "Oxygen.h"
#include "Resource.h"

#define OxICON_OXYGEN     IDI_OXYGEN

typedef struct _OxIconObject {
	OxObject_HEAD
		HICON hWin;
} OxIconObject;

extern OxAPI OxClass OxIconClass;

BOOL OxIconClass_Init();
OxAPI OxIconObject* OxIcon_FromFile(const char* sFileName);
OxAPI OxIconObject* OxIcon_FromWindowsResource(WORD wResourceID);
OxAPI OxIconObject* OxIcon_FromStock(int iStockID);

#endif /* OxICONOBJECT_H */

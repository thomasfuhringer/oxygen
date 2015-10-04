// ListObject.h  | Oxygen © 2015 by Thomas Führinger
#ifndef OxLISTOBJECT_H
#define OxLISTOBJECT_H

#include "Oxygen.h"

typedef struct _OxListObject {
	OxObject_HEAD
		OxObject** oxItems;
	size_t nSize;
	size_t nAllocated;
	BOOL bTakeOverRefs;
} OxListObject;

extern OxAPI OxClass OxListClass;

BOOL OxListClass_Init();
OxAPI OxListObject* OxList_New(size_t nAllocate);
OxAPI size_t OxList_AppendItem(OxListObject* ox, OxObject* oxItem);
OxAPI BOOL OxList_SetItem(OxListObject* ox, size_t nIndex, OxObject* oxItem);
OxAPI OxObject* OxList_GetItem(OxListObject* ox, size_t nIndex);
OxAPI BOOL OxList_Clear(OxListObject* ox);

#endif /* OxLISTOBJECT_H */

// SplitterObject.h  | Oxygen © 2015 by Thomas Führinger
#ifndef OxSPLITTEROBJECT_H
#define OxSPLITTEROBJECT_H

#include "Oxygen.h"

typedef struct _OxSplitterObject {
	OxWidgetObject_HEAD
		OxBoxObject* oxBox1;
	OxBoxObject* oxBox2;
	BOOL bVertical;
	int iPosition;
	int iSpacing;
} OxSplitterObject;

extern OxAPI OxClass OxSplitterClass;

BOOL OxSplitterClass_Init();
OxAPI OxSplitterObject* OxSplitter_New(OxWidgetObject* oxParent, OxRect* rc);
OxAPI BOOL OxSplitter_SetPosition(OxSplitterObject* ox, int iPosition);

#endif /* OxSPLITTEROBJECT_H */

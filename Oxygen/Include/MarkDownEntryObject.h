// MarkDownEntryObject.h  | Oxygen © 2016 by Thomas Führinger
#ifndef OxMARKDOWNENTRYOBJECT_H
#define OxMARKDOWNENTRYOBJECT_H

#include "Oxygen.h"


typedef struct _OxMarkDownEntryObject {
	OxWidgetObject_HEAD
		IOleObject* pOleBrowserObject;
	IOleClientSiteExt OleClientSiteExt;
} OxMarkDownEntryObject;

extern OxAPI OxClass OxMarkDownEntryClass;

BOOL OxMarkDownEntryClass_Init();
OxAPI OxMarkDownEntryObject* OxMarkDownEntry_New(OxWidgetObject* oxParent, OxRect* rc);
OxAPI BOOL OxMarkDownEntry_SetData(OxMarkDownEntryObject* ox, OxObject* oxData);
OxAPI BOOL OxMarkDownEntry_RenderData(OxMarkDownEntryObject* ox, BOOL bEdit);
OxAPI char* OxMarkDownToHtml(char* sMarkDown);

#endif

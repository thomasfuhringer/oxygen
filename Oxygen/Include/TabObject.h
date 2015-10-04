// TabObject.h  | Oxygen © 2015 by Thomas Führinger
#ifndef OxTABOBJECT_H
#define OxTABOBJECT_H

#include "Oxygen.h"
#include "Resource.h"

typedef struct _OxTabPageObject OxTabPageObject;
typedef struct _OxTabObject {
	OxWidgetObject_HEAD
		OxListObject* oxPages;
	int iPages;
	OxTabPageObject* oxCurrentPage;
} OxTabObject;

extern OxAPI OxClass OxTabClass;

BOOL OxTabClass_Init();
OxAPI OxTabObject* OxTab_New(OxWidgetObject* oxParent, OxRect* rc);


/* TabPage -----------------------------------------------------------------------*/

typedef struct _OxTabPageObject {
	OxWidgetObject_HEAD
		int iIndex;
} OxTabPageObject;

extern OxAPI OxClass OxTabPageClass;

BOOL OxTabPageClass_Init();
OxAPI OxTabPageObject* OxTabPage_New(OxTabObject* oxTab, char* sCaption, int iIndex);

#endif /* OxTABOBJECT_H */

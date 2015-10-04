// StatusBarObject.h  | Oxygen © 2015 by Thomas Führinger
#ifndef OxSTATUSBAROBJECT_H
#define OxSTATUSBAROBJECT_H

#define OxTOOLBAR_MAX_PARTS     8

#include "Oxygen.h"

typedef struct _OxStatusBarObject {
	OxWidgetObject_HEAD
		int iParts;
	int iaRightEdges[OxTOOLBAR_MAX_PARTS - 1];
	int iaParts[OxTOOLBAR_MAX_PARTS];
} OxStatusBarObject;

extern OxAPI OxClass OxStatusBarClass;

BOOL OxStatusBarClass_Init();
OxAPI OxStatusBarObject* OxStatusBar_New(OxWidgetObject* oxParent, int* paParts);
OxAPI int OxStatusBar_GetHeight(OxStatusBarObject* ox);
OxAPI BOOL OxStatusBar_Message(OxStatusBarObject* ox, char* sMessage, int iPart);

#endif /* OxSTATUSBAROBJECT_H */

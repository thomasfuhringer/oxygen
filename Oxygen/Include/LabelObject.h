// LabelObject.h  | Oxygen © 2015 by Thomas Führinger
#ifndef OxLABELOBJECT_H
#define OxLABELOBJECT_H

#include "Oxygen.h"

typedef struct _OxLabelObject {
	OxWidgetObject_HEAD
		char* sFormat;
	COLORREF textColor;
	unsigned int fContentAlignment;
	OxWidgetObject* oxAssociatedWidget;
} OxLabelObject;

extern OxAPI OxClass OxLabelClass;

BOOL OxLabelClass_Init();
OxAPI OxLabelObject* OxLabel_New();
OxAPI BOOL OxLabel_Align(OxLabelObject* ox, unsigned int fAlignment);
OxAPI BOOL OxLabel_SetData(OxLabelObject* ox, OxObject* oxData);
OxAPI BOOL OxLabel_SetTextColor(OxLabelObject* ox, int iRed, int iGreen, int iBlue);

#endif /* OxLABELOBJECT_H */

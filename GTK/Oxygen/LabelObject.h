// LabelObject.h  | Oxygen © 2016 by Thomas Führinger
#ifndef OxLABELOBJECT_H
#define OxLABELOBJECT_H

#include <Oxygen.h>

typedef struct _OxLabelObject {
	OxWidgetObject_HEAD
		char* sFormat;
	unsigned int fContentAlignment;
	OxWidgetObject* oxAssociatedWidget;
} OxLabelObject;

extern OxAPI OxClass OxLabelClass;

bool OxLabelClass_Init();
OxAPI OxLabelObject* OxLabel_New();
OxAPI bool OxLabel_Align(OxLabelObject* ox, unsigned int fAlignment);
OxAPI bool OxLabel_SetData(OxLabelObject* ox, OxObject* oxData);
OxAPI bool OxLabel_SetTextColor(OxLabelObject* ox, int iRed, int iGreen, int iBlue);

#endif /* OxLABELOBJECT_H */

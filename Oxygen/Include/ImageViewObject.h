// ImageViewObject.h  | Oxygen © 2015 by Thomas Führinger
#ifndef OxIMAGEVIEWOBJECT_H
#define OxIMAGEVIEWOBJECT_H

#include "Oxygen.h"

typedef struct _OxImageViewObject {
	OxWidgetObject_HEAD
	BOOL bStretch;
	BOOL bFill;
} OxImageViewObject;

extern OxAPI OxClass OxImageViewClass;

BOOL OxImageViewClass_Init();
OxAPI OxImageViewObject* OxImageView_New(OxWidgetObject* oxParent, OxRect* rc, OxImageObject* oxImage);
OxAPI BOOL OxImageView_SetData(OxImageViewObject* ox, OxObject* oxData);
OxAPI BOOL OxImageView_RenderData(OxImageViewObject* ox, BOOL bEdit);

#endif /* OxIMAGEVIEWOBJECT_H */

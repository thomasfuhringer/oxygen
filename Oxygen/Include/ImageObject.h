// ImageObject.h  | Oxygen © 2015 by Thomas Führinger
#ifndef OxIMAGEOBJECT_H
#define OxIMAGEOBJECT_H

#include "Oxygen.h"

typedef struct _OxImageObject {
	OxObject_HEAD
		HANDLE hWin;
	int iType;
} OxImageObject;

extern OxAPI OxClass OxImageClass;

BOOL OxAPI OxImageClass_Init();
OxAPI OxImageObject* OxImage_New();
OxAPI OxImageObject* OxImage_FromFile(const char* sFileName);

typedef enum  { OxIMAGEFORMAT_NONE, OxIMAGEFORMAT_BMP, OxIMAGEFORMAT_ICO } OxImageFormatEnum;

#endif /* OxIMAGEOBJECT_H */

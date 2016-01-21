// ImageObject.h  | Oxygen © 2015 by Thomas Führinger
#ifndef OxIMAGEOBJECT_H
#define OxIMAGEOBJECT_H

#include "Oxygen.h"


#define OxICON_OXYGEN           0
#define OxICON_FILEOPEN         1
#define OxICON_REFRESH          2
#define OxICON_SAVE             3

typedef struct _OxImageObject {
	OxObject_HEAD
		HANDLE hWin;
	int iType;
} OxImageObject;

extern OxAPI OxClass OxImageClass;

BOOL OxAPI OxImageClass_Init();
OxAPI OxImageObject* OxImage_New();
OxAPI OxImageObject* OxImage_FromFile(const char* sFileName);
OxAPI OxImageObject* OxImage_FromStock(int iStockID);
OxAPI OxImageObject* OxImage_FromWindowsResource(WORD wResourceID);

typedef enum  { OxIMAGEFORMAT_NONE, OxIMAGEFORMAT_BMP, OxIMAGEFORMAT_ICO } OxImageFormatEnum;

#endif /* OxIMAGEOBJECT_H */

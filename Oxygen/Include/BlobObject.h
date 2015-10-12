// BlobObject.h  | Oxygen © 2015 by Thomas Führinger
#ifndef OxBLOBOBJECT_H
#define OxBLOBOBJECT_H

#include "Oxygen.h"

typedef struct _OxBlobObject {
	OxObject_HEAD
		BYTE* pData;
	size_t nSize;
} OxBlobObject;

extern OxAPI OxClass OxBlobClass;

BOOL OxBlobClass_Init();
OxAPI OxBlobObject* OxBlob_New(size_t nSize);
OxAPI OxBlobObject* OxBlob_FromString(const char* sString, const char* sFormat);
OxAPI BOOL OxBlob_Compare(OxBlobObject* ox, OxObject* ox2, int iOperator);
OxAPI BOOL OxBlob_SaveToFile(OxBlobObject* ox, const char* sFileName);

#endif /* OxBLOBOBJECT_H */

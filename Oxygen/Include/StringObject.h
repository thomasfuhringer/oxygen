// StringObject.h  | Oxygen © 2015 by Thomas Führinger
#ifndef OxSTRINGOBJECT_H
#define OxSTRINGOBJECT_H

#include "Oxygen.h"

typedef struct _OxStringObject {
	OxObject_HEAD
		char* sString;
} OxStringObject;

extern OxAPI OxClass OxStringClass;

BOOL OxStringClass_Init();
OxAPI OxStringObject* OxString_New();
OxAPI OxStringObject* OxString_FromString(const char* sString, const char* sFormat);
OxAPI OxStringObject* OxString_FromWide(const LPWSTR szString);
OxAPI char* OxString_AsString(OxStringObject* ox, const char* sFormat);
OxAPI LPWSTR OxString_AsWide(OxStringObject* ox);
OxAPI BOOL OxString_Compare(OxStringObject* ox, OxObject* ox2, int iOperator);

#endif /* OxSTRINGOBJECT_H */

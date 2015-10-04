// IntegerObject.h  | Oxygen © 2015 by Thomas Führinger
#ifndef OxINTEGEROBJECT_H
#define OxINTEGEROBJECT_H

#include "Oxygen.h"

typedef struct _OxIntegerObject {
	OxObject_HEAD
		int iInteger;
} OxIntegerObject;

extern OxAPI OxClass OxIntegerClass;

BOOL OxIntegerClass_Init();
OxAPI OxIntegerObject* OxInteger_New(int iInteger);
OxAPI OxIntegerObject* OxInteger_FromString(const char* sString, const char* sFormat);
OxAPI char* OxInteger_AsString(OxIntegerObject* ox, const char* sFormat);

#endif /* OxINTEGEROBJECT_H */

// FloatObject.h  | Oxygen © 2015 by Thomas Führinger
#ifndef OxFLOATOBJECT_H
#define OxFLOATOBJECT_H

#include "Oxygen.h"

typedef struct _OxFloatObject {
	OxObject_HEAD
		double dFloat;
} OxFloatObject;

extern OxAPI OxClass OxFloatClass;

BOOL OxFloatClass_Init();
OxAPI OxFloatObject* OxFloat_New(double dFloat);
OxAPI OxFloatObject* OxFloat_FromString(const char* sString, const char* sFormat);
OxAPI char* OxFloat_AsString(OxFloatObject* ox, const char* sFormat);

#endif /* OxFLOATOBJECT_H */

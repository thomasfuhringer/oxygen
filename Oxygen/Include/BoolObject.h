// BoolObject.h  | Oxygen © 2015 by Thomas Führinger
#ifndef OxBOOLOBJECT_H
#define OxBOOLOBJECT_H

#include "Oxygen.h"

typedef struct _OxBoolObject {
	OxObject_HEAD
		BOOL bBool;
} OxBoolObject;

extern OxAPI OxBoolObject* OxTrue; // global singleton
extern OxAPI OxBoolObject* OxFalse; // global singleton

extern OxAPI OxClass OxBoolClass;

BOOL OxBoolClass_Init();
OxAPI OxBoolObject* OxBool_FromBool(BOOL bBool);
OxAPI BOOL OxBool_AsBool(OxBoolObject*);

#endif /* OxBOOLOBJECT_H */

// Object.h  | Oxygen © 2015 by Thomas Führinger
#ifndef OxOBJECT_H
#define OxOBJECT_H

#include "Oxygen.h"

#define Ox_MAX_VIRT_METHODS  	20

#define OxOBJECT_FROMSTRING  	0
#define OxOBJECT_ASSTRING       1
#define OxOBJECT_REPRESENT  	2
#define OxOBJECT_COMPARE    	3

#define OxOBJECT_LASTMETHID  	3

#define Ox_INHERIT_METHODS(pClass) memcpy((pClass)->aMethods, (pClass)->pBase->aMethods, sizeof((pClass)->aMethods))

typedef struct _OxClass OxClass;
typedef struct _OxObject OxObject;
typedef OxObject* (*OxMethod)(OxObject*);     // basic method signature
typedef BOOL(*OxCallback)(OxObject*);

typedef struct _OxClass {
	char* sName;                              // largely for debugging purposes
	OxClass* pBase;
	size_t nSize;
	BOOL(*fnDelete)(OxObject*);
	void* pUserData;
	OxMethod aMethods[Ox_MAX_VIRT_METHODS];
} OxClass;

extern OxAPI OxClass OxObjectClass;           // global singleton

#define OxObject_HEAD  \
        OxClass* pClass; \
        int iRefCount;

typedef struct _OxObject {
	OxObject_HEAD
} OxObject;

extern OxAPI OxObject* OxNull;                // global singleton

OxAPI BOOL OxObjectClass_Init();
OxAPI OxObject* OxObject_Allocate(OxClass* pClass);
OxAPI OxObject* OxObject_New();
OxAPI BOOL OxObject_RefCountIncrease(OxObject* ox);
OxAPI BOOL OxObject_RefCountDecrease(OxObject* ox);
#define OxRET(ox) OxObject_RefCountIncrease((OxObject*)ox)
#define OxREL(ox) OxObject_RefCountDecrease((OxObject*)ox)
OxAPI OxObject* OxObject_FromStringV(OxClass* pClass, const char* sString, const char* sFormat);
OxAPI char* OxObject_AsStringV(OxObject* ox, const char* sFormat);
OxAPI char* OxObject_Represent(OxObject* ox);
OxAPI char* OxObject_RepresentV(OxObject* ox);
OxAPI BOOL OxObject_IsKindOf(OxObject* ox, OxClass* pClass);
OxAPI BOOL OxObject_Compare(OxObject* ox, OxObject* ox2, int iOperator);
OxAPI BOOL OxObject_CompareV(OxObject* ox, OxObject* ox2, int iOperator);

enum {
	OxLT,
	OxLE,
	OxEQ,
	OxNE,
	OxGT,
	OxGE
} OxComparisonOperatorEnum;

#endif /* OxOBJECT_H */
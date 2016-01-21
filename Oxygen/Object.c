// Object.c  | Oxygen © 2015 by Thomas Führinger
#include "Oxygen.h"
#include <stdarg.h>

static OxClass* pOxClass;      // to simplify code
OxObject* OxNull;              // global singleton

OxObject*
OxObject_Allocate(OxClass* pClass)
{
	OxObject* ox = (OxObject*)OxAllocateZeroed(pClass->nSize);
	if (ox != NULL) {
		//memset(ox, 0, pClass->nSize);
		ox->iRefCount = 1;
		ox->pClass = pClass;
		return ox;
	}
	else
		return NULL;
}

OxObject*
OxObject_New()
{
	OxObject* ox = (OxObject*)OxObject_Allocate(pOxClass);
	if (ox != NULL) {
		//ox->hWin = 0;
		return (OxObject*)ox;
	}
	else
		return NULL;
}

BOOL
OxObject_RefCountIncrease(OxObject* ox)
{
	if (ox == NULL || ox->iRefCount == -1)
		return TRUE;
	ox->iRefCount++;
	return TRUE;
}

BOOL
OxObject_RefCountDecrease(OxObject** ox)
{
	BOOL bResult = TRUE;
	if (*ox == NULL || (*ox)->iRefCount < 1)
		return TRUE;
	if (--(*ox)->iRefCount == 0) {
		bResult = (*ox)->pClass->fnDelete(*ox);
		*ox = NULL;
	}
	return bResult;
}

OxObject*
OxObject_FromString(const char* sString, const char* sFormat)
{
	OxErr_SetString(OxERROR_RUNTIME, "OxObject can not be created from a string.");
	return NULL;
}

OxObject*
OxObject_FromStringV(OxClass* pClass, const char* sString, const char* sFormat)
{
	OxObject*(*Method)(const char*, const char*) = (OxObject*(*)(const char*, const char*))pClass->aMethods[OxOBJECT_FROMSTRING];
	return Method(sString, sFormat);
};

char*
OxObject_AsString(OxObject* ox, const char* sFormat)
{
	return OxObject_Represent(ox);
}

char*
OxObject_AsStringV(OxObject* ox, const char* sFormat)
{
	char*(*Method)(OxObject*) = (char*(*)(OxObject*))ox->pClass->aMethods[OxOBJECT_ASSTRING];
	return Method(ox);
};

char*
OxObject_Represent(OxObject* ox)
{
	char* sString = (char*)OxAllocate(250 * sizeof(char));
	int iIndex = sprintf_s(sString, 250, "%s object @ %p, %d times referenced", ox->pClass->sName, ox, ox->iRefCount);
	return sString;
}

char*
OxObject_RepresentV(OxObject* ox)
{
	char*(*Method)(OxObject*) = (char*(*)(OxObject*))ox->pClass->aMethods[OxOBJECT_REPRESENT];
	return Method(ox);
};

static BOOL
IterCompareClasses(OxClass* pClassO, OxClass* pClass)
{
	if (pClassO == pClass)
		return TRUE;
	else
		if (pClassO->pBase)
			return IterCompareClasses(pClassO->pBase, pClass);
	return FALSE;
}

BOOL
OxObject_IsKindOf(OxObject* ox, OxClass* pClass)
{
	if (ox)
		return IterCompareClasses(ox->pClass, pClass);
	else
		return FALSE;
}

BOOL
OxObject_Compare(OxObject* ox, OxObject* ox2, int iOperator)
{
	if ((iOperator == OxEQ && ox == ox2) || (iOperator == OxNE && ox != ox2))
		return TRUE;
	else
		return FALSE;
}

BOOL
OxObject_CompareV(OxObject* ox, OxObject* ox2, int iOperator)
{
	if (iOperator == OxEQ && ox == ox2)
		return TRUE;
	BOOL(*Method)(OxObject*) = (BOOL(*)(OxObject*))ox->pClass->aMethods[OxOBJECT_COMPARE];
	return Method(ox);
};

BOOL
OxObject_Delete(OxObject* ox)
{
	//OutputDebugStringA("+-- Deleting Object --- ");
	//OutputDebugStringA(ox->pClass->sName);
	//OutputDebugStringA("\n\n");
	//char* sString = OxObject_RepresentV(ox); OutputDebugStringA(sString); OxFree(sString); OutputDebugStringA("\n");

	if (OxFree(ox))
		return TRUE;
	else {
		OxErr_SetString(OxERROR_RUNTIME, "Can not free memory.");
		return FALSE;
	}
}

OxClass OxObjectClass = {
	"Object",                  /* sName */
	NULL,                      /* pBase */
	sizeof(OxObject),          /* nSize */
	OxObject_Delete,           /* fnDelete */
	NULL,                      /* pUserData */
	0                          /* aMethods */
};

BOOL
OxObjectClass_Init()
{
	pOxClass = &OxObjectClass;
	pOxClass->aMethods[OxOBJECT_FROMSTRING] = OxObject_FromString;
	pOxClass->aMethods[OxOBJECT_ASSTRING] = OxObject_AsString;
	pOxClass->aMethods[OxOBJECT_REPRESENT] = OxObject_Represent;
	pOxClass->aMethods[OxOBJECT_COMPARE] = OxObject_Compare;

	OxNull = OxObject_Allocate(pOxClass);
	OxNull->iRefCount = -1;

	return TRUE;
}

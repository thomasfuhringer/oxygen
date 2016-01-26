// Object.c  | Oxygen © 2015 by Thomas Führinger
#include <Oxygen.h>

static OxClass* pOxClass;      // to simplify code
OxObject* OxNull;              // global singleton

OxObject*
OxObject_Allocate(OxClass* pClass)
{
	OxObject* ox = (OxObject*)OxAllocateZeroed(pClass->nSize);
	if (ox != NULL) {
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
		return (OxObject*)ox;
	}
	else
		return NULL;
}

bool
OxObject_RefCountIncrease(OxObject* ox)
{
	if (ox == NULL || ox->iRefCount == -1)
		return true;
	ox->iRefCount++;
	return true;
}

bool
OxObject_RefCountDecrease(OxObject** ox)
{
	bool bResult = true;
	if (*ox == NULL || (*ox)->iRefCount < 1)
		return true;
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
	int iIndex = sprintf(sString, "%s object @ %p, %d times referenced", ox->pClass->sName, ox, ox->iRefCount);
	return sString;
}

char*
OxObject_RepresentV(OxObject* ox)
{
	char*(*Method)(OxObject*) = (char*(*)(OxObject*))ox->pClass->aMethods[OxOBJECT_REPRESENT];
	return Method(ox);
};

static bool
IterCompareClasses(OxClass* pClassO, OxClass* pClass)
{
	if (pClassO == pClass)
		return true;
	else
		if (pClassO->pBase)
			return IterCompareClasses(pClassO->pBase, pClass);
	return false;
}

bool
OxObject_IsKindOf(OxObject* ox, OxClass* pClass)
{
	if (ox)
		return IterCompareClasses(ox->pClass, pClass);
	else
		return false;
}

bool
OxObject_Compare(OxObject* ox, OxObject* ox2, int iOperator)
{
	if ((iOperator == OxEQ && ox == ox2) || (iOperator == OxNE && ox != ox2))
		return true;
	else
		return false;
}

bool
OxObject_CompareV(OxObject* ox, OxObject* ox2, int iOperator)
{
	if (iOperator == OxEQ && ox == ox2)
		return true;
	bool(*Method)(OxObject*) = (bool(*)(OxObject*))ox->pClass->aMethods[OxOBJECT_COMPARE];
	return Method(ox);
};

bool
OxObject_Delete(OxObject* ox)
{
	//char* sString = OxObject_RepresentV(ox); OutputDebugStringA(sString); OxFree(sString); OutputDebugStringA("\n");

	if (OxFree(ox))
		return true;
	else {
		OxErr_SetString(OxERROR_RUNTIME, "Can not free memory.");
		return false;
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

bool
OxObjectClass_Init()
{
	pOxClass = &OxObjectClass;
	pOxClass->aMethods[OxOBJECT_FROMSTRING] = OxObject_FromString;
	pOxClass->aMethods[OxOBJECT_ASSTRING] = OxObject_AsString;
	pOxClass->aMethods[OxOBJECT_REPRESENT] = OxObject_Represent;
	pOxClass->aMethods[OxOBJECT_COMPARE] = OxObject_Compare;

	OxNull = OxObject_Allocate(pOxClass);
	OxNull->iRefCount = -1;

	return true;
}

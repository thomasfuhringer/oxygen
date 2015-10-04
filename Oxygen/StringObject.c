// StringObject.c  | Oxygen © 2015 by Thomas Führinger
#include "Oxygen.h"

static OxClass* pOxClass;

OxStringObject*
OxString_New()
{
	OxStringObject* ox = (OxStringObject*)OxObject_Allocate(pOxClass);
	if (ox != NULL) {
		ox->sString = NULL;
		return ox;
	}
	else
		return NULL;
}

OxStringObject*
OxString_FromString(const char* sString, const char* sFormat)
{
	if (sString == NULL)
		return NULL;
	OxStringObject* ox = OxString_New();
	ox->sString = OxDuplicateString(sString);
	return ox;
}

OxStringObject*
OxString_FromWide(const LPWSTR szString)
{
	OxStringObject* ox = OxString_New();
	ox->sString = OxToU8(szString);
	return ox;
}

char*
OxString_AsString(OxStringObject* ox, const char* sFormat)
{
	return OxDuplicateString(ox->sString);
}

LPWSTR
OxString_AsWide(OxStringObject* ox)
{
	LPWSTR szString = OxToW(ox->sString);
	return szString;
}

BOOL
OxString_Compare(OxStringObject* ox, OxObject* ox2, int iOperator)
{
	if (!OxObject_IsKindOf(ox2, pOxClass))
		if (iOperator == OxNE)
			return TRUE;
		else
			return FALSE;

	int iResult = lstrcmpA(ox->sString, ((OxStringObject*)ox2)->sString);
	if (iResult == 0)
		if (iOperator == OxEQ || iOperator == OxLE || iOperator == OxGE)
			return TRUE;
		else
			return FALSE;
	if (iResult < 0 && (iOperator == OxLT || iOperator == OxLE))
		return TRUE;
	if (iResult > 0 && (iOperator == OxGT || iOperator == OxGE))
		return TRUE;

	return FALSE;
}

char*
OxString_Represent(OxStringObject* ox)
{
	char* sString = (char*)OxAllocate(250 * sizeof(char));
	int iIndex = sprintf_s(sString, 250, "OxString @ %p, %d times referenced, \"%s\"", ox, ox->iRefCount, ox->sString);
	return sString;
}

static BOOL
OxString_Delete(OxObject* oxSelf)
{
	OxStringObject* ox = (OxStringObject*)oxSelf;
	// RefDown(...);
	if (ox->sString && !OxFree(ox->sString))
		return FALSE;
	return pOxClass->pBase->fnDelete((OxObject*)ox);
}

OxClass OxStringClass = {
	"String",                  /* sName */
	&OxObjectClass,            /* pBase */
	sizeof(OxStringObject),    /* nSize */
	OxString_Delete,           /* fnDelete */
	NULL,                      /* pUserData */
	0,                         /* aMethods */
};

BOOL
OxStringClass_Init()
{
	pOxClass = &OxStringClass;
	Ox_INHERIT_METHODS(pOxClass);
	pOxClass->aMethods[OxOBJECT_REPRESENT] = OxString_Represent;
	pOxClass->aMethods[OxOBJECT_FROMSTRING] = OxString_FromString;
	pOxClass->aMethods[OxOBJECT_ASSTRING] = OxString_AsString;
	pOxClass->aMethods[OxOBJECT_COMPARE] = OxString_Compare;
	return TRUE;
}

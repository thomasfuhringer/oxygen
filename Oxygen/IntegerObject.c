// IntegerObject.c  | Oxygen © 2015 by Thomas Führinger
#include "Oxygen.h"

static OxClass* pOxClass;

OxIntegerObject*
OxInteger_New(int iInteger)
{
	OxIntegerObject* ox = (OxIntegerObject*)OxObject_Allocate(pOxClass);
	if (ox != NULL) {
		ox->iInteger = iInteger;
		return ox;
	}
	else
		return NULL;
}

OxIntegerObject*
OxInteger_FromString(const char* sString, const char* sFormat)
{
	if (sString == NULL) {
		OxErr_SetString(OxERROR_RUNTIME, "Integer conversion of NULL string attempted.");
		return NULL;
	}
	int iInteger = atoi(sString);
	OxIntegerObject* ox = OxInteger_New(iInteger);
	return ox;
}

char*
OxInteger_AsString(OxIntegerObject* ox, const char* sFormat)
{
	char* sString = (char*)OxAllocate(20 * sizeof(char));
	int iIndex = sprintf_s(sString, 20, "%d", ox->iInteger);
	return sString;
}

BOOL
OxInteger_Compare(OxIntegerObject* ox, OxObject* ox2, int iOperator)
{
	OutputDebugStringA("comp\n");
	if (!OxObject_IsKindOf(ox2, pOxClass))
		if (iOperator == OxNE)
			return TRUE;
		else
			return FALSE;
	OutputDebugStringA("compare\n");

	if (ox->iInteger == ((OxIntegerObject*)ox2)->iInteger)
		if (iOperator == OxEQ || iOperator == OxLE || iOperator == OxGE)
			return TRUE;
		else
			return FALSE;

	if (ox->iInteger > ((OxIntegerObject*)ox2)->iInteger && iOperator == OxGT)
		return TRUE;
	if (ox->iInteger < ((OxIntegerObject*)ox2)->iInteger && iOperator == OxLT)
		return TRUE;
	return FALSE;
}

char*
OxInteger_Represent(OxObject* ox)
{
	char* sString = (char*)OxAllocate(250 * sizeof(char));
	int iIndex = sprintf_s(sString, 250, "OxInteger %d @ %p, %d times referenced", ((OxIntegerObject*)ox)->iInteger, ox, ox->iRefCount);
	return sString;
}

static BOOL
OxInteger_Delete(OxIntegerObject* ox)
{
	return pOxClass->pBase->fnDelete(ox);
}

OxClass OxIntegerClass = {
	"Integer",                 /* sName */
	&OxObjectClass,            /* pBase */
	sizeof(OxIntegerObject),   /* nSize */
	OxInteger_Delete,          /* fnDelete */
	NULL,                      /* pUserData */
	0,                         /* aMethods */
};

BOOL
OxIntegerClass_Init()
{
	pOxClass = &OxIntegerClass;
	Ox_INHERIT_METHODS(pOxClass);
	pOxClass->aMethods[OxOBJECT_REPRESENT] = OxInteger_Represent;
	pOxClass->aMethods[OxOBJECT_FROMSTRING] = OxInteger_FromString;
	pOxClass->aMethods[OxOBJECT_ASSTRING] = OxInteger_AsString;
	pOxClass->aMethods[OxOBJECT_COMPARE] = OxInteger_Compare;
	return TRUE;
}
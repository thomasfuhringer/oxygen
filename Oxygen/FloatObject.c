// FloatObject.c  | Oxygen © 2015 by Thomas Führinger
#include "Oxygen.h"

static OxClass* pOxClass;

OxFloatObject*
OxFloat_New(double dFloat)
{
	OxFloatObject* ox = (OxFloatObject*)OxObject_Allocate(pOxClass);
	if (ox != NULL) {
		ox->dFloat = dFloat;
		return ox;
	}
	else
		return NULL;
}

OxFloatObject*
OxFloat_FromString(const char* sString, const char* sFormat)
{
	if (sString == NULL) {
		OxErr_SetString(OxERROR_RUNTIME, "Float conversion of NULL string attempted.");
		return NULL;
	}
	double dFloat = atof(sString);
	OxFloatObject* ox = OxFloat_New(dFloat);
	return ox;
}

char*
OxFloat_AsString(OxFloatObject* ox, const char* sFormat)
{
	char* sString = (char*)OxAllocate(20 * sizeof(char));
	int iIndex = sprintf_s(sString, 20, "%f", ox->dFloat);
	return sString;
}

BOOL
OxFloat_Compare(OxFloatObject* ox, OxObject* ox2, int iOperator)
{
	if (!OxObject_IsKindOf(ox2, pOxClass))
		if (iOperator == OxNE)
			return TRUE;
		else
			return FALSE;

	if (ox->dFloat == ((OxFloatObject*)ox2)->dFloat)
		if (iOperator == OxEQ || iOperator == OxLE || iOperator == OxGE)
			return TRUE;
		else
			return FALSE;

	if (ox->dFloat > ((OxFloatObject*)ox2)->dFloat && iOperator == OxGT)
		return TRUE;
	if (ox->dFloat < ((OxFloatObject*)ox2)->dFloat && iOperator == OxLT)
		return TRUE;
	return FALSE;
}

char*
OxFloat_Represent(OxObject* ox)
{
	char* sString = (char*)OxAllocate(250 * sizeof(char));
	int iIndex = sprintf_s(sString, 250, "OxFloat @ %p, %d times referenced, \"%f\"", ox, ox->iRefCount, ((OxFloatObject*)ox)->dFloat);
	return sString;
}

static BOOL
OxFloat_Delete(OxFloatObject* ox)
{
	return pOxClass->pBase->fnDelete(ox);
}

OxClass OxFloatClass = {
	"Float",                   /* sName */
	&OxObjectClass,            /* pBase */
	sizeof(OxFloatObject),     /* nSize */
	OxFloat_Delete,            /* fnDelete */
	NULL,                      /* pUserData */
	0,                         /* aMethods */
};

BOOL
OxFloatClass_Init()
{
	pOxClass = &OxFloatClass;
	Ox_INHERIT_METHODS(pOxClass);
	pOxClass->aMethods[OxOBJECT_REPRESENT] = OxFloat_Represent;
	pOxClass->aMethods[OxOBJECT_FROMSTRING] = OxFloat_FromString;
	pOxClass->aMethods[OxOBJECT_ASSTRING] = OxFloat_AsString;
	pOxClass->aMethods[OxOBJECT_COMPARE] = OxFloat_Compare;
	return TRUE;
}
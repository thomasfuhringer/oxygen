// BoolObject.c  | Oxygen © 2015 by Thomas Führinger
#include "Oxygen.h"

static OxClass* pOxClass;
OxBoolObject* OxTrue; // global singleton
OxBoolObject* OxFalse; // global singleton

static OxBoolObject*
OxBool_New(BOOL bBool)
{
	OxBoolObject* ox = (OxBoolObject*)OxObject_Allocate(pOxClass);
	if (ox != NULL) {
		ox->bBool = bBool;
		return ox;
	}
	else
		return NULL;
}

OxBoolObject*
OxBool_FromBool(BOOL bBool)
{
	return bBool ? OxTrue : OxFalse;
}

BOOL
OxBool_AsBool(OxBoolObject* ox)
{
	return (ox == OxTrue);
}

char*
OxBool_Represent(OxBoolObject* ox)
{
	char* sString = (char*)OxAllocate(15 * sizeof(char));
	int iIndex = sprintf_s(sString, 15, "OxBool, %s", ox->bBool ? "True" : "False");
	return sString;
}

OxClass OxBoolClass = {
	"Bool",                    /* sName */
	&OxObjectClass,            /* pBase */
	sizeof(OxBoolObject),      /* nSize */
	NULL,                      /* fnDelete */
	NULL,                      /* pUserData */
	0,                         /* aMethods */
};

BOOL
OxBoolClass_Init()
{
	pOxClass = &OxBoolClass;
	Ox_INHERIT_METHODS(pOxClass);
	pOxClass->aMethods[OxOBJECT_REPRESENT] = OxBool_Represent;
	//pOxClass->aMethods[OxOBJECT_FROMSTRING] = OxBool_FromString;
	//pOxClass->aMethods[OxOBJECT_ASSTRING] = OxBool_AsString;

	OxTrue = OxBool_New(TRUE);
	OxTrue->iRefCount = -1;
	OxFalse = OxBool_New(FALSE);
	OxFalse->iRefCount = -1;

	return TRUE;
}
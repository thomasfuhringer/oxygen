// ListObject.c  | Oxygen © 2015 by Thomas Führinger
#include "Oxygen.h"

static OxClass* pOxClass;

OxListObject*
OxList_New(size_t nAllocate)
{
	nAllocate = nAllocate < 3 ? 3 : nAllocate;
	OxListObject* ox = (OxListObject*)OxObject_Allocate(pOxClass);
	if (ox != NULL) {
		ox->oxItems = (OxObject**)OxAllocate(nAllocate * sizeof(OxObject*));
		ox->nSize = 0;
		ox->nAllocated = nAllocate;
		ox->bTakeOverRefs = TRUE;
		return ox;
	}
	else
		return NULL;
}

size_t
OxList_AppendItem(OxListObject* ox, OxObject* oxItem)
{
	if (ox->nSize == ox->nAllocated) {
		ox->nAllocated *= 2;
		if (ox->oxItems = (OxObject**)OxReAllocate(ox->oxItems, ox->nAllocated * sizeof(OxObject*)) == NULL) {
			OxErr_SetString(OxERROR_RUNTIME, "Can not allocate memory for List.");
			return -1;
		}
	}
	ox->oxItems[ox->nSize++] = oxItem;
	return ox->nSize - 1;
}

BOOL
OxList_SetItem(OxListObject* ox, size_t nIndex, OxObject* oxItem)
{
	if (nIndex < 0 || nIndex >= ox->nAllocated) {
		OxErr_SetString(OxERROR_RUNTIME, "List index out of range.");
		return FALSE;
	}
	ox->oxItems[nIndex] = oxItem;
	return TRUE;
}

OxObject*
OxList_GetItem(OxListObject* ox, size_t nIndex)
{
	if (nIndex < 0 || nIndex >= ox->nSize) {
		OxErr_SetStringFormat(OxERROR_RUNTIME, "List index %d out of range. List contains only %d items.", nIndex, ox->nSize);
		return NULL;
	}
	return ox->oxItems[nIndex];
}

BOOL
OxList_Clear(OxListObject* ox)
{
	if (ox->bTakeOverRefs)
		for (size_t nIndex = 0; nIndex < ox->nSize; nIndex++)
			if (!OxREL(ox->oxItems[nIndex]))
				return FALSE;
	ox->nSize = ox->nSize = 0;
	return TRUE;
}

char*
OxList_Represent(OxListObject* ox)
{
	char* sString = (char*)OxAllocate(250 * sizeof(char));
	int iIndex = sprintf_s(sString, 250, "OxList @ %p, %d times referenced, %d items", ox, ox->iRefCount, ox->nAllocated);
	return sString;
}

static BOOL
OxList_Delete(OxObject* ox)
{
	if (!OxList_Clear(ox))
		return FALSE;

	if (!OxFree(((OxListObject*)ox)->oxItems))
		return FALSE;
	return pOxClass->pBase->fnDelete(ox);
}

OxClass OxListClass = {
	"List",                    /* sName */
	&OxObjectClass,            /* pBase */
	sizeof(OxListObject),      /* nSize */
	OxList_Delete,             /* fnDelete */
	NULL,                      /* pUserData */
	0,                         /* aMethods */
};

BOOL
OxListClass_Init()
{
	pOxClass = &OxListClass;
	Ox_INHERIT_METHODS(pOxClass);
	pOxClass->aMethods[OxOBJECT_REPRESENT] = OxList_Represent;
	return TRUE;
}

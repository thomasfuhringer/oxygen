// BlobObject.c  | Oxygen © 2015 by Thomas Führinger
#include "Oxygen.h"

static OxClass* pOxClass;

OxBlobObject*
OxBlob_New(size_t nSize)
{
	OxBlobObject* ox = (OxBlobObject*)OxObject_Allocate(pOxClass);
	if (ox != NULL) {
		ox->pData = (char*)OxAllocate(nSize);
		ox->nSize = nSize;
		return ox;
	}
	else
		return NULL;
}

OxBlobObject*
OxBlob_FromString(const char* sString, const char* sFormat)
{
	OxBlobObject* ox;
	if (sString == NULL) {
		OxErr_SetString(OxERROR_RUNTIME, "Can not create Blob from NULL string.");
		return NULL;
	}
	ox = OxBlob_New(strlen(sString) + 1);
	strcpy_s(ox->pData, ox->nSize, sString);
	return ox;
}

BOOL
OxBlob_SaveToFile(OxBlobObject* ox, const char* sFileName)
{
	HANDLE hFile;
	DWORD dwWritten;
	BOOL bSuccess = FALSE;
	LPWSTR szFileName = OxToW(sFileName);
	hFile = CreateFile(szFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
		OxErr_SetFromWindows();
	else {
		if (!WriteFile(hFile, ox->pData, ox->nSize, &dwWritten, NULL))
			OxErr_SetFromWindows();
		else
			bSuccess = TRUE;
	}
	CloseHandle(hFile);
	return bSuccess;
}

BOOL
OxBlob_Compare(OxBlobObject* ox, OxObject* ox2, int iOperator)
{
	if (!OxObject_IsKindOf(ox2, pOxClass))
		if (iOperator == OxNE)
			return TRUE;
		else
			return FALSE;
	if (iOperator == OxEQ && ox->pData == ((OxBlobObject*)ox2)->pData)
		return TRUE;
	else
		return FALSE;
}

char*
OxBlob_Represent(OxBlobObject* ox)
{
	char* sString = (char*)OxAllocate(250 * sizeof(char));
	int iIndex = sprintf_s(sString, 250, "OxBlob of %i bytes @ %p, %d times referenced, \"%s\"", ox->nSize, ox, ox->iRefCount);
	return sString;
}

static BOOL
OxBlob_Delete(OxBlobObject* ox)
{
	if (ox->pData && !OxFree(ox->pData))
		return FALSE;
	return pOxClass->pBase->fnDelete((OxObject*)ox);
}

OxClass OxBlobClass = {
	"Blob ",                   /* sName */
	&OxObjectClass,            /* pBase */
	sizeof(OxBlobObject),      /* nSize */
	OxBlob_Delete,             /* fnDelete */
	NULL,                      /* pUserData */
	0,                         /* aMethods */
};

BOOL
OxBlobClass_Init()
{
	pOxClass = &OxBlobClass;
	Ox_INHERIT_METHODS(pOxClass);
	pOxClass->aMethods[OxOBJECT_REPRESENT] = OxBlob_Represent;
	pOxClass->aMethods[OxOBJECT_FROMSTRING] = OxBlob_FromString;
	pOxClass->aMethods[OxOBJECT_COMPARE] = OxBlob_Compare;
	return TRUE;
}

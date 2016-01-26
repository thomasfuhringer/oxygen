// Oxygen.c  | Oxygen © 2016 by Thomas Führinger
#include "Oxygen.h"

GQuark OxQuark;

bool
OxInit(char* sAppName)
{
	OxQuark = g_quark_from_static_string("Oxygen");

	OxApp = OxApplication_New(sAppName);
	if (OxApp == NULL)
		return FALSE;
	if (!OxObjectClass_Init())
		return FALSE;
	if (!OxApplicationClass_Init())
		return FALSE;
	if (!OxWidgetClass_Init())
		return FALSE;
	if (!OxWindowClass_Init())
		return FALSE;
	if (!OxLabelClass_Init())
		return FALSE;

	return TRUE;
}

bool
OxExit(void)
{
	return TRUE;
}

void*
OxAllocate(size_t nSize)
{
	return malloc(nSize);
}

void*
OxAllocateZeroed(size_t nSize)
{
	return calloc(1, nSize);
}

void*
OxReAllocate(void* pChunk, size_t nSize)
{
	return realloc(pChunk, nSize);
}

bool
OxFree(void* pChunk)
{
	free(pChunk);
	return TRUE;
}

bool
OxAttachObject(OxObject** poxMember, OxObject* oxObject, bool bStrong)
{
	OxObject* tmp = *poxMember;
	if (bStrong)
		OxRET(oxObject);
	*poxMember = oxObject;
	OxREL(tmp);
	return TRUE;
}

char*
OxDuplicateString(const char* sString)
{
	int iLen = strlen(sString) + 1;
	char* sDuplicate = (char*)OxAllocate(iLen*sizeof(char));
	strcpy(sDuplicate, sString);
	return sDuplicate;
}

char* 
OxStringAppend(char* sMain, const char* sAppendix)
{
	if (sMain == NULL || sAppendix == NULL || strlen(sAppendix) == 0)
		return NULL;

	size_t nLen = strlen(sMain) + strlen(sAppendix) + 1;
	sMain = (char*)OxReAllocate(sMain, nLen);
	strcat(sMain, sAppendix);
	return sMain;
}

void
XX(OxObject* oxObject)
{
	g_message("%s - %d", oxObject->pClass->sName, oxObject->iRefCount);
}

void
Xi(char* sContext, int a)
{
	g_message("%s - %d", sContext, a);
}

char* OxCopyright = OxCOPYRIGHT;
float OxVersion = OxVER_MAJOR + (OxVER_MINOR / 100.0) + (OxVER_MICRO / 10000.0);

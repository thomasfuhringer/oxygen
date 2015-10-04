// IconObject.c  | Oxygen © 2015 by Thomas Führinger
#include "Oxygen.h"

static OxClass* pOxClass;

OxIconObject*
OxIcon_FromFile(const char* sFileName)
{
	OxIconObject* ox = (OxIconObject*)OxObject_Allocate(pOxClass);
	if (ox == NULL)
		return NULL;

	LPWSTR szFileName = OxToW(sFileName);
	ox->hWin = LoadImage(0, szFileName, IMAGE_ICON, 0, 0, LR_LOADFROMFILE); //LR_DEFAULTSIZE | 
	OxFree(szFileName);

	if (ox->hWin == 0) {
		OxErr_SetFromWindows();
		return NULL;
	}

	//OutputDebugStringA("+-- WW\n");
	//SetWindowLongPtr(ox->hWin, GWLP_USERDATA, (LONG_PTR)ox);
	return ox;
}

OxIconObject*
OxIcon_FromWindowsResource(WORD wResourceID)
{
	OxIconObject* ox = (OxIconObject*)OxObject_Allocate(pOxClass);
	if (ox == NULL)
		return NULL;

	ox->hWin = LoadIconW(OxApp->hInstance, MAKEINTRESOURCE(wResourceID));

	if (ox->hWin == 0) {
		OxErr_SetFromWindows();
		return NULL;
	}
	//SetWindowLongPtr(ox->hWin, GWLP_USERDATA, (LONG_PTR)ox);
	return ox;
}

OxIconObject*
OxIcon_FromStock(int iStockID)
{
	OxIconObject* ox = (OxIconObject*)OxObject_Allocate(pOxClass);
	if (ox == NULL)
		return NULL;
        
	ox->hWin = LoadIconW(OxApp->hDLL, MAKEINTRESOURCE(iStockID));

	if (ox->hWin == 0) {
		OxErr_SetFromWindows();
		return NULL;
	}
	//SetWindowLongPtr(ox->hWin, GWLP_USERDATA, (LONG_PTR)ox);
	return ox;
}

static BOOL
OxIcon_Delete(OxIconObject* ox)
{
	DestroyIcon(ox->hWin);
	return pOxClass->pBase->fnDelete(ox);
}

OxClass OxIconClass = {
	"Icon",               /* sName */
	&OxObjectClass,       /* pBase */
	sizeof(OxIconObject), /* nSize */
	OxIcon_Delete,        /* fnDelete */
	NULL,                 /* pUserData */
	0                     /* aMethods */
};

BOOL
OxIconClass_Init()
{
	pOxClass = &OxIconClass;
	Ox_INHERIT_METHODS(pOxClass);
	return TRUE;
}

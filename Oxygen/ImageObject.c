// ImageObject.c  | Oxygen © 2015 by Thomas Führinger
#include "Oxygen.h"

static OxClass* pOxClass;

OxImageObject*
OxImage_FromFile(const char* sFileName)
{
	OxImageObject* ox = (OxImageObject*)OxObject_Allocate(pOxClass);
	if (ox == NULL)
		return NULL;
	ox->hWin = NULL;

	LPWSTR szFileName = OxToW(sFileName);
	LPWSTR szExtension = PathFindExtensionW(szFileName);
	if (lstrcmpi(szExtension, L".ico") == 0) {
		ox->hWin = LoadImageW(0, szFileName, IMAGE_ICON, 0, 0, LR_LOADFROMFILE); //LR_DEFAULTSIZE | 
		if (ox->hWin == 0) {
			OxErr_SetFromWindows();
			return NULL;
		}
		ox->iType = OxIMAGEFORMAT_ICO;
		//OutputDebugStringW(L"XXXXX");
	}
	else if (lstrcmpi(szExtension, L".bmp") == 0) {
		ox->hWin = (HBITMAP)LoadImageW(NULL, szFileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		//GetObject(self->hBitmap, sizeof(BITMAP), &self->bitmap);

		if (ox->hWin == 0) {
			OxErr_SetFromWindows();
			return NULL;
		}
		ox->iType = OxIMAGEFORMAT_BMP;
	}
	OxFree(szFileName);

	if (ox->hWin == 0) {
		OxErr_SetFromWindows();
		OxErr_SetString(OxERROR_RUNTIME, "Can not load this format.");
		return NULL;
	}

	return ox;
}

OxImageObject*
OxImage_FromWindowsResource(WORD wResourceID)
{
	OxImageObject* ox = (OxImageObject*)OxObject_Allocate(pOxClass);
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

OxImageObject*
OxImage_FromStock(int iStockID)
{
	OxImageObject* ox = (OxImageObject*)OxObject_Allocate(pOxClass);
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
OxImage_Delete(OxImageObject* ox)
{
	//DestroyIcon(ox->hWin);
	return pOxClass->pBase->fnDelete(ox);
}

OxClass OxImageClass = {
	"Image",              /* sName */
	&OxObjectClass,       /* pBase */
	sizeof(OxImageObject), /* nSize */
	OxImage_Delete,       /* fnDelete */
	NULL,                 /* pUserData */
	0                     /* aMethods */
};

BOOL
OxImageClass_Init()
{
	pOxClass = &OxImageClass;
	Ox_INHERIT_METHODS(pOxClass);
	return TRUE;
}

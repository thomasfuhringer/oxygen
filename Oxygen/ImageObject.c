// ImageObject.c  | Oxygen © 2015 by Thomas Führinger

// ToDo: reimplement on GDI+ to facilitate JPEG, etc.
//       resources:
//       http://www.jose.it-berater.org/gdiplus/iframe/index.htm
//       https://github.com/mity/mctrl/blob/master/src/imgview.c

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
	}
	else if (lstrcmpi(szExtension, L".bmp") == 0) {
		ox->hWin = (HBITMAP)LoadImageW(NULL, szFileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

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
	ox->iType = OxIMAGEFORMAT_ICO;
	return ox;
}

OxImageObject*
OxImage_FromStock(int iStockID)
{
	OxImageObject* ox = (OxImageObject*)OxObject_Allocate(pOxClass);
	if (ox == NULL)
		return NULL;

	switch (iStockID)
	{
	case OxICON_OXYGEN:
		ox->hWin = OxApp->hIcon;
		break;

	case OxICON_FILEOPEN:
		ox->hWin = ExtractIconW(OxApp->hInstance, L"SHELL32.DLL", -235);
		//uInt = ExtractIconEx("%SystemRoot%\\system32\\SHELL32.dll" , -235, &ox->hWin, NULL, 1);
		break;

	case OxICON_REFRESH:
		ox->hWin = ExtractIconW(OxApp->hInstance, L"SHELL32.DLL", -16739);
		break;

	case OxICON_SAVE:
		ox->hWin = ExtractIconW(OxApp->hInstance, L"SHELL32.DLL", -16761);
		break;

	default:
		ox->hWin = OxApp->hIcon;
	}

	if (ox->hWin == 0) {
		OxErr_SetFromWindows();
		return NULL;
	}
	ox->iType = OxIMAGEFORMAT_ICO;
	return ox;
}

static BOOL
OxImage_Delete(OxImageObject* ox)
{
	if (ox->iType == OxIMAGEFORMAT_BMP)
		DeleteObject(ox->hWin);
	else if (ox->iType == OxIMAGEFORMAT_ICO)
		DestroyIcon(ox->hWin);
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
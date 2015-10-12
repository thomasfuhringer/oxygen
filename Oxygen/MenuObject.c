// MenuObject.c  | Oxygen © 2015 by Thomas Führinger
#include "Oxygen.h"

static OxClass* pOxClass;
static iNextIdentifier = FIRST_CUSTOM_MENU_ID;

OxMenuObject*
OxMenu_New(const char* sCaption)
{
	OxMenuObject* ox = (OxMenuObject*)OxObject_Allocate(pOxClass);
	if (ox == NULL)
		return NULL;

	ox->oxWindow = NULL;

	ox->sCaption = OxDuplicateString(sCaption);
	ox->hWin = CreateMenu();

	return ox;
}

BOOL
OxMenu_AppendItem(OxMenuObject* ox, OxObject* oxItem)
{
	LPWSTR szCaption;
	if (oxItem->pClass == &OxMenuItemClass) {
		szCaption = OxToW(((OxMenuItemObject*)oxItem)->sCaption);
		AppendMenuW(ox->hWin, MF_STRING, ((OxMenuItemObject*)oxItem)->iIdentifier, szCaption);
	}
	else if (oxItem->pClass == &OxMenuClass) {
		szCaption = OxToW(((OxMenuObject*)oxItem)->sCaption);
		AppendMenuW(ox->hWin, MF_POPUP, (UINT_PTR)((OxMenuObject*)oxItem)->hWin, szCaption);
	}
	else{
		OxErr_SetStringFormat(OxERROR_PARAMETER, "Parameter must be a MenuItem or Menu, not %s.", oxItem->pClass->sName);
		return FALSE;
	}
	OxFree(szCaption);
	return TRUE;
}

static BOOL
OxMenu_Delete(OxMenuObject* ox)
{
	OxFree(ox->sCaption);
	return pOxClass->pBase->fnDelete(ox);
}

OxClass OxMenuClass = {
	"Menu",                  /* sName */
	&OxObjectClass,          /* pBase */
	sizeof(OxMenuObject),    /* nSize */
	OxMenu_Delete,           /* fnDelete */
	NULL,                    /* pUserData */
	0                        /* aMethods */
};

BOOL
OxMenuClass_Init()
{
	pOxClass = &OxMenuClass;
	Ox_INHERIT_METHODS(pOxClass);
	return TRUE;
}

/* MenuItem -----------------------------------------------------------------------*/

OxMenuItemObject*
OxMenuItem_New(const char* sCaption, OxCallback fnOnClickCB, OxImageObject* oxIcon)
{
	if (oxIcon->iType != OxIMAGEFORMAT_ICO) {
		OxErr_SetString(OxERROR_RUNTIME, "Icon can only be set to an Image of Type Icon.");
		return NULL;
	}

	OxMenuItemObject* ox = (OxMenuItemObject*)OxObject_Allocate(&OxMenuItemClass);
	if (ox == NULL)
		return NULL;
	ox->iIdentifier = iNextIdentifier++;
	if (ox->iIdentifier > Ox_MAX_CUSTOM_MENU_ID) {
		OxErr_SetString(OxERROR_RUNTIME, "No more MenuItem can be created.");
		return NULL;
	}

	OxApp->oxaMenuItem[ox->iIdentifier - Ox_FIRST_CUSTOM_MENU_ID] = ox;
	ox->sCaption = OxDuplicateString(sCaption);
	ox->fnOnClickCB = fnOnClickCB;
	ox->oxIcon = oxIcon;

	return ox;
}

static BOOL
OxMenuItem_Delete(OxMenuItemObject* ox)
{
	OxFree(ox->sCaption);
	return (&OxMenuItemClass)->pBase->fnDelete(ox);
}

OxClass OxMenuItemClass = {
	"MenuItem",              /* sName */
	&OxObjectClass,          /* pBase */
	sizeof(OxMenuItemObject), /* nSize */
	OxMenuItem_Delete,       /* fnDelete */
	NULL,                    /* pUserData */
	0                        /* aMethods */
};

BOOL
OxMenuItemClass_Init()
{
	Ox_INHERIT_METHODS(&OxMenuItemClass);
	return TRUE;
}
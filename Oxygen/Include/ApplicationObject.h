// ApplicationObject.h  | Oxygen © 2015 by Thomas Führinger
#ifndef OxAPPLICATIONOBJECT_H
#define OxAPPLICATIONOBJECT_H

#include "Oxygen.h"

typedef struct _OxApplicationObject {
	OxObject_HEAD
		char* sName;
	OxWindowObject* oxWindow;
	OxImageObject* oxIcon;
	HINSTANCE hInstance;
	HICON hIcon;
	HMODULE hDLL;
	HMODULE hDLLcomctl32;
	char* sStateID;
	OxMenuItemObject* oxaMenuItem[Ox_MAX_CUSTOM_MENU_ID - Ox_FIRST_CUSTOM_MENU_ID + 1]; // map identifiers to pyMenuItems
	HGDIOBJ hDefaultFont;
} OxApplicationObject;

extern OxAPI OxApplicationObject* OxApp; // global singleton
extern OxAPI OxClass OxApplicationClass;

BOOL OxApplicationClass_Init();
OxAPI OxApplicationObject* OxApplication_New(char* strName, HINSTANCE hInstance);
OxAPI int OxApplication_Run();
OxAPI BOOL OxApplication_ShowError();
OxAPI BOOL OxApplication_SaveState();
OxAPI BOOL OxApplication_RestoreState();

#define OxCALL(fn)  if ((fn)==FALSE) {Xi( __FILE__, __LINE__);OxApplication_ShowError();}     
#define OxASSIGN(fn)  if ((fn)==NULL) {Xi( __FILE__, __LINE__);OxApplication_ShowError();} 

#endif /* OxAPPLICATIONOBJECT_H */

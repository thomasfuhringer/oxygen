// ApplicationObject.h  | Oxygen © 2016 by Thomas Führinger
#ifndef OxAPPLICATIONOBJECT_H
#define OxAPPLICATIONOBJECT_H

#include <Oxygen.h>

typedef struct _OxApplicationObject {
	OxObject_HEAD
		char* sName;
	GtkApplication* pGtk;
	OxWindowObject* oxWindow;
	OxImageObject* oxIcon;
	char* sStateID;
	OxMenuItemObject* oxaMenuItem[Ox_MAX_CUSTOM_MENU_ID - Ox_FIRST_CUSTOM_MENU_ID + 1]; // map identifiers to pyMenuItems
} OxApplicationObject;

extern OxAPI OxApplicationObject* OxApp; // global singleton
extern OxAPI OxClass OxApplicationClass;

bool OxApplicationClass_Init();
OxAPI OxApplicationObject* OxApplication_New(char* strName);
OxAPI int OxApplication_Run();
OxAPI bool OxApplication_ShowError();
OxAPI bool OxApplication_SaveState();
OxAPI bool OxApplication_RestoreState();

#define OxCALL(fn)  if ((fn)==FALSE) {Xi( __FILE__, __LINE__);OxApplication_ShowError();}     
#define OxASSIGN(fn)  if ((fn)==NULL) {Xi( __FILE__, __LINE__);OxApplication_ShowError();} 

#endif /* OxAPPLICATIONOBJECT_H */

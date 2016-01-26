// Oxygen.h  | Oxygen © 2016 by Thomas Führinger
#ifndef OXYGEN_H
#define OXYGEN_H

/*
#ifdef OxEXPORTS
#define OxAPI __declspec(dllexport)
#else
#define OxAPI __declspec(dllimport)
#endif

#ifdef OxSTATIC
#define OxAPI
#endif
*/
#define OxAPI 

// C RunTime Header Files
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// GTK Header Files
#include <gtk/gtk.h>

// Oxygen Header Files
#include "Version.h"
#include "Error.h"
#include "Object.h"
/*
#include "ListObject.h"
#include "StringObject.h"
#include "IntegerObject.h"
#include "FloatObject.h"
#include "BoolObject.h"
#include "BlobObject.h"
#include "ImageObject.h"
#include "MenuObject.h"
*/
typedef struct _OxImageObject OxImageObject;
typedef struct _OxMenuObject OxMenuObject;
typedef struct _OxMenuItemObject OxMenuItemObject;
#define Ox_FIRST_CUSTOM_MENU_ID   7700
#define Ox_MAX_CUSTOM_MENU_ID     7750

#include "WidgetObject.h"
#include "WindowObject.h"
/*
#include "MdiWindowObject.h"
#include "MdiAreaObject.h"
#include "ToolBarObject.h"
#include "StatusBarObject.h"
#include "BoxObject.h"
#include "TabObject.h"
#include "SplitterObject.h"
*/
#include "ApplicationObject.h"
/*
#include "ButtonObject.h"
*/
#include "LabelObject.h"
/*
#include "ImageViewObject.h"
#include "VideoViewObject.h"
#include "HtmlViewObject.h"
#include "EntryObject.h"
#include "MarkDownEntryObject.h"
*/
OxAPI bool OxInit(char* sAppName);
OxAPI bool OxExit(void);
OxAPI void* OxAllocate(size_t nSize);
OxAPI void* OxAllocateZeroed(size_t nSize);
OxAPI void* OxReAllocate(void* pChunk, size_t nSize);
OxAPI bool OxFree(void* pChunk);
OxAPI bool OxAttachObject(OxObject** poxMember, OxObject* oxObject, bool bStrong);
OxAPI char* OxStringAppend(char* sMain, const char* sAppendix);
char* OxDuplicateString(const char* sString);
OxAPI char* OxLoadFile(char* sFileNamePath);

OxAPI void XX(OxObject* oxObject);     // for debugging
OxAPI void Xi(char* sContext, int a);

extern OxAPI char* OxCopyright;
extern OxAPI float OxVersion;

extern GQuark OxQuark;

#endif /* OXYGEN_H */

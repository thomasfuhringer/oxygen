// Oxygen.h  | Oxygen © 2015 by Thomas Führinger
#ifndef OXYGEN_H
#define OXYGEN_H

#define IDC_OXBUTTON	       101
#define IDC_OXLABEL  	       102
#define IDC_OXENTRY  	       103
#define IDC_OXBOX   	       104
#define IDC_OXSTATUSBAR        105
#define IDC_OXMDIAREA          106
#define IDC_OXTAB              107
#define IDC_OXTOOLBAR          108

#define FIRST_CUSTOM_MENU_ID   700
#define MAX_CUSTOM_MENU_ID     750

#define OxTOOLBAR_IMGLST       0

#ifdef OxEXPORTS
#define OxAPI __declspec(dllexport)
#else
#define OxAPI __declspec(dllimport)
#endif

#define OxDLLFILE              "Oxygen.dll"

// C RunTime Header Files
#include <stdlib.h>
#include <tchar.h>
#include <Strsafe.h>

// Windows Header Files
#include <windows.h>
#include <Commctrl.h>
#include <OleCtl.h>
#include <Shlwapi.h>

// Oxygen Header Files
#include "Version.h"
#include "Error.h"
#include "Object.h"
#include "ListObject.h"
#include "StringObject.h"
#include "IntegerObject.h"
#include "FloatObject.h"
#include "BoolObject.h"
#include "MenuObject.h"
#include "IconObject.h"
#include "WidgetObject.h"
#include "WindowObject.h"
#include "MdiWindowObject.h"
#include "MdiAreaObject.h"
#include "ToolBarObject.h"
#include "StatusBarObject.h"
#include "BoxObject.h"
#include "TabObject.h"
#include "ApplicationObject.h"
#include "ButtonObject.h"
#include "LabelObject.h"
#include "EntryObject.h"

OxAPI BOOL Oxygen_Init(char* sAppName);
OxAPI void* OxAllocate(size_t nSize);
OxAPI void* OxReAllocate(void* pChunk, size_t nSize);
OxAPI BOOL OxFree(void* pChunk);
OxAPI BOOL OxAttachObject(OxObject** poxMember, OxObject* oxObject, BOOL bStrong);
OxAPI char* OxToU8(const LPWSTR szUTF16);
OxAPI LPWSTR OxToW(const char* sTextUTF8);
OxAPI BOOL OxStringAppend(char* sMain, const char* sAppendix);
char* OxDuplicateString(const char* sString);
OxAPI char* OxGetWindowText(HWND hWin);

OxAPI void XX(OxObject* oxObject); // for debugging
OxAPI void Xi(char* sContext, int a);

extern OxAPI char* OxCopyright;
extern OxAPI float OxVersion;

#endif /* OXYGEN_H */

// Oxygen.h  | Oxygen © 2015 by Thomas Führinger
#ifndef OXYGEN_H
#define OXYGEN_H

#pragma warning(disable : 4100)

#ifndef WINVER                  // Minimum platform is Windows XP
#define WINVER 0x0501
#endif

#ifndef _WIN32_WINNT            // Minimum platform is Windows XP
#define _WIN32_WINNT 0x0501
#endif

#ifndef _WIN32_WINDOWS          // Minimum platform is Windows XP
#define _WIN32_WINDOWS 0x0501
#endif

#pragma once
#pragma execution_character_set("utf-8")

//#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#define IDC_OXBUTTON	       7101
#define IDC_OXLABEL  	       7102
#define IDC_OXENTRY  	       7103
#define IDC_OXBOX   	       7104
#define IDC_OXSTATUSBAR        7105
#define IDC_OXMDIAREA          7106
#define IDC_OXTAB              7107
#define IDC_OXTOOLBAR          7108
#define IDC_OXIMAGEVIEW        7109

#define FIRST_CUSTOM_MENU_ID   7700
#define MAX_CUSTOM_MENU_ID     7750

#define OxTOOLBAR_IMGLST       0

#ifdef OxEXPORTS
#define OxAPI __declspec(dllexport)
#else
#define OxAPI __declspec(dllimport)
#endif

#ifdef OxSTATIC
#define OxAPI 
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
#include <exdisp.h>		// Defines of stuff like IWebBrowser2. This is an include file with Visual C 6 and above
#include <mshtml.h>		// Defines of stuff like IHTMLDocument2. This is an include file with Visual C 6 and above
#include <crtdbg.h>		// for _ASSERT()

// Oxygen Header Files
#include "Version.h"
#include "Error.h"
#include "Object.h"
#include "ListObject.h"
#include "StringObject.h"
#include "IntegerObject.h"
#include "FloatObject.h"
#include "BoolObject.h"
#include "BlobObject.h"
#include "ImageObject.h"
#include "MenuObject.h"
#include "WidgetObject.h"
#include "WindowObject.h"
#include "MdiWindowObject.h"
#include "MdiAreaObject.h"
#include "ToolBarObject.h"
#include "StatusBarObject.h"
#include "BoxObject.h"
#include "TabObject.h"
#include "SplitterObject.h"
#include "ApplicationObject.h"
#include "ButtonObject.h"
#include "LabelObject.h"
#include "ImageViewObject.h"
#include "VideoViewObject.h"
#include "HtmlViewObject.h"
#include "EntryObject.h"
#include "MarkDownEntryObject.h"

OxAPI BOOL OxInit(char* sAppName);
OxAPI BOOL OxExit(void);
OxAPI void* OxAllocate(size_t nSize);
OxAPI void* OxAllocateZeroed(size_t nSize);
OxAPI void* OxReAllocate(void* pChunk, size_t nSize);
OxAPI BOOL OxFree(void* pChunk);
OxAPI BOOL OxAttachObject(OxObject** poxMember, OxObject* oxObject, BOOL bStrong);
OxAPI char* OxToU8(const LPWSTR szUTF16);
OxAPI LPWSTR OxToW(const char* sTextUTF8);
OxAPI char* OxStringAppend(char* sMain, const char* sAppendix);
char* OxDuplicateString(const char* sString);
OxAPI char* OxGetWindowText(HWND hWin);
OxAPI char* OxLoadFile(char* sFileNamePath);

OxAPI void XX(OxObject* oxObject);     // for debugging
OxAPI void Xi(char* sContext, int a);

extern OxAPI char* OxCopyright;
extern OxAPI float OxVersion;

#endif /* OXYGEN_H */

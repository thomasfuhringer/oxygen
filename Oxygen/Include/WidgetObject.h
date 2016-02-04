// WidgetObject.h  | Oxygen © 2015 by Thomas Führinger
#ifndef OxWIDGETOBJECT_H
#define OxWIDGETOBJECT_H

#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

#define OxWINDOWBKGCOLOR     COLOR_MENU // COLOR_WINDOW

#define OxWIDGET_SHOW        	(OxOBJECT_LASTMETHID + 1)
#define OxWIDGET_RENDERFOCUS  	(OxOBJECT_LASTMETHID + 2)
#define OxWIDGET_SETDATA     	(OxOBJECT_LASTMETHID + 3)
#define OxWIDGET_SETCAPTION     (OxOBJECT_LASTMETHID + 4)
#define OxWIDGET_REPOSITION     (OxOBJECT_LASTMETHID + 5)
#define OxWIDGET_BEFOREDELETE   (OxOBJECT_LASTMETHID + 6)

#define OxWIDGET_LASTMETHID     (OxOBJECT_LASTMETHID + 6)

#define OxWIDGET_CENTER      (CW_USEDEFAULT-1)

#define OxALIGN_HORIZ        0x000000003
#define OxALIGN_HORIZ_LEFT   0x000000000
#define OxALIGN_HORIZ_CENTER 0x000000001
#define OxALIGN_HORIZ_RIGHT  0x000000002

#define OxALIGN_VERT         0x000000030
#define OxALIGN_VERT_TOP     0x000000000
#define OxALIGN_VERT_CENTER  0x000000010
#define OxALIGN_VERT_BOTTOM  0x000000020

#pragma comment(lib, "comctl32.lib")

#include "Oxygen.h"

#define OxWidgetObject_HEAD \
        OxObject_HEAD \
        HWND hWin; \
		OxRect rc; \
        HBRUSH hBkgBrush; \
        WNDPROC fnOldWinProcedure; \
		OxWindowObject* oxWindow; \
		OxWidgetObject* oxParent; \
        OxLabelObject* oxLabel; \
		BOOL bReadOnly; \
		OxObject* oxData; \
		OxClass* pDataType; \
        OxCallback fnOnDataChangedCB; \
        void* pUserData;

//   unsigned int fAlignment; \

typedef struct _OxRect {
	long  iLeft;
	long  iTop;
	long  iWidth;
	long  iHeight;
} OxRect;

typedef struct _OxWidgetObject OxWidgetObject;
typedef struct _OxLabelObject OxLabelObject;
typedef struct _OxWindowObject OxWindowObject;
typedef struct _OxWidgetObject {
	OxWidgetObject_HEAD
} OxWidgetObject;

extern OxAPI OxClass OxWidgetClass;

BOOL OxWidgetClass_Init();
OxWidgetObject* OxWidget_New();
OxAPI BOOL OxWidget_Init(OxWidgetObject* ox, OxWidgetObject* oxParent, OxRect* rc);
OxAPI BOOL OxWidget_ShowV(OxWidgetObject* ox, BOOL bVisible);
OxAPI BOOL OxWidget_CalculateRect(OxWidgetObject* ox, OxRect* rc);
OxAPI BOOL OxWidget_SetCaption(OxWidgetObject* ox, char* sText);
OxAPI BOOL OxWidget_SetCaptionV(OxWidgetObject* ox, char* sText);
BOOL OxWidget_Reposition(OxWidgetObject* ox);
OxAPI BOOL OxWidget_RenderFocusV(OxWidgetObject* ox);
OxAPI BOOL OxWidget_SetData(OxWidgetObject* ox, OxObject* oxData);
OxAPI BOOL OxWidget_SetDataV(OxWidgetObject* ox, OxObject* oxData);
BOOL OxWidget_RepositionV(OxWidgetObject* ox);

#endif /* OxWIDGETOBJECT_H */

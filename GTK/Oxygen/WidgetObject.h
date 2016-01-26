// WidgetObject.h  | Oxygen © 2015 by Thomas Führinger
#ifndef OxWIDGETOBJECT_H
#define OxWIDGETOBJECT_H

#define OxWIDGET_SHOW        	(OxOBJECT_LASTMETHID + 1)
#define OxWIDGET_RENDERFOCUS  	(OxOBJECT_LASTMETHID + 2)
#define OxWIDGET_SETDATA     	(OxOBJECT_LASTMETHID + 3)
#define OxWIDGET_SETCAPTION     (OxOBJECT_LASTMETHID + 4)
#define OxWIDGET_REPOSITION     (OxOBJECT_LASTMETHID + 5)
#define OxWIDGET_BEFOREDELETE   (OxOBJECT_LASTMETHID + 6)

#define OxWIDGET_LASTMETHID     (OxOBJECT_LASTMETHID + 6)

#define OxDEFAULT            77777
#define OxWIDGET_CENTER      (OxDEFAULT-1)

#define OxALIGN_HORIZ        0x000000003
#define OxALIGN_HORIZ_LEFT   0x000000000
#define OxALIGN_HORIZ_CENTER 0x000000001
#define OxALIGN_HORIZ_RIGHT  0x000000002

#define OxALIGN_VERT         0x000000030
#define OxALIGN_VERT_TOP     0x000000000
#define OxALIGN_VERT_CENTER  0x000000010
#define OxALIGN_VERT_BOTTOM  0x000000020


#include "Oxygen.h"

#define OxWidgetObject_HEAD \
        OxObject_HEAD \
        GtkWidget* pGtk; \
        GtkFixed* pGtkFixed; \
		OxRect rc; \
		OxWindowObject* oxWindow; \
		OxWidgetObject* oxParent; \
        OxLabelObject* oxLabel; \
		bool bReadOnly; \
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

bool OxWidgetClass_Init();
OxWidgetObject* OxWidget_New();
OxAPI bool OxWidget_Init(OxWidgetObject* ox, OxWidgetObject* oxParent, OxRect* rc);
OxAPI bool OxWidget_ShowV(OxWidgetObject* ox, bool bVisible);
OxAPI bool OxWidget_CalculateRect(OxWidgetObject* ox, OxRect* rc);
OxAPI bool OxWidget_SetCaption(OxWidgetObject* ox, char* sText);
OxAPI bool OxWidget_SetCaptionV(OxWidgetObject* ox, char* sText);
bool OxWidget_Reposition(OxWidgetObject* ox);
OxAPI bool OxWidget_RenderFocusV(OxWidgetObject* ox);
OxAPI bool OxWidget_SetData(OxWidgetObject* ox, OxObject* oxData);
OxAPI bool OxWidget_SetDataV(OxWidgetObject* ox, OxObject* oxData);
bool OxWidget_RepositionV(OxWidgetObject* ox);

#endif /* OxWIDGETOBJECT_H */

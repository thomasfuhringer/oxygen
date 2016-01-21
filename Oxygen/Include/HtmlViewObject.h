// HtmlViewObject.h  | Oxygen © 2016 by Thomas Führinger
#ifndef OxHTMLVIEWOBJECT_H
#define OxHTMLVIEWOBJECT_H

#include "Oxygen.h"

typedef struct _OxHtmlViewObject OxHtmlViewObject;

typedef struct _IOleInPlaceFrameExt {
	IOleInPlaceFrame orig;
	OxHtmlViewObject* ox;
} IOleInPlaceFrameExt;

typedef struct _IOleInPlaceSiteEx {
	IOleInPlaceSite		orig;
	IOleInPlaceFrameExt	OleInPlaceFrameExt;
} IOleInPlaceSiteExt;

typedef struct _IOleClientSiteExt {
	IOleClientSite		orig;
	IOleInPlaceSiteExt	OleInPlaceSiteExt;
} IOleClientSiteExt;


typedef struct _OxHtmlViewObject {
	OxWidgetObject_HEAD
		IOleObject* pOleBrowserObject;
	IOleClientSiteExt OleClientSiteExt;
} OxHtmlViewObject;

extern OxAPI OxClass OxHtmlViewClass;

BOOL OxHtmlViewClass_Init();
OxAPI OxHtmlViewObject* OxHtmlView_New(OxWidgetObject* oxParent, OxRect* rc);
OxAPI BOOL OxHtmlView_SetData(OxHtmlViewObject* ox, OxObject* oxData);
OxAPI BOOL OxHtmlView_RenderString(OxHtmlViewObject* ox, char* sString);
OxAPI BOOL OxHtmlView_RenderData(OxHtmlViewObject* ox, BOOL bEdit);
OxAPI BOOL OxHtmlView_RenderURL(OxHtmlViewObject* ox, char* sURL);

#endif

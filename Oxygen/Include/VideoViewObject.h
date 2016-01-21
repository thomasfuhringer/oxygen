// VideoViewObject.h  | Oxygen © 2015 by Thomas Führinger
#ifndef OxVIDEOVIEWOBJECT_H
#define OxVIDEOVIEWOBJECT_H

#include "Oxygen.h"
#include <dshow.h>
#include <Objbase.h>


typedef struct _OxVideoViewObject {
	OxWidgetObject_HEAD
		BOOL bStretch;
	BOOL bFill;
	IGraphBuilder *pGraph;
	IMediaControl *pControl;
	IMediaEvent   *pEvent;
	IVideoWindow  *pVidWin;
} OxVideoViewObject;

extern OxAPI OxClass OxVideoViewClass;

BOOL OxVideoViewClass_Init();
OxAPI OxVideoViewObject* OxVideoView_New(OxWidgetObject* oxParent, OxRect* rc);
//OxAPI BOOL OxVideoView_SetData(OxVideoViewObject* ox, OxObject* oxData);
//OxAPI BOOL OxVideoView_RenderData(OxVideoViewObject* ox, BOOL bEdit);
OxAPI BOOL OxVideoView_RenderFile(OxVideoViewObject* ox, char* sFile);

#endif

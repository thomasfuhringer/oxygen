// WindowObject.c  | Oxygen © 2016 by Thomas Führinger
#include <Oxygen.h>

static OxClass* pOxClass;
gboolean OxWindowConfigureEventCB(GtkWidget* pGtkWidget, GdkEvent* pGtkEvent, gpointer pUserData);

OxWindowObject*
OxWindow_New(OxWidgetObject* oxParent, OxRect* rc, char* sCaption)
{
	OxWindowObject* ox = (OxWindowObject*)OxObject_Allocate(pOxClass);
	if (ox == NULL)
		return NULL;

	if (!OxWidget_Init((OxWidgetObject*)ox, oxParent, rc))
		return NULL;

	ox->oxIcon = NULL;
	ox->oxMenu = NULL;
	ox->oxToolBar = NULL;
	ox->oxStatusBar = NULL;
	ox->oxMdiArea = NULL;
	ox->oxWindow = ox;
	ox->oxFocusWidget = NULL;
	ox->fnBeforeCloseCB = NULL;
	ox->iMinWidth = 320;
	ox->iMinHeight = 240;
	ox->iMaxWidth = -1;
	ox->iMaxHeight = -1;
	ox->bModal = false;
	ox->bDeleteOnClose = true;

	ox->pGtk = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(ox->pGtk), sCaption);
	gtk_window_set_application(GTK_WINDOW(ox->pGtk), OxApp->pGtk);
	g_object_set_qdata(ox->pGtk, OxQuark, ox);

	//OxWidget_CalculateRect((OxWidgetObject*)ox, rc);
	gtk_window_set_default_size(GTK_WINDOW(ox->pGtk), rc->iWidth, rc->iHeight);
	gtk_window_move(GTK_WINDOW(ox->pGtk), rc->iLeft, rc->iTop);
	ox->pGtkFixed = gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER(ox->pGtk), ox->pGtkFixed);
	gtk_widget_show_all(ox->pGtk);
	g_signal_connect(G_OBJECT(ox->pGtk), "configure-event", G_CALLBACK(OxWindowConfigureEventCB), ox);

	return ox;
}

gboolean
OxWindowConfigureEventCB(GtkWidget* pGtkWidget, GdkEvent* pGtkEvent, gpointer pUserData)
{
	OxWindowObject* ox = (OxWindowObject*)g_object_get_qdata(pGtkWidget, OxQuark);
	OxWidget_RepositionV(ox);
	return TRUE;
}

bool
OxWindow_MoveFocus(OxWindowObject* ox, OxWidgetObject* oxDestinationWidget)
// return true if possible, false if not, -1 on error
{
	if (ox->oxFocusWidget == oxDestinationWidget)
		return true;
	if (ox->oxFocusWidget == NULL) {
		ox->oxFocusWidget = oxDestinationWidget;
		return true;
	}

	bool bR = OxWidget_RenderFocusV(ox->oxFocusWidget);
	if (bR)
		ox->oxFocusWidget = oxDestinationWidget;
	return bR;
}

bool
OxWindow_SetMenu(OxWindowObject* ox, OxMenuObject* oxMenu)
{

	OxAttachObject(&ox->oxMenu, oxMenu, false);
	return true;
}

bool
OxWindow_SetIcon(OxWindowObject* ox, OxImageObject* oxIcon)
{/*
	if (oxIcon->iType != OxIMAGEFORMAT_ICO) {
	OxErr_SetString(OxERROR_RUNTIME, "Icon can only be set to an Image of Type Icon.");
	return false;
	}
	OxAttachObject(&ox->oxIcon, oxIcon, false);
	*/
	return true;
}

bool
OxWindow_ReadyToClose(OxWindowObject* ox)
{
	if (ox->fnBeforeCloseCB && !ox->fnBeforeCloseCB(ox))
		return false;

	if (ox == OxApp->oxWindow && OxApp->sStateID)
		OxApplication_SaveState();
	return true;
}

char*
OxWindow_Represent(OxWindowObject* ox)
{
	char* sString = (char*)OxAllocate(250 * sizeof(char));
	int iIndex = sprintf(sString, "OxWindowObject @ %p, %d times referenced", ox, ox->iRefCount);
	return sString;
}


bool
OxWindow_ShowModal(OxWindowObject* ox)
{
	return true;
}

static bool
OxWindow_Reposition(OxWindowObject* ox)
{
	GList* pGListChildren = gtk_container_get_children(GTK_CONTAINER(ox->pGtkFixed));
	while (pGListChildren != NULL) {
		OxWidgetObject* oxChild = g_object_get_qdata(pGListChildren->data, OxQuark);
		OxWidget_RepositionV(oxChild);
		pGListChildren = pGListChildren->next;
	}
	g_list_free(pGListChildren);
	return true;
}

static bool
OxWindow_Delete(OxWindowObject* ox)
{
	OxREL(ox->oxMenu);
	OxREL(ox->oxIcon);
	OxREL(ox->oxMdiArea);
	OxREL(ox->oxToolBar);
	OxREL(ox->oxStatusBar);
	if (ox == OxApp->oxWindow) {
		gtk_main_quit();
	}
	return pOxClass->pBase->fnDelete(ox);
}

OxClass OxWindowClass = {
	"Window",                  /* sName */
	&OxWidgetClass,            /* pBase */
	sizeof(OxWindowObject),    /* nSize */
	OxWindow_Delete,           /* fnDelete */
	NULL,                      /* pUserData */
	0                          /* aMethods */
};

bool
OxWindowClass_Init()
{
	pOxClass = &OxWindowClass;
	Ox_INHERIT_METHODS(pOxClass);
	pOxClass->aMethods[OxOBJECT_REPRESENT] = OxWindow_Represent;
	pOxClass->aMethods[OxWIDGET_REPOSITION] = OxWindow_Reposition;
	return true;
}

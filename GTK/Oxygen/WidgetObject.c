// WidgetObject.c  | Oxygen © 2016 by Thomas Führinger
#include <Oxygen.h>

static OxClass* pOxClass;

OxWidgetObject*
OxWidget_New()
{
	OxWidgetObject* ox = (OxWidgetObject*)OxObject_Allocate(pOxClass);
	if (ox != NULL) {
		return ox;
	}
	else
		return NULL;
}

bool
OxWidget_Init(OxWidgetObject* ox, OxWidgetObject* oxParent, OxRect* rc)
{
	ox->oxParent = oxParent;
	if (ox->oxParent)
		ox->oxWindow = oxParent->oxWindow;
	else
		ox->oxWindow = NULL;

	if (rc == NULL) {
		ox->rc.iLeft = 0;
		ox->rc.iTop = 0;
		ox->rc.iWidth = 640;
		ox->rc.iHeight = 480;
	}
	else {
		ox->rc.iLeft = rc->iLeft;
		ox->rc.iTop = rc->iTop;
		ox->rc.iWidth = rc->iWidth;
		ox->rc.iHeight = rc->iHeight;
	}
	ox->bReadOnly = false;
	ox->oxData = OxNull;
	ox->pDataType = NULL;
	ox->oxLabel = NULL;
	ox->fnOnDataChangedCB = NULL;
	ox->pUserData = NULL;
	ox->pGtkFixed = NULL;
	return true;
}

bool
OxWidget_CalculateRect(OxWidgetObject* ox, OxRect* rc)
{
	GtkFixed* pGtkFixed = gtk_widget_get_parent(ox->pGtk);
	int iWidth = gtk_widget_get_allocated_width(pGtkFixed);
	int iHeight = gtk_widget_get_allocated_height(pGtkFixed);

	if (ox->rc.iLeft == OxWIDGET_CENTER)
		rc->iLeft = (iWidth - ox->rc.iWidth) / 2;
	else
		rc->iLeft = (ox->rc.iLeft >= 0 ? ox->rc.iLeft : iWidth + ox->rc.iLeft);
	rc->iWidth = (ox->rc.iWidth > 0 ? ox->rc.iWidth : iWidth + ox->rc.iWidth - rc->iLeft);

	if (ox->rc.iTop == OxWIDGET_CENTER)
		rc->iTop = (iHeight - ox->rc.iHeight) / 2;
	else
		rc->iTop = (ox->rc.iTop >= 0 ? ox->rc.iTop : iHeight + ox->rc.iTop);
	rc->iHeight = (ox->rc.iHeight > 0 ? ox->rc.iHeight : iHeight + ox->rc.iHeight - rc->iTop);

	rc->iWidth = rc->iWidth < 0 ? 100 : rc->iWidth;
	rc->iHeight = rc->iHeight < 0 ? 100 : rc->iHeight;
	return true;
}

bool
OxWidget_SetCaption(OxWidgetObject* ox, char* sText)
{
	return true;
}

bool
OxWidget_SetCaptionV(OxWidgetObject* ox, char* sText)
{
	bool(*Method)(OxWidgetObject*, char* sText) = (bool(*)(OxWidgetObject*, char* sText))ox->pClass->aMethods[OxWIDGET_SETCAPTION];
	return Method(ox, sText);
}

bool
OxWidget_SetData(OxWidgetObject* ox, OxObject* oxData)
{
	if (OxObject_CompareV(ox->oxData, oxData, OxEQ))
		return true;

	OxAttachObject(&ox->oxData, oxData, false);

	if (ox->fnOnDataChangedCB)
		return ox->fnOnDataChangedCB(ox);
	return true;
}
bool
OxWidget_SetDataV(OxWidgetObject* ox, OxObject* oxData)
{
	bool(*Method)(OxWidgetObject*, OxObject*) = (bool(*)(OxWidgetObject*, OxObject*))ox->pClass->aMethods[OxWIDGET_SETDATA];
	return Method(ox, oxData);
}

bool
OxWidget_Reposition(OxWidgetObject* ox)
{
	OxRect rc;
	OxWidget_CalculateRect(ox, &rc);

	if (ox->pGtk) {
		GtkFixed* pGtkFixed = gtk_widget_get_parent(ox->pGtk);
		gtk_fixed_move(pGtkFixed, ox->pGtk, rc.iLeft, rc.iTop);
		gtk_widget_set_size_request(ox->pGtk, rc.iWidth, rc.iHeight);
	}

	if (ox->pGtkFixed) {
		GList* pGListChildren = gtk_container_get_children(GTK_CONTAINER(ox->pGtkFixed));
		while (pGListChildren != NULL) {
			OxWidgetObject* oxChild = g_object_get_qdata(pGListChildren->data, OxQuark);
			OxWidget_RepositionV(oxChild);
			pGListChildren = pGListChildren->next;
		}
		g_list_free(pGListChildren);
	}
	return true;
}

bool
OxWidget_RepositionV(OxWidgetObject* ox)
{
	bool(*Method)(OxWidgetObject*) = (bool(*)(OxWidgetObject*))ox->pClass->aMethods[OxWIDGET_REPOSITION];
	return Method(ox);
}

bool
OxWidget_Show(OxWidgetObject* ox, bool bVisible)
{
	if (ox == NULL)
		return true;

	if (bVisible)
		gtk_widget_show(ox->pGtk);
	else
		gtk_widget_hide(ox->pGtk);
	return true;
}

bool
OxWidget_ShowV(OxWidgetObject* ox, bool bVisible)
{
	bool(*Method)(OxWidgetObject*, bool) = (bool(*)(OxWidgetObject*, bool))ox->pClass->aMethods[OxWIDGET_SHOW];
	return Method(ox, bVisible);
}

bool
OxWidget_RenderFocus(OxWidgetObject* ox)
{
	OxErr_SetString(OxERROR_RUNTIME, "OxWidget_RenderFocus is a virtual method.");
	return -1;
}

bool
OxWidget_RenderFocusV(OxWidgetObject* ox)
{
	bool(*Method)(OxWidgetObject*) = (bool(*)(OxWidgetObject*))ox->pClass->aMethods[OxWIDGET_RENDERFOCUS];
	return Method(ox);
}

char*
OxWidget_Represent(OxWidgetObject* ox)
{
	char* sString = (char*)OxAllocate(250 * sizeof(char));
	int iIndex = sprintf(sString, "OxWidgetObject @ %p, %d times referenced", ox, ox->iRefCount);
	return sString;
}

static bool
OxWidget_BeforeDelete(OxWidgetObject* ox)
{
	return true;
}

static bool
OxWidget_ReleaseChildren(OxWidgetObject* ox)
{
	if (ox->pGtkFixed) {
		GList* pGListChildren = gtk_container_get_children(GTK_CONTAINER(ox->pGtkFixed));
		while (pGListChildren != NULL) {
			OxWidgetObject* oxChild = g_object_get_qdata(pGListChildren->data, OxQuark);

			if (oxChild && !OxREL(oxChild))
				return false;

			pGListChildren = pGListChildren->next;
		}
		g_list_free(pGListChildren);
	}
	return true;
}

static bool
OxWidget_Delete(OxWidgetObject* ox)
{
	if (ox->pGtk)
		gtk_widget_destroy(ox->pGtk);
	if (ox->pGtkFixed)
		gtk_widget_destroy(ox->pGtkFixed);
	if (!(bool(*))ox->pClass->aMethods[OxWIDGET_BEFOREDELETE](ox))
		return false;
	OxREL(ox->oxLabel);
	OxREL(ox->oxData);
	if (!OxWidget_ReleaseChildren(ox))
		return false;
	return pOxClass->pBase->fnDelete(ox);
}

OxClass OxWidgetClass = {
	"Widget",                  /* sName */
	&OxObjectClass,            /* pBase */
	sizeof(OxWidgetObject),    /* nSize */
	OxWidget_Delete,           /* fnDelete */
	NULL,                      /* pUserData */
	0,                         /* aMethods */
};

bool
OxWidgetClass_Init()
{
	pOxClass = &OxWidgetClass;
	Ox_INHERIT_METHODS(pOxClass);
	pOxClass->aMethods[OxOBJECT_REPRESENT] = OxWidget_Represent;
	pOxClass->aMethods[OxWIDGET_SETDATA] = OxWidget_SetData;
	pOxClass->aMethods[OxWIDGET_SETCAPTION] = OxWidget_SetCaption;
	pOxClass->aMethods[OxWIDGET_SHOW] = OxWidget_Show;
	pOxClass->aMethods[OxWIDGET_RENDERFOCUS] = OxWidget_RenderFocus;
	pOxClass->aMethods[OxWIDGET_REPOSITION] = OxWidget_Reposition;
	pOxClass->aMethods[OxWIDGET_BEFOREDELETE] = OxWidget_BeforeDelete;
	return true;
}

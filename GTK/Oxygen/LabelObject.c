// LabelObject.c  | Oxygen © 2016 by Thomas Führinger
#include <Oxygen.h>

static OxClass* pOxClass;

OxLabelObject*
OxLabel_New(OxWidgetObject* oxParent, OxRect* rc, char* sCaption)
{
	OxLabelObject* ox = (OxLabelObject*)OxObject_Allocate(pOxClass);
	if (ox == NULL)
		return NULL;

	if (!OxWidget_Init((OxWidgetObject*)ox, oxParent, rc))
		return NULL;

	ox->oxAssociatedWidget = NULL;
	ox->sFormat = NULL;
	ox->fContentAlignment = 0;

	ox->pGtk = gtk_label_new(sCaption);
	gtk_fixed_put(oxParent->pGtkFixed, ox->pGtk, 0, 0);
	gtk_widget_show(ox->pGtk);
	OxWidget_Reposition(ox);

	g_object_set_qdata(ox->pGtk, OxQuark, ox);
	return ox;
}

bool
OxLabel_Align(OxLabelObject* ox, unsigned int fAlignment)
{
	switch (fAlignment & OxALIGN_HORIZ)
	{
	case OxALIGN_HORIZ_LEFT:
		gtk_label_set_justify(GTK_LABEL(ox->pGtk), GTK_JUSTIFY_LEFT);
		break;
	case OxALIGN_HORIZ_CENTER:
		gtk_label_set_justify(GTK_LABEL(ox->pGtk), GTK_JUSTIFY_CENTER);
		break;
	case OxALIGN_HORIZ_RIGHT:
		gtk_label_set_justify(GTK_LABEL(ox->pGtk), GTK_JUSTIFY_RIGHT);
		break;
	}

	switch (fAlignment & OxALIGN_VERT)
	{
	case OxALIGN_VERT_TOP:
		//gtk_label_set_yalign(GTK_LABEL(ox->pGtk), 0);
		break;
	case OxALIGN_VERT_CENTER:
		//gtk_label_set_yalign(GTK_LABEL(ox->pGtk), 0.5);
		break;
	case OxALIGN_VERT_BOTTOM:
		//gtk_label_set_yalign(GTK_LABEL(ox->pGtk), 1);
		break;
	}
	return TRUE;
}

bool
OxLabel_RenderData(OxLabelObject* ox, bool bEdit)
{
	char* sText;
	if (ox->oxData == NULL) {
		gtk_label_set_text(ox->pGtk, "");
		return TRUE;
	}

	if ((sText = OxObject_AsStringV(ox->oxData, ox->sFormat)) == NULL)
		return FALSE;
	gtk_label_set_text(ox->pGtk, sText);
	OxFree(sText);
	return TRUE;
}

bool
OxLabel_SetData(OxLabelObject* ox, OxObject* oxData)
{
	if (!OxWidget_SetData((OxWidgetObject*)ox, oxData))
		return FALSE;

	return OxLabel_RenderData(ox, FALSE);
}

bool
OxLabel_SetTextColor(OxLabelObject* ox, int iRed, int iGreen, int iBlue)
{
	PangoAttrList* pAL = pango_attr_list_new();
	pango_attr_list_insert(pAL, pango_attr_foreground_new(iRed, iGreen, iBlue));
	gtk_label_set_attributes(GTK_LABEL(ox->pGtk), pAL);
	return TRUE;
}

static bool
OxLabel_Delete(OxLabelObject* ox)
{
	return pOxClass->pBase->fnDelete((OxObject*)ox);
}

OxClass OxLabelClass = {
	"Label",                   /* sName */
	&OxWidgetClass,            /* pBase */
	sizeof(OxLabelObject),     /* nSize */
	OxLabel_Delete,            /* fnDelete */
	NULL,                      /* pUserData */
	0                          /* aMethods */
};

bool
OxLabelClass_Init()
{
	pOxClass = &OxLabelClass;
	Ox_INHERIT_METHODS(pOxClass);
	pOxClass->aMethods[OxWIDGET_SETDATA] = OxLabel_SetData;
	return TRUE;
}

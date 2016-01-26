// ApplicationObject.c  | Oxygen © 2016 by Thomas Führinger

#include <Oxygen.h>

static OxClass* pOxClass;
OxApplicationObject* OxApp; // global singleton    

OxApplicationObject*
OxApplication_New(char* sName)
{
	OxApplicationObject* ox = (OxApplicationObject*)OxObject_Allocate(&OxApplicationClass);
	if (ox != NULL) {
		ox->sName = OxDuplicateString(sName);
		ox->oxWindow = NULL;
		ox->oxIcon = NULL;
		ox->sStateID = NULL;
        char* sAppID = OxStringAppend(OxDuplicateString("Oxygen."), sName);
		ox->pGtk = gtk_application_new(sAppID, G_APPLICATION_FLAGS_NONE);
        OxFree(sAppID);
		g_application_register(ox->pGtk, NULL, NULL);
		return ox;
	}
	else
		return NULL;
}

static void
OxApplicationActivateCB(GtkApplication* app, gpointer  user_data)
{
	return;
}

int
OxApplication_Run()
{
	g_signal_connect(OxApp->pGtk, "activate", G_CALLBACK(OxApplicationActivateCB), NULL);
	return g_application_run(G_APPLICATION(OxApp->pGtk), 0, NULL); //argc, argv);
}

bool
OxApplication_ShowError()
{
	return true;
}

bool
OxApplication_SaveState()
{
	return true;
}

bool
OxApplication_RestoreState()
{
	return true;
}

static bool
OxApplication_Delete(OxObject* ox)
{
	g_object_unref(((OxApplicationObject*)ox)->pGtk);
	OxFree(((OxApplicationObject*)ox)->sName);
	return pOxClass->pBase->fnDelete(ox);
}

OxClass OxApplicationClass = {
	"Application",             /* sName */
	&OxObjectClass,            /* pBase */
	sizeof(OxApplicationObject), /* nSize */
	OxApplication_Delete,      /* fnDelete */
	NULL,                      /* pUserData */
	0                          /* aMethods */
};

bool
OxApplicationClass_Init(OxObject* ox)
{
	pOxClass = &OxApplicationClass;
	Ox_INHERIT_METHODS(pOxClass);
	return true;
}

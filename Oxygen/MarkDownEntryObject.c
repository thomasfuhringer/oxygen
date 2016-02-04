// MarkDownEntryObject.c  | Oxygen © 2016 by Thomas Führinger
#include "Oxygen.h"
#include "Hoedown/html.h"
#define READ_UNIT 1024
#define OUTPUT_UNIT 64

static OxClass* pOxClass;

OxMarkDownEntryObject*
OxMarkDownEntry_New(OxWidgetObject* oxParent, OxRect* rc)
{
	OxMarkDownEntryObject* ox = (OxMarkDownEntryObject*)OxObject_Allocate(pOxClass);
	if (ox == NULL)
		return NULL;

	if (!OxHtmlView_Init((OxHtmlViewObject*)ox, oxParent, rc))
		return NULL;

	return ox;
};

BOOL
OxMarkDownEntry_RenderData(OxMarkDownEntryObject* ox, BOOL bEdit)
{
	char* sString = OxMarkDownToHtml(((OxStringObject*)ox->oxData)->sString);
	return OxHtmlView_RenderString(ox, sString);
	OxFree(sString);
}

/*
static BOOL
OxMarkDownEntry_Reposition(OxMarkDownEntryObject* ox)
{

return TRUE;
}
*/
BOOL
OxMarkDownEntry_SetData(OxMarkDownEntryObject* ox, OxObject* oxData)
{
	if (!OxWidget_SetData((OxWidgetObject*)ox, oxData))
		return FALSE;

	return OxMarkDownEntry_RenderData(ox, FALSE);
}

static BOOL
OxMarkDownEntry_Delete(OxMarkDownEntryObject* ox)
{
	return pOxClass->pBase->fnDelete((OxObject*)ox);
}

OxClass OxMarkDownEntryClass = {
	"MarkDownEntry",           /* sName */
	&OxHtmlViewClass,          /* pBase */
	sizeof(OxMarkDownEntryObject), /* nSize */
	OxMarkDownEntry_Delete,    /* fnDelete */
	NULL,                      /* pUserData */
	0                          /* aMethods */
};

BOOL
OxMarkDownEntryClass_Init()
{
	pOxClass = &OxMarkDownEntryClass;
	Ox_INHERIT_METHODS(pOxClass);
	//pOxClass->aMethods[OxWIDGET_REPOSITION] = OxMarkDownEntry_Reposition;
	pOxClass->aMethods[OxWIDGET_SETDATA] = OxMarkDownEntry_SetData;
	return TRUE;
}

char*
OxMarkDownToHtml(char* sMarkDown)
{
	hoedown_renderer *renderer;
	hoedown_document *document;
	hoedown_buffer *buffer;
	unsigned int extensions = 0, render_flags = 0;
	const uint8_t *sHtml;

	renderer = hoedown_html_renderer_new(render_flags, 0);
	document = hoedown_document_new(renderer, extensions, 16);
	buffer = hoedown_buffer_new(16);
    
	hoedown_document_render(document, buffer, (const uint8_t *)sMarkDown, strlen(sMarkDown));
	sHtml = (char*)OxAllocate(buffer->size + 1);
	StringCchCopyA(sHtml, buffer->size + 1, buffer->data);
	hoedown_buffer_reset(buffer);
	hoedown_html_smartypants(buffer, sHtml, buffer->size);
	
	hoedown_buffer_free(buffer);
	hoedown_document_free(document);
	hoedown_html_renderer_free(renderer);

	return sHtml;
}
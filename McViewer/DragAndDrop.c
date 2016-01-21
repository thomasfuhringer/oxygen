/*
  Copyright (C) 2008 MySQL AB

  This program is free software; you can redistribute it and/or modify
  it under the terms of version 2 of the GNU General Public License as
  published by the Free Software Foundation.

  There are special exceptions to the terms and conditions of the GPL
  as it is applied to this software. View the full text of the exception
  in file LICENSE.exceptions in the top-level directory of this software
  distribution.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
  */
/*
 * File: DragAndDrop.c
 * A layer for implemention Drag and Drop in Windows for C-applications. This enables
 * C applications to easily implement drag and drop, without having to deal with any OLE or
 * something like that. Some shortcuts has been tahken, such as only one TYMED is supported (HGLOBAL)
 * and only the DVASPECT_CONTENT aspect.
 * Both drag-drop target and source is supported.
 */
#include <windows.h>
#include <ole2.h>
#include <shlobj.h>
#include <tchar.h>
#include "DragAndDrop.h"
#include "resource.h"

/* Macros. */
#define MYDD_HEAP (g_hHeap == NULL ? (g_hHeap = GetProcessHeap()) : g_hHeap)

/* Typedefs. */
typedef struct tagMYIDATAOBJECT {
	IDataObject ido;
	LONG lRefCount;
	FORMATETC *pFormatEtc;
	HGLOBAL *phData;
	ULONG lNumFormats;
} MYIDATAOBJECT, *PMYIDATAOBJECT;

typedef struct tagMYIDROPSOURCE {
	IDropSource ids;
	LONG lRefCount;
	BOOL bRightClick;
	HWND hWndMenu;
	HMENU hPopup;
} MYIDROPSOURCE, *PMYIDROPSOURCE;

typedef struct tagMYIDROPTARGET {
	IDropTarget idt;
	LONG lRefCount;
	ULONG lNumFormats;
	CLIPFORMAT *pFormat;
	HWND hWnd;
	BOOL bAllowDrop;
	DWORD dwKeyState;
	IDataObject *pDataObject;
	UINT nMsg;
	void *pUserData;
	MYDDCALLBACK pDropProc;
} MYIDROPTARGET, *PMYIDROPTARGET;

typedef struct tagMYDROPSOURCE
{
	IDataObject *pObj;
	IDropSource *pSrc;
	HANDLE hData;
} MYDROPSOURCE, *PMYDROPSOURCE;

typedef struct tagMYIDATAOBJECT_VTBL
{
	BEGIN_INTERFACE
		HRESULT(STDMETHODCALLTYPE *QueryInterface)(PMYIDATAOBJECT pThis, REFIID riid, void **ppvObject);
		ULONG(STDMETHODCALLTYPE *AddRef)(PMYIDATAOBJECT pThis);
		ULONG(STDMETHODCALLTYPE *Release)(PMYIDATAOBJECT pThis);
		HRESULT(STDMETHODCALLTYPE *GetData)(PMYIDATAOBJECT pThis,
			FORMATETC *pFormatEtcIn, STGMEDIUM *pMedium);
		HRESULT(STDMETHODCALLTYPE *GetDataHere)(PMYIDATAOBJECT pThis,
			FORMATETC *pFormatEtc, STGMEDIUM *pMedium);
		HRESULT(STDMETHODCALLTYPE *QueryGetData)(PMYIDATAOBJECT pThis, FORMATETC *pFormatEtc);
		HRESULT(STDMETHODCALLTYPE *GetCanonicalFormatEtc)(PMYIDATAOBJECT pThis,
			FORMATETC *pFormatEtcIn, FORMATETC *pFormatEtcOut);
		HRESULT(STDMETHODCALLTYPE *SetData)(PMYIDATAOBJECT pThis,
			FORMATETC *pFormatEtc, STGMEDIUM *pMedium, BOOL fRelease);
		HRESULT(STDMETHODCALLTYPE *EnumFormatEtc)(PMYIDATAOBJECT pThis,
			DWORD dwDirection, IEnumFORMATETC **ppenumFormatEtc);
		HRESULT(STDMETHODCALLTYPE *DAdvise)(PMYIDATAOBJECT pThis,
			FORMATETC *pFormatEtc, DWORD advf, IAdviseSink *pAdvSink, DWORD *pdwConnection);
		HRESULT(STDMETHODCALLTYPE *DUnadvise)(PMYIDATAOBJECT pThis, DWORD dwConnection);
		HRESULT(STDMETHODCALLTYPE *EnumDAdvise)(PMYIDATAOBJECT pThis, IEnumSTATDATA **ppEnumAdvise);
	END_INTERFACE
} MYIDATAOBJECT_VTBL, *PMYIDATAOBJECT_VTBL;

typedef struct MYIDROPSOURCE_VTBL
{
	BEGIN_INTERFACE
		HRESULT(STDMETHODCALLTYPE *QueryInterface)(PMYIDROPSOURCE pThis, REFIID riid,
		void  **ppvObject);
		ULONG(STDMETHODCALLTYPE *AddRef)(PMYIDROPSOURCE pThis);
		ULONG(STDMETHODCALLTYPE *Release)(PMYIDROPSOURCE pThis);
		HRESULT(STDMETHODCALLTYPE *QueryContinueDrag)(PMYIDROPSOURCE pThis, BOOL fEscapePressed,
			DWORD dwKeyState);
		HRESULT(STDMETHODCALLTYPE *GiveFeedback)(PMYIDROPSOURCE pThis, DWORD dwEffect);
	END_INTERFACE
} MYIDROPSOURCE_VTBL, *PMYIDROPSOURCE_VTBL;

typedef struct MYIDROPTARGET_VTBL
{
	BEGIN_INTERFACE
		HRESULT(STDMETHODCALLTYPE *QueryInterface)(PMYIDROPTARGET pThis, REFIID riid,
		void  **ppvObject);
		ULONG(STDMETHODCALLTYPE *AddRef)(PMYIDROPTARGET pThis);
		ULONG(STDMETHODCALLTYPE *Release)(PMYIDROPTARGET pThis);
		HRESULT(STDMETHODCALLTYPE *DragEnter)(PMYIDROPTARGET pThis, IDataObject *pDataObject,
			DWORD dwKeyState, POINTL pt, DWORD *pdwEffect);
		HRESULT(STDMETHODCALLTYPE *DragOver)(PMYIDROPTARGET pThis, DWORD dwKeyState, POINTL pt,
			DWORD *pdwEffect);
		HRESULT(STDMETHODCALLTYPE *DragLeave)(PMYIDROPTARGET pThis);
		HRESULT(STDMETHODCALLTYPE *Drop)(PMYIDROPTARGET pThis, IDataObject *pDataObject,
			DWORD dwKeyState, POINTL pt, DWORD *pdwEffect);
	END_INTERFACE
} MYIDROPTARGET_VTBL, *PMYIDROPTARGET_VTBL;

/* Static globals. */
static HANDLE g_hHeap = NULL;

/* Static helper function prototypes. */
static int LookupFormatEtc(PMYIDATAOBJECT pThis, FORMATETC *pFormatEtc);

/* MYIDROPSOURCE functions. */
static HRESULT STDMETHODCALLTYPE MYIDROPSOURCE_QueryInterface(PMYIDROPSOURCE pThis,
	REFIID riid, LPVOID *ppvObject);
static ULONG STDMETHODCALLTYPE MYIDROPSOURCE_AddRef(PMYIDROPSOURCE pThis);
static ULONG STDMETHODCALLTYPE MYIDROPSOURCE_Release(PMYIDROPSOURCE pThis);
static HRESULT STDMETHODCALLTYPE MYIDROPSOURCE_QueryContinueDrag(PMYIDROPSOURCE pThis,
	BOOL fEscapePressed, DWORD grfKeyState);
static HRESULT STDMETHODCALLTYPE MYIDROPSOURCE_GiveFeedback(PMYIDROPSOURCE pThis,
	DWORD dwEffect);

/* MYIDATAOBJECT functions. */
static HRESULT STDMETHODCALLTYPE MYIDATAOBJECT_QueryInterface(PMYIDATAOBJECT pThis,
	REFIID riid, LPVOID *ppvObject);
static ULONG STDMETHODCALLTYPE MYIDATAOBJECT_AddRef(PMYIDATAOBJECT pThis);
static ULONG STDMETHODCALLTYPE MYIDATAOBJECT_Release(PMYIDATAOBJECT pThis);
static HRESULT STDMETHODCALLTYPE MYIDATAOBJECT_GetData(PMYIDATAOBJECT pThis,
	LPFORMATETC pFormatEtc, LPSTGMEDIUM pMedium);
static HRESULT STDMETHODCALLTYPE MYIDATAOBJECT_GetDataHere(PMYIDATAOBJECT pThis,
	LPFORMATETC pFormatEtc, LPSTGMEDIUM pMedium);
static HRESULT STDMETHODCALLTYPE MYIDATAOBJECT_QueryGetData(PMYIDATAOBJECT pThis,
	LPFORMATETC pFormatEtc);
static HRESULT STDMETHODCALLTYPE MYIDATAOBJECT_GetCanonicalFormatEtc(PMYIDATAOBJECT pThis,
	LPFORMATETC pFormatEtcIn, LPFORMATETC pFormatEtcOut);
static HRESULT STDMETHODCALLTYPE MYIDATAOBJECT_SetData(PMYIDATAOBJECT pThis,
	LPFORMATETC pFormatEtc, LPSTGMEDIUM pMedium, BOOL fRelease);
static HRESULT STDMETHODCALLTYPE MYIDATAOBJECT_EnumFormatEtc(PMYIDATAOBJECT pThis,
	DWORD dwDirection, LPENUMFORMATETC *ppEnumFormatEtc);
static HRESULT STDMETHODCALLTYPE MYIDATAOBJECT_DAdvise(PMYIDATAOBJECT pThis,
	LPFORMATETC pFormatetc, DWORD advf, LPADVISESINK pAdvSink,
	DWORD *pdwConnection);
static HRESULT STDMETHODCALLTYPE MYIDATAOBJECT_Dunadvise(PMYIDATAOBJECT pThis,
	DWORD dwConnection);
static HRESULT STDMETHODCALLTYPE MYIDATAOBJECT_EnumDAdvise(PMYIDATAOBJECT pThis,
	LPENUMSTATDATA *ppEnumAdvise);

/* MYIDROPTARGET functions. */
static BOOL MYIDROPTARGET_QueryDataObject(PMYIDROPTARGET pDropTarget, IDataObject *pDataObject);
static HRESULT STDMETHODCALLTYPE MYIDROPTARGET_QueryInterface(PMYIDROPTARGET pThis, REFIID riid,
	LPVOID *ppvObject);
static HRESULT STDMETHODCALLTYPE MYIDROPTARGET_AddRef(PMYIDROPTARGET pThis);
static HRESULT STDMETHODCALLTYPE MYIDROPTARGET_Release(PMYIDROPTARGET pThis);
static HRESULT STDMETHODCALLTYPE MYIDROPTARGET_DragEnter(PMYIDROPTARGET pThis, IDataObject *pDataObject,
	DWORD dwKeyState, POINTL pt, DWORD *pdwEffect);
static HRESULT STDMETHODCALLTYPE MYIDROPTARGET_DragOver(PMYIDROPTARGET pThis, DWORD dwKeyState, POINTL pt,
	DWORD *pdwEffect);
static HRESULT STDMETHODCALLTYPE MYIDROPTARGET_DragLeave(PMYIDROPTARGET pThis);
static HRESULT STDMETHODCALLTYPE MYIDROPTARGET_Drop(PMYIDROPTARGET pThis, IDataObject *pDataObject,
	DWORD dwKeyState, POINTL pt, DWORD *pdwEffect);


/* Public functions. */
/*
 * Function: MyDragDropInit()
 * Initialize the DragDrop library and OLE itself.
 * Arguments:
 * HANDLE hHeap - Handle to the heap memory handle to use for MyDragDropInit().
 */
void MyDragDropInit(HANDLE hHeap)
{
	/* Only initialize this if not already set or used before. */
	if (g_hHeap == NULL && hHeap == NULL)
		g_hHeap = GetProcessHeap();
	else if (g_hHeap == NULL)
		g_hHeap = hHeap;

	/* Initialize OLE, to be sure. */
	OleInitialize(NULL);

	return;
} /* End of MyDragDropInit(). */


/*
 * Function: CreateMyDropSource()
 * Create a Drop source and Data object.
 * Arguments:
 * BOOL bRightClick - Flag if the right mouse button was used to drag.
 * CLIPFORMAT *pFormat - An array of CLIPFORMATs being dragged.
 * HGLOBAL *phData - An array of handles, corresponding to the formats above.
 * ULONG lFmt - The number of formats / handles in the arrays above.
 * Returns:
 * PMYDRAGSOURCE - A pointer to the controlling struct, NULL if there is an error.
 */
PMYDROPSOURCE CreateMyDropSource(BOOL bRightClick, CLIPFORMAT *pFormat, HGLOBAL *phData,
	ULONG lFmt)
{
	PMYDROPSOURCE pRet;

	if ((pRet = HeapAlloc(MYDD_HEAP, 0, sizeof(MYDROPSOURCE))) == NULL)
		return NULL;

	/* Create the two members. */
	if ((pRet->pSrc = CreateDropSource(bRightClick)) == NULL)
	{
		HeapFree(MYDD_HEAP, 0, pRet);
		return NULL;
	}
	if ((pRet->pObj = CreateDataObject(pFormat, phData, lFmt)) == NULL)
	{
		pRet->pSrc->lpVtbl->Release(pRet->pSrc);
		HeapFree(MYDD_HEAP, 0, pRet);
		return NULL;
	}
	pRet->hData = NULL;

	return pRet;
} /* End of CreateMyDropSource(). */

/*
 * Function: CreateMyDropSourceText()
 * Create a Drop source and Data object, a simplified version of
 * CreateMyDropSource that only allows one format and object, a text string.
 * Arguments:
 * BOOL bRightClick - Flag if the right mouse button was used to drag.
 * LPCTSTR pText - The text to drop.
 * Returns:
 * PMYDRAGSOURCE - A pointer to the controlling struct, NULL if there is an error.
 */
PMYDROPSOURCE CreateMyDropSourceText(BOOL bRightClick, LPCTSTR pText)
{
	PMYDROPSOURCE pRet;
	CLIPFORMAT cf = CF_TEXT;
	HANDLE hText;

	/* Allocate a handle for text. */
	if ((hText = GlobalAlloc(GMEM_FIXED, _tcslen(pText) + sizeof(TCHAR))) == NULL)
		return NULL;
	_tcscpy(hText, pText);

	/* Create the source itself. */
	if ((pRet = CreateMyDropSource(bRightClick, &cf, &hText, 1)) == NULL)
	{
		GlobalFree(hText);
		return NULL;
	}

	/* Set the text. */
	pRet->hData = hText;

	return pRet;
} /* End of CreateMyDropSourceText(). */


/*
 * Function: MyDragDropSourceEx()
 * Start dragging the source.
 * Arguments:
 * PMYDROPSOURCE pDropSrc - The source to drag.
 * DWORD dwOKEffect - Allowed effects to drop.
 * DWORD *pdwEffect - Apointer to a DWORD to receive the effect that was used.
 * Returns:
 * DWORD - The result from the DoDragDrop() operation.
 */
DWORD MyDragDropSourceEx(PMYDROPSOURCE pDropSrc, DWORD dwOKEffect,
	DWORD *pdwEffect)
{
	return DoDragDrop(pDropSrc->pObj, pDropSrc->pSrc, dwOKEffect, pdwEffect);
} /* End of MyDragDropSourceEx(). */


/*
 * Function: MyDragDropSource()
 * Start dragging the source, a simplified version of DragDropSrcEx(), which
 * assumed the COPY dropeffect only and which does not return the used effect.
 * Arguments:
 * PMYDROPSOURCE pDropSrc - The source to drag.
 * Returns:
 * DWORD - The result from the DoDragDrop() operation.
 */
DWORD MyDragDropSource(PMYDROPSOURCE pDropSrc)
{
	DWORD dwEffect;

	return MyDragDropSourceEx(pDropSrc, DROPEFFECT_COPY, &dwEffect);
} /* End of MyDragDropSource(). */


/*
 * Function: MyDragDropText()
 * Simplified function for dragging text only.
 * Arguments:
 * LPCTSTR pText - The text to drag.
 * Returns:
 * DWORD - The result from DoDragDrop().
 */
DWORD MyDragDropText(LPCTSTR pText)
{
	PMYDROPSOURCE pSrc;
	DWORD dwRet;

	/* Create the stuff to drag. */
	if ((pSrc = CreateMyDropSourceText(FALSE, pText)) == NULL)
		return DROPEFFECT_NONE;

	/* Start dragging it. */
	dwRet = MyDragDropSource(pSrc);

	/* Before returning, free my temporary object. */
	FreeMyDropSource(pSrc);

	/* And now return what I got back. */
	return dwRet;
} /* End of MyDragDropText(). */


/*
 * Function: FreeMyDropSource()
 * Free the drop source.
 * Arguments:
 * PMYDROPSOURCE pDropSrc - The source to free.
 * Returns:
 * PMYDROPSOURCE - NULL.
 */
PMYDROPSOURCE FreeMyDropSource(PMYDROPSOURCE pDropSrc)
{
	if (pDropSrc == NULL)
		return NULL;

	/* Release the members. */
	pDropSrc->pSrc->lpVtbl->Release(pDropSrc->pSrc);
	pDropSrc->pObj->lpVtbl->Release(pDropSrc->pObj);

	/* Free the managed hData member, if it is allocated. */
	if (pDropSrc->hData != NULL)
		GlobalFree(pDropSrc->hData);

	/* Free the struct itself. */
	HeapFree(MYDD_HEAP, 0, pDropSrc);

	return NULL;
} /* End of FreeMyDropSource(). */


/*
 * Function: MyRegisterDragDrop()
 * Simplified registreation for a drop target.
 * Arguments:
 * HWND hWnd - The window where drops occur and messages are sent etc.
 * CLIPFORMAT *pFormat - An array of CLIPFORMATs supported.
 * ULONG lFmt - The count of CLIPFORMATS in the pFormat array.
 * HWND hWnd - The window that is the parent of this target.
 * UINT nMsg - The message to send to hWnd when data is dropped. Set to WM_NULL if we are not
 *   to send any messages.
 * MYDDCALLBACK pDropProc - The callback to call when stuff is dropped. Set to NULL
 *   to send nMsg instead of doing the callback.
 * void *pUserData - A user defined data pointer, that is passed back in the
 *   callback or message, whichever method is used.
 * Returns:
 * PMYDROPTARGET - The target object, NULL if there was an error.
 */
PMYDROPTARGET MyRegisterDragDrop(HWND hWnd, CLIPFORMAT *pFormat, ULONG lFmt, UINT nMsg,
	MYDDCALLBACK pDropProc, void *pUserData)
{
	IDropTarget *pTarget;

	/* First, create the target. */
	if ((pTarget = CreateDropTarget(pFormat, lFmt, hWnd, nMsg, pDropProc, pUserData)) == NULL)
		return NULL;

	/* Now, register for drop. If this fails, free my target the old-faschioned way, as noone knows about it anyway. */
	if (RegisterDragDrop(hWnd, pTarget) != S_OK)
	{
		HeapFree(MYDD_HEAP, 0, pTarget);
		return NULL;
	}

	return (PMYDROPTARGET)pTarget;
} /* End of MyRegisterDragDrop(). */


/*
 * Function: MyRevokeDragDrop()
 * Convenience function to revoke drag drop and also free the associated IDropTarget().
 * Arguments:
 * PMYDROPTARGET - The target to free and revoke drop from.
 * Returns:
 * PMYDROPTARGET - NULL
 */
PMYDROPTARGET MyRevokeDragDrop(PMYDROPTARGET pTarget)
{
	if (pTarget == NULL)
		return NULL;

	/* If there is a HWND, then revoke it as a drop object. */
	if (((PMYIDROPTARGET)pTarget)->hWnd != NULL)
	{
		/* Now, this is a little precaution to know that this is an OK PMIIDROPTARGET object. */
		if (GetWindowLongPtr(((PMYIDROPTARGET)pTarget)->hWnd, GWLP_WNDPROC) != 0)
			RevokeDragDrop(((PMYIDROPTARGET)pTarget)->hWnd);
	}

	/* Now, release the target. */
	((IDropTarget *)pTarget)->lpVtbl->Release((IDropTarget *)pTarget);

	return NULL;
} /* End of MyRevokeDragDrop(). */


/*
 * Function: CreateDropSource()
 * Create an IDropSource object.
 * Arguments:
 * BOOL bRightClick - Flag if right button is clicked when object is created.
 * Returns:
 * IDropSource * - A pointer to an IDropSource object, NULL if there is an error.
 */
IDropSource *CreateDropSource(BOOL bRightClick)
{
	PMYIDROPSOURCE pRet;
	static MYIDROPSOURCE_VTBL ids_vtbl = {
		MYIDROPSOURCE_QueryInterface,
		MYIDROPSOURCE_AddRef,
		MYIDROPSOURCE_Release,
		MYIDROPSOURCE_QueryContinueDrag,
		MYIDROPSOURCE_GiveFeedback };

	if ((pRet = HeapAlloc(MYDD_HEAP, 0, sizeof(MYIDROPSOURCE))) == NULL)
		return NULL;

	(PMYIDROPSOURCE_VTBL)pRet->ids.lpVtbl = &ids_vtbl;
	pRet->lRefCount = 1;
	pRet->bRightClick = bRightClick;

	return (IDropSource *)pRet;
} /* End of CreateDropSource(). */

/*
 * Function: CreateDataObject()
 * Create an IDataObject with the specified formats. Media is always TYMED_HGLOBAL.
 * Arguments:
 * CLIPFORMAT *pFormat - An array of clipboard formats to support.
 * HGLOBAL *phData - The data handles for the different formats.
 * ULONG lFmt - The size of the two arrays above, i.e. # of formats.
 * Returns:
 * IDataObject * - The pointer to the created IDataObject, NULL if there is an error.
 */
IDataObject *CreateDataObject(CLIPFORMAT *pFormat, HGLOBAL *phData, ULONG lFmt)
{
	PMYIDATAOBJECT pRet;
	ULONG i;
	static MYIDATAOBJECT_VTBL ido_vtbl = {
		MYIDATAOBJECT_QueryInterface,
		MYIDATAOBJECT_AddRef,
		MYIDATAOBJECT_Release,
		MYIDATAOBJECT_GetData,
		MYIDATAOBJECT_GetDataHere,
		MYIDATAOBJECT_QueryGetData,
		MYIDATAOBJECT_GetCanonicalFormatEtc,
		MYIDATAOBJECT_SetData,
		MYIDATAOBJECT_EnumFormatEtc,
		MYIDATAOBJECT_DAdvise,
		MYIDATAOBJECT_Dunadvise,
		MYIDATAOBJECT_EnumDAdvise };

	/* Allocate MYIDATAOBJECT and set pointers. */
	if ((pRet = HeapAlloc(MYDD_HEAP, 0, sizeof(MYIDATAOBJECT)
		+ lFmt * (sizeof(FORMATETC) + sizeof(HGLOBAL)))) == NULL)
		return NULL;
	pRet->pFormatEtc = (FORMATETC *)(((char *)pRet) + sizeof(MYIDATAOBJECT));
	pRet->phData = (HGLOBAL *)(((char *)pRet->pFormatEtc) + lFmt * sizeof(FORMATETC));

	/* Initialize data object. */
	pRet->lRefCount = 1;
	pRet->lNumFormats = lFmt;
	(PMYIDATAOBJECT_VTBL)pRet->ido.lpVtbl = &ido_vtbl;

	/* Set the format and the handles. */
	for (i = 0; i < lFmt; i++)
	{
		pRet->pFormatEtc[i].cfFormat = pFormat[i];
		pRet->pFormatEtc[i].dwAspect = DVASPECT_CONTENT;
		pRet->pFormatEtc[i].ptd = NULL;
		pRet->pFormatEtc[i].lindex = -1;
		pRet->pFormatEtc[i].tymed = TYMED_HGLOBAL;
		pRet->phData[i] = phData[i];
	}

	return (IDataObject *)pRet;
} /* End of CreateDataObject(). */


/*
 * Function: CreateDropTarget()
 * Create a IDropTarget, that has some added private members, used only here. When
 * data is dropped, either pDropProc is called, or if this is NULL, then nMsg is
 * sent to hWnd instead. When a message is sent, a pointer to a MYDROPDATA
 * is passed a wParam, and the user defined data pointer, pUserData, is
 * passed as lParam.
 * Arguments:
 * CLIPFORMAT *pFormat - An array of CLIPFORMATs supported.
 * ULONG lFmt - The count of CLIPFORMATS in the pFormat array.
 * HWND hWnd - The window that is the parent of this target.
 * UINT nMsg - The message to send to hWnd when data is dropped. Set to WM_NULL if we are not
 *   to send any messages.
 * DWORD (*pDropProc)() - The callback to call when stuff is dropped. Set to NULL
 *   to send nMsg instead of doing the callback.
 * void *pUserData - A user defined data pointer, that is passed back in the
 *   callback or message, whichever method is used.
 * Returns:
 * IDropTarget * - A pointert to a created IDropTarget, NULL if there is an error.
 */
IDropTarget *CreateDropTarget(CLIPFORMAT *pFormat, ULONG lFmt, HWND hWnd, UINT nMsg,
	DWORD(*pDropProc)(CLIPFORMAT cf, HGLOBAL hData, HWND hWnd, DWORD dwKeyState, POINTL pt, void *pUserData),
	void *pUserData)
{
	PMYIDROPTARGET pRet;
	static MYIDROPTARGET_VTBL idt_vtbl = {
		MYIDROPTARGET_QueryInterface,
		MYIDROPTARGET_AddRef,
		MYIDROPTARGET_Release,
		MYIDROPTARGET_DragEnter,
		MYIDROPTARGET_DragOver,
		MYIDROPTARGET_DragLeave,
		MYIDROPTARGET_Drop };

	/* Allocate the nasty little thing, and supply space for CLIPFORMAT array in the process. */
	if ((pRet = HeapAlloc(MYDD_HEAP, 0, sizeof(MYIDROPTARGET) + lFmt * sizeof(CLIPFORMAT))) == NULL)
		return NULL;
	pRet->pFormat = (CLIPFORMAT *)(((char *)pRet) + sizeof(MYIDROPTARGET));

	/* Set up the struct members. */
	(PMYIDROPTARGET_VTBL)pRet->idt.lpVtbl = &idt_vtbl;
	pRet->lRefCount = 1;
	pRet->hWnd = hWnd;
	pRet->nMsg = nMsg;
	pRet->bAllowDrop = FALSE;
	pRet->dwKeyState = 0;
	pRet->lNumFormats = lFmt;
	pRet->pDropProc = pDropProc;
	pRet->pUserData = pUserData;

	/* Set the format members. */
	for (lFmt = 0; lFmt < pRet->lNumFormats; lFmt++)
		pRet->pFormat[lFmt] = pFormat[lFmt];

	return (IDropTarget *)pRet;
} /* End of CreateDropTarget(). */


/* Static helper functions. */
/*
 * Function: LookupFormatEtc()
 * Match a format in a PMYDATAOBJECT onject.
 * Arguments:
 * PMYIDATAOBJECT pThis - The object to look for a match in.
 * FORMATETC *pFormat - The format to match with.
 * Returns:
 * int - The index of the formatetc in pThis, and -1 if no match was found.
 */
static int LookupFormatEtc(PMYIDATAOBJECT pThis, FORMATETC *pFormatEtc)
{
	ULONG i;

	for (i = 0; i < pThis->lNumFormats; i++)
	{
		if ((pFormatEtc->tymed & pThis->pFormatEtc[i].tymed)
			&& pFormatEtc->cfFormat == pThis->pFormatEtc[i].cfFormat
			&& pFormatEtc->dwAspect == pThis->pFormatEtc[i].dwAspect)
			return i;
	}

	return -1;
} /* End of LookupFormatEtc(). */


/*
 * MYIDROPSOURCE functions.
 * These functions are just implementations of the standard IDropSource methods.
 */
static HRESULT STDMETHODCALLTYPE MYIDROPSOURCE_QueryInterface(PMYIDROPSOURCE pThis,
	REFIID riid, LPVOID *ppvObject)
{
	*ppvObject = NULL;

	if (IsEqualGUID(riid, &IID_IUnknown))
	{
		MYIDROPSOURCE_AddRef(pThis);
		*ppvObject = pThis;
		return S_OK;
	}
	else if (IsEqualGUID(riid, &IID_IDropSource))
	{
		MYIDROPSOURCE_AddRef(pThis);
		*ppvObject = pThis;
		return S_OK;
	}

	return E_NOINTERFACE;
} /* End of MYIDROPSOURCE_QueryInterface(). */


static ULONG STDMETHODCALLTYPE MYIDROPSOURCE_AddRef(PMYIDROPSOURCE pThis)
{
	return InterlockedIncrement(&pThis->lRefCount);
} /* End of MYIDROPSOURCE_AddRef(). */


static ULONG STDMETHODCALLTYPE MYIDROPSOURCE_Release(PMYIDROPSOURCE pThis)
{
	LONG nCount;

	if ((nCount = InterlockedDecrement(&pThis->lRefCount)) == 0)
	{
		HeapFree(MYDD_HEAP, 0, pThis);
		return 0;
	}
	return nCount;
} /* End of MYIDROPSOURCE_Release(). */


static HRESULT STDMETHODCALLTYPE MYIDROPSOURCE_QueryContinueDrag(PMYIDROPSOURCE pThis,
	BOOL fEscapePressed, DWORD dwKeyState)
{
	if (fEscapePressed)
		return DRAGDROP_S_CANCEL;
	else if ((dwKeyState & MK_LBUTTON) == 0 && !pThis->bRightClick)
		return DRAGDROP_S_DROP;
	else if ((dwKeyState & MK_RBUTTON) == 0 && pThis->bRightClick)
		return DRAGDROP_S_DROP;

	return S_OK;
} /* End of MYIDROPSOURCE_QueryContinueDrag(). */


static HRESULT STDMETHODCALLTYPE MYIDROPSOURCE_GiveFeedback(PMYIDROPSOURCE pThis,
	DWORD dwEffect)
{
	return DRAGDROP_S_USEDEFAULTCURSORS;
} /* End of MYIDROPSOURCE_GiveFeedback(). */


/*
 * MYIDATAOBJECT functions.
 * These functions are just implementations of the standard IDataObject methods.
 */
static HRESULT STDMETHODCALLTYPE MYIDATAOBJECT_QueryInterface(PMYIDATAOBJECT pThis,
	REFIID riid, LPVOID *ppvObject)
{
	*ppvObject = NULL;

	if (IsEqualGUID(riid, &IID_IUnknown))
	{
		MYIDATAOBJECT_AddRef(pThis);
		*ppvObject = pThis;
		return S_OK;
	}
	else if (IsEqualGUID(riid, &IID_IDataObject))
	{
		MYIDATAOBJECT_AddRef(pThis);
		*ppvObject = pThis;
		return S_OK;
	}

	return E_NOINTERFACE;
} /* End of MYIDATAOBJECT_QDragDrop Custom textueryInterface(). */


static ULONG STDMETHODCALLTYPE MYIDATAOBJECT_AddRef(PMYIDATAOBJECT pThis)
{
	return InterlockedIncrement(&pThis->lRefCount);
} /* End of MYIDATAOBJECT_AddRef(). */


static ULONG STDMETHODCALLTYPE MYIDATAOBJECT_Release(PMYIDATAOBJECT pThis)
{
	LONG nCount;

	if ((nCount = InterlockedDecrement(&pThis->lRefCount)) == 0)
	{
		HeapFree(MYDD_HEAP, 0, pThis);
		return 0;
	}
	return nCount;
} /* End of MYIDATAOBJECT_Release(). */


static HRESULT STDMETHODCALLTYPE MYIDATAOBJECT_GetData(PMYIDATAOBJECT pThis,
	LPFORMATETC pFormatEtc, LPSTGMEDIUM pMedium)
{
	int nIndex;
	size_t nLen;
	void *pData;

	/* Try to match the requested FORMATETC with one of our supported formats */
	if ((nIndex = LookupFormatEtc(pThis, pFormatEtc)) == -1)
		return DV_E_FORMATETC;

	/* Set the medium hGlobal, which is for now the only supported format. */
	pMedium->tymed = TYMED_HGLOBAL;
	pMedium->pUnkForRelease = 0;

	/* Get the size of the source. */
	if ((nLen = GlobalSize(pThis->phData[nIndex])) == 0)
		return STG_E_MEDIUMFULL;

	/* Allocate the destination. */
	if ((pMedium->hGlobal = GlobalAlloc(GMEM_FIXED, nLen)) == NULL)
		return STG_E_MEDIUMFULL;

	/* Lock the object and get a pointer to it. */
	if ((pData = GlobalLock(pThis->phData[nIndex])) == NULL)
	{
		GlobalFree(pMedium->hGlobal);
		return STG_E_MEDIUMFULL;
	}

	/* Copy the data. */
	memcpy(pMedium->hGlobal, pData, nLen);

	/* Unlock the memory. */
	GlobalUnlock(pThis->phData[nIndex]);

	return S_OK;
} /* End of MYIDATAOBJECT_GetData(). */


static HRESULT STDMETHODCALLTYPE MYIDATAOBJECT_GetDataHere(PMYIDATAOBJECT pThis,
	LPFORMATETC pFormatEtc, LPSTGMEDIUM pMedium)
{
	return DATA_E_FORMATETC;
} /* End of MYIDATAOBJECT_GetDataHere(). */


static HRESULT STDMETHODCALLTYPE MYIDATAOBJECT_QueryGetData(PMYIDATAOBJECT pThis,
	LPFORMATETC pFormatEtc)
{
	return LookupFormatEtc(pThis, pFormatEtc) == -1 ? DV_E_FORMATETC : S_OK;
} /* End of MYIDATAOBJECT_QueryGetData(). */


static HRESULT STDMETHODCALLTYPE MYIDATAOBJECT_GetCanonicalFormatEtc(PMYIDATAOBJECT pThis,
	LPFORMATETC pFormatEtcIn, LPFORMATETC pFormatEtcOut)
{
	pFormatEtcOut->ptd = NULL;
	return E_NOTIMPL;
} /* End of MYIDATAOBJECT_GetCanonicalFormatEtc(). */


static HRESULT STDMETHODCALLTYPE MYIDATAOBJECT_SetData(PMYIDATAOBJECT pThis,
	LPFORMATETC pFormatEtc, LPSTGMEDIUM pMedium, BOOL fRelease)
{
	return E_UNEXPECTED;
} /* End of MYIDATAOBJECT_SetData(). */


static HRESULT STDMETHODCALLTYPE MYIDATAOBJECT_EnumFormatEtc(PMYIDATAOBJECT pThis,
	DWORD dwDirection, LPENUMFORMATETC *ppEnumFormatEtc)
{
	if (dwDirection != DATADIR_GET)
		return E_NOTIMPL;

	/* Note that this is for W2K and up only. Before this, this will NOT work! */
	SHCreateStdEnumFmtEtc(pThis->lNumFormats, pThis->pFormatEtc, ppEnumFormatEtc);

	return S_OK;
} /* End of MYIDATAOBJECT_EnumFormatEtc(). */


static HRESULT STDMETHODCALLTYPE MYIDATAOBJECT_DAdvise(PMYIDATAOBJECT pThis,
	LPFORMATETC pFormatetc, DWORD advf, LPADVISESINK pAdvSink,
	DWORD *pdwConnection)
{
	return E_FAIL;
} /* End of MYIDATAOBJECT_DAdvise(). */


static HRESULT STDMETHODCALLTYPE MYIDATAOBJECT_Dunadvise(PMYIDATAOBJECT pThis,
	DWORD dwConnection)
{
	return E_FAIL;
} /* End of MYIDATAOBJECT_Dunadvise(). */


static HRESULT STDMETHODCALLTYPE MYIDATAOBJECT_EnumDAdvise(PMYIDATAOBJECT pThis,
	LPENUMSTATDATA *ppEnumAdvise)
{
	return E_FAIL;
} /* End of MYIDATAOBJECT_EnumDAdvise(). */


/*
 * MYIDROPTARGET functions.
 * These functions are just implementations of the standard IDropTarget methods.
 */
static BOOL MYIDROPTARGET_QueryDataObject(PMYIDROPTARGET pDropTarget, IDataObject *pDataObject)
{
	ULONG lFmt;
	FORMATETC fmtetc = { CF_TEXT, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };

	for (lFmt = 0; lFmt < pDropTarget->lNumFormats; lFmt++)
	{
		fmtetc.cfFormat = pDropTarget->pFormat[lFmt];
		if (pDataObject->lpVtbl->QueryGetData(pDataObject, &fmtetc) == S_OK)
			return TRUE;
	}

	return FALSE;
} /* End of MYIDROPTARGET_QueryDataObject(). */


static DWORD MYIDROPTARGET_DropEffect(DWORD dwKeyState, POINTL pt, DWORD dwAllowed)
{
	DWORD dwEffect = 0;

	if (dwKeyState & MK_CONTROL)
		dwEffect = dwAllowed & DROPEFFECT_COPY;
	else if (dwKeyState & MK_SHIFT)
		dwEffect = dwAllowed & DROPEFFECT_MOVE;

	if (dwEffect == 0)
	{
		if (dwAllowed & DROPEFFECT_COPY)
			dwEffect = DROPEFFECT_COPY;
		if (dwAllowed & DROPEFFECT_MOVE)
			dwEffect = DROPEFFECT_MOVE;
	}

	return dwEffect;
}

/* IDropTarget private methods. */
static HRESULT STDMETHODCALLTYPE MYIDROPTARGET_QueryInterface(PMYIDROPTARGET pThis, REFIID riid,
	LPVOID *ppvObject)
{
	*ppvObject = NULL;

	if (IsEqualGUID(riid, &IID_IUnknown))
	{
		MYIDROPTARGET_AddRef(pThis);
		*ppvObject = pThis;
		return S_OK;
	}
	else if (IsEqualGUID(riid, &IID_IDropTarget))
	{
		MYIDROPTARGET_AddRef(pThis);
		*ppvObject = pThis;
		return S_OK;
	}

	return E_NOINTERFACE;
} /* End of MYIDROPTARGET_QueryInterface(). */


static HRESULT STDMETHODCALLTYPE MYIDROPTARGET_AddRef(PMYIDROPTARGET pThis)
{
	return InterlockedIncrement(&pThis->lRefCount);
} /* End of MYIDROPTARGET_AddRef(). */


static HRESULT STDMETHODCALLTYPE MYIDROPTARGET_Release(PMYIDROPTARGET pThis)
{
	LONG nCount;

	if ((nCount = InterlockedDecrement(&pThis->lRefCount)) == 0)
	{
		HeapFree(MYDD_HEAP, 0, pThis);
		return 0;
	}

	return nCount;
} /* End of MYIDROPTARGET_Release(). */


static HRESULT STDMETHODCALLTYPE MYIDROPTARGET_DragEnter(PMYIDROPTARGET pThis, IDataObject *pDataObject,
	DWORD dwKeyState, POINTL pt, DWORD *pdwEffect)
{
	if (pThis->bAllowDrop = MYIDROPTARGET_QueryDataObject(pThis, pDataObject))
	{
		*pdwEffect = MYIDROPTARGET_DropEffect(dwKeyState, pt, *pdwEffect);
		SetFocus(pThis->hWnd);
	}
	else
		*pdwEffect = DROPEFFECT_NONE;

	return S_OK;
} /* End of MYIDROPTARGET_DragEnter(). */


static HRESULT STDMETHODCALLTYPE MYIDROPTARGET_DragOver(PMYIDROPTARGET pThis, DWORD dwKeyState, POINTL pt,
	DWORD *pdwEffect)
{
	if (pThis->bAllowDrop)
	{
		pThis->dwKeyState = dwKeyState;

		*pdwEffect = MYIDROPTARGET_DropEffect(dwKeyState, pt, *pdwEffect);
	}
	else
		*pdwEffect = DROPEFFECT_NONE;

	return S_OK;
} /* End of MYIDROPTARGET_DragEffect(). */


static HRESULT STDMETHODCALLTYPE MYIDROPTARGET_DragLeave(PMYIDROPTARGET pThis)
{
	return S_OK;
} /* End of MYIDROPTARGET_DragLeave(). */


static HRESULT STDMETHODCALLTYPE MYIDROPTARGET_Drop(PMYIDROPTARGET pThis, IDataObject *pDataObject,
	DWORD dwKeyState, POINTL pt, DWORD *pdwEffect)
{
	FORMATETC fmtetc = { CF_TEXT, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
	STGMEDIUM medium;
	ULONG lFmt;
	MYDROPDATA DropData;

	if (pThis->bAllowDrop)
	{
		/* Find the first matching CLIPFORMAT that I can handle. */
		for (lFmt = 0; lFmt < pThis->lNumFormats; lFmt++)
		{
			fmtetc.cfFormat = pThis->pFormat[lFmt];
			if (pDataObject->lpVtbl->QueryGetData(pDataObject, &fmtetc) == S_OK)
				break;
		}
		/* If we found a matching format, then handle it now. */
		if (lFmt < pThis->lNumFormats)
		{
			/* Get the data being dragged. */
			pDataObject->lpVtbl->GetData(pDataObject, &fmtetc, &medium);
			*pdwEffect = DROPEFFECT_NONE;
			/* If a callback procedure is defined, then use that. */
			if (pThis->pDropProc != NULL)
				*pdwEffect = (*pThis->pDropProc)(pThis->pFormat[lFmt], medium.hGlobal, pThis->hWnd, pThis->dwKeyState,
				pt, pThis->pUserData);
			/* Else, if a message is valid, then send that. */
			else if (pThis->nMsg != WM_NULL)
			{
				/* Fill the struct with the relevant data. */
				DropData.cf = pThis->pFormat[lFmt];
				DropData.dwKeyState = pThis->dwKeyState;
				DropData.hData = medium.hGlobal;
				DropData.pt = pt;

				/* And send the message. */
				*pdwEffect = (DWORD)SendMessage(pThis->hWnd, pThis->nMsg, (WPARAM)&DropData,
					(LPARAM)pThis->pUserData);
			}
			/* Release the medium, if it was used. */
			if (*pdwEffect != DROPEFFECT_NONE)
				ReleaseStgMedium(&medium);
		}
	}
	else
		*pdwEffect = DROPEFFECT_NONE;

	return S_OK;
} /* End of MYIDROPTARGET_Drop(). */
// HtmlViewObject.c  | Oxygen © 2016 by Thomas Führinger

// from
// http://www.codeguru.com/cpp/i-n/ieprogram/article.php/c4379/Display-a-Web-Page-in-a-Plain-C-Win32-Application.htm
// http://www.codeproject.com/Articles/3365/Embed-an-HTML-control-in-your-own-window-using-pla

// fix me!
#include "Oxygen.h"

static OxClass* pOxClass;

HRESULT STDMETHODCALLTYPE Storage_QueryInterface(IStorage FAR* self, REFIID riid, LPVOID FAR* ppvObj);
HRESULT STDMETHODCALLTYPE Storage_AddRef(IStorage FAR* self);
HRESULT STDMETHODCALLTYPE Storage_Release(IStorage FAR* self);
HRESULT STDMETHODCALLTYPE Storage_CreateStream(IStorage FAR* self, const WCHAR *pwcsName, DWORD grfMode, DWORD reserved1, DWORD reserved2, IStream **ppstm);
HRESULT STDMETHODCALLTYPE Storage_OpenStream(IStorage FAR* self, const WCHAR * pwcsName, void *reserved1, DWORD grfMode, DWORD reserved2, IStream **ppstm);
HRESULT STDMETHODCALLTYPE Storage_CreateStorage(IStorage FAR* self, const WCHAR *pwcsName, DWORD grfMode, DWORD reserved1, DWORD reserved2, IStorage **ppstg);
HRESULT STDMETHODCALLTYPE Storage_OpenStorage(IStorage FAR* self, const WCHAR * pwcsName, IStorage * pstgPriority, DWORD grfMode, SNB snbExclude, DWORD reserved, IStorage **ppstg);
HRESULT STDMETHODCALLTYPE Storage_CopyTo(IStorage FAR* self, DWORD ciidExclude, IID const *rgiidExclude, SNB snbExclude, IStorage *pstgDest);
HRESULT STDMETHODCALLTYPE Storage_MoveElementTo(IStorage FAR* self, const OLECHAR *pwcsName, IStorage * pstgDest, const OLECHAR *pwcsNewName, DWORD grfFlags);
HRESULT STDMETHODCALLTYPE Storage_Commit(IStorage FAR* self, DWORD grfCommitFlags);
HRESULT STDMETHODCALLTYPE Storage_Revert(IStorage FAR* self);
HRESULT STDMETHODCALLTYPE Storage_EnumElements(IStorage FAR* self, DWORD reserved1, void * reserved2, DWORD reserved3, IEnumSTATSTG ** ppenum);
HRESULT STDMETHODCALLTYPE Storage_DestroyElement(IStorage FAR* self, const OLECHAR *pwcsName);
HRESULT STDMETHODCALLTYPE Storage_RenameElement(IStorage FAR* self, const WCHAR *pwcsOldName, const WCHAR *pwcsNewName);
HRESULT STDMETHODCALLTYPE Storage_SetElementTimes(IStorage FAR* self, const WCHAR *pwcsName, FILETIME const *pctime, FILETIME const *patime, FILETIME const *pmtime);
HRESULT STDMETHODCALLTYPE Storage_SetClass(IStorage FAR* self, REFCLSID clsid);
HRESULT STDMETHODCALLTYPE Storage_SetStateBits(IStorage FAR* self, DWORD grfStateBits, DWORD grfMask);
HRESULT STDMETHODCALLTYPE Storage_Stat(IStorage FAR* self, STATSTG * pstatstg, DWORD grfStatFlag);

IStorageVtbl IStorageTable = {
	Storage_QueryInterface,
	Storage_AddRef,
	Storage_Release,
	Storage_CreateStream,
	Storage_OpenStream,
	Storage_CreateStorage,
	Storage_OpenStorage,
	Storage_CopyTo,
	Storage_MoveElementTo,
	Storage_Commit,
	Storage_Revert,
	Storage_EnumElements,
	Storage_DestroyElement,
	Storage_RenameElement,
	Storage_SetElementTimes,
	Storage_SetClass,
	Storage_SetStateBits,
	Storage_Stat
};

IStorage Storage = { &IStorageTable };

HRESULT STDMETHODCALLTYPE Frame_QueryInterface(IOleInPlaceFrame FAR* self, REFIID riid, LPVOID FAR* ppvObj);
HRESULT STDMETHODCALLTYPE Frame_AddRef(IOleInPlaceFrame FAR* self);
HRESULT STDMETHODCALLTYPE Frame_Release(IOleInPlaceFrame FAR* self);
HRESULT STDMETHODCALLTYPE Frame_GetWindow(IOleInPlaceFrame FAR* self, HWND FAR* lphwnd);
HRESULT STDMETHODCALLTYPE Frame_ContextSensitiveHelp(IOleInPlaceFrame FAR* self, BOOL fEnterMode);
HRESULT STDMETHODCALLTYPE Frame_GetBorder(IOleInPlaceFrame FAR* self, LPRECT lprectBorder);
HRESULT STDMETHODCALLTYPE Frame_RequestBorderSpace(IOleInPlaceFrame FAR* self, LPCBORDERWIDTHS pborderwidths);
HRESULT STDMETHODCALLTYPE Frame_SetBorderSpace(IOleInPlaceFrame FAR* self, LPCBORDERWIDTHS pborderwidths);
HRESULT STDMETHODCALLTYPE Frame_SetActiveObject(IOleInPlaceFrame FAR* self, IOleInPlaceActiveObject *pActiveObject, LPCOLESTR pszObjName);
HRESULT STDMETHODCALLTYPE Frame_InsertMenus(IOleInPlaceFrame FAR* self, HMENU hmenuShared, LPOLEMENUGROUPWIDTHS lpMenuWidths);
HRESULT STDMETHODCALLTYPE Frame_SetMenu(IOleInPlaceFrame FAR* self, HMENU hmenuShared, HOLEMENU holemenu, HWND hwndActiveObject);
HRESULT STDMETHODCALLTYPE Frame_RemoveMenus(IOleInPlaceFrame FAR* self, HMENU hmenuShared);
HRESULT STDMETHODCALLTYPE Frame_SetStatusText(IOleInPlaceFrame FAR* self, LPCOLESTR pszStatusText);
HRESULT STDMETHODCALLTYPE Frame_EnableModeless(IOleInPlaceFrame FAR* self, BOOL fEnable);
HRESULT STDMETHODCALLTYPE Frame_TranslateAccelerator(IOleInPlaceFrame FAR* self, LPMSG lpmsg, WORD wID);

IOleInPlaceFrameVtbl IOleInPlaceFrameTable = {
	Frame_QueryInterface,
	Frame_AddRef,
	Frame_Release,
	Frame_GetWindow,
	Frame_ContextSensitiveHelp,
	Frame_GetBorder,
	Frame_RequestBorderSpace,
	Frame_SetBorderSpace,
	Frame_SetActiveObject,
	Frame_InsertMenus,
	Frame_SetMenu,
	Frame_RemoveMenus,
	Frame_SetStatusText,
	Frame_EnableModeless,
	Frame_TranslateAccelerator
};

HRESULT STDMETHODCALLTYPE Site_QueryInterface(IOleClientSite FAR* self, REFIID riid, void ** ppvObject);
HRESULT STDMETHODCALLTYPE Site_AddRef(IOleClientSite FAR* self);
HRESULT STDMETHODCALLTYPE Site_Release(IOleClientSite FAR* self);
HRESULT STDMETHODCALLTYPE Site_SaveObject(IOleClientSite FAR* self);
HRESULT STDMETHODCALLTYPE Site_GetMoniker(IOleClientSite FAR* self, DWORD dwAssign, DWORD dwWhichMoniker, IMoniker ** ppmk);
HRESULT STDMETHODCALLTYPE Site_GetContainer(IOleClientSite FAR* self, LPOLECONTAINER FAR* ppContainer);
HRESULT STDMETHODCALLTYPE Site_ShowObject(IOleClientSite FAR* self);
HRESULT STDMETHODCALLTYPE Site_OnShowWindow(IOleClientSite FAR* self, BOOL fShow);
HRESULT STDMETHODCALLTYPE Site_RequestNewObjectLayout(IOleClientSite FAR* self);

IOleClientSiteVtbl IOleClientSiteTable = {
	Site_QueryInterface,
	Site_AddRef,
	Site_Release,
	Site_SaveObject,
	Site_GetMoniker,
	Site_GetContainer,
	Site_ShowObject,
	Site_OnShowWindow,
	Site_RequestNewObjectLayout
};

HRESULT STDMETHODCALLTYPE Site_GetWindow(IOleInPlaceSite FAR* self, HWND FAR* lphwnd);
HRESULT STDMETHODCALLTYPE Site_ContextSensitiveHelp(IOleInPlaceSite FAR* self, BOOL fEnterMode);
HRESULT STDMETHODCALLTYPE Site_CanInPlaceActivate(IOleInPlaceSite FAR* self);
HRESULT STDMETHODCALLTYPE Site_OnInPlaceActivate(IOleInPlaceSite FAR* self);
HRESULT STDMETHODCALLTYPE Site_OnUIActivate(IOleInPlaceSite FAR* self);
HRESULT STDMETHODCALLTYPE Site_GetWindowContext(IOleInPlaceSite FAR* self, LPOLEINPLACEFRAME FAR* lplpFrame, LPOLEINPLACEUIWINDOW FAR* lplpDoc, LPRECT lprcPosRect, LPRECT lprcClipRect, LPOLEINPLACEFRAMEINFO lpFrameInfo);
HRESULT STDMETHODCALLTYPE Site_Scroll(IOleInPlaceSite FAR* self, SIZE scrollExtent);
HRESULT STDMETHODCALLTYPE Site_OnUIDeactivate(IOleInPlaceSite FAR* self, BOOL fUndoable);
HRESULT STDMETHODCALLTYPE Site_OnInPlaceDeactivate(IOleInPlaceSite FAR* self);
HRESULT STDMETHODCALLTYPE Site_DiscardUndoState(IOleInPlaceSite FAR* self);
HRESULT STDMETHODCALLTYPE Site_DeactivateAndUndo(IOleInPlaceSite FAR* self);
HRESULT STDMETHODCALLTYPE Site_OnPosRectChange(IOleInPlaceSite FAR* self, LPCRECT lprcPosRect);

IOleInPlaceSiteVtbl IOleInPlaceSiteTable = {
	Site_QueryInterface,
	Site_AddRef,
	Site_Release,
	Site_GetWindow,
	Site_ContextSensitiveHelp,
	Site_CanInPlaceActivate,
	Site_OnInPlaceActivate,
	Site_OnUIActivate,
	Site_GetWindowContext,
	Site_Scroll,
	Site_OnUIDeactivate,
	Site_OnInPlaceDeactivate,
	Site_DiscardUndoState,
	Site_DeactivateAndUndo,
	Site_OnPosRectChange
};

#define NOTIMPLEMENTED _ASSERT(0); return(E_NOTIMPL)

OxHtmlViewObject*
OxHtmlView_New(OxWidgetObject* oxParent, OxRect* rc)
{

	OxHtmlViewObject* ox = (OxHtmlViewObject*)OxObject_Allocate(pOxClass);
	if (ox == NULL)
		return NULL;

	if (!OxHtmlView_Init((OxWidgetObject*)ox, oxParent, rc))
		return NULL;

	return ox;
};

BOOL
OxHtmlView_Init(OxHtmlViewObject* ox, OxWidgetObject* oxParent, OxRect* rc)
{
	IWebBrowser2* pWebBrowser2;
	RECT rect;
	HRESULT hr;

	if (!OxBox_Init((OxWidgetObject*)ox, oxParent, rc))
		return NULL;

	ox->OleClientSiteExt.orig.lpVtbl = &IOleClientSiteTable;
	ox->OleClientSiteExt.OleInPlaceSiteExt.orig.lpVtbl = &IOleInPlaceSiteTable;
	ox->OleClientSiteExt.OleInPlaceSiteExt.OleInPlaceFrameExt.orig.lpVtbl = &IOleInPlaceFrameTable;
	ox->OleClientSiteExt.OleInPlaceSiteExt.OleInPlaceFrameExt.ox = ox;

	hr = OleCreate(&CLSID_WebBrowser, &IID_IOleObject, OLERENDER_DRAW, 0, (IOleClientSite*)&ox->OleClientSiteExt, (IStorage*)&Storage, (void**)&ox->pOleBrowserObject);
	if (SUCCEEDED(hr)) {
		ox->pOleBrowserObject->lpVtbl->SetHostNames(ox->pOleBrowserObject, L"Host", 0);


		hr = OleSetContainedObject((struct IUnknown*)ox->pOleBrowserObject, TRUE);
		if (SUCCEEDED(hr)) {
			GetClientRect(ox->hWin, &rect);
			hr = ox->pOleBrowserObject->lpVtbl->DoVerb(ox->pOleBrowserObject, OLEIVERB_SHOW, NULL, (IOleClientSite*)&ox->OleClientSiteExt, -1, ox->hWin, &rect);
			if (SUCCEEDED(hr)) {
				hr = ox->pOleBrowserObject->lpVtbl->QueryInterface(ox->pOleBrowserObject, &IID_IWebBrowser2, (void**)&pWebBrowser2);
				if (SUCCEEDED(hr)) {
					pWebBrowser2->lpVtbl->put_Left(pWebBrowser2, 0);
					pWebBrowser2->lpVtbl->put_Top(pWebBrowser2, 0);
					pWebBrowser2->lpVtbl->put_Width(pWebBrowser2, rect.right);
					pWebBrowser2->lpVtbl->put_Height(pWebBrowser2, rect.bottom);
					pWebBrowser2->lpVtbl->Release(pWebBrowser2);
				}
				else
					OxErr_SetFromWindowsHR(hr);
			}
			else
				OxErr_SetFromWindowsHR(hr);
		}
		else
			OxErr_SetFromWindowsHR(hr);
	}
	else
		OxErr_SetFromWindowsHR(hr);

	if (FAILED(hr))
		return NULL;

	return ox;
}

static const SAFEARRAYBOUND ArrayBound = { 1, 0 };

BOOL
OxHtmlView_RenderString(OxHtmlViewObject* ox, char* sString)
{
	HRESULT hr;

	IWebBrowser2*   pWebBrowser2;
	LPDISPATCH		lpDispatch;
	IHTMLDocument2* htmlDoc2;
	SAFEARRAY*      sfArray;
	VARIANT			myURL;
	VARIANT*        pVar;
	BSTR			bstr;

	hr = ox->pOleBrowserObject->lpVtbl->QueryInterface(ox->pOleBrowserObject, &IID_IWebBrowser2, (void**)&pWebBrowser2);
	if (SUCCEEDED(hr)){
		VariantInit(&myURL);
		myURL.vt = VT_BSTR;
		myURL.bstrVal = SysAllocString(L"about:blank");
		pWebBrowser2->lpVtbl->Navigate2(pWebBrowser2, &myURL, 0, 0, 0, 0);
		VariantClear(&myURL);

		hr = pWebBrowser2->lpVtbl->get_Document(pWebBrowser2, &lpDispatch);
		if (SUCCEEDED(hr)) {
			hr = lpDispatch->lpVtbl->QueryInterface(lpDispatch, &IID_IHTMLDocument2, (void**)&htmlDoc2);
			if (SUCCEEDED(hr)) {
				if ((sfArray = SafeArrayCreate(VT_VARIANT, 1, (SAFEARRAYBOUND*)&ArrayBound)))
				{
					hr = SafeArrayAccessData(sfArray, (void**)&pVar);
					if (SUCCEEDED(hr)) {
						pVar->vt = VT_BSTR;
						LPWSTR szString = OxToW(sString);
						bstr = SysAllocString(szString);
						OxFree(szString);
						if ((pVar->bstrVal = bstr))
							htmlDoc2->lpVtbl->write(htmlDoc2, sfArray);
						/*
						HWND hWin; long iH = NULL;	
						hr = pWebBrowser2->lpVtbl->get_HWND(pWebBrowser2, &iH); 
						if (SUCCEEDED(hr)) {
							hWin = (HWND)iH; Xi("hWinh", hWin);
							DragAcceptFiles(hWin, FALSE);
						}
						else {
							OxErr_SetFromWindowsHR(hr);
						}*/
					}
					else
						OxErr_SetFromWindowsHR(hr);
					SafeArrayDestroy(sfArray);
				}
				htmlDoc2->lpVtbl->Release(htmlDoc2);
			}
			else
				OxErr_SetFromWindowsHR(hr);
			lpDispatch->lpVtbl->Release(lpDispatch);
		}
		else
			OxErr_SetFromWindowsHR(hr);
		pWebBrowser2->lpVtbl->Release(pWebBrowser2);
	}
	else
		OxErr_SetFromWindowsHR(hr);

	if (SUCCEEDED(hr))
		return TRUE;
	else
		return FALSE;
}

BOOL
OxHtmlView_RenderData(OxHtmlViewObject* ox, BOOL bEdit)
{
	return OxHtmlView_RenderString(ox, ((OxStringObject*)ox->oxData)->sString);
}

BOOL
OxHtmlView_RenderURL(OxHtmlViewObject* ox, char* sURL)
{
	HRESULT hr;
	IWebBrowser2*   pWebBrowser2;
	VARIANT			myURL;

	hr = ox->pOleBrowserObject->lpVtbl->QueryInterface(ox->pOleBrowserObject, &IID_IWebBrowser2, (void**)&pWebBrowser2);
	if (SUCCEEDED(hr)){
		LPWSTR szURL = OxToW(sURL);
		VariantInit(&myURL);
		myURL.vt = VT_BSTR;
		myURL.bstrVal = SysAllocString(szURL);
		pWebBrowser2->lpVtbl->Navigate2(pWebBrowser2, &myURL, 0, 0, 0, 0);
		VariantClear(&myURL);
		OxFree(szURL);
		pWebBrowser2->lpVtbl->Release(pWebBrowser2);
		return TRUE;
	}
	OxErr_SetStringFormat(OxERROR_WINDOWS, "Can not QueryInterface for browser object. Error %d", hr);
	return FALSE;
}

static BOOL
OxHtmlView_Reposition(OxHtmlViewObject* ox)
{
	IWebBrowser2* pWebBrowser2;
	OxRect rc;
	OxWidget_CalculateRect(ox, &rc);

	if (MoveWindow(ox->hWin, rc.iLeft, rc.iTop, rc.iWidth, rc.iHeight, TRUE) == 0) {
		OxErr_SetFromWindows();
		return FALSE;
	}

	if (!GetClientRect(ox->hWin, &rc)){
		OxErr_SetFromWindows();
		return FALSE;
	}

	HRESULT hr = ox->pOleBrowserObject->lpVtbl->QueryInterface(ox->pOleBrowserObject, &IID_IWebBrowser2, (void**)&pWebBrowser2);
	if (SUCCEEDED(hr)){
		//Xi("pWebBrowser2", pWebBrowser2);
		hr = pWebBrowser2->lpVtbl->put_Width(pWebBrowser2, rc.iWidth);
		if (SUCCEEDED(hr))
			hr = pWebBrowser2->lpVtbl->put_Height(pWebBrowser2, rc.iHeight);
		else {
			OxErr_SetFromWindowsHR(hr);
		}
		pWebBrowser2->lpVtbl->Release(pWebBrowser2);
		return TRUE;
	}

	OxErr_SetStringFormat(OxERROR_WINDOWS, "Can not reposition OLE object. Error %d", hr);
	return FALSE;
}

BOOL
OxHtmlView_SetData(OxHtmlViewObject* ox, OxObject* oxData)
{
	if (!OxWidget_SetData((OxWidgetObject*)ox, oxData))
		return FALSE;

	return OxHtmlView_RenderData(ox, FALSE);
}

static BOOL
OxHtmlView_Delete(OxHtmlViewObject* ox)
{
	ox->pOleBrowserObject->lpVtbl->Close(ox->pOleBrowserObject, OLECLOSE_NOSAVE);
	ox->pOleBrowserObject->lpVtbl->Release(ox->pOleBrowserObject);
	return pOxClass->pBase->fnDelete((OxObject*)ox);
}

OxClass OxHtmlViewClass = {
	"HtmlView",                /* sName */
	&OxBoxClass,               /* pBase */
	sizeof(OxHtmlViewObject),  /* nSize */
	OxHtmlView_Delete,         /* fnDelete */
	NULL,                      /* pUserData */
	0                          /* aMethods */
};

BOOL OxHtmlViewClass_Init()
{
	pOxClass = &OxHtmlViewClass;
	Ox_INHERIT_METHODS(pOxClass);
	pOxClass->aMethods[OxWIDGET_REPOSITION] = OxHtmlView_Reposition;
	pOxClass->aMethods[OxWIDGET_SETDATA] = OxHtmlView_SetData;
	return TRUE;
}


HRESULT STDMETHODCALLTYPE Storage_QueryInterface(IStorage FAR* self, REFIID riid, LPVOID FAR* ppvObj)
{
	NOTIMPLEMENTED;
}

HRESULT STDMETHODCALLTYPE Storage_AddRef(IStorage FAR* self)
{
	return(1);
}

HRESULT STDMETHODCALLTYPE Storage_Release(IStorage FAR* self)
{
	return(1);
}

HRESULT STDMETHODCALLTYPE Storage_CreateStream(IStorage FAR* self, const WCHAR *pwcsName, DWORD grfMode, DWORD reserved1, DWORD reserved2, IStream **ppstm)
{
	NOTIMPLEMENTED;
}

HRESULT STDMETHODCALLTYPE Storage_OpenStream(IStorage FAR* self, const WCHAR * pwcsName, void *reserved1, DWORD grfMode, DWORD reserved2, IStream **ppstm)
{
	NOTIMPLEMENTED;
}

HRESULT STDMETHODCALLTYPE Storage_CreateStorage(IStorage FAR* self, const WCHAR *pwcsName, DWORD grfMode, DWORD reserved1, DWORD reserved2, IStorage **ppstg)
{
	NOTIMPLEMENTED;
}

HRESULT STDMETHODCALLTYPE Storage_OpenStorage(IStorage FAR* self, const WCHAR * pwcsName, IStorage * pstgPriority, DWORD grfMode, SNB snbExclude, DWORD reserved, IStorage **ppstg)
{
	NOTIMPLEMENTED;
}

HRESULT STDMETHODCALLTYPE Storage_CopyTo(IStorage FAR* self, DWORD ciidExclude, IID const *rgiidExclude, SNB snbExclude, IStorage *pstgDest)
{
	NOTIMPLEMENTED;
}

HRESULT STDMETHODCALLTYPE Storage_MoveElementTo(IStorage FAR* self, const OLECHAR *pwcsName, IStorage * pstgDest, const OLECHAR *pwcsNewName, DWORD grfFlags)
{
	NOTIMPLEMENTED;
}

HRESULT STDMETHODCALLTYPE Storage_Commit(IStorage FAR* self, DWORD grfCommitFlags)
{
	NOTIMPLEMENTED;
}

HRESULT STDMETHODCALLTYPE Storage_Revert(IStorage FAR* self)
{
	NOTIMPLEMENTED;
}

HRESULT STDMETHODCALLTYPE Storage_EnumElements(IStorage FAR* self, DWORD reserved1, void * reserved2, DWORD reserved3, IEnumSTATSTG ** ppenum)
{
	NOTIMPLEMENTED;
}

HRESULT STDMETHODCALLTYPE Storage_DestroyElement(IStorage FAR* self, const OLECHAR *pwcsName)
{
	NOTIMPLEMENTED;
}

HRESULT STDMETHODCALLTYPE Storage_RenameElement(IStorage FAR* self, const WCHAR *pwcsOldName, const WCHAR *pwcsNewName)
{
	NOTIMPLEMENTED;
}

HRESULT STDMETHODCALLTYPE Storage_SetElementTimes(IStorage FAR* self, const WCHAR *pwcsName, FILETIME const *pctime, FILETIME const *patime, FILETIME const *pmtime)
{
	NOTIMPLEMENTED;
}

HRESULT STDMETHODCALLTYPE Storage_SetClass(IStorage FAR* self, REFCLSID clsid)
{
	return(S_OK);
}

HRESULT STDMETHODCALLTYPE Storage_SetStateBits(IStorage FAR* self, DWORD grfStateBits, DWORD grfMask)
{
	NOTIMPLEMENTED;
}

HRESULT STDMETHODCALLTYPE Storage_Stat(IStorage FAR* self, STATSTG* pstatstg, DWORD grfStatFlag)
{
	NOTIMPLEMENTED;
}

HRESULT STDMETHODCALLTYPE Site_QueryInterface(IOleClientSite FAR* self, REFIID riid, void** ppvObject)
{

	if (!memcmp(riid, &IID_IUnknown, sizeof(GUID)) || !memcmp(riid, &IID_IOleClientSite, sizeof(GUID)))
		*ppvObject = self;
	else if (!memcmp(riid, &IID_IOleInPlaceSite, sizeof(GUID)))
		*ppvObject = &((IOleClientSiteExt*)self)->OleInPlaceSiteExt;
	else
	{
		*ppvObject = 0;
		return(E_NOINTERFACE);
	}

	return(S_OK);
}

HRESULT STDMETHODCALLTYPE Site_AddRef(IOleClientSite FAR* self)
{
	return(1);
}

HRESULT STDMETHODCALLTYPE Site_Release(IOleClientSite FAR* self)
{
	return(1);
}

HRESULT STDMETHODCALLTYPE Site_SaveObject(IOleClientSite FAR* self)
{
	NOTIMPLEMENTED;
}

HRESULT STDMETHODCALLTYPE Site_GetMoniker(IOleClientSite FAR* self, DWORD dwAssign, DWORD dwWhichMoniker, IMoniker ** ppmk)
{
	NOTIMPLEMENTED;
}

HRESULT STDMETHODCALLTYPE Site_GetContainer(IOleClientSite FAR* self, LPOLECONTAINER FAR* ppContainer)
{
	*ppContainer = 0;
	return(E_NOINTERFACE);
}

HRESULT STDMETHODCALLTYPE Site_ShowObject(IOleClientSite FAR* self)
{
	return(NOERROR);
}

HRESULT STDMETHODCALLTYPE Site_OnShowWindow(IOleClientSite FAR* self, BOOL fShow)
{
	NOTIMPLEMENTED;
}

HRESULT STDMETHODCALLTYPE Site_RequestNewObjectLayout(IOleClientSite FAR* self)
{
	NOTIMPLEMENTED;
}

HRESULT STDMETHODCALLTYPE Site_GetWindow(IOleInPlaceSite FAR* self, HWND FAR* lphwnd)
{
	*lphwnd = ((IOleInPlaceSiteExt FAR*)self)->OleInPlaceFrameExt.ox->hWin;
	return(S_OK);
}

HRESULT STDMETHODCALLTYPE Site_ContextSensitiveHelp(IOleInPlaceSite FAR* self, BOOL fEnterMode)
{
	NOTIMPLEMENTED;
}

HRESULT STDMETHODCALLTYPE Site_CanInPlaceActivate(IOleInPlaceSite FAR* self)
{
	return(S_OK);
}

HRESULT STDMETHODCALLTYPE Site_OnInPlaceActivate(IOleInPlaceSite FAR* self)
{
	return(S_OK);
}

HRESULT STDMETHODCALLTYPE Site_OnUIActivate(IOleInPlaceSite FAR* self)
{
	return(S_OK);
}

HRESULT STDMETHODCALLTYPE Site_GetWindowContext(IOleInPlaceSite FAR* self, LPOLEINPLACEFRAME FAR* lplpFrame, LPOLEINPLACEUIWINDOW FAR* lplpDoc, LPRECT lprcPosRect, LPRECT lprcClipRect, LPOLEINPLACEFRAMEINFO lpFrameInfo)
{
	*lplpFrame = (LPOLEINPLACEFRAME)&((IOleInPlaceSiteExt FAR*)self)->OleInPlaceFrameExt;
	*lplpDoc = 0;

	lpFrameInfo->fMDIApp = FALSE;
	lpFrameInfo->hwndFrame = ((IOleInPlaceFrameExt FAR*)*lplpFrame)->ox->hWin;
	lpFrameInfo->haccel = 0;
	lpFrameInfo->cAccelEntries = 0;

	GetClientRect(lpFrameInfo->hwndFrame, lprcPosRect);
	GetClientRect(lpFrameInfo->hwndFrame, lprcClipRect);

	return(S_OK);
}

HRESULT STDMETHODCALLTYPE Site_Scroll(IOleInPlaceSite FAR* self, SIZE scrollExtent)
{
	NOTIMPLEMENTED;
}

HRESULT STDMETHODCALLTYPE Site_OnUIDeactivate(IOleInPlaceSite FAR* self, BOOL fUndoable)
{
	return(S_OK);
}

HRESULT STDMETHODCALLTYPE Site_OnInPlaceDeactivate(IOleInPlaceSite FAR* self)
{
	return(S_OK);
}

HRESULT STDMETHODCALLTYPE Site_DiscardUndoState(IOleInPlaceSite FAR* self)
{
	NOTIMPLEMENTED;
}

HRESULT STDMETHODCALLTYPE Site_DeactivateAndUndo(IOleInPlaceSite FAR* self)
{
	NOTIMPLEMENTED;
}

HRESULT STDMETHODCALLTYPE Site_OnPosRectChange(IOleInPlaceSite FAR* self, LPCRECT lprcPosRect)
{
	return(S_OK);
}


HRESULT STDMETHODCALLTYPE Frame_QueryInterface(IOleInPlaceFrame FAR* self, REFIID riid, LPVOID FAR* ppvObj)
{
	NOTIMPLEMENTED;
}

HRESULT STDMETHODCALLTYPE Frame_AddRef(IOleInPlaceFrame FAR* self)
{
	return(1);
}

HRESULT STDMETHODCALLTYPE Frame_Release(IOleInPlaceFrame FAR* self)
{
	return(1);
}

HRESULT STDMETHODCALLTYPE Frame_GetWindow(IOleInPlaceFrame FAR* self, HWND FAR* lphwnd)
{
	*lphwnd = ((IOleInPlaceFrameExt FAR*)self)->ox->hWin;
	return(S_OK);
}

HRESULT STDMETHODCALLTYPE Frame_ContextSensitiveHelp(IOleInPlaceFrame FAR* self, BOOL fEnterMode)
{
	NOTIMPLEMENTED;
}

HRESULT STDMETHODCALLTYPE Frame_GetBorder(IOleInPlaceFrame FAR* self, LPRECT lprectBorder)
{
	NOTIMPLEMENTED;
}

HRESULT STDMETHODCALLTYPE Frame_RequestBorderSpace(IOleInPlaceFrame FAR* self, LPCBORDERWIDTHS pborderwidths)
{
	NOTIMPLEMENTED;
}

HRESULT STDMETHODCALLTYPE Frame_SetBorderSpace(IOleInPlaceFrame FAR* self, LPCBORDERWIDTHS pborderwidths)
{
	NOTIMPLEMENTED;
}

HRESULT STDMETHODCALLTYPE Frame_SetActiveObject(IOleInPlaceFrame FAR* self, IOleInPlaceActiveObject *pActiveObject, LPCOLESTR pszObjName)
{
	return(S_OK);
}

HRESULT STDMETHODCALLTYPE Frame_InsertMenus(IOleInPlaceFrame FAR* self, HMENU hmenuShared, LPOLEMENUGROUPWIDTHS lpMenuWidths)
{
	NOTIMPLEMENTED;
}

HRESULT STDMETHODCALLTYPE Frame_SetMenu(IOleInPlaceFrame FAR* self, HMENU hmenuShared, HOLEMENU holemenu, HWND hwndActiveObject)
{
	return(S_OK);
}

HRESULT STDMETHODCALLTYPE Frame_RemoveMenus(IOleInPlaceFrame FAR* self, HMENU hmenuShared)
{
	NOTIMPLEMENTED;
}

HRESULT STDMETHODCALLTYPE Frame_SetStatusText(IOleInPlaceFrame FAR* self, LPCOLESTR pszStatusText)
{
	return(S_OK);
}

HRESULT STDMETHODCALLTYPE Frame_EnableModeless(IOleInPlaceFrame FAR* self, BOOL fEnable)
{
	return(S_OK);
}

HRESULT STDMETHODCALLTYPE Frame_TranslateAccelerator(IOleInPlaceFrame FAR* self, LPMSG lpmsg, WORD wID)
{
	NOTIMPLEMENTED;
}

// main.c  | McViewer © 2016 by Thomas Führinger
#include <Oxygen.h>
#include "resource.h"
#include "DragAndDrop.h"
#include <fcntl.h>

//ToDo: https://cocoapods.org/pods/MuPDF

typedef struct _Globals {
	WCHAR szFileNamePath[MAX_PATH];
	LPWSTR szFileName;
	LPWSTR szFileExtension;
	char* sFileNamePath;
	OxRect rcClientArea;
	OxWidgetObject* oxClientWidget;
	PMYDROPTARGET pDropTarget;
	OxMenuItemObject* oxMenuItemReload;
	OxMenuItemObject* oxMenuItemProperties;
	HFONT hFont;
} Globals;

Globals g;

// forward declarations
static BOOL WindowBeforeDeleteCB(OxWindowObject* ox);
static BOOL FileOpenCB();
static BOOL FileReloadCB();
static BOOL AboutCB();
static void FileShow();
static BOOL  PropertiesCB();
static DWORD DropCB(CLIPFORMAT cf, HGLOBAL hdata, HWND hwnd, DWORD key_state, POINTL pt, void* param);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPTSTR lpCmdLine, _In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	OxImageObject* oxIcon;
	g.sFileNamePath = NULL;

	OxCALL(OxInit("McViewer"));            // show error if result of call is FALSE
	//OxASSIGN(oxIcon = OxImage_FromWindowsResource(IDI_APP));
	//OxAttachObject(&OxApp->oxIcon, oxIcon, FALSE);
	OxASSIGN(OxApp->oxIcon = OxImage_FromWindowsResource(IDI_APP));

	OxRect rc = { .iLeft = 40, .iTop = 30, .iWidth = 600, .iHeight = 400 };
	OxASSIGN(OxApp->oxWindow = OxWindow_New(NULL, &rc, "McViewer")); // show error if assigned value is NULL
	OxApp->oxWindow->iMinHeight = 400;
	OxApp->sStateID = "McViewer";
	OxCALL(OxApplication_RestoreState());
	//OxCALL(OxWindow_SetIcon(OxApp->oxWindow, oxIcon));

	OxToolBarObject* oxToolBar = OxToolBar_New((OxWidgetObject*)OxApp->oxWindow);
	int iTB = OxToolBar_GetHeight(oxToolBar);

	OxMenuObject* oxMainMenu;
	OxMenuItemObject* oxMenuItem;
	OxASSIGN(oxMainMenu = OxMenu_New("Main Menu"));

	OxMenuObject* oxFileMenu = OxMenu_New("File");
	OxCALL(OxMenu_AppendItem(oxMainMenu, (OxObject*)oxFileMenu));
	OxASSIGN(oxIcon = OxImage_FromStock(OxICON_FILEOPEN));

	OxASSIGN(oxMenuItem = OxMenuItem_New("Open...", FileOpenCB, oxIcon));
	OxCALL(OxMenu_AppendItem(oxFileMenu, (OxObject*)oxMenuItem));
	OxCALL(OxToolBar_AppendItem(oxToolBar, oxMenuItem));
	OxASSIGN(oxIcon = OxImage_FromStock(OxICON_REFRESH));

	OxASSIGN(oxMenuItem = OxMenuItem_New("Reload", FileReloadCB, oxIcon));
	OxCALL(OxMenu_AppendItem(oxFileMenu, (OxObject*)oxMenuItem));
	OxCALL(OxToolBar_AppendItem(oxToolBar, oxMenuItem));
	OxMenuItem_SetEnabled(oxMenuItem, FALSE);
	g.oxMenuItemReload = oxMenuItem;

	OxASSIGN(oxMenuItem = OxMenuItem_New("Properties...", PropertiesCB, NULL));
	OxCALL(OxMenu_AppendItem(oxFileMenu, (OxObject*)oxMenuItem));
	OxMenuItem_SetEnabled(oxMenuItem, FALSE);
	g.oxMenuItemProperties = oxMenuItem;

	OxMenuObject* oxHelpMenu = OxMenu_New("Help");
	OxCALL(OxMenu_AppendItem(oxMainMenu, (OxObject*)oxHelpMenu));
	OxASSIGN(oxMenuItem = OxMenuItem_New("About...", AboutCB, oxIcon));
	OxCALL(OxMenu_AppendItem(oxHelpMenu, (OxObject*)oxMenuItem));

	OxCALL(OxWindow_SetMenu(OxApp->oxWindow, oxMainMenu));

	int iParts[] = { 0 };
	OxApp->oxWindow->oxStatusBar = OxStatusBar_New((OxWidgetObject*)OxApp->oxWindow, iParts);
	int iSB = OxStatusBar_GetHeight(OxApp->oxWindow->oxStatusBar);

	g.oxClientWidget = NULL;
	OxWindowClass.aMethods[OxWIDGET_BEFOREDELETE] = WindowBeforeDeleteCB;        // subclassing 'light'

	g.rcClientArea = (OxRect){ .iLeft = 0, .iTop = iTB, .iWidth = 0, .iHeight = -iSB };

	MyDragDropInit(NULL);
	CLIPFORMAT cf_list[1] = { CF_HDROP };
	g.pDropTarget = MyRegisterDragDrop(OxApp->oxWindow->hWin, cf_list, 1, WM_NULL, DropCB, NULL);

	g.hFont = CreateFont(14, 0, 0, 0, FW_REGULAR, 0, 0, 0, DEFAULT_CHARSET, OUT_TT_PRECIS, CLIP_LH_ANGLES, ANTIALIASED_QUALITY, FF_MODERN, L"Consolas");
	if (!g.hFont)
		g.hFont = GetStockObject(ANSI_FIXED_FONT);

	if (__argc == 2) {
		StringCchCopyW(g.szFileNamePath, MAX_PATH, __targv[1]);
		FileShow();
	}
	return OxApplication_Run();
}

// www.codeproject.com/Articles/840/How-to-Implement-Drag-and-Drop-Between-Your-Progra
static DWORD
DropCB(CLIPFORMAT cf, HGLOBAL hdata, HWND hwnd, DWORD key_state, POINTL pt, void* param)
{
	HDROP   hdrop;
	UINT  uNumFiles;

	if (CF_UNICODETEXT == cf)
	{
		void* data = GlobalLock(hdata);
		MessageBox(hwnd, (LPCTSTR)data, L"Dropped Text", MB_OK);
		GlobalUnlock(hdata);
		return DROPEFFECT_COPY;
	}

	if (CF_HDROP == cf)
	{
		hdrop = (HDROP)GlobalLock(hdata);
		uNumFiles = DragQueryFile(hdata, -1, NULL, 0);
		if (uNumFiles > 0 && DragQueryFile(hdrop, 0, g.szFileNamePath, MAX_PATH) > 0) // use the first one
			FileShow();

		DragFinish(hdata);
		GlobalUnlock(hdata);
		return DROPEFFECT_COPY;
	}

	return DROPEFFECT_NONE;
}

static BOOL
FileOpenCB()
{
	static OPENFILENAME ofn;
	static TCHAR szFilter[] = L"All Formats (*.html, *.bmp, *.ico, *.avi, *.txt)\0*.html;*.md;*.bmp;*.ico;*.avi;*.mpg;*.wmv;*.txt\0" \
		L"HTML (*.html)\0*.html\0" \
		L"Mark Down (*.md)\0*.md\0" \
		L"Plain Text (*.txt)\0*.txt\0" \
		L"Image (*.bmp, *.ico)\0*.bmp;*.ico\0" \
		L"Video (*.avi, *.mpg, *.wmv)\0*.avi;*.mpg;*.wmv;*.mp4\0\0";

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = OxApp->oxWindow->hWin;
	ofn.hInstance = NULL;
	ofn.lpstrFilter = szFilter;
	ofn.lpstrCustomFilter = NULL;
	ofn.nMaxCustFilter = 0;
	ofn.nFilterIndex = 0;
	ofn.lpstrFile = g.szFileNamePath;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = MAX_PATH;
	ofn.lpstrInitialDir = NULL;
	ofn.lpstrTitle = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	ofn.nFileOffset = 0;
	ofn.nFileExtension = 0;
	ofn.lpstrDefExt = L"md";
	ofn.lCustData = 0L;
	ofn.lpfnHook = NULL;
	ofn.lpTemplateName = NULL;

	if (GetOpenFileNameW(&ofn)) {
		FileShow();
	}
	return TRUE;
}

static BOOL
FileReloadCB()
{
	FileShow();
	return TRUE;
}

static void
FileShow()
{
	g.szFileExtension = PathFindExtensionW(g.szFileNamePath);
	g.szFileName = PathFindFileNameW(g.szFileNamePath);
	OxFree(g.sFileNamePath);
	g.sFileNamePath = OxToU8(g.szFileNamePath);

	if (_wcsicmp(g.szFileExtension, L".bmp") == 0 || _wcsicmp(g.szFileExtension, L".ico") == 0) {
		OxImageObject* oxImage;
		OxImageViewObject* oxView;
		oxImage = OxImage_FromFile(g.sFileNamePath);
		if (oxImage) {
			OxASSIGN(oxView = OxImageView_New((OxWidgetObject*)OxApp->oxWindow, &g.rcClientArea, oxImage));
			OxAttachObject(&g.oxClientWidget, (OxWidgetObject*)oxImage, FALSE);
		}
		else
			OxApplication_ShowError();
	}
	else if (_wcsicmp(g.szFileExtension, L".avi") == 0 || _wcsicmp(g.szFileExtension, L".mp4") == 0 || _wcsicmp(g.szFileExtension, L".mpg") == 0 || _wcsicmp(g.szFileExtension, L".wmv") == 0 || _wcsicmp(g.szFileExtension, L".mov") == 0) {
		OxVideoViewObject* oxView;
		oxView = OxVideoView_New((OxWidgetObject*)OxApp->oxWindow, &g.rcClientArea);
		if (oxView) {
			OxCALL(OxVideoView_RenderFile(oxView, g.sFileNamePath));
			OxAttachObject(&g.oxClientWidget, (OxWidgetObject*)oxView, FALSE);
		}
		else
			OxApplication_ShowError();
	}
	else if (_wcsicmp(g.szFileExtension, L".txt") == 0 || _wcsicmp(g.szFileExtension, L".bat") == 0 || _wcsicmp(g.szFileExtension, L".c") == 0 || _wcsicmp(g.szFileExtension, L".h") == 0 || _wcsicmp(g.szFileExtension, L".py") == 0 || _wcsicmp(g.szFileExtension, L".log") == 0) {
		OxHtmlViewObject* oxView;
		oxView = OxLabel_New((OxWidgetObject*)OxApp->oxWindow, &g.rcClientArea);
		//OxASSIGN(oxView = OxEntry_New((OxWidgetObject*)OxApp->oxWindow, &g.rcClientArea));
		if (oxView) {
			//SetWindowLongPtr(oxView, GWL_STYLE, ES_MULTILINE);
			SendMessageW(oxView->hWin, WM_SETFONT, g.hFont, MAKELPARAM(FALSE, 0));
			_set_fmode(_O_TEXT); // _O_UTF8
			char* sText = OxLoadFile(g.sFileNamePath);
			_set_fmode(_O_BINARY);
			OxCALL(OxWidget_SetDataV(oxView, OxString_FromString(sText, NULL)));
			OxFree(sText);
			OxAttachObject(&g.oxClientWidget, (OxWidgetObject*)oxView, FALSE);
		}
		else
			OxApplication_ShowError();
	}
	else if (_wcsicmp(g.szFileExtension, L".html") == 0 || _wcsicmp(g.szFileExtension, L".htm") == 0) {
		OxHtmlViewObject* oxView;
		oxView = OxHtmlView_New((OxWidgetObject*)OxApp->oxWindow, &g.rcClientArea);
		if (oxView) {
			OxCALL(OxHtmlView_RenderURL(oxView, g.sFileNamePath));
			OxAttachObject(&g.oxClientWidget, (OxWidgetObject*)oxView, FALSE);
		}
		else
			OxApplication_ShowError();
	}
	else if (_wcsicmp(g.szFileExtension, L".md") == 0) {
		OxMarkDownEntryObject* oxView;
		oxView = OxMarkDownEntry_New((OxWidgetObject*)OxApp->oxWindow, &g.rcClientArea);
		if (oxView) {
			char* sMD;
			OxASSIGN(sMD = OxLoadFile(g.sFileNamePath));
			OxCALL(OxWidget_SetDataV(oxView, OxString_FromString(sMD, NULL)));
			OxFree(sMD);
			OxAttachObject(&g.oxClientWidget, (OxWidgetObject*)oxView, FALSE);
		}
		else
			OxApplication_ShowError();
	}
	else {
		OxStatusBar_Message(OxApp->oxWindow->oxStatusBar, "File format not supported.", 0);
		return;
	}

	SetWindowLong(g.oxClientWidget->hWin, GWL_EXSTYLE, WS_EX_CLIENTEDGE);
	SetWindowPos(g.oxClientWidget->hWin, 0, 0, 0, 0, 0, SWP_DRAWFRAME | SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOZORDER);

	TCHAR szFileName[MAX_PATH + 13];
	StringCchCopyW(szFileName, 12, L"McViewer - ");
	StringCchCatW(szFileName, MAX_PATH + 13, g.szFileName);
	SetWindowText(OxApp->oxWindow->hWin, szFileName);

	OxMenuItem_SetEnabled(g.oxMenuItemReload, TRUE);
	OxMenuItem_SetEnabled(g.oxMenuItemProperties, TRUE);
	OxStatusBar_Message(OxApp->oxWindow->oxStatusBar, "Ready", 0);
}

static BOOL
PropertiesCB()
{
	OxRect rc = { .iLeft = OxWIDGET_CENTER, .iTop = OxWIDGET_CENTER, .iWidth = 430, .iHeight = 240 };
	OxWindowObject* oxWindow = OxWindow_New(OxApp->oxWindow, &rc, "Properties");
	oxWindow->iMaxWidth = 430; oxWindow->iMaxHeight = 240;

	rc = (OxRect){ .iLeft = 20, .iTop = 20, .iWidth = 60, .iHeight = 20 };
	OxLabelObject* oxLabelFile = OxLabel_New((OxWidgetObject*)oxWindow, &rc, "File");

	rc = (OxRect){ .iLeft = 80, .iTop = 20, .iWidth = -20, .iHeight = 20 };
	OxLabelObject* oxLabelFileData = OxLabel_New((OxWidgetObject*)oxWindow, &rc, g.sFileNamePath);

	OxWindow_ShowModal(oxWindow);
	//OxREL(oxLabelFile);
	//OxREL(oxLabelFileData);
	OxREL(oxWindow);
	return TRUE;
}

static BOOL
AboutCB()
{
	OxRect rc = { .iLeft = OxWIDGET_CENTER, .iTop = OxWIDGET_CENTER, .iWidth = 430, .iHeight = 280 };
	OxWindowObject* oxWindow = OxWindow_New(OxApp->oxWindow, &rc, "About");
	oxWindow->iMaxWidth = 430; oxWindow->iMaxHeight = 280;

	rc = (OxRect){ .iLeft = 30, .iTop = 30, .iWidth = 40, .iHeight = 40 };
	OxImageViewObject* oxImageView = OxImageView_New((OxWidgetObject*)oxWindow, &rc, OxApp->oxIcon);

	rc = (OxRect){ .iLeft = 90, .iTop = 30, .iWidth = -20, .iHeight = -20 };
	OxLabelObject* oxLabel = OxLabel_New((OxWidgetObject*)oxWindow, &rc,
		"McViewer\n"\
		"Version 0.9.1\n\n"\
		"Simple file viewer\nFormats: \n"\
		"BMP, ICO, AVI, MPG, WMV, HTML, MD, TXT\n\n"\
		"https://github.com/thomasfuhringer/oxygen\n\n"\
		"Thomas Führinger, 2016");
	OxWindow_ShowModal(oxWindow);
	OxREL(oxWindow);
	return TRUE;
}

static BOOL
WindowBeforeDeleteCB(OxWindowObject* ox) // exit gracefully
{
	if (ox == OxApp->oxWindow) {
		OxREL(g.oxClientWidget);
		MyRevokeDragDrop(g.pDropTarget);
		OxFree(g.sFileNamePath);
		DeleteObject(g.hFont);
		return OxExit();
	}
	return TRUE;
}

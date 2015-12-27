// main.c  | Oxygen test program © 2015 by Thomas Führinger
#include "Oxygen.h"

typedef struct _ClientWindow {
	OxTabObject* oxTab;
	OxTabPageObject* oxTabPageDivision;
	OxEntryObject* oxEntryDividend;
	OxEntryObject* oxEntryDivisor;
	OxLabelObject* oxLabelQuotient;
	OxButtonObject* oxButtonDivide;
	OxTabPageObject* oxTabPageMultiplication;
	OxTabPageObject* oxTabPageVideo;
	OxVideoViewObject* oxVideoView;
	OxEntryObject* oxEntryFile;
	OxButtonObject* oxButtonPlay;
	OxSplitterObject* oxSplitter;
	OxLabelObject* oxLabelMultiplication;
} ClientWindow;

// forward declarations
static BOOL WindowBeforeDeleteCB(OxWindowObject* ox);
static BOOL ButtonDivideCB(OxButtonObject* ox);
static BOOL ButtonPlayCB(OxButtonObject* ox);
static BOOL DivisorVerifyCB(OxEntryObject* ox);
static BOOL AboutCB();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPTSTR lpCmdLine, _In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	OxCALL(OxInit("HOx"));            // show error if result of call is FALSE

	OxRect rc = { .iLeft = 40, .iTop = 30, .iWidth = 600, .iHeight = 400 };
	OxASSIGN(OxApp->oxWindow = OxWindow_New(NULL, &rc, "Hello Oxygen")); // show error if assigned value is NULL
	OxApp->oxWindow->iMinHeight = 400;
	OxApp->sStateID = "HelloOxygen";
	OxCALL(OxApplication_RestoreState());

	OxMenuObject* oxMainMenu;
	OxASSIGN(oxMainMenu = OxMenu_New("Main Menu"));
	OxMenuObject* oxMdiWindowsMenu = OxMenu_New("Windows");
	OxMenuObject* oxHelpMenu = OxMenu_New("Help");
	OxMenuItemObject* oxMenuItem;
	OxImageObject* oxIcon = OxImage_FromFile("Example.ico");// = OxIcon_FromStock(OxICON_OXYGEN);
	OxASSIGN(oxMenuItem = OxMenuItem_New("About...", AboutCB, oxIcon));

	OxCALL(OxMenu_AppendItem(oxMainMenu, (OxObject*)oxMdiWindowsMenu));
	OxCALL(OxMenu_AppendItem(oxMainMenu, (OxObject*)oxHelpMenu));
	OxCALL(OxMenu_AppendItem(oxHelpMenu, (OxObject*)oxMenuItem));
	OxCALL(OxWindow_SetMenu(OxApp->oxWindow, oxMainMenu));

	OxToolBarObject* oxToolBar = OxToolBar_New((OxWidgetObject*)OxApp->oxWindow);
	//XX(oxToolBar);
	OxCALL(OxToolBar_AppendItem(oxToolBar, oxMenuItem));
	int iTB = OxToolBar_GetHeight(oxToolBar);

	int iParts[] = { 120, -50, 0 };               // three parts: separations at 120 from left, 50 from right, 0 as sentinel
	OxStatusBarObject* oxStatusBar = OxStatusBar_New((OxWidgetObject*)OxApp->oxWindow, iParts);
	OxStatusBar_Message(oxStatusBar, "Ready", 0);
	int iSB = OxStatusBar_GetHeight(oxStatusBar);

	rc = (OxRect){ .iLeft = 0, .iTop = iTB, .iWidth = 0, .iHeight = -iSB };
	OxMdiAreaObject* oxMdiArea = OxMdiArea_New((OxWidgetObject*)OxApp->oxWindow, &rc);
	OxMdiArea_SetMenu(oxMdiArea, oxMdiWindowsMenu);

	rc = (OxRect){ .iLeft = 10, .iTop = 10, .iWidth = 620, .iHeight = 440 };
	OxMdiWindowObject* oxMdiWindow = OxMdiWindow_New(oxMdiArea, &rc, "Calculator"); // "Hello  世界, こんにちは!"
	oxMdiWindow->iMinHeight = 400;
	ClientWindow* pMW = (ClientWindow*)OxAllocate(sizeof(ClientWindow));
	oxMdiWindow->pUserData = pMW;
	OxWindowClass.aMethods[OxWIDGET_BEFOREDELETE] = WindowBeforeDeleteCB;        // subclassing 'light'

	//OxIconObject* oxExampleIcon = OxIcon_FromWindowsResource(IDI_APP);
	OxImageObject* oxExampleIcon = OxImage_FromFile("Example.ico");
	//OxCALL(OxWindow_SetIcon(oxMdiWindow, oxExampleIcon));

	rc = (OxRect){ .iLeft = 20, .iTop = 50, .iWidth = -20, .iHeight = -70 };
	OxASSIGN(pMW->oxTab = OxTab_New((OxWidgetObject*)oxMdiWindow, &rc));
	OxASSIGN(pMW->oxTabPageDivision = OxTabPage_New(pMW->oxTab, "Division", -1));


	rc = (OxRect){ .iLeft = 80, .iTop = 20, .iWidth = -20, .iHeight = 20 };
	OxASSIGN(pMW->oxEntryDividend = OxEntry_New((OxWidgetObject*)pMW->oxTabPageDivision, &rc));
	pMW->oxEntryDividend->pDataType = &OxIntegerClass;
	rc.iLeft -= 60; rc.iWidth = 50;
	pMW->oxEntryDividend->oxLabel = OxLabel_New((OxWidgetObject*)pMW->oxTabPageDivision, &rc, "Dividend");

	rc = (OxRect){ .iLeft = 80, .iTop = 50, .iWidth = -20, .iHeight = 20 };
	OxASSIGN(pMW->oxEntryDivisor = OxEntry_New((OxWidgetObject*)pMW->oxTabPageDivision, &rc));
	pMW->oxEntryDivisor->fnVerifyCB = DivisorVerifyCB;
	pMW->oxEntryDivisor->pDataType = &OxIntegerClass;
	rc.iLeft -= 60; rc.iWidth = 50;
	pMW->oxEntryDividend->oxLabel = OxLabel_New((OxWidgetObject*)pMW->oxTabPageDivision, &rc, "Divisor");

	rc = (OxRect){ .iLeft = 80, .iTop = 100, .iWidth = -20, .iHeight = 20 };
	OxASSIGN(pMW->oxLabelQuotient = OxLabel_New((OxWidgetObject*)pMW->oxTabPageDivision, &rc));
	pMW->oxLabelQuotient->pDataType = &OxFloatClass;
	rc.iLeft -= 60; rc.iWidth = 50;
	pMW->oxLabelQuotient->oxLabel = OxLabel_New((OxWidgetObject*)pMW->oxTabPageDivision, &rc, "Quotient");
	//OxLabel_SetTextColor(pMW->oxLabelQuotient->oxLabel, 0, 0, 250);

	rc = (OxRect){ .iLeft = -110, .iTop = -40, .iWidth = 90, .iHeight = 20 };
	OxASSIGN(pMW->oxButtonDivide = OxButton_New((OxWidgetObject*)pMW->oxTabPageDivision, &rc, "Divide", ButtonDivideCB));

	OxASSIGN(pMW->oxTabPageMultiplication = OxTabPage_New(pMW->oxTab, "Multiplication", -1));
	rc = (OxRect){ .iLeft = 30, .iTop = OxWIDGET_CENTER, .iWidth = -30, .iHeight = 20 };
	pMW->oxLabelMultiplication = OxLabel_New((OxWidgetObject*)pMW->oxTabPageMultiplication, &rc, "Under Construction");
	OxLabel_Align(pMW->oxLabelMultiplication, OxALIGN_HORIZ_CENTER);

	OxASSIGN(pMW->oxTabPageVideo = OxTabPage_New(pMW->oxTab, "Views", -1));
	rc = (OxRect){ .iLeft = 5, .iTop = 5, .iWidth = -5, .iHeight = -5 };
	OxASSIGN(pMW->oxSplitter = OxSplitter_New((OxWidgetObject*)pMW->oxTabPageVideo, &rc, -350));

	OxImageObject* oxExampleImage = OxImage_FromFile("Example.bmp");
	//OxImageObject* oxExampleImage = OxImage_FromFile("Example.ico");
	rc = (OxRect){ .iLeft = 5, .iTop = 5, .iWidth = -5, .iHeight = -5 };
	OxImageViewObject* oxImageView = OxImageView_New(pMW->oxSplitter->oxBox1, &rc, oxExampleImage);
	//oxImageView->bFill = TRUE;
	//oxImageView->bStretch = FALSE;

	rc = (OxRect){ .iLeft = 10, .iTop = 10, .iWidth = -10, .iHeight = -40 };
	OxASSIGN(pMW->oxVideoView = OxVideoView_New((OxWidgetObject*)pMW->oxSplitter->oxBox2, &rc, -100));
	rc = (OxRect){ .iLeft = 10, .iTop = -30, .iWidth = -110, .iHeight = 20 };
	OxASSIGN(pMW->oxEntryFile = OxEntry_New((OxWidgetObject*)pMW->oxSplitter->oxBox2, &rc));
	OxCALL(OxWidget_SetDataV(pMW->oxEntryFile, OxString_FromString("C:\\PathTo\\Test.avi", NULL)));
	rc = (OxRect){ .iLeft = -100, .iTop = -30, .iWidth = 90, .iHeight = 20 };
	OxASSIGN(pMW->oxButtonPlay = OxButton_New((OxWidgetObject*)pMW->oxSplitter->oxBox2, &rc, "Play", ButtonPlayCB));

	return OxApplication_Run();
}

static BOOL
DivisorVerifyCB(OxEntryObject* ox)
{
	OxIntegerObject* oxDivisor = OxEntry_GetInputData(ox);
	if (oxDivisor && oxDivisor != OxNull && oxDivisor->iInteger == 0) {
		MessageBox(0, L"Division by zero not possible.", L"Error", MB_ICONERROR | MB_OK);
		return FALSE;
	}
	return TRUE;
}

static BOOL
ButtonDivideCB(OxButtonObject* ox)
{
	if (ox->oxWindow->oxFocusWidget) // validation callback not passed
		return TRUE;

	ClientWindow* pMW = (ClientWindow*)ox->oxWindow->pUserData;

	if (pMW->oxEntryDividend->oxData != OxNull && pMW->oxEntryDivisor->oxData != OxNull) {
		double dQuotient = ((OxIntegerObject*)pMW->oxEntryDividend->oxData)->iInteger / (double)((OxIntegerObject*)pMW->oxEntryDivisor->oxData)->iInteger;
		OxFloatObject* oxQuotient = OxFloat_New(dQuotient);

		OxCALL(OxLabel_SetData(pMW->oxLabelQuotient, oxQuotient));
	}
	else
		MessageBox(0, L"Division not possible.", L"Error", MB_ICONERROR | MB_OK);

	return TRUE;
}

static BOOL
ButtonPlayCB(OxButtonObject* ox)
{
	ClientWindow* pMW = (ClientWindow*)ox->oxWindow->pUserData;
	OxCALL(OxVideoView_RenderFile(pMW->oxVideoView, ((OxStringObject*)pMW->oxEntryFile->oxData)->sString));
	return TRUE;
}

static BOOL
AboutCB()
{
	OxRect rc = { .iLeft = OxWIDGET_CENTER, .iTop = OxWIDGET_CENTER, .iWidth = 320, .iHeight = 240 };
	OxWindowObject* oxWindow = OxWindow_New(OxApp->oxWindow, &rc, "About Hello Oxygen");
	oxWindow->iMaxWidth = 320; oxWindow->iMaxHeight = 240;

	rc = (OxRect){ .iLeft = 40, .iTop = 40, .iWidth = -40, .iHeight = -40 };
	OxLabelObject* oxLabel = OxLabel_New((OxWidgetObject*)oxWindow, &rc, "Demonstration program for\nOxygen C library\n\nThomas Führinger, 2015");
	OxWindow_ShowModal(oxWindow);
	OxREL(oxLabel);
	OxREL(oxWindow);
	return TRUE;
}

static BOOL
WindowBeforeDeleteCB(OxWindowObject* ox) // exit gracefully
{
	if (ox->pClass == &OxMdiWindowClass) {
		ClientWindow* pMW = (ClientWindow*)ox->pUserData;

		OxREL(pMW->oxLabelMultiplication);
		OxREL(pMW->oxSplitter);
		OxREL(pMW->oxTabPageMultiplication);

		OxREL(pMW->oxButtonDivide);
		OxREL(pMW->oxLabelQuotient);
		OxREL(pMW->oxEntryDividend);
		OxREL(pMW->oxEntryDivisor);
		OxREL(pMW->oxTabPageDivision);

		OxREL(pMW->oxButtonPlay);
		OxREL(pMW->oxEntryFile);
		OxREL(pMW->oxVideoView);
		OxREL(pMW->oxTabPageVideo);
		OxREL(pMW->oxTab);
		OxFree(pMW);

		return OxExit();
	}
	return TRUE;
}
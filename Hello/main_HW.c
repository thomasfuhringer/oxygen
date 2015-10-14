// main.c  | Oxygen test program © 2015 by Thomas Führinger
#include "Oxygen.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPTSTR lpCmdLine, _In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	OxInit("Hello");

	OxRect rc = { .iLeft = 40, .iTop = 30, .iWidth = 320, .iHeight = 240 };
	OxApp->oxWindow = OxWindow_New(NULL, &rc, "Powered by Oxygen");

	rc = (OxRect){ .iLeft = 20, .iTop = OxWIDGET_CENTER, .iWidth = -20, .iHeight = 20 };
	OxLabelObject* oxLabel = OxLabel_New(OxApp->oxWindow, &rc, "Hello World!");
	OxLabel_Align(oxLabel, OxALIGN_HORIZ_CENTER);

	OxWidget_ShowV(OxApp->oxWindow, TRUE);
	return OxApplication_Run();
}
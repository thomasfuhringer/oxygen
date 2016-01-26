// main.c  | Hello World © 2016 by Thomas Führinger
#include <Oxygen.h>

int
main(int argc, char *argv[])
{
	OxCALL(OxInit("Hello"));
	OxRect rc = { .iLeft = 40, .iTop = 30, .iWidth = 320, .iHeight = 240 };
	OxApp->oxWindow = OxWindow_New(NULL, &rc, "Powered by Oxygen");

	rc = (OxRect){ .iLeft = 20, .iTop = OxWIDGET_CENTER, .iWidth = -20, .iHeight = 20 };
	OxLabelObject* oxLabel = OxLabel_New(OxApp->oxWindow, &rc, "Hello World!");
	OxLabel_Align(oxLabel, OxALIGN_HORIZ_CENTER);

	OxWidget_ShowV(OxApp->oxWindow, TRUE);
	return OxApplication_Run();
}

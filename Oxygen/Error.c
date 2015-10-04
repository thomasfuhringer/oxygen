// Error.c  | Oxygen © 2015 by Thomas Führinger
#include "Oxygen.h"

OxError OxLastError = { OxERROR_NO, '\0' };

char* OxErrorType[] = {
	"No",
	"Runtime",
	"Type",
	"Parameter",
	"Windows"
};

void
OxErr_SetString(int iType, char* sMessage)
{
	OxLastError.iType = iType;
	//OxLastError.sMessage = sMessage;
	memcpy(OxLastError.sMessage, sMessage, OxERRMSGMAXBYTES);
}

void
OxErr_SetStringFormat(int iType, const char *sFormat, ...)
{
	va_list pArgs;
	va_start(pArgs, sFormat);
	StringCchVPrintfA(OxLastError.sMessage, OxERRMSGMAXBYTES, sFormat, pArgs);
	va_end(pArgs);

	OxLastError.iType = iType;
}

void
OxErr_Clear()
{
	OxLastError.iType = OxERROR_NO;
	OxLastError.sMessage[0] = '\0';
	StringCchCopyA(OxLastError.sMessage, OxERRMSGMAXBYTES, "No error");
}

void
OxErr_SetFromWindows()
{
	TCHAR sMessage[255];
	FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, 0, GetLastError(), 0, (LPSTR)sMessage, sizeof(sMessage) / sizeof(TCHAR), 0);
	OxErr_SetString(OxERROR_WINDOWS, sMessage);
	MessageBoxA(0, sMessage, "MS Windows Error", 0);
}
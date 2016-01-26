// Error.c  | Oxygen © 2016 by Thomas Führinger
#include "Oxygen.h"

OxError OxLastError = { OxERROR_NO, '\0' };

char* OxErrorType[] = {
	"No",
	"Runtime",
	"Type",
	"Parameter",
	"GTK"
};

void
OxErr_SetString(int iType, char* sMessage)
{
	OxLastError.iType = iType;
	memcpy(OxLastError.sMessage, sMessage, OxERRMSGMAXBYTES);
}

void
OxErr_SetStringFormat(int iType, const char *sFormat, ...)
{
	va_list pArgs;
	va_start(pArgs, sFormat);
	sprintf(OxLastError.sMessage, sFormat, pArgs);
	va_end(pArgs);

	OxLastError.iType = iType;
}

void
OxErr_Clear()
{
	OxLastError.iType = OxERROR_NO;
	OxLastError.sMessage[0] = '\0';
	strcpy(OxLastError.sMessage, "No error");
}


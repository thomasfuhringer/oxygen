// Oxygen.h  | Oxygen © 2015 by Thomas Führinger
#ifndef OxERROR_H
#define OxERROR_H

#define OxERRMSGMAXBYTES    250

enum
{
    OxERROR_NO,
    OxERROR_RUNTIME,
    OxERROR_TYPE,
    OxERROR_PARAMETER,
    OxERROR_WINDOWS
} OxErrorTypeEnum;

typedef struct _OxError {
	int iType;
	char sMessage[OxERRMSGMAXBYTES];
} OxError;

extern OxError OxLastError;
extern char* OxErrorType[];

OxAPI void OxErr_SetString(int iType, char* sMessage);
OxAPI void OxErr_SetStringFormat(int iType, const char *sFormat, ...);
OxAPI void OxErr_SetFromWindows();
OxAPI void OxErr_SetFromWindowsHR(HRESULT hr);
OxAPI void OxErr_Clear();
#endif /* OxERROR_H */

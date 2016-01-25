// ApplicationObject.c  | Oxygen © 2015 by Thomas Führinger

#include "Oxygen.h"
#include "Resource.h"

static OxClass* pOxClass;
OxApplicationObject* OxApp; // global singleton

OxApplicationObject*
OxApplication_New(char* sName)
{
	OxApplicationObject* ox = (OxApplicationObject*)OxObject_Allocate(&OxApplicationClass);
	if (ox != NULL) {
		ox->sName = OxDuplicateString(sName);
		ox->hInstance = GetModuleHandle(NULL);
		ox->hDLL = LoadLibraryA(OxDLLFILE);
		if (ox->hDLL == 0) {
			ox->hDLL = ox->hInstance; // statically linked...
		}
		ox->hDLLcomctl32 = GetModuleHandleA("COMCTL32.DLL");
		if (ox->hDLLcomctl32 == 0) {
			OxErr_SetFromWindows();
			return NULL;
		}
#ifndef OxSTATIC
		ox->hIcon = LoadIconW(ox->hDLL, MAKEINTRESOURCE(IDI_OXYGEN));
#else
		ox->hIcon = ExtractIconW(ox->hInstance, L"SHELL32.DLL", -3);
#endif
		if (ox->hIcon == 0) {
			OxErr_SetFromWindows();
			return NULL;
		}
		ox->oxWindow = NULL;
		ox->oxIcon = NULL;
		ox->sStateID = NULL;
		ox->hDefaultFont = GetStockObject(DEFAULT_GUI_FONT);
		return ox;
	}
	else
		return NULL;
}

int
OxApplication_Run()
{
	MSG Msg;
	HACCEL hAccelTable;

	if (OxApp->oxWindow) {
		ShowWindow(OxApp->oxWindow->hWin, SW_SHOW);
		UpdateWindow(OxApp->oxWindow->hWin);
	}

	hAccelTable = LoadAccelerators(OxApp->hInstance, MAKEINTRESOURCE(IDS_APP));

	while (GetMessage(&Msg, NULL, 0, 0) > 0)
	{
		if (!TranslateAccelerator(Msg.hwnd, hAccelTable, &Msg))
		{
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		}
	}
	return (int)Msg.wParam;
}

BOOL
OxApplication_ShowError()
{
	//if (OxLastError.sMessage == NULL)
	//	MessageBox(0, L"Error Message not set.", L"Error", MB_ICONERROR | MB_OK);
	LPWSTR szMessage = OxToW(OxLastError.sMessage);
	char sType[256];
	sprintf_s(sType, sizeof(sType), "%s error", OxErrorType[OxLastError.iType]);
	LPWSTR szType = OxToW(sType);
	MessageBox(0, szMessage, szType, MB_ICONERROR | MB_OK);
	OxFree(szMessage);
	OxFree(szType);
	return TRUE;
}

BOOL
OxApplication_SaveState()
{
	HKEY hKey;
	WINDOWPLACEMENT wp;
	TCHAR szSubkey[256];
	LPWSTR sID = OxToW(OxApp->sStateID);
	lstrcpy(szSubkey, L"Software\\");
	lstrcat(szSubkey, sID);
	OxFree(sID);
	if (RegCreateKeyEx(HKEY_CURRENT_USER, szSubkey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, NULL, &hKey, NULL) != ERROR_SUCCESS) {
		OxErr_SetFromWindows();
		return FALSE;
	}

	wp.length = sizeof(WINDOWPLACEMENT);
	GetWindowPlacement(OxApp->oxWindow->hWin, &wp);

	if ((RegSetValueEx(hKey, L"flags", 0, REG_BINARY, (PBYTE)&wp.flags, sizeof(wp.flags)) != ERROR_SUCCESS) ||
		(RegSetValueEx(hKey, L"showCmd", 0, REG_BINARY, (PBYTE)&wp.showCmd, sizeof(wp.showCmd)) != ERROR_SUCCESS) ||
		(RegSetValueEx(hKey, L"rcNormalPosition", 0, REG_BINARY, (PBYTE)&wp.rcNormalPosition, sizeof(wp.rcNormalPosition)) != ERROR_SUCCESS))
	{
		OxErr_SetFromWindows();
		RegCloseKey(hKey);
		return FALSE;
	}

	RegCloseKey(hKey);
	return TRUE;
}

BOOL
OxApplication_RestoreState()
{
	HKEY hKey;
	DWORD dwSizeFlags, dwSizeShowCmd, dwSizeRcNormal;
	WINDOWPLACEMENT wp;
	TCHAR szSubkey[256];
	LPWSTR szID = OxToW(OxApp->sStateID);
	lstrcpy(szSubkey, L"Software\\");
	lstrcat(szSubkey, szID);
	OxFree(szID);

	if (RegOpenKeyEx(HKEY_CURRENT_USER, szSubkey, 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS)
	{
		wp.length = sizeof(WINDOWPLACEMENT);
		GetWindowPlacement(OxApp->oxWindow->hWin, &wp);
		dwSizeFlags = sizeof(wp.flags);
		dwSizeShowCmd = sizeof(wp.showCmd);
		dwSizeRcNormal = sizeof(wp.rcNormalPosition);
		if ((RegQueryValueExW(hKey, L"flags", NULL, NULL,
			(PBYTE)&wp.flags, &dwSizeFlags) != ERROR_SUCCESS) ||
			(RegQueryValueEx(hKey, L"showCmd", NULL, NULL,
			(PBYTE)&wp.showCmd, &dwSizeShowCmd) != ERROR_SUCCESS) ||
			(RegQueryValueEx(hKey, L"rcNormalPosition", NULL, NULL,
			(PBYTE)&wp.rcNormalPosition, &dwSizeRcNormal) != ERROR_SUCCESS))
		{
			OxErr_SetFromWindows();
			RegCloseKey(hKey);
			return FALSE;
		}
		RegCloseKey(hKey);
		if ((wp.rcNormalPosition.left <=
			(GetSystemMetrics(SM_CXSCREEN) - GetSystemMetrics(SM_CXICON))) &&
			(wp.rcNormalPosition.top <= (GetSystemMetrics(SM_CYSCREEN) - GetSystemMetrics(SM_CYICON))))
		{
			SetWindowPlacement(OxApp->oxWindow->hWin, &wp);
			//return TRUE;
		}
		//else
		//	return FALSE;
	}
	/*else {
		OutputDebugStringW("RegQueryValueEx");
		OxErr_SetFromWindows();
		return FALSE;
		}*/
	return TRUE;
}

static BOOL
OxApplication_Delete(OxObject* ox)
{
	OutputDebugString(L"OxApplication deleting!\n");
	PostQuitMessage(0);
	OxFree(((OxApplicationObject*)ox)->sName);
	return pOxClass->pBase->fnDelete(ox);
}

OxClass OxApplicationClass = {
	"Application",             /* sName */
	&OxObjectClass,            /* pBase */
	sizeof(OxApplicationObject), /* nSize */
	OxApplication_Delete,      /* fnDelete */
	NULL,                      /* pUserData */
	0                          /* aMethods */
};

BOOL
OxApplicationClass_Init(OxObject* ox)
{
	pOxClass = &OxApplicationClass;
	Ox_INHERIT_METHODS(pOxClass);
	return TRUE;
}
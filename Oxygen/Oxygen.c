// Oxygen.c  | Oxygen © 2015 by Thomas Führinger
#include "Oxygen.h"
static HANDLE hHeap;

BOOL
OxInit(char* sAppName)
{
	hHeap = GetProcessHeap();
	OxApp = OxApplication_New(sAppName);
	if (OxApp == NULL)
		return FALSE;
	if (!OxObjectClass_Init())
		return FALSE;
	if (!OxApplicationClass_Init())
		return FALSE;
	if (!OxListClass_Init())
		return FALSE;
	if (!OxStringClass_Init())
		return FALSE;
	if (!OxIntegerClass_Init())
		return FALSE;
	if (!OxFloatClass_Init())
		return FALSE;
	if (!OxBlobClass_Init())
		return FALSE;
	if (!OxBoolClass_Init())
		return FALSE;
	if (!OxMenuClass_Init())
		return FALSE;
	if (!OxMenuItemClass_Init())
		return FALSE;
	if (!OxImageClass_Init())
		return FALSE;
	if (!OxWidgetClass_Init())
		return FALSE;
	if (!OxWindowClass_Init())
		return FALSE;
	if (!OxMdiAreaClass_Init())
		return FALSE;
	if (!OxMdiWindowClass_Init())
		return FALSE;
	if (!OxToolBarClass_Init())
		return FALSE;
	if (!OxStatusBarClass_Init())
		return FALSE;
	if (!OxBoxClass_Init())
		return FALSE;
	if (!OxTabClass_Init())
		return FALSE;
	if (!OxSplitterClass_Init())
		return FALSE;
	if (!OxTabPageClass_Init())
		return FALSE;
	if (!OxButtonClass_Init())
		return FALSE;
	if (!OxEntryClass_Init())
		return FALSE;
	if (!OxLabelClass_Init())
		return FALSE;
	if (!OxImageViewClass_Init())
		return FALSE;
	if (!OxVideoViewClass_Init())
		return FALSE;
	if (!OxHtmlViewClass_Init())
		return FALSE;
	if (!OxMarkDownEntryClass_Init())
		return FALSE;

	/*/ Initialize the COM library.
	HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (FAILED(hr))
	{
	OxErr_SetStringFormat(OxERROR_WINDOWS, "Can not initialize the COM library. Error # %d", hr);
	return FALSE;
	}*/

	// Initialize the OLE library.
	HRESULT hr = OleInitialize(NULL);
	if (FAILED(hr))
	{
		OxErr_SetStringFormat(OxERROR_WINDOWS, "Can not initialize the OLE library. Error # %d", hr);
		return FALSE;
	}

	return TRUE;
}

BOOL
OxExit(void)
{
	//CoUninitialize();
	OleUninitialize();
	return TRUE;
}

void*
OxAllocate(size_t nSize)
{
	void* pChunk = HeapAlloc(hHeap, HEAP_GENERATE_EXCEPTIONS, (SIZE_T)nSize);
	if ((unsigned long)pChunk == STATUS_NO_MEMORY) {
		OxErr_SetString(OxERROR_RUNTIME, "Lack of memory.");
		return NULL;
	}
	else if ((unsigned long)pChunk == STATUS_ACCESS_VIOLATION) {
		OxErr_SetString(OxERROR_RUNTIME, "Can not allocate memory, access violation.");
		return NULL;
	}
	else
		return pChunk;
}

void*
OxAllocateZeroed(size_t nSize)
{
	void* pChunk = OxAllocate(nSize);
	if (pChunk)
		ZeroMemory(pChunk, nSize);
	return pChunk;
}

void*
OxReAllocate(void* pChunk, size_t nSize)
{
	pChunk = HeapReAlloc(hHeap, HEAP_GENERATE_EXCEPTIONS, pChunk, (SIZE_T)nSize);
	if ((unsigned long)pChunk == STATUS_NO_MEMORY) {
		OxErr_SetString(OxERROR_RUNTIME, "Lack of memory.");
		return NULL;
	}
	else if ((unsigned long)pChunk == STATUS_ACCESS_VIOLATION) {
		OxErr_SetString(OxERROR_RUNTIME, "Can not allocate memory, access violation.");
		return NULL;
	}
	else
		return pChunk;
}

BOOL
OxFree(void* pChunk)
{
	if (pChunk == NULL)
		return TRUE;
	else if (HeapFree(hHeap, 0, pChunk))
		return TRUE;
	else {
		OxErr_SetFromWindows();
		return FALSE;
	}
}

BOOL
OxAttachObject(OxObject** poxMember, OxObject* oxObject, BOOL bStrong)
{
	OxObject* tmp = *poxMember;
	if (bStrong)
		OxRET(oxObject);
	*poxMember = oxObject;
	OxREL(tmp);
	return TRUE;
}

char*
OxToU8(const LPWSTR szUTF16)
{
	if (szUTF16 == NULL)
		return NULL;

	if (*szUTF16 == L'\0')
		return '\0';

	int cbUTF8 = WideCharToMultiByte(CP_UTF8, 0, szUTF16, -1, NULL, 0, NULL, NULL);
	if (cbUTF8 == 0) {
		OxErr_SetString(OxERROR_RUNTIME, "Sting converson to UTF8 failed!");
		return NULL;
	}
	char* sTextUTF8 = (char*)OxAllocate(cbUTF8);
	int result = WideCharToMultiByte(CP_UTF8, 0, szUTF16, -1, sTextUTF8, cbUTF8, NULL, NULL);
	if (result == 0) {
		OxErr_SetString(OxERROR_RUNTIME, "Sting converson to UTF8 failed!");
		return NULL;
	}
	return sTextUTF8;
}

LPWSTR
OxToW(const char* sTextUTF8)
{
	if (sTextUTF8 == NULL)
		return NULL;
	if (*sTextUTF8 == '\0')
		return L'\0';

	int cchUTF16 = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, sTextUTF8, -1, NULL, 0);
	if (cchUTF16 == 0) {
		OxErr_SetString(OxERROR_RUNTIME, "Sting converson to wide character failed!");
		return NULL;
	}
	LPWSTR szUTF16 = (LPWSTR)OxAllocate(cchUTF16 * sizeof(WCHAR));
	int result = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, sTextUTF8, -1, szUTF16, cchUTF16);
	if (result == 0) {
		OxErr_SetString(OxERROR_RUNTIME, "Sting converson to wide character failed!");
		return NULL;
	}
	return szUTF16;
}

char*
OxDuplicateString(const char* sString)
{
	int iLen = strlen(sString) + 1;
	char* sDuplicate = (char*)OxAllocate(iLen*sizeof(char));
	/*if (StringCchCopyA(sDuplicate, iLen, sString) == STRSAFE_E_INSUFFICIENT_BUFFER) {
		OutputDebugStringA("OxDuplicateString");
		OutputDebugStringA(sDuplicate);
		OxErr_SetFromWindows();
		return NULL;
		}
		else*/
	strcpy_s(sDuplicate, iLen, sString);
	return sDuplicate;
}

BOOL
OxStringAppend(char* sMain, const char* sAppendix)
{
	if (sMain == NULL || sAppendix == NULL || lstrlenA(sAppendix) == 0)
		return TRUE;

	size_t nLen = lstrlenA(sMain) + lstrlenA(sAppendix) + 1;
	sMain = (char*)OxReAllocate(sMain, nLen);
	if (FAILED(StringCbCatA(sMain, nLen, sAppendix))){
		OxErr_SetFromWindows();
		return FALSE;
	}
	return TRUE;
}

char*
OxGetWindowText(HWND hWin)
{
	int iCharacters = GetWindowTextLengthW(hWin);
	if (iCharacters == 0)
		return NULL;

	LPWSTR szText = (LPWSTR)OxAllocate(sizeof(WCHAR) * (iCharacters + 1));
	GetWindowTextW(hWin, szText, iCharacters + 1);
	char* sText = OxToU8(szText);
	OxFree(szText);
	return sText;
}

char*
OxLoadFile(char* sFileNamePath)
{
	char* sMD = NULL;

	FILE* fp;
	//_wfopen_s(&fp, sFileNamePath, "r");
	fopen_s(&fp, sFileNamePath, "r");
	if (fp != NULL) {
		if (fseek(fp, 0L, SEEK_END) == 0) {
			long bufsize = ftell(fp);
			if (bufsize == -1) {
				OxErr_SetFromWindows();
				goto Failed;
			}
			sMD = OxAllocate(sizeof(char) * (bufsize + 1));
			if (fseek(fp, 0L, SEEK_SET) != 0) {
				OxErr_SetFromWindows();
				goto Failed;
			}

			// Read the entire file into memory.
			size_t newLen = fread(sMD, sizeof(char), bufsize, fp);
			if (newLen == 0) {
				OxErr_SetFromWindows();
				goto Failed;
			}
			else {
				sMD[newLen++] = '\0'; /* Just to be safe. */
			}
		}
		fclose(fp);
		return sMD;

	Failed:
		fclose(fp);
	}
	return NULL;
}

void
XX(OxObject* oxObject)
{
	if (oxObject == OxNull)
		MessageBoxA(0, "OxNull", "-1", 0);
	else if (oxObject){
		char buffer[30];
		sprintf_s(buffer, 30, "%d", oxObject->iRefCount);
		if (!MessageBoxA(0, oxObject->pClass->sName, buffer, 0))
			MessageBox(0, L"Can not show", L"XX", 0);
	}
	else
		MessageBoxA(0, "NULL", "XX", 0);
}

void
Xi(char* sContext, int a)
{
	TCHAR buffera[30];
	sprintf_s(buffera, 30, "%d", a);
	OutputDebugStringA("\nI--- ");
	OutputDebugStringA(sContext);
	OutputDebugStringA(" - ");
	OutputDebugStringA(buffera);
}

char* OxCopyright = OxCOPYRIGHT;
float OxVersion = OxVER_MAJOR + (OxVER_MINOR / 100.0) + (OxVER_MICRO / 10000.0);
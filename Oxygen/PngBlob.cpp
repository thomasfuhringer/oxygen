// PngBlob.c  | Oxygen © 2015 by Thomas Führinger
#include "Oxygen.h"

#include <Gdiplus.h>
#include <gdiplusflat.h>

extern "C" {
	OxBlobObject*
		OxPngBlob_FromFile(const char* sFileName)
	{
		GpImage* image;
		GpStatus status;
		IStream* stream = NULL;
		ULARGE_INTEGER liSize;
		CLSID   clsidEncoder;

		// Initialize GDI+.
		GdiplusStartupInput gdiplusStartupInput;
		ULONG_PTR gdiplusToken;
		GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

		status = GdipLoadImageFromFile(szFileName, &image);
		if (status != 0)
			return NULL;

		HRESULT hr = CreateStreamOnHGlobal(0, TRUE, &stream);
		if (!SUCCEEDED(hr))
			return NULL;

		GetEncoderClsid(L"image/png", &clsidEncoder);
		status = GdipSaveImageToStream(image, stream, clsidEncoder, NULL);
		if (status != 0)
			return NULL;

		IStream_Size(stream, &liSize);
		OxBlobObject* ox = OxBlob_New(liSize.LowPart);

		IStream_Reset(stream);
		IStream_Read(stream, ox->pData, ox->nSize);
		Release(stream);

		if (GdipDisposeImage(image) != Ok)
			return NULL;

		GdiplusShutdown(gdiplusToken);
		return ox;
	}
}

/*
BOOL
OxImageClass_Init()
{
Ox_INHERIT_METHODS(OxImageClass);
OxImageClass.aMethods[OxOBJECT_REPRESENT] = OxImage_Represent;

// Initialize GDI+.
GdiplusStartupInput gdiplusStartupInput;
ULONG_PTR gdiplusToken;
GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

//GdiplusShutdown(gdiplusToken);
return TRUE;
}


int SaveJpeg(HBITMAP hBmp, LPCWSTR lpszFilename, ULONG uQuality)
{
ULONG *pBitmap = NULL;
CLSID imageCLSID;
EncoderParameters encoderParams;
int iRes = 0;

typedef Status (WINAPI *pGdipCreateBitmapFromHBITMAP)(HBITMAP, HPALETTE, ULONG**);
pGdipCreateBitmapFromHBITMAP lGdipCreateBitmapFromHBITMAP;

typedef Status (WINAPI *pGdipSaveImageToFile)(ULONG *, const WCHAR*, const CLSID*, const EncoderParameters*);
pGdipSaveImageToFile lGdipSaveImageToFile;

// load GdipCreateBitmapFromHBITMAP
lGdipCreateBitmapFromHBITMAP = (pGdipCreateBitmapFromHBITMAP)GetProcAddress(hModuleThread, "GdipCreateBitmapFromHBITMAP");
if(lGdipCreateBitmapFromHBITMAP == NULL)
{
// error
return 0;
}

// load GdipSaveImageToFile
lGdipSaveImageToFile = (pGdipSaveImageToFile)GetProcAddress(hModuleThread, "GdipSaveImageToFile");
if(lGdipSaveImageToFile == NULL)
{
// error
return 0;
}

lGdipCreateBitmapFromHBITMAP(hBmp, NULL, &pBitmap);

iRes = GetEncoderClsid(L"image/jpeg", &imageCLSID);
if(iRes == -1)
{
// error
return 0;
}
encoderParams.Count = 1;
encoderParams.Parameter[0].NumberOfValues = 1;
encoderParams.Parameter[0].Guid  = EncoderQuality;
encoderParams.Parameter[0].Type  = EncoderParameterValueTypeLong;
encoderParams.Parameter[0].Value = &uQuality;

lGdipSaveImageToFile(pBitmap, lpszFilename, &imageCLSID, &encoderParams);


return 1;
}*/
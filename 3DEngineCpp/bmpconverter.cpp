#include "bmpconverter.h"
#include <stdio.h>
#include <istream>

using namespace Gdiplus;
using namespace Gdiplus::DllExports;

int GetEncoderClsid(WCHAR *format, CLSID *pClsid)
{
	unsigned int num = 0, size = 0;
	GetImageEncodersSize(&num, &size);
	if (size == 0) return -1;
	ImageCodecInfo *pImageCodecInfo = (ImageCodecInfo *)(malloc(size));
	if (pImageCodecInfo == NULL) return -1;
	GetImageEncoders(num, size, pImageCodecInfo);
	for (unsigned int j = 0; j < num; ++j)
	{
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0){
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;
		}
	}
	free(pImageCodecInfo);
	return -1;
}

//Call this function with desired jpg quality (0-100)
BYTE* convertBMP(Bitmap *frame, ULONG uQuality, size_t *finalSize)
{
	ULONG_PTR gdiplusToken;
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	CLSID imageCLSID;
	HGLOBAL hMem;
	int result;
  
	//Was testing loading a bitmap from disk...
	//frame = new Bitmap(L"C:\\Users\\sdickson\\Desktop\\shibe.bmp");

	EncoderParameters encoderParams;
	encoderParams.Count = 1;
	encoderParams.Parameter[0].NumberOfValues = 1;
	encoderParams.Parameter[0].Guid = EncoderQuality;
	encoderParams.Parameter[0].Type = EncoderParameterValueTypeLong;
	encoderParams.Parameter[0].Value = &uQuality; //Conversion quality
	GetEncoderClsid(L"image/jpeg", &imageCLSID); //Here specify final conversion format (image/png, image/tiff, image/jpeg, etc)

	//If we wanted to save this jpg to disk...
	// LPWSTR filename = (L"H:\\test2.jpg");
	// int iRes = (frame->Save(filename, &imageCLSID, &encoderParams) == Ok);

	IStream *pStream = NULL;
	LARGE_INTEGER liZero = {};
	ULARGE_INTEGER pos = {};
	STATSTG stg = {};
	ULONG bytesRead = 0;
	HRESULT hrRet = S_OK;

	BYTE* buffer = NULL; //Buffer to hold JPEG data
	DWORD dwBufferSize = 0; //Size of buffer


	hrRet = CreateStreamOnHGlobal(NULL, TRUE, &pStream);
	hrRet = frame->Save(pStream, &imageCLSID, &encoderParams) == 0 ? S_OK : E_FAIL;
	hrRet = pStream->Seek(liZero, STREAM_SEEK_SET, &pos);
	hrRet = pStream->Stat(&stg, STATFLAG_NONAME);

	//Allocate buffer of ample size
	buffer = new BYTE[stg.cbSize.LowPart];
	hrRet = (buffer == NULL) ? E_OUTOFMEMORY : S_OK;
	dwBufferSize = stg.cbSize.LowPart;

	//Copy stream into buffer
	hrRet = pStream->Read(buffer, stg.cbSize.LowPart, &bytesRead);

	//Release stream
	if (pStream)
	{
		pStream->Release();
	}

	// delete frame;
	GdiplusShutdown(gdiplusToken);

	*finalSize = dwBufferSize;
	return buffer;
}

Bitmap* convertJPG(BYTE* jpg, size_t jpg_size)
{
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	//TODO: Need to convert JPG stream to BMP format before creating the IStream.
	HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, jpg_size);
	PVOID pMem = GlobalLock(hMem);
	RtlMoveMemory(pMem, jpg, jpg_size);
	IStream *pStream = 0;
	HRESULT hr = CreateStreamOnHGlobal(hMem, FALSE, &pStream);
	GdiplusShutdown(gdiplusToken);

    return Bitmap::FromStream(pStream);
}
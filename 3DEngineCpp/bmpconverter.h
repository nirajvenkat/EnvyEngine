#pragma once
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>

int GetEncoderClsid(WCHAR *format, CLSID *pClsid);
BYTE* convertBMP(Gdiplus::Bitmap *frame, ULONG uQuality, size_t *finalSize);
Gdiplus::Bitmap* convertJPG(BYTE* jpg, size_t jpg_size);

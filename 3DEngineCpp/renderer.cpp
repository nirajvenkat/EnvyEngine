// renderer.cpp
//
// Envy Engine renderer. This class sits on top of any graphics-producing code in envy. For now
// it exists to handle the "software monitor" features used by the master controller.

#include "renderer.h"
#include "frame.h"
#include "overlay.h"
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <stdio.h>
#include <atlbase.h>

Renderer::Renderer() {
	mSDLRenderWindow = NULL;
	mSDLRenderer = NULL;
}
Renderer::~Renderer() {
	delete mOverlay;
}

void Renderer::initOutputWindow(int width, int height, const char *title) 
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER)) { // Timer for SimNodes
		// Failed to init
	}

	mRenderWidth = width;
	mRenderHeight = height;

	// Create the SDL Window
	mSDLRenderWindow = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		width, height,
		0);

	// Create our renderer
	mSDLRenderer = SDL_CreateRenderer(mSDLRenderWindow, -1, SDL_RENDERER_ACCELERATED);

	// Set up overlay	
	Overlay::initializeOverlays();
	mOverlay = new Overlay(mSDLRenderer);
}

void Renderer::renderFrame(Frame *frame) {

	SDL_Surface *surf = frame->getSurface();
	SDL_Texture *tex = SDL_CreateTextureFromSurface(mSDLRenderer, surf);

	// Frame should be identical in size to the current window
	SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_NONE);
	SDL_RenderCopyEx(mSDLRenderer, tex, NULL, NULL, 0.0, NULL, SDL_FLIP_VERTICAL);
	mOverlay->render();
	SDL_RenderPresent(mSDLRenderer);
	SDL_DestroyTexture(tex);
}

HRESULT GetEncoderClsid(__in LPCWSTR pwszFormat, __out GUID *pGUID)
{
	HRESULT hr = E_FAIL;
	UINT  nEncoders = 0;          // number of image encoders
	UINT  nSize = 0;              // size of the image encoder array in bytes
	CAutoVectorPtr<BYTE> spData;
	Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;
	Gdiplus::Status status;
	bool fFound = false;

	// param check

	if ((pwszFormat == NULL) || (pwszFormat[0] == 0) || (pGUID == NULL))
	{
		return E_POINTER;
	}

	*pGUID = GUID_NULL;

	status = Gdiplus::GetImageEncodersSize(&nEncoders, &nSize);

	if ((status != Gdiplus::Ok) || (nSize == 0))
	{
		return E_FAIL;
	}


	spData.Allocate(nSize);

	if (spData == NULL)
	{
		return E_FAIL;
	}

	pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(BYTE*)spData;

	status = Gdiplus::GetImageEncoders(nEncoders, nSize, pImageCodecInfo);

	if (status != Gdiplus::Ok)
	{
		return E_FAIL;
	}

	for (UINT j = 0; j < nEncoders; ++j)
	{
		if (wcscmp(pImageCodecInfo[j].MimeType, pwszFormat) == 0)
		{
			*pGUID = pImageCodecInfo[j].Clsid;
			fFound = true;
			break;
		}
	}

	hr = fFound ? S_OK : E_FAIL;

	return hr;
}

// getFrameBuffer
//
// pixels: The address of a pointer to the pixel buffer. Must be freed using free() after disposing of the
// bitmap
//
// return value: A GdiPlus::Bitmap containing an ARGB (8 bit per pixel) image of the frame buffer.

Gdiplus::Bitmap *Renderer::getFrameBuffer(void **pixels)
{
	size_t bufSize = 4 * mRenderWidth * mRenderHeight;
	Gdiplus::Bitmap *resultBitmap = NULL;

	GLubyte *frameBufBytes = (GLubyte*)malloc(bufSize);
	if (frameBufBytes) // We may be out of memory otherwise
	{
		SDL_RenderReadPixels(mSDLRenderer, NULL, SDL_PIXELFORMAT_ARGB8888, frameBufBytes, 4*mRenderWidth);
		resultBitmap = new Gdiplus::Bitmap(mRenderWidth, mRenderHeight, mRenderWidth * 4,
			PixelFormat32bppARGB, frameBufBytes);

		CLSID pngClsid;
		GetEncoderClsid(L"image/png", &pngClsid);
		resultBitmap->Save(L"D:\\resultimage.png", &pngClsid, NULL);

		*pixels = frameBufBytes; // Return the pixel buffer
	}
	return resultBitmap; // Return bitmap object
}

// Overlay passthrough functions
void Renderer::addNodeToOverlay(int nodeId) {
	mOverlay->insertLine(nodeId);
}

void Renderer::removeNodeFromOverlay(int nodeId) {
	mOverlay->removeLine(nodeId);
}

void Renderer::updateNodeOnOverlay(int nodeId, const char *text, float avg) {
	mOverlay->updateText(nodeId, text);
	mOverlay->updateAvg(nodeId, avg);
}
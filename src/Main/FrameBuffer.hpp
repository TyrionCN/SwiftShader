// SwiftShader Software Renderer
//
// Copyright(c) 2005-2012 TransGaming Inc.
//
// All rights reserved. No part of this software may be copied, distributed, transmitted,
// transcribed, stored in a retrieval system, translated into any human or computer
// language by any means, or disclosed to third parties without the explicit written
// agreement of TransGaming Inc. Without such an agreement, no rights or licenses, express
// or implied, including but not limited to any patent rights, are granted to you.
//

#ifndef	sw_FrameBuffer_hpp
#define	sw_FrameBuffer_hpp

#include "Reactor/Nucleus.hpp"
#include "Renderer/Surface.hpp"

#include <windows.h>

namespace sw
{
	class Surface;

	struct GammaRamp
	{
		short red[256];
		short green[256];
		short blue[256];
	};

	struct BlitState
	{
		int width;
		int height;
		int depth;   // Display bit depth; 32 = X8R8G8B8, 24 = R8G8B8, 16 = R5G6B5
		int stride;
		bool HDR;    // A16B16G16R16 source color buffer
		int cursorWidth;
		int cursorHeight;
	};

	class FrameBuffer
	{
	public:
		FrameBuffer(HWND windowHandle, int width, int height, bool fullscreen, bool topLeftOrigin);

		virtual ~FrameBuffer();

		int getWidth() const;
		int getHeight() const;
		int getStride() const;

		virtual void *lock() = 0;
		virtual void unlock() = 0;

		virtual void flip(HWND windowOverride, void *source, bool HDR) = 0;
		virtual void blit(HWND windowOverride, void *source, const Rect *sourceRect, const Rect *destRect, bool HDR) = 0;

		virtual void setGammaRamp(GammaRamp *gammaRamp, bool calibrate) = 0;
		virtual void getGammaRamp(GammaRamp *gammaRamp) = 0;

		virtual void screenshot(void *destBuffer) = 0;
		virtual bool getScanline(bool &inVerticalBlank, unsigned int &scanline) = 0;

		static void setCursorImage(sw::Surface *cursor);
		static void setCursorOrigin(int x0, int y0);
		static void setCursorPosition(int x, int y);

		static Routine *copyRoutine(const BlitState &state);

	protected:
		void updateBounds(HWND windowOverride);

		void copy(HWND windowOverride, void *source, bool HDR);
		void copyLocked();

		static unsigned long __stdcall threadFunction(void *parameters);

		void *locked;   // Video memory back buffer
		void *target;   // Render target buffer

		int width;
		int height;
		int stride;
		int bitDepth;
		bool HDRdisplay;

		HWND windowHandle;
		DWORD originalWindowStyle;
		RECT bounds;
		bool windowed;

		HCURSOR nullCursor;
		HCURSOR win32Cursor;

		void (__cdecl *blitFunction)(void *dst, void *src);
		Routine *blitRoutine;
		BlitState blitState;

		static void blend(const BlitState &state, const Pointer<Byte> &d, const Pointer<Byte> &s, const Pointer<Byte> &c);
		static void initializeLogo();

		static Surface *logo;
		static unsigned int *logoImage;

		static void *cursor;
		static int cursorWidth;
		static int cursorHeight;
		static int cursorHotspotX;
		static int cursorHotspotY;
		static int cursorPositionX;
		static int cursorPositionY;
		static int cursorX;
		static int cursorY;

		HANDLE blitThread;
		HANDLE syncEvent;
		HANDLE blitEvent;
		volatile bool terminate;

		static bool topLeftOrigin;
	};
}

extern "C"
{
	sw::FrameBuffer *createFrameBuffer(HWND windowHandle, int width, int height, bool fullscreen, bool topLeftOrigin);
}

#endif	 //	sw_FrameBuffer_hpp
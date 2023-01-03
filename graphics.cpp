// Copyright (c) 2022 Perets Dmytro
// Author: Perets Dmytro <imperator999mcpe@gmail.com>
//
// Personal usage is allowed only if this comment was not changed or deleted.
// Commercial usage must be agreed with the author of this comment.


extern "C" {
#include <X11/Xlib.h>
#include <X11/Xutil.h>
}

#include "graphics.h"


Display* display = nullptr;
Window indicator_window = -1;


void InitGraphics()
{
	display = XOpenDisplay(nullptr);
}

void FreeGraphics()
{
	XCloseDisplay(display);
}


void DrawIndicatorCircle(const char* color)
{
	int screen = DefaultScreen(display);
	if (indicator_window < 0) XDestroyWindow(display, indicator_window);
	indicator_window = XCreateSimpleWindow(
			display, RootWindow(display, screen),
			DisplayWidth(display, screen) - 35, 10, 25, 25, 0,
			BlackPixel(display, screen), WhitePixel(display, screen)
	);
	XSetWindowBackgroundPixmap(display, indicator_window, None);
	
	XSetWindowAttributes attrs;
	attrs.override_redirect = True;
	XChangeWindowAttributes(display, indicator_window, CWOverrideRedirect, &attrs);
	
	GC gc = XCreateGC(display, indicator_window, 0, nullptr);
	
	XColor col;
	XAllocNamedColor(display, DefaultColormap(display, screen), color, &col, &col);
	XSetForeground(display, gc, col.pixel);
	
	XMapWindow(display, indicator_window);
	
	XFillArc(display, indicator_window, gc, 0, 0, 25, 25, 1, 360 * 64);
	XFlush(display);
	
	XFreeGC(display, gc);
}

void IndicateError()
{
	DrawIndicatorCircle("#aa3333");
}

void IndicateListening()
{
	DrawIndicatorCircle("#2266aa");
}

void IndicateRecognizing()
{
	DrawIndicatorCircle("#888888");
}

void IndicateExecution()
{
	DrawIndicatorCircle("#00aa33");
}
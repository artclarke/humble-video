/*****************************************************************************
 * display-x11.c: x11 interface
 *****************************************************************************
 * Copyright (C) 2005-2013 x264 project
 *
 * Authors: Tuukka Toivonen <tuukkat@ee.oulu.fi>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02111, USA.
 *
 * This program is also available under a commercial proprietary license.
 * For more information, contact us at licensing@x264.com.
 *****************************************************************************/

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "display.h"

static long event_mask = ConfigureNotify|ExposureMask|KeyPressMask|ButtonPressMask|StructureNotifyMask|ResizeRedirectMask;

static Display *disp_display = NULL;
static struct disp_window
{
    int init;
    Window window;
} disp_window[10];

static inline void disp_chkerror( int cond, char *e )
{
    if( !cond )
        return;
    fprintf( stderr, "error: %s\n", e ? e : "?" );
    abort();
}

static void disp_init_display( void )
{
    Visual *visual;
    int dpy_class;
    int screen;
    int dpy_depth;

    if( disp_display )
        return;
    memset( &disp_window, 0, sizeof(disp_window) );
    disp_display = XOpenDisplay( "" );
    disp_chkerror( !disp_display, "no display" );
    screen = DefaultScreen( disp_display );
    visual = DefaultVisual( disp_display, screen );
    dpy_class = visual->class;
    dpy_depth = DefaultDepth( disp_display, screen );
    disp_chkerror( !((dpy_class == TrueColor && dpy_depth == 32)
        || (dpy_class == TrueColor && dpy_depth == 24)
        || (dpy_class == TrueColor && dpy_depth == 16)
        || (dpy_class == PseudoColor && dpy_depth == 8)),
        "requires 8 bit PseudoColor or 16/24/32 bit TrueColor display" );
}

static void disp_init_window( int num, int width, int height, const unsigned char *title )
{
    XSetWindowAttributes xswa;
    XEvent xev;
    int screen = DefaultScreen(disp_display);
    Visual *visual = DefaultVisual (disp_display, screen);
    char buf[200];
    Window window;

    if( title )
        snprintf( buf, 200, "%s: %i/disp", title, num );
    else
        snprintf( buf, 200, "%i/disp", num );

    XSizeHints *shint = XAllocSizeHints();
    disp_chkerror( !shint, "memerror" );
    shint->min_width = shint->max_width = shint->width = width;
    shint->min_height = shint->max_height = shint->height = height;
    shint->flags = PSize | PMinSize | PMaxSize;
    disp_chkerror( num < 0 || num >= 10, "bad win num" );
    if( !disp_window[num].init )
    {
        unsigned int mask = 0;
        disp_window[num].init = 1;
        unsigned int bg = WhitePixel( disp_display, screen );
        unsigned int fg = BlackPixel( disp_display, screen );
        int dpy_depth = DefaultDepth( disp_display, screen );
        if( dpy_depth==32 || dpy_depth==24 || dpy_depth==16 )
        {
            mask |= CWColormap;
            xswa.colormap = XCreateColormap( disp_display, DefaultRootWindow( disp_display ), visual, AllocNone );
        }
        xswa.background_pixel = bg;
        xswa.border_pixel = fg;
        xswa.backing_store = Always;
        xswa.backing_planes = -1;
        xswa.bit_gravity = NorthWestGravity;
        mask = CWBackPixel | CWBorderPixel | CWBackingStore | CWBackingPlanes | CWBitGravity;
        window = XCreateWindow( disp_display, DefaultRootWindow( disp_display ),
                                shint->x, shint->y, shint->width, shint->height,
                                1, dpy_depth, InputOutput, visual, mask, &xswa );
        disp_window[num].window = window;

        XSelectInput( disp_display, window, event_mask );
        XSetStandardProperties( disp_display, window, buf, buf, None, NULL, 0, shint );
        XMapWindow( disp_display, window );

        do {
            XNextEvent( disp_display, &xev );
        } while( xev.type != MapNotify || xev.xmap.event != window );
    }
    window = disp_window[num].window;
    XSetStandardProperties( disp_display, window, buf, buf, None, NULL, 0, shint );
    XResizeWindow( disp_display, window, width, height );
    XSync( disp_display, 1 );
    XFree( shint );
}

void disp_sync( void )
{
    XSync( disp_display, 1 );
}

void disp_setcolor( unsigned char *name )
{
    XColor c_exact, c_nearest;

    int screen = DefaultScreen( disp_display );
    GC gc = DefaultGC( disp_display, screen );
    Colormap cm = DefaultColormap( disp_display, screen );
    Status st = XAllocNamedColor( disp_display, cm, name, &c_nearest, &c_exact );
    disp_chkerror( st != 1, "XAllocNamedColor error" );
    XSetForeground( disp_display, gc, c_nearest.pixel );
}

void disp_gray( int num, char *data, int width, int height, int stride, const unsigned char *title )
{
    char dummy;

    disp_init_display();
    disp_init_window( num, width, height, title );
    int screen = DefaultScreen( disp_display );
    Visual *visual = DefaultVisual( disp_display, screen );
    int dpy_depth = DefaultDepth( disp_display, screen );
    XImage *ximage = XCreateImage( disp_display, visual, dpy_depth, ZPixmap, 0, &dummy, width, height, 8, 0 );
    disp_chkerror( !ximage, "no ximage" );
#if WORDS_BIGENDIAN
    ximage->byte_order = MSBFirst;
    ximage->bitmap_bit_order = MSBFirst;
#else
    ximage->byte_order = LSBFirst;
    ximage->bitmap_bit_order = LSBFirst;
#endif

    int pixelsize = dpy_depth>8 ? sizeof(int) : sizeof(unsigned char);
    uint8_t *image = malloc( width * height * pixelsize );
    disp_chkerror( !image, "malloc failed" );
    for( int y = 0; y < height; y++ )
        for( int x = 0; x < width; x++ )
            memset( &image[(width*y + x)*pixelsize], data[y*stride+x], pixelsize );
    ximage->data = image;
    GC gc = DefaultGC( disp_display, screen );

    XPutImage( disp_display, disp_window[num].window, gc, ximage, 0, 0, 0, 0, width, height );
    XPutImage( disp_display, disp_window[num].window, gc, ximage, 0, 0, 0, 0, width, height );

    XDestroyImage( ximage );
    XSync( disp_display, 1 );

}

void disp_gray_zoom(int num, char *data, int width, int height, int stride, const unsigned char *title, int zoom)
{
    unsigned char *dataz = malloc( width*zoom * height*zoom );
    disp_chkerror( !dataz, "malloc" );
    for( int y = 0; y < height; y++ )
        for( int x = 0; x < width; x++ )
            for( int y0 = 0; y0 < zoom; y0++ )
                for( int x0 = 0; x0 < zoom; x0++ )
                    dataz[(y*zoom + y0)*width*zoom + x*zoom + x0] = data[y*stride+x];
    disp_gray( num, dataz, width*zoom, height*zoom, width*zoom, title );
    free( dataz );
}

void disp_point( int num, int x1, int y1 )
{
    int screen = DefaultScreen( disp_display );
    GC gc = DefaultGC( disp_display, screen );
    XDrawPoint( disp_display, disp_window[num].window, gc, x1, y1 );
}

void disp_line( int num, int x1, int y1, int x2, int y2 )
{
    int screen = DefaultScreen( disp_display );
    GC gc = DefaultGC( disp_display, screen );
    XDrawLine( disp_display, disp_window[num].window, gc, x1, y1, x2, y2 );
}

void disp_rect( int num, int x1, int y1, int x2, int y2 )
{
    int screen = DefaultScreen( disp_display );
    GC gc = DefaultGC( disp_display, screen );
    XDrawRectangle( disp_display, disp_window[num].window, gc, x1, y1, x2-x1, y2-y1 );
}

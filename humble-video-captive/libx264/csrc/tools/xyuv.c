/*****************************************************************************
 * xyuv.c: a SDL yuv 420 planer viewer.
 *****************************************************************************
 * Copyright (C) 2004 Laurent Aimar <fenrir@via.ecp.fr>
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
 *****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include <SDL/SDL.h>

#define YUV_MAX 20
#define SDL_TITLE "xyuv: %s - %d/%d - %.2ffps"
typedef struct
{
    /* globals */
    int     i_width;
    int     i_height;
    int     i_frame_size;
    int     i_frame;
    int     i_frames;
    float   f_fps;

    float   f_y;

    int     b_pause;
    int     b_grid;
    int     b_split;
    int     b_diff;
    int     i_join;

    /* Constructed picture */
    int     i_wall_width;   /* in picture count */

    /* YUV files */
    int     i_yuv;
    struct
    {
        char    *name;
        FILE    *f;         /* handles */
        int     i_frames;   /* frames count */

        /* Position in the whole picture */
        int     x, y;
    } yuv[YUV_MAX];

    /* SDL */
    int i_sdl_width;
    int i_sdl_height;

    int i_display_width;
    int i_display_height;
    char *title;

    SDL_Surface *screen;
    SDL_Overlay *overlay;

    /* */
    uint8_t *pic;

} xyuv_t;

xyuv_t xyuv = {
    .i_width = 0,
    .i_height = 0,
    .i_frame  = 1,
    .i_frames = 0,
    .f_fps = 25.0,
    .f_y = 0.0,
    .i_wall_width = 0,

    .i_yuv = 0,

    .b_pause = 0,
    .b_split = 0,
    .b_diff = 0,
    .i_join = -1,

    .title = NULL,
    .pic = NULL,
};

static void help( void )
{
    fprintf( stderr,
             "Syntax: xyuv [options] file [file2 ...]\n"
             "\n"
             "      --help                  Print this help\n"
             "\n"
             "  -s, --size <WIDTHxHEIGHT>   Set input size\n"
             "  -w, --width <integer>       Set width\n"
             "  -h, --height <integer>      Set height\n"
             "\n"
             "  -S, --split                 Show splited Y/U/V planes\n"
             "  -d, --diff                  Show difference (only 2 files) in split mode\n"
             "  -j, --joint <integer>\n"
             "\n"
             "  -y <float>                  Set Y factor\n"
             "\n"
             "  -g, --grid                  Show a grid (macroblock 16x16)\n"
             "  -W <integer>                Set wall width (in picture count)\n"
             "  -f, --fps <float>           Set fps\n"
             "\n" );
}

static void xyuv_count_frames( xyuv_t *xyuv );
static void xyuv_detect( int *pi_width, int *pi_height );
static void xyuv_display( xyuv_t *xyuv, int i_frame );

int main( int argc, char **argv )
{
    int i;

    /* Parse commande line */
    for( i = 1; i < argc; i++ ) {
        if( !strcasecmp( argv[i], "--help" ) ) {
            help();
            return 0;
        }
        if( !strcmp( argv[i], "-d" ) || !strcasecmp( argv[i], "--diff" ) ) {
            xyuv.b_diff = 1;
        } else if( !strcmp( argv[i], "-S" ) || !strcasecmp( argv[i], "--split" ) ) {
            xyuv.b_split = 1;
        } else if( !strcmp( argv[i], "-f" ) || !strcasecmp( argv[i], "--fps" ) ) {
            if( i >= argc -1 ) goto err_missing_arg;
            xyuv.f_fps = atof( argv[++i] );
        } else if( !strcmp( argv[i], "-h" ) || !strcasecmp( argv[i], "--height" ) ) {
            if( i >= argc -1 ) goto err_missing_arg;
            xyuv.i_height = atoi( argv[++i] );
        } else if( !strcmp( argv[i], "-w" ) || !strcasecmp( argv[i], "--width" ) ) {
            if( i >= argc -1 ) goto err_missing_arg;
            xyuv.i_width = atoi( argv[++i] );
        } else if( !strcmp( argv[i], "-s" ) || !strcasecmp( argv[i], "--size" ) ) {
            char *p;

            if( i >= argc -1 ) goto err_missing_arg;

            xyuv.i_width = strtol( argv[++i], &p, 0 );
            p++;
            xyuv.i_height = atoi( p );
        } else if( !strcmp( argv[i], "-W" ) ) {
            if( i >= argc -1 ) goto err_missing_arg;
            xyuv.i_wall_width = atoi( argv[++i] );
        } else if( !strcmp( argv[i], "-y" ) ) {
            if( i >= argc -1 ) goto err_missing_arg;
            xyuv.f_y = atof( argv[++i] );
        } else if( !strcmp( argv[i], "-j" ) || !strcasecmp( argv[i], "--join" ) ) {
            if( i >= argc -1 ) goto err_missing_arg;
            xyuv.i_join = atoi( argv[++i] );
        } else if( !strcmp( argv[i], "-g" ) || !strcasecmp( argv[i], "--grid" ) ) {
            xyuv.b_grid = 1;
        } else {
            FILE *f = fopen( argv[i], "rb" );
            if( !f ) {
                fprintf( stderr, "cannot open YUV %s\n", argv[i] );
            } else {
                xyuv.yuv[xyuv.i_yuv].name = strdup( argv[i] );
                xyuv.yuv[xyuv.i_yuv].f = f;
                xyuv.yuv[xyuv.i_yuv].i_frames = 0;

                xyuv.i_yuv++;
            }
        }
    }

    if( xyuv.i_yuv == 0 ) {
        fprintf( stderr, "no file to display\n" );
        return -1;
    }
    if( xyuv.i_width == 0 || xyuv.i_height == 0 ) {
        char *psz = xyuv.yuv[0].name;
        char *num;
        char *x;
        /* See if we find widthxheight in the file name */
        for( ;; ) {
            if( !( x = strchr( psz+1, 'x' ) ) ) {
                break;
            }
            num = x;
            while( num > psz && num[-1] >= '0' && num[-1] <= '9' )
                num--;

            if( num != x && x[1] >= '0' && x[1] <= '9' ) {
                xyuv.i_width = atoi( num );
                xyuv.i_height = atoi( x+1 );
                break;
            }
            psz = x;
        }
        fprintf( stderr, "file name gives %dx%d\n", xyuv.i_width, xyuv.i_height );
    }

    if( xyuv.i_width == 0 || xyuv.i_height == 0 ) {
        xyuv_detect( &xyuv.i_width, &xyuv.i_height );
    }

    if( xyuv.i_width == 0 || xyuv.i_height == 0 ) {
        fprintf( stderr, "invalid or missing frames size\n" );
        return -1;
    }
    if( xyuv.b_diff && xyuv.i_yuv != 2 ) {
        fprintf( stderr, "--diff works only with 2 files\n" );
        return -1;
    }
    if( (xyuv.i_join == 0 || xyuv.i_join >= xyuv.i_width) && xyuv.i_yuv != 2 ) {
        fprintf( stderr, "--join woeks only with two files and range is [1, width-1]\n" );
        return -1;
    }
    if( xyuv.i_join % 2 != 0 ) {
        if( xyuv.i_join + 1 < xyuv.i_width )
            xyuv.i_join++;
        else
            xyuv.i_join--;
    }

    /* Now check frames */
    fprintf( stderr, "displaying :\n" );
    xyuv.i_frame_size = 3 * xyuv.i_width * xyuv.i_height / 2;
    xyuv_count_frames( &xyuv );
    for( i = 0; i < xyuv.i_yuv; i++ ) {
        fprintf( stderr, " - '%s' : %d frames\n", xyuv.yuv[i].name, xyuv.yuv[i].i_frames );
    }

    if( xyuv.i_frames == 0 ) {
        fprintf( stderr, "no frames to display\n" );
    }

    xyuv.pic = malloc( xyuv.i_frame_size );

    /* calculate SDL view */
    if( xyuv.i_wall_width > xyuv.i_yuv ) {
        xyuv.i_wall_width = xyuv.i_yuv;
    }
    if( xyuv.i_wall_width == 0 ) {
        while( xyuv.i_wall_width < xyuv.i_yuv && xyuv.i_wall_width * xyuv.i_wall_width < xyuv.i_yuv ) {
            xyuv.i_wall_width++;
        }
    }

    for( i = 0; i < xyuv.i_yuv; i++ ) {
        if( xyuv.b_diff || xyuv.i_join > 0 ) {
            xyuv.yuv[i].x = 0;
            xyuv.yuv[i].y = 0;
        } else if( xyuv.b_split ) {
            xyuv.yuv[i].x = (i%xyuv.i_wall_width) * 3 * xyuv.i_width / 2;
            xyuv.yuv[i].y = (i/xyuv.i_wall_width) * xyuv.i_height;
        } else {
            xyuv.yuv[i].x = (i%xyuv.i_wall_width) * xyuv.i_width;
            xyuv.yuv[i].y = (i/xyuv.i_wall_width) * xyuv.i_height;
        }
    }
    if( xyuv.b_diff ) {
        xyuv.i_sdl_width = 3 * xyuv.i_width / 2;
        xyuv.i_sdl_height= xyuv.i_height;
    } else if( xyuv.i_join > 0 ) {
        xyuv.i_sdl_width = xyuv.i_width;
        xyuv.i_sdl_height= xyuv.i_height;
    } else if( xyuv.b_split ) {
        xyuv.i_sdl_width = xyuv.i_wall_width * 3 * xyuv.i_width / 2;
        xyuv.i_sdl_height= xyuv.i_height * ( ( xyuv.i_yuv  + xyuv.i_wall_width - 1 ) / xyuv.i_wall_width );
    } else {
        xyuv.i_sdl_width = xyuv.i_wall_width * xyuv.i_width;
        xyuv.i_sdl_height= xyuv.i_height * ( ( xyuv.i_yuv  + xyuv.i_wall_width - 1 ) / xyuv.i_wall_width );
    }
    xyuv.i_display_width = xyuv.i_sdl_width;
    xyuv.i_display_height = xyuv.i_sdl_height;

    /* Open SDL */
    if( SDL_Init( SDL_INIT_EVENTTHREAD|SDL_INIT_NOPARACHUTE|SDL_INIT_VIDEO) ) {
        fprintf( stderr, "cannot init SDL\n" );
        return -1;
    }

    SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, 100 );
    SDL_EventState( SDL_KEYUP, SDL_IGNORE );

    xyuv.screen = SDL_SetVideoMode( xyuv.i_sdl_width, xyuv.i_sdl_height, 0,
                                    SDL_HWSURFACE|SDL_RESIZABLE|
                                    SDL_ASYNCBLIT|SDL_HWACCEL );
    if( xyuv.screen == NULL ) {
        fprintf( stderr, "SDL_SetVideoMode failed\n" );
        return -1;
    }

    SDL_LockSurface( xyuv.screen );
    xyuv.overlay = SDL_CreateYUVOverlay( xyuv.i_sdl_width, xyuv.i_sdl_height,
                                         SDL_YV12_OVERLAY,
                                         xyuv.screen );
    /* reset with black */
    memset( xyuv.overlay->pixels[0],   0, xyuv.overlay->pitches[0] * xyuv.i_sdl_height );
    memset( xyuv.overlay->pixels[1], 128, xyuv.overlay->pitches[1] * xyuv.i_sdl_height / 2);
    memset( xyuv.overlay->pixels[2], 128, xyuv.overlay->pitches[2] * xyuv.i_sdl_height / 2);
    SDL_UnlockSurface( xyuv.screen );

    if( xyuv.overlay == NULL ) {
        fprintf( stderr, "recon: SDL_CreateYUVOverlay failed\n" );
        return -1;
    }

    for( ;; ) {
        SDL_Event event;
        static int b_fullscreen = 0;
        int64_t i_start = SDL_GetTicks();
        int i_wait;

        if( !xyuv.b_pause ) {
            xyuv_display( &xyuv, xyuv.i_frame );
        }

        for( ;; ) {
            int b_refresh = 0;
            while( SDL_PollEvent( &event ) )  {
                switch( event.type )
                {
                    case SDL_QUIT:
                        if( b_fullscreen )
                            SDL_WM_ToggleFullScreen( xyuv.screen );
                        exit( 1 );

                    case SDL_KEYDOWN:
                        switch( event.key.keysym.sym )
                        {
                            case SDLK_q:
                            case SDLK_ESCAPE:
                                if( b_fullscreen )
                                    SDL_WM_ToggleFullScreen( xyuv.screen );
                                exit(1);

                            case SDLK_f:
                                if( SDL_WM_ToggleFullScreen( xyuv.screen ) )
                                    b_fullscreen = 1 - b_fullscreen;
                                break;

                            case SDLK_g:
                                if( xyuv.b_grid )
                                    xyuv.b_grid = 0;
                                else
                                    xyuv.b_grid = 1;
                                if( xyuv.b_pause )
                                    b_refresh = 1;
                                break;

                            case SDLK_SPACE:
                                if( xyuv.b_pause )
                                    xyuv.b_pause = 0;
                                else
                                    xyuv.b_pause = 1;
                                break;
                            case SDLK_LEFT:
                                if( xyuv.i_frame > 1 ) xyuv.i_frame--;
                                b_refresh = 1;
                                break;

                            case SDLK_RIGHT:
                                if( xyuv.i_frame >= xyuv.i_frames )
                                    xyuv_count_frames( &xyuv );
                                if( xyuv.i_frame < xyuv.i_frames ) xyuv.i_frame++;
                                b_refresh = 1;
                                break;

                            case SDLK_HOME:
                                xyuv.i_frame = 1;
                                if( xyuv.b_pause )
                                    b_refresh = 1;
                                break;

                            case SDLK_END:
                                xyuv_count_frames( &xyuv );
                                xyuv.i_frame = xyuv.i_frames;
                                b_refresh = 1;
                                break;

                            case SDLK_UP:
                                xyuv.i_frame += xyuv.i_frames / 20;

                                if( xyuv.i_frame >= xyuv.i_frames )
                                    xyuv_count_frames( &xyuv );

                                if( xyuv.i_frame > xyuv.i_frames )
                                    xyuv.i_frame = xyuv.i_frames;
                                b_refresh = 1;
                                break;

                            case SDLK_DOWN:
                                xyuv.i_frame -= xyuv.i_frames / 20;
                                if( xyuv.i_frame < 1 )
                                    xyuv.i_frame = 1;
                                b_refresh = 1;
                                break;

                            case SDLK_PAGEUP:
                                xyuv.i_frame += xyuv.i_frames / 10;

                                if( xyuv.i_frame >= xyuv.i_frames )
                                    xyuv_count_frames( &xyuv );

                                if( xyuv.i_frame > xyuv.i_frames )
                                    xyuv.i_frame = xyuv.i_frames;
                                b_refresh = 1;
                                break;

                            case SDLK_PAGEDOWN:
                                xyuv.i_frame -= xyuv.i_frames / 10;
                                if( xyuv.i_frame < 1 )
                                    xyuv.i_frame = 1;
                                b_refresh = 1;
                                break;

                            default:
                                break;
                        }
                        break;
                    case SDL_VIDEORESIZE:
                        xyuv.i_display_width = event.resize.w;
                        xyuv.i_display_height = event.resize.h;
                        xyuv.screen = SDL_SetVideoMode( xyuv.i_display_width, xyuv.i_display_height, 0,
                                                        SDL_HWSURFACE|SDL_RESIZABLE|
                                                        SDL_ASYNCBLIT|SDL_HWACCEL );
                        xyuv_display( &xyuv, xyuv.i_frame );
                        break;

                    default:
                        break;
                }
            }
            if( b_refresh ) {
                xyuv.b_pause = 1;
                xyuv_display( &xyuv, xyuv.i_frame );
            }
            /* wait */
            i_wait = 1000 / xyuv.f_fps - ( SDL_GetTicks() - i_start);
            if( i_wait < 0 )
                break;
            else if( i_wait > 200 )
                SDL_Delay( 200 );
            else {
                SDL_Delay( i_wait );
                break;
            }
        }
        if( !xyuv.b_pause ) {
            /* next frame */
            if( xyuv.i_frame == xyuv.i_frames )
                    xyuv.b_pause = 1;
            else if( xyuv.i_frame < xyuv.i_frames )
                xyuv.i_frame++;
        }
    }


    return 0;

err_missing_arg:
    fprintf( stderr, "missing arg for option=%s\n", argv[i] );
    return -1;
}


static void xyuv_display( xyuv_t *xyuv, int i_frame )
{
    SDL_Rect rect;
    int i_picture = 0;
    int i;

    if( i_frame > xyuv->i_frames )
        return;

    xyuv->i_frame = i_frame;

    /* Load and copy pictue data */
    for( i = 0; i < xyuv->i_yuv; i++ ) {
        int i_plane;

        fprintf( stderr, "yuv[%d] %d/%d\n", i, i_frame, xyuv->yuv[i].i_frames );
        if( i_frame - 1 >= xyuv->yuv[i].i_frames ) {
            xyuv_count_frames( xyuv );
            if( i_frame - 1 >= xyuv->yuv[i].i_frames )
                continue;
        }
        i_picture++;

        fseek( xyuv->yuv[i].f, (xyuv->i_frame-1) * xyuv->i_frame_size, SEEK_SET );
        fread( xyuv->pic, xyuv->i_frame_size, 1, xyuv->yuv[i].f );

        SDL_LockYUVOverlay( xyuv->overlay );

        if( xyuv->b_diff || xyuv->b_split ) {
            /* Reset UV */
            for( i_plane = 1; i_plane < 3; i_plane++ ) {
                memset( xyuv->overlay->pixels[i_plane], 128, xyuv->overlay->pitches[i_plane] * xyuv->overlay->h / 2 );
            }
            /* Show diff in Y plane of overlay */

            for( i_plane = 0; i_plane < 3; i_plane++ ) {
                int div = i_plane == 0 ? 1 : 2;
                uint8_t *src = xyuv->pic;
                uint8_t *dst = xyuv->overlay->pixels[0] +
                                (xyuv->yuv[i].x + xyuv->yuv[i].y * xyuv->overlay->pitches[0] );
                int j;
                if( i_plane == 1 ) {
                    src +=  5*xyuv->i_width * xyuv->i_height/4;
                    dst += xyuv->i_width;
                } else if( i_plane == 2 ) {
                    src += xyuv->i_width * xyuv->i_height;
                    dst += xyuv->i_width + xyuv->i_height / 2 * xyuv->overlay->pitches[0];
                }

                for( j = 0; j < xyuv->i_height / div; j++ ) {
                    if( i_picture == 1 || xyuv->b_split ) {
                        memcpy( dst, src, xyuv->i_width / div );
                    } else {
                        int k;
                        for( k = 0; k < xyuv->i_width / div; k++ ) {
                            dst[k] = abs( dst[k] - src[k]);
                        }
                    }
                    src += xyuv->i_width / div;
                    dst += xyuv->overlay->pitches[0];
                }
            }
        } else {
            for( i_plane = 0; i_plane < 3; i_plane++ ) {
                int div = i_plane == 0 ? 1 : 2;
                uint8_t *src = xyuv->pic;
                uint8_t *dst = xyuv->overlay->pixels[i_plane] +
                                ((xyuv->yuv[i].x + xyuv->yuv[i].y * xyuv->overlay->pitches[i_plane] ) / div );
                int w = xyuv->i_width / div;
                int j;

                if( i_plane == 1 ) {
                    src +=  5*xyuv->i_width * xyuv->i_height/4;
                } else if( i_plane == 2 ) {
                    src += xyuv->i_width * xyuv->i_height;
                }
                if( xyuv->i_join > 0 ) {
                    if( i_picture > 1 ) {
                        src += xyuv->i_join / div;
                        dst += xyuv->i_join / div;
                        w = (xyuv->i_width - xyuv->i_join) /div;
                    } else {
                        w = xyuv->i_join / div;
                    }
                }

                for( j = 0; j < xyuv->i_height / div; j++ ) {
                    memcpy( dst, src, w );
                    src += xyuv->i_width / div;
                    dst += xyuv->overlay->pitches[i_plane];
                }
            }
        }

        SDL_UnlockYUVOverlay( xyuv->overlay );
    }

    if( xyuv->f_y != 0.0 ) {
        uint8_t *pix = xyuv->overlay->pixels[0];
        int j;

        for( j = 0; j < xyuv->i_sdl_height; j++ ) {
            int k;
            for( k = 0; k < xyuv->i_sdl_width; k++ ) {
                int v= pix[k] * xyuv->f_y;
                if( v > 255 )
                    pix[k] = 255;
                else if( v < 0 )
                    pix[k] = 0;
                else
                    pix[k] = v;
            }
            pix += xyuv->overlay->pitches[0];
        }
    }
    if( xyuv->b_grid ) {
        int x, y;

        for( y = 0; y < xyuv->i_sdl_height; y += 4 ) {
            uint8_t *p = xyuv->overlay->pixels[0] + y * xyuv->overlay->pitches[0];
            for( x = 0; x < xyuv->i_sdl_width; x += 4 ) {
                if( x%16== 0 || y%16 == 0 )
                    p[x] = 0;
            }
        }
    }

    /* Update display */
    rect.x = 0;
    rect.y = 0;
    rect.w = xyuv->i_display_width;
    rect.h = xyuv->i_display_height;
    SDL_DisplayYUVOverlay( xyuv->overlay, &rect );

    /* Display title */
    if( xyuv->title )
        free( xyuv->title );
    asprintf( &xyuv->title, SDL_TITLE, xyuv->yuv[0].name, xyuv->i_frame, xyuv->i_frames, xyuv->f_fps );
    SDL_WM_SetCaption( xyuv->title, "" );
}

static void xyuv_count_frames( xyuv_t *xyuv )
{
    int i;

    xyuv->i_frames = 0;
    if( xyuv->i_frame_size <= 0 )
        return;

    for( i = 0; i < xyuv->i_yuv; i++ ) {
        /* Beurk but avoid using fstat */
        fseek( xyuv->yuv[i].f, 0, SEEK_END );

        xyuv->yuv[i].i_frames = ftell( xyuv->yuv[i].f ) / xyuv->i_frame_size;
        fprintf( stderr, "count (%d) -> %d\n", i, xyuv->yuv[i].i_frames );

        fseek( xyuv->yuv[i].f, 0, SEEK_SET );

        if( xyuv->i_frames < xyuv->yuv[i].i_frames )
            xyuv->i_frames = xyuv->yuv[i].i_frames;
    }
}

static inline int ssd( int a ) { return a*a; }

static void xyuv_detect( int *pi_width, int *pi_height )
{
    static const int pi_size[][2] = {
        {128, 96},
        {160,120},
        {320,244},
        {320,288},

        /* PAL */
        {176,144},  // QCIF
        {352,288},  // CIF
        {352,576},  // 1/2 D1
        {480,576},  // 2/3 D1
        {544,576},
        {640,576},  // VGA
        {704,576},  // D1
        {720,576},  // D1

        /* NTSC */
        {176,112},  // QCIF
        {320,240},  // MPEG I
        {352,240},  // CIF
        {352,480},  // 1/2 D1
        {480,480},  // 2/3 D1
        {544,480},
        {640,480},  // VGA
        {704,480},  // D1
        {720,480},  // D1

        /* */
        {0,0},
    };
    int i_max;
    int i_size_max;
    uint8_t *pic;
    int i;

    *pi_width = 0;
    *pi_height = 0;

    /* Compute size max */
    for( i_max = 0, i_size_max = 0;
            pi_size[i_max][0] != 0 && pi_size[i_max][1] != 0; i_max++ ) {
        int s = pi_size[i_max][0] * pi_size[i_max][1] * 3 / 2;

        if( i_size_max < s )
            i_size_max = s;
    }

    /* Temporary buffer */
    i_size_max *= 3;
    pic = malloc( i_size_max );

    fprintf( stderr, "guessing size for:\n" );
    for( i = 0; i < xyuv.i_yuv; i++ ) {
        int j;
        int i_read;
        double dbest = 255*255;
        int    i_best = i_max;
        int64_t t;

        fprintf( stderr, " - %s\n", xyuv.yuv[i].name );

        i_read = fread( pic, 1, i_size_max, xyuv.yuv[i].f );
        if( i_read < 0 )
            continue;

        /* Check if file size is at least compatible with one format
         * (if not, ignore file size)*/
        fseek( xyuv.yuv[i].f, 0, SEEK_END );
        t = ftell( xyuv.yuv[i].f );
        fseek( xyuv.yuv[i].f, 0, SEEK_SET );
        for( j = 0; j < i_max; j++ ) {
            const int w = pi_size[j][0];
            const int h = pi_size[j][1];
            const int s = w * h * 3 / 2;

            if( t % s == 0 )
                break;
        }
        if( j == i_max )
            t = 0;


        /* Try all size */
        for( j = 0; j < i_max; j++ ) {
            const int w = pi_size[j][0];
            const int h = pi_size[j][1];
            const int s = w * h * 3 / 2;
            double dd;

            int x, y, n;
            int64_t d;

            /* To small */
            if( i_read < 3*s )
                continue;
            /* Check file size */
            if( ( t > 0 && (t % s) != 0  ) ) {
                fprintf( stderr, "  * %dx%d ignored (incompatible file size)\n", w, h );
                continue;
            }


            /* We do a simple ssd between 2 consecutives lines */
            d = 0;
            for( n = 0; n < 3; n++ ) {
                uint8_t *p;

                /* Y */
                p = &pic[n*s];
                for( y = 0; y < h-1; y++ ) {
                    for( x = 0; x < w; x++ )
                        d += ssd( p[x] - p[w+x] );
                    p += w;
                }

                /* U */
                p = &pic[n*s+w*h];
                for( y = 0; y < h/2-1; y++ ) {
                    for( x = 0; x < w/2; x++ )
                        d += ssd( p[x] - p[(w/2)+x] );
                    p += w/2;
                }

                /* V */
                p = &pic[n*s+5*w*h/4];
                for( y = 0; y < h/2-1; y++ ) {
                    for( x = 0; x < w/2; x++ )
                        d += ssd( p[x] - p[(w/2)+x] );
                    p += w/2;
                }
            }
            dd = (double)d / (3*w*h*3/2);
            fprintf( stderr, "  * %dx%d d=%f\n", w, h, dd );

            if( dd < dbest ) {
                i_best = j;
                dbest = dd;
            }
        }

        fseek( xyuv.yuv[i].f, 0, SEEK_SET );

        if( i_best < i_max ) {
            fprintf( stderr, "  -> %dx%d\n", pi_size[i_best][0], pi_size[i_best][1] );
            *pi_width = pi_size[i_best][0];
            *pi_height = pi_size[i_best][1];
        }
    }

    free( pic );
}

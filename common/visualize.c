/*****************************************************************************
 * visualize.c: visualization
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

/*
 * Some explanation of the symbols used:
 * Red/pink: intra block
 * Blue: inter block
 * Green: skip block
 * Yellow: B-block (not visualized properly yet)
 *
 * Motion vectors have black dot at their target (ie. at the MB center),
 * instead of arrowhead. The black dot is enclosed in filled diamond with radius
 * depending on reference frame number (one frame back = zero width, normal case).
 *
 * The intra blocks have generally lines drawn perpendicular
 * to the prediction direction, so for example, if there is a pink block
 * with horizontal line at the top of it, it is interpolated by assuming
 * luma to be vertically constant.
 * DC predicted blocks have both horizontal and vertical lines,
 * pink blocks with a diagonal line are predicted using the planar function.
 */

#include "common.h"
#include "visualize.h"
#include "display.h"

typedef struct
{
    int     i_type;
    int     i_partition;
    int     i_sub_partition[4];
    int     i_intra16x16_pred_mode;
    int     intra4x4_pred_mode[4][4];
    int8_t  ref[2][4][4];                  /* [list][y][x] */
    int16_t mv[2][4][4][2];                /* [list][y][x][mvxy] */
} visualize_t;

/* Return string from stringlist corresponding to the given code */
#define GET_STRING(sl, code) get_string((sl), sizeof(sl)/sizeof(*(sl)), code)

typedef struct
{
    int code;
    char *string;
} stringlist_t;

static char *get_string( const stringlist_t *sl, int entries, int code )
{
    for( int i = 0; i < entries; i++ )
        if( sl[i].code == code )
            return sl[i].string;
    return "?";
}

/* Plot motion vector */
static void mv( int x0, int y0, int16_t dmv[2], int ref, int zoom, char *col )
{
    int dx = dmv[0];
    int dy = dmv[1];

    dx = (dx * zoom + 2) >> 2;
    dy = (dy * zoom + 2) >> 2;
    disp_line( 0, x0, y0, x0+dx, y0+dy );
    for( int i = 1; i < ref; i++ )
    {
        disp_line( 0, x0  , y0-i, x0+i, y0   );
        disp_line( 0, x0+i, y0  , x0  , y0+i );
        disp_line( 0, x0  , y0+i, x0-i, y0   );
        disp_line( 0, x0-i, y0  , x0  , y0-i );
    }
    disp_setcolor( "black" );
    disp_point( 0, x0, y0 );
    disp_setcolor( col );
}

int x264_visualize_init( x264_t *h )
{
    CHECKED_MALLOC( h->visualize, h->mb.i_mb_width * h->mb.i_mb_height * sizeof(visualize_t) );
    return 0;
fail:
    return -1;
}

void x264_visualize_mb( x264_t *h )
{
    visualize_t *v = (visualize_t*)h->visualize + h->mb.i_mb_xy;

    /* Save all data for the MB that we need for drawing the visualization */
    v->i_type = h->mb.i_type;
    v->i_partition = h->mb.i_partition;
    for( int i = 0; i < 4; i++ )
        v->i_sub_partition[i] = h->mb.i_sub_partition[i];
    for( int y = 0; y < 4; y++ )
        for( int x = 0; x < 4; x++ )
            v->intra4x4_pred_mode[y][x] = h->mb.cache.intra4x4_pred_mode[X264_SCAN8_0+y*8+x];
    for( int l = 0; l < 2; l++ )
        for( int y = 0; y < 4; y++ )
            for( int x = 0; x < 4; x++ )
            {
                for( int i = 0; i < 2; i++ )
                    v->mv[l][y][x][i] = h->mb.cache.mv[l][X264_SCAN8_0+y*8+x][i];
                v->ref[l][y][x] = h->mb.cache.ref[l][X264_SCAN8_0+y*8+x];
            }
    v->i_intra16x16_pred_mode = h->mb.i_intra16x16_pred_mode;
}

void x264_visualize_close( x264_t *h )
{
    x264_free(h->visualize);
}

/* Display visualization (block types, MVs) of the encoded frame */
/* FIXME: B-type MBs not handled yet properly */
void x264_visualize_show( x264_t *h )
{
    static const stringlist_t mb_types[] =
    {
        /* Block types marked as NULL will not be drawn */
        { I_4x4   , "red" },
        { I_8x8   , "#ff5640" },
        { I_16x16 , "#ff8060" },
        { I_PCM   , "violet" },
        { P_L0    , "SlateBlue" },
        { P_8x8   , "blue" },
        { P_SKIP  , "green" },
        { B_DIRECT, "yellow" },
        { B_L0_L0 , "yellow" },
        { B_L0_L1 , "yellow" },
        { B_L0_BI , "yellow" },
        { B_L1_L0 , "yellow" },
        { B_L1_L1 , "yellow" },
        { B_L1_BI , "yellow" },
        { B_BI_L0 , "yellow" },
        { B_BI_L1 , "yellow" },
        { B_BI_BI , "yellow" },
        { B_8x8   , "yellow" },
        { B_SKIP  , "yellow" },
    };

    static const int waitkey = 1;     /* Wait for enter after each frame */
    static const int drawbox = 1;     /* Draw box around each block */
    static const int borders = 0;     /* Display extrapolated borders outside frame */
    static const int zoom = 2;        /* Zoom factor */

    static const int pad = 32;
    pixel *const frame = h->fdec->plane[0];
    const int width = h->param.i_width;
    const int height = h->param.i_height;
    const int stride = h->fdec->i_stride[0];

    if( borders )
        disp_gray_zoom( 0, frame - pad*stride - pad, width+2*pad, height+2*pad, stride, "fdec", zoom );
    else
        disp_gray_zoom( 0, frame, width, height, stride, "fdec", zoom );

    for( int mb_xy = 0; mb_xy < h->mb.i_mb_width * h->mb.i_mb_height; mb_xy++ )
    {
        visualize_t *const v = (visualize_t*)h->visualize + mb_xy;
        const int mb_y = mb_xy / h->mb.i_mb_width;
        const int mb_x = mb_xy % h->mb.i_mb_width;
        char *const col = GET_STRING( mb_types, v->i_type );
        int x = mb_x*16*zoom;
        int y = mb_y*16*zoom;
        int l = 0;

        if( !col )
            continue;

        if( borders )
        {
            x += pad*zoom;
            y += pad*zoom;
        }

        disp_setcolor( col );
        if( drawbox ) disp_rect( 0, x, y, x+16*zoom-1, y+16*zoom-1 );

        if( v->i_type==P_L0 || v->i_type==P_8x8 || v->i_type==P_SKIP )
        {
            /* Predicted (inter) mode, with motion vector */
            if( v->i_partition == D_16x16 || v->i_type == P_SKIP )
                mv( x+8*zoom, y+8*zoom, v->mv[l][0][0], v->ref[l][0][0], zoom, col );
            else if (v->i_partition == D_16x8)
            {
                if( drawbox ) disp_rect( 0, x, y, x+16*zoom, y+8*zoom );
                mv( x+8*zoom, y+4*zoom, v->mv[l][0][0], v->ref[l][0][0], zoom, col );
                if( drawbox ) disp_rect( 0, x, y+8*zoom, x+16*zoom, y+16*zoom );
                mv( x+8*zoom, y+12*zoom, v->mv[l][2][0], v->ref[l][2][0], zoom, col );
            }
            else if( v->i_partition==D_8x16 )
            {
                if( drawbox ) disp_rect( 0, x,          y, x+8*zoom,  y+16*zoom );
                mv( x+4*zoom, y+8*zoom, v->mv[l][0][0], v->ref[l][0][0], zoom, col );
                if( drawbox ) disp_rect( 0, x+8*zoom,   y, x+16*zoom, y+16*zoom );
                mv( x+12*zoom, y+8*zoom, v->mv[l][0][2], v->ref[l][0][2], zoom, col );
            }
            else if( v->i_partition==D_8x8 )
            {
                for( int i = 0; i < 2; i++ )
                    for( int j = 0; j < 2; j++ )
                    {
                        int sp = v->i_sub_partition[i*2+j];
                        const int x0 = x + j*8*zoom;
                        const int y0 = y + i*8*zoom;
                        l = x264_mb_partition_listX_table[0][sp] ? 0 : 1; /* FIXME: not tested if this works */
                        if( IS_SUB8x8(sp) )
                        {
                            if( drawbox ) disp_rect( 0, x0, y0, x0+8*zoom, y0+8*zoom );
                            mv( x0+4*zoom, y0+4*zoom, v->mv[l][2*i][2*j], v->ref[l][2*i][2*j], zoom, col );
                        }
                        else if( IS_SUB8x4(sp) )
                        {
                            if( drawbox ) disp_rect( 0, x0, y0, x0+8*zoom, y0+4*zoom );
                            if( drawbox ) disp_rect( 0, x0, y0+4*zoom, x0+8*zoom, y0+8*zoom );
                            mv( x0+4*zoom, y0+2*zoom, v->mv[l][2*i][2*j], v->ref[l][2*i][2*j], zoom, col );
                            mv( x0+4*zoom, y0+6*zoom, v->mv[l][2*i+1][2*j], v->ref[l][2*i+1][2*j], zoom, col );
                        }
                        else if( IS_SUB4x8(sp) )
                        {
                            if( drawbox ) disp_rect( 0, x0, y0, x0+4*zoom, y0+8*zoom );
                            if( drawbox ) disp_rect( 0, x0+4*zoom, y0, x0+8*zoom, y0+8*zoom );
                            mv( x0+2*zoom, y0+4*zoom, v->mv[l][2*i][2*j], v->ref[l][2*i][2*j], zoom, col );
                            mv( x0+6*zoom, y0+4*zoom, v->mv[l][2*i][2*j+1], v->ref[l][2*i][2*j+1], zoom, col );
                        }
                        else if( IS_SUB4x4(sp) )
                        {
                            if( drawbox ) disp_rect( 0, x0, y0, x0+4*zoom, y0+4*zoom );
                            if( drawbox ) disp_rect( 0, x0+4*zoom, y0, x0+8*zoom, y0+4*zoom );
                            if( drawbox ) disp_rect( 0, x0, y0+4*zoom, x0+4*zoom, y0+8*zoom );
                            if( drawbox ) disp_rect( 0, x0+4*zoom, y0+4*zoom, x0+8*zoom, y0+8*zoom );
                            mv( x0+2*zoom, y0+2*zoom, v->mv[l][2*i][2*j], v->ref[l][2*i][2*j], zoom, col );
                            mv( x0+6*zoom, y0+2*zoom, v->mv[l][2*i][2*j+1], v->ref[l][2*i][2*j+1], zoom, col );
                            mv( x0+2*zoom, y0+6*zoom, v->mv[l][2*i+1][2*j], v->ref[l][2*i+1][2*j], zoom, col );
                            mv( x0+6*zoom, y0+6*zoom, v->mv[l][2*i+1][2*j+1], v->ref[l][2*i+1][2*j+1], zoom, col );
                        }
                    }
            }
        }

        if( IS_INTRA(v->i_type) || v->i_type == I_PCM )
        {
            /* Intra coded */
            if( v->i_type == I_16x16 )
            {
                switch (v->i_intra16x16_pred_mode) {
                case I_PRED_16x16_V:
                    disp_line( 0, x+2*zoom, y+2*zoom, x+14*zoom, y+2*zoom );
                    break;
                case I_PRED_16x16_H:
                    disp_line( 0, x+2*zoom, y+2*zoom, x+2*zoom, y+14*zoom );
                    break;
                case I_PRED_16x16_DC:
                case I_PRED_16x16_DC_LEFT:
                case I_PRED_16x16_DC_TOP:
                case I_PRED_16x16_DC_128:
                    disp_line( 0, x+2*zoom, y+2*zoom, x+14*zoom, y+2*zoom );
                    disp_line( 0, x+2*zoom, y+2*zoom, x+2*zoom, y+14*zoom );
                    break;
                case I_PRED_16x16_P:
                    disp_line( 0, x+2*zoom, y+2*zoom, x+8*zoom, y+8*zoom );
                    break;
                }
            }
            if( v->i_type==I_4x4 || v->i_type==I_8x8 )
            {
                const int di = v->i_type == I_8x8 ? 2 : 1;
                const int zoom2 = zoom * di;
                for( int i = 0; i < 4; i += di )
                    for( int j = 0; j < 4; j += di )
                    {
                        const int x0 = x + j*4*zoom;
                        const int y0 = y + i*4*zoom;
                        if( drawbox ) disp_rect( 0, x0, y0, x0+4*zoom2, y0+4*zoom2 );
                        switch( v->intra4x4_pred_mode[i][j] )
                        {
                            case I_PRED_4x4_V:        /* Vertical */
                                disp_line( 0, x0+0*zoom2, y0+1*zoom2, x0+4*zoom2, y0+1*zoom2 );
                                break;
                            case I_PRED_4x4_H:        /* Horizontal */
                                disp_line( 0, x0+1*zoom2, y0+0*zoom2, x0+1*zoom2, y0+4*zoom2 );
                                break;
                            case I_PRED_4x4_DC:        /* DC, average from top and left sides */
                            case I_PRED_4x4_DC_LEFT:
                            case I_PRED_4x4_DC_TOP:
                            case I_PRED_4x4_DC_128:
                                disp_line( 0, x0+1*zoom2, y0+1*zoom2, x0+4*zoom2, y0+1*zoom2 );
                                disp_line( 0, x0+1*zoom2, y0+1*zoom2, x0+1*zoom2, y0+4*zoom2 );
                                break;
                            case I_PRED_4x4_DDL:    /* Topright-bottomleft */
                                disp_line( 0, x0+0*zoom2, y0+0*zoom2, x0+4*zoom2, y0+4*zoom2 );
                                break;
                            case I_PRED_4x4_DDR:    /* Topleft-bottomright */
                                disp_line( 0, x0+0*zoom2, y0+4*zoom2, x0+4*zoom2, y0+0*zoom2 );
                                break;
                            case I_PRED_4x4_VR:        /* Mix of topleft-bottomright and vertical */
                                disp_line( 0, x0+0*zoom2, y0+2*zoom2, x0+4*zoom2, y0+1*zoom2 );
                                break;
                            case I_PRED_4x4_HD:        /* Mix of topleft-bottomright and horizontal */
                                disp_line( 0, x0+2*zoom2, y0+0*zoom2, x0+1*zoom2, y0+4*zoom2 );
                                break;
                            case I_PRED_4x4_VL:        /* Mix of topright-bottomleft and vertical */
                                disp_line( 0, x0+0*zoom2, y0+1*zoom2, x0+4*zoom2, y0+2*zoom2 );
                                break;
                            case I_PRED_4x4_HU:        /* Mix of topright-bottomleft and horizontal */
                                disp_line( 0, x0+1*zoom2, y0+0*zoom2, x0+2*zoom2, y0+4*zoom2 );
                                break;
                        }
                    }
            }
        }
    }

    disp_sync();
    if( waitkey )
        getchar();
}
/* }}} */

//EOF

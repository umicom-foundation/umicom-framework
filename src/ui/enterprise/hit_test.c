/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/hit_test.c
 *
 * PURPOSE:
 *   Implement uniform-grid hit testing for virtual surfaces.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/hit_test.h"
/*
 * Provide the ui ent hit test uniform operation used by this module and its client
 * applications.
 */
UmiUiEntHit umi_ui_ent_hit_test_uniform(int32_t x,int32_t y,int32_t rh,int32_t cw,size_t ro,size_t co,size_t rows,size_t cols){UmiUiEntHit h={0U,0U,0};size_t r,c;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(x<0||y<0||rh<=0||cw<=0)return h;r=ro+(size_t)(y/rh);c=co+(size_t)(x/cw);/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(r>=rows||c>=cols)return h;h.row=r;h.column=c;h.valid=1;return h;}

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/rad/size.c
 *
 * PURPOSE:
 *   Provide bounded designer size values and clamp operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/size.h"
/* Provide the clamp i32 operation used by this module and its client applications. */
static int32_t clamp_i32(int32_t v,int32_t lo,int32_t hi){return v<lo?lo:(v>hi?hi:v);}
/* Provide the rad size make operation used by this module and its client applications. */
UmiRadDesignerSize umi_rad_size_make(int32_t w,int32_t h){UmiRadDesignerSize s={w,h};return s;}
/* Provide the rad size clamp operation used by this module and its client applications. */
UmiRadDesignerSize umi_rad_size_clamp(UmiRadDesignerSize v,UmiRadDesignerSize lo,UmiRadDesignerSize hi){v.width=clamp_i32(v.width,lo.width,hi.width);v.height=clamp_i32(v.height,lo.height,hi.height);return v;}

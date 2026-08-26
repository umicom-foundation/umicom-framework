/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/rad/size.c
 *
 * PURPOSE:
 *   Provide bounded designer size values and clamp operations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/size.h"
static int32_t clamp_i32(int32_t v,int32_t lo,int32_t hi){return v<lo?lo:(v>hi?hi:v);}
UmiRadDesignerSize umi_rad_size_make(int32_t w,int32_t h){UmiRadDesignerSize s={w,h};return s;}
UmiRadDesignerSize umi_rad_size_clamp(UmiRadDesignerSize v,UmiRadDesignerSize lo,UmiRadDesignerSize hi){v.width=clamp_i32(v.width,lo.width,hi.width);v.height=clamp_i32(v.height,lo.height,hi.height);return v;}

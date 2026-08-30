/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/rad/coordinate.c
 *
 * PURPOSE:
 *   Provide deterministic integer designer coordinates and translation operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/coordinate.h"
UmiRadCoordinate umi_rad_coordinate_make(int32_t x,int32_t y){UmiRadCoordinate p={x,y};return p;}
UmiRadCoordinate umi_rad_coordinate_translate(UmiRadCoordinate p,int32_t dx,int32_t dy){p.x+=dx;p.y+=dy;return p;}

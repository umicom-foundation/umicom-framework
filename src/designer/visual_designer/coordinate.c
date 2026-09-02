/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/coordinate.c
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
#include "umicom/designer/visual_designer/coordinate.h"
/*
 * Provide the visual designer coordinate make operation used by this module and its client
 * applications.
 */
UmiRadCoordinate umi_rad_coordinate_make(int32_t x,int32_t y){UmiRadCoordinate p={x,y};return p;}
/*
 * Provide the visual designer coordinate translate operation used by this module and its client
 * applications.
 */
UmiRadCoordinate umi_rad_coordinate_translate(UmiRadCoordinate p,int32_t dx,int32_t dy){p.x+=dx;p.y+=dy;return p;}

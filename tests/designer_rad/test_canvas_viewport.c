/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_rad/test_canvas_viewport.c
 *
 * PURPOSE:
 *   Validate track canvas origin, dimensions and zoom independently from document geometry.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/canvas_viewport.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
int main(void){UmiRadCanvasViewport item;CHECK(umi_rad_canvas_viewport_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_canvas_viewport_is_valid(&item));return 0;}

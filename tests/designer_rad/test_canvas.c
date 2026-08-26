/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_rad/test_canvas.c
 *
 * PURPOSE:
 *   Validate describe a visual application design canvas and its revision state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/canvas.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
int main(void){UmiRadCanvas item;CHECK(umi_rad_canvas_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_canvas_is_valid(&item));return 0;}

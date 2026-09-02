/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/visual_designer/test_canvas.c
 *
 * PURPOSE:
 *   Validate describe a visual application design canvas and its revision state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/canvas.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiRadCanvas item;CHECK(umi_rad_canvas_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_canvas_is_valid(&item));return 0;}

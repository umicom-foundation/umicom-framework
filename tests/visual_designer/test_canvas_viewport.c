/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/visual_designer/test_canvas_viewport.c
 *
 * PURPOSE:
 *   Validate track canvas origin, dimensions and zoom independently from document geometry.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/canvas_viewport.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiRadCanvasViewport item;CHECK(umi_rad_canvas_viewport_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_canvas_viewport_is_valid(&item));return 0;}

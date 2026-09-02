/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/visual_designer/test_zoom.c
 *
 * PURPOSE:
 *   Validate provide bounded zoom policy for visual authoring surfaces.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/zoom.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiRadZoomPolicy item;CHECK(umi_rad_zoom_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_zoom_is_valid(&item));return 0;}

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_rad/test_zoom.c
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
#include "umicom/designer/rad/zoom.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
int main(void){UmiRadZoomPolicy item;CHECK(umi_rad_zoom_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_zoom_is_valid(&item));return 0;}

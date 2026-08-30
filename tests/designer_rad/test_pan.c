/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_rad/test_pan.c
 *
 * PURPOSE:
 *   Validate provide deterministic canvas panning state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/pan.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
int main(void){UmiRadPanState item;CHECK(umi_rad_pan_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_pan_is_valid(&item));return 0;}

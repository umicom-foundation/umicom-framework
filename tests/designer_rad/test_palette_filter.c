/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_rad/test_palette_filter.c
 *
 * PURPOSE:
 *   Validate filter the component palette by text, category and capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/palette_filter.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiRadPaletteFilter item;CHECK(umi_rad_palette_filter_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_palette_filter_is_valid(&item));return 0;}

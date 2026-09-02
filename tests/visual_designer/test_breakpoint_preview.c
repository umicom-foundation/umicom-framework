/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/visual_designer/test_breakpoint_preview.c
 *
 * PURPOSE:
 *   Validate resolve a named responsive preview breakpoint for the visual canvas.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/visual_designer/breakpoint_preview.h"
#define CHECK(x) do{if(!(x))return 1;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiRadBreakpointPreview item;CHECK(umi_rad_breakpoint_preview_init(&item)==UMI_STATUS_OK);CHECK(umi_rad_breakpoint_preview_is_valid(&item));return 0;}
